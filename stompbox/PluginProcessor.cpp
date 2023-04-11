#include <iostream>
#include <sstream>

#include "architecture.hpp"

#include "PluginProcessor.h"

#if _WIN32
void GetDataPathFromModule(std::filesystem::path& dataPath)
{
    char path[MAX_PATH];

    HMODULE hm = NULL;

    if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCSTR)&GetDataPathFromModule, &hm) == 0)
    {
        int ret = GetLastError();
        fprintf(stderr, "GetModuleHandle failed, error = %d\n", ret);
        // Return or however you want to handle an error.
    }
    if (GetModuleFileNameA(hm, path, MAX_PATH) == 0)
    {
        int ret = GetLastError();
        fprintf(stderr, "GetModuleFileName failed, error = %d\n", ret);
        // Return or however you want to handle an error.
    }

    int len = strlen(path);

    for (int i = len - 1; i > 0; i--)
    {
        if ((path[i] == '/') || (path[i] == '\\'))
        {
            path[i] = 0;

            break;
        }
    }

    dataPath.assign(path);
}
#endif

#if _WIN32
PluginProcessor::PluginProcessor(std::filesystem::path dataPath, bool dawMode)
{
    this->dataPath.assign(dataPath);
#else
PluginProcessor::PluginProcessor(bool dawMode)
{
    dataPath.assign(std::filesystem::current_path());
#endif    

    presetPath.assign(dataPath);
    presetPath.append("Presets");    

    if (!std::filesystem::exists(presetPath))
        std::filesystem::create_directory(presetPath);

    pluginFactory.SetDataPath(dataPath);

    inputGain = (Gain *)CreatePlugin("Input");
    inputGain->Enabled = true;

    masterVolume = (Gain*)CreatePlugin("Master");
    masterVolume->Enabled = true;

    preamp = (AmpStage*)CreatePlugin("Preamp");
    preamp->Enabled = true;

    poweramp = (AmpStage*)CreatePlugin("Poweramp");
    poweramp->Enabled = true;

    tonestack = (Tonestack*)CreatePlugin("Tonestack");
    tonestack->Enabled = true;

    cabinet = (GuitarConvolver*)CreatePlugin("Cabinet");
    cabinet->Enabled = true;

    tuner = (PitchDetector*)CreatePlugin("Tuner");

    audioFilePlayer = (AudioFilePlayer*)CreatePlugin("AudioFilePlayer");

    if (!dawMode)
    {
        audioFileRecorder = (AudioFileRecorder*)CreatePlugin("AudioFileRecorder");
        audioFileRecorder->Enabled = true;
    }

    tmpBuf1 = new double[tmpBufSize];
    tmpBuf2 = new double[tmpBufSize];

    for (int i = 0; i < tmpBufSize; i++)
    {
        tmpBuf1[i] = 0;
    }

    for (int i = 0; i < 16; i++)
    {
        stompCC[i] = -1;
    }

    UpdatePlugins();

    LoadSettings();

    serialDisplayInterface.Connect();

    //StartServer();

    //guitarClient.SetLineCallback([this](std::string line) { return HandleCommand(line); });
    //guitarClient.Start();
}

PluginProcessor::~PluginProcessor()
{
    if (tmpBuf1 != nullptr)
    {
        delete[] tmpBuf1;
        delete[] tmpBuf2;
    }

    if (serialDisplayInterface.IsConnected())
        serialDisplayInterface.Close();
}

void PluginProcessor::Init(double sampleRate)
{
    fprintf(stderr, "Init with sample rate: %d\n", (int)sampleRate);

    this->sampleRate = sampleRate;

    bool wasInitialized = initialized;
    initialized = true;

    for (const auto& plugin : plugins)
    {
        // Initialize right now if it is the first time. Otherwise we could be running, so set a flag to initialize in Process()
        if (!wasInitialized)
            InitPlugin(plugin);
        else
            plugin->NeedsInit = true;
    }

    initialized = true;

    fprintf(stderr, "Starting client update thread\n");

    clientUpdateThread = new std::thread(&PluginProcessor::UpdateClient, this);
}

void PluginProcessor::SetBPM(double bpm)
{
    this->bpm = bpm;

    for (const auto& plugin : plugins)
    {
        plugin->SetBPM(bpm);
    }
}

void PluginProcessor::StartServer()
{
    guitarServer.SetLineCallback([this](std::string line) { return HandleCommand(line); });
    guitarServer.Start();

    serverIsRunning = true;
}

void PluginProcessor::SetMessageCallback(std::function<void(const std::string)> callback)
{
    messageCallback = callback;

    if (clientUpdateThread == nullptr)
    {
        clientUpdateThread = new std::thread(&PluginProcessor::UpdateClient, this);
    }
}

void PluginProcessor::ReportDSPLoad(float currentLoad)
{
    if (currentLoad < minDSPLoad)
        minDSPLoad = currentLoad;

    if (currentLoad > maxDSPLoad)
        maxDSPLoad = currentLoad;
}

void PluginProcessor::UpdateClient()
{
    std::string cmd;

    do
    {
        if (!(HaveClient() && pluginOutputOn) && (messageCallback == nullptr) && !serialDisplayInterface.IsConnected())
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            if (serialDisplayInterface.IsConnected())
            {
                switch (currentMidiMode)
                {
                case MIDI_MODE_TUNER:
                    if (tuner->Enabled && (tuner->OutputValue != nullptr))
                    {
                        serialDisplayInterface.UpdateTuner(*(tuner->OutputValue));
                    }

                    break;

                case MIDI_MODE_RECORDER:
                    if ((audioFileRecorder != nullptr) && audioFileRecorder->Enabled)
                    {
                        double secondsRecorded = audioFileRecorder->GetParameterValue(AUDIOFILERECORDER_SECONDSRECORDED);

                        serialDisplayInterface.UpdateRecordSeconds(secondsRecorded);
                    }

                    break;
                }
            }

            if ((HaveClient() && pluginOutputOn) || (messageCallback == nullptr))
            {
                clientUpdateFrame++;

                for (const auto& plugin : plugins)
                {
                    if (plugin->Enabled && (plugin->OutputValue != nullptr))
                    {
                        cmd.clear();

                        cmd.append("SetOutput ");
                        cmd.append(plugin->ID);
                        cmd.append(" ");
                        cmd.append(std::to_string(*(plugin->OutputValue)));
                        cmd.append("\r\n");

                        SendClientMessage(cmd);
                    }

                    if (plugin->ParamIsDirty || plugin->EnabledIsDirty)
                    {
                        cmd.clear();

                        AppendPluginParams(cmd, plugin, true);

                        plugin->ParamIsDirty = false;
                        plugin->EnabledIsDirty = false;

                        SendClientMessage(cmd);
                    }
                }

                if (((clientUpdateFrame % 10) == 0) && (maxDSPLoad != 0))
                {
                    SendClientMessage("DSPLoad " + std::to_string(maxDSPLoad) + " " + std::to_string(minDSPLoad) + "\r\n");

                    minDSPLoad = 1;
                    maxDSPLoad = 0;
                }
            }
        }
    }
    while (true);
}

void PluginProcessor::SendClientMessage(const std::string message)
{
    if (messageCallback != nullptr)
        messageCallback(message);

    if (HaveClient())
        guitarServer.SendData(message);
}

void PluginProcessor::UpdatePlugins()
{
    std::list<StompBox*>& newPlugins = (plugins == pluginList1) ? pluginList2 : pluginList1;

    fprintf(stderr, "Update Plugins\n");

    newPlugins.clear();

    newPlugins.push_back(tuner);

    //if (audioFileRecorder != nullptr)
    //    newPlugins.push_back(audioFileRecorder);

    newPlugins.push_back(inputGain);

    for (const auto& plugin : inputChain)
    {
        newPlugins.push_back(plugin);
    }

    newPlugins.push_back(preamp);
    newPlugins.push_back(tonestack);

    for (const auto& plugin : fxLoop)
    {
        newPlugins.push_back(plugin);
    }

    newPlugins.push_back(poweramp);
    newPlugins.push_back(cabinet);

    for (const auto& plugin : outputChain)
    {
        newPlugins.push_back(plugin);
    }

    newPlugins.push_back(masterVolume);

    newPlugins.push_back(audioFilePlayer);

    if (audioFileRecorder != nullptr)
        newPlugins.push_back(audioFileRecorder);

    plugins = newPlugins;

    for (const auto& plugin : plugins)
    {
        fprintf(stderr, "%s ", plugin->ID.c_str());
    }

    fprintf(stderr, "\n");
}

StompBox* PluginProcessor::CreatePlugin(std::string const& id)
{
    StompBox* plugin = pluginFactory.CreatePlugin(id);

    plugin->MidiCallback = midiCallback;

    InitPlugin(plugin);

    return plugin;
}

void PluginProcessor::InitPlugin(StompBox* plugin)
{
    if (initialized)
    {
        fprintf(stderr, "Init plugin: %s\n", plugin->Name.c_str());

        plugin->init(sampleRate);
        plugin->SetBPM(bpm);

        if (plugin->RampTime > 0)
        {
            plugin->RampSamples = sampleRate * plugin->RampTime;
            plugin->RampPos = 0; // -plugin->RampSamples;
            //int numPasses = ceil((sampleRate * 0.5) / (double)tmpBufSize);

            //fprintf(stderr, "**Ramp %d passes of %d samples\n", numPasses, tmpBufSize);

            //for (int pass = 0; pass < numPasses; pass++)
            //{
            //    plugin->compute(tmpBufSize, tmpBuf1, tmpBuf2);
            //}
        }

        plugin->NeedsInit = false;
    }
}

std::string PluginProcessor::DumpConfig()
{
    std::string dump;

    for (const auto& plugin : pluginFactory.GetAllPlugins())
    {
        StompBox* component = pluginFactory.CreatePlugin(plugin, plugin);

        dump.append("PluginConfig ");
        dump.append(component->Name);

        if (!component->BackgroundColor.empty())
        {
            dump.append(" BackgroundColor ");
            dump.append(component->BackgroundColor);
        }

        if (!component->ForegroundColor.empty())
        {
            dump.append(" ForegroundColor ");
            dump.append(component->ForegroundColor);
        }

        dump.append(" IsUserSelectable ");
        dump.append(component->IsUserSelectable ? "1" : "0");

        dump.append("\r\n");

        for (int i = 0; i < component->NumParameters; i++)
        {
            StompBoxParameter* param = component->GetParameter(i);

            dump.append("ParameterConfig ");
            dump.append(component->Name);
            dump.append(" ");
            dump.append(param->Name);
            
            dump.append(" Type ");

            switch (param->ParameterType)
            {
            case PARAMETER_TYPE_KNOB:
                dump.append("Knob");
                break;
            case PARAMETER_TYPE_BOOL:
                dump.append("Bool");
                break;
            case PARAMETER_TYPE_INT:
                dump.append("Int");
                break;
            case PARAMETER_TYPE_VSLIDER:
                dump.append("VSlider");
                break;
            case PARAMETER_TYPE_ENUM:
                dump.append("Enum");
                break;
            }

            dump.append(" MinValue ");
            dump.append(std::to_string(param->MinValue));

            dump.append(" MaxValue ");
            dump.append(std::to_string(param->MaxValue));

            dump.append(" DefaultValue ");
            dump.append(std::to_string(param->DefaultValue));

            dump.append(" ValueFormat ");
            dump.append(param->DisplayFormat);

            dump.append(" CanSyncToHostBPM ");
            dump.append(param->CanSyncToHostBPM ? "1" : "0");

            dump.append(" IsAdvanced ");
            dump.append(param->IsAdvanced ? "1" : "0");

            dump.append("\r\n");

            if (param->ParameterType == PARAMETER_TYPE_ENUM)
            {
                dump.append("ParameterEnumValues ");

                dump.append(component->Name);
                dump.append(" ");
                dump.append(param->Name);


                for (const auto& enumValue : *(param->EnumValues))
                {
                    dump.append(" ");
                    dump.append(enumValue);
                }

                dump.append("\r\n");
            }           
        }

        dump.append("EndConfig\r\n");
    }

    return dump;
}

void PluginProcessor::AppendPluginParams(std::string& dump, StompBox* plugin, bool dirtyOnly)
{
    if (!dirtyOnly || plugin->EnabledIsDirty)
    {
        dump.append("SetParam ");
        dump.append(plugin->ID);
        dump.append(" Enabled ");
        dump.append(plugin->Enabled ? "1" : "0");
        dump.append("\r\n");
    }

    for (int i = 0; i < plugin->NumParameters; i++)
    {
        StompBoxParameter* param = plugin->GetParameter(i);

        if (!param->SuppressSave && (!dirtyOnly || param->IsDirty))
        {
            dump.append("SetParam ");
            dump.append(plugin->ID);
            dump.append(" ");
            dump.append(param->Name);
            dump.append(" ");

            if (param->ParameterType == PARAMETER_TYPE_ENUM)
            {
                int enumIndex = (int)plugin->GetParameterValue(i);

                if ((enumIndex >= 0) && (enumIndex <= param->MaxValue))
                    dump.append((*param->EnumValues)[enumIndex]);
            }
            else if ((param->ParameterType == PARAMETER_TYPE_INT) || (param->ParameterType == PARAMETER_TYPE_BOOL))
            {
                dump.append(std::to_string((int)plugin->GetParameterValue(i)));
            }
            else if (param->CanSyncToHostBPM && (param->BPMSyncNumerator != 0) && (param->BPMSyncDenominator != 0))
            {
                dump.append(std::to_string(param->BPMSyncNumerator));
                dump.append("/");
                dump.append(std::to_string(param->BPMSyncDenominator));
            }
            else
            {
                dump.append(std::to_string(plugin->GetParameterValue(i)));
            }
            dump.append("\r\n");
        }
    }
}

std::string PluginProcessor::DumpSettings()
{
    std::string dump;

    AppendPluginParams(dump, inputGain, false);
    AppendPluginParams(dump, masterVolume, false);

    if (audioFileRecorder != nullptr)
        AppendPluginParams(dump, audioFileRecorder, false);

    AppendPluginParams(dump, audioFilePlayer, false);
    AppendPluginParams(dump, tuner, false);

    if (modeChangeCC != -1)
    {
        dump.append("MapModeController ");
        dump.append(std::to_string(modeChangeCC));
        dump.append("\r\n");
    }

    for (int i = 0; i < 16; i++)
    {
        if (stompCC[i] != -1)
        {
            dump.append("MapStompController ");
            dump.append(std::to_string(i + 1));
            dump.append(" ");
            dump.append(std::to_string(stompCC[i]));
            dump.append("\r\n");
        }
    }

    return dump;
}

std::string PluginProcessor::DumpProgram()
{
    std::string dump;

    dump.append("SetPreset ");
    dump.append(currentPreset);
    dump.append("\r\n");

    dump.append("SetChain Input ");

    for (const auto& plugin : inputChain)
    {
        dump.append(plugin->ID);
        dump.append(" ");
    }

    dump.append("\r\n");

    dump.append("SetChain FxLoop ");

    for (const auto& plugin : fxLoop)
    {
        dump.append(plugin->ID);
        dump.append(" ");
    }

    dump.append("\r\n");

    dump.append("SetChain Output ");

    for (const auto& plugin : outputChain)
    {
        dump.append(plugin->ID);
        dump.append(" ");
    }

    dump.append("\r\n");

    AppendPluginParams(dump, preamp, false);
    AppendPluginParams(dump, tonestack, false);
    AppendPluginParams(dump, poweramp, false);
    AppendPluginParams(dump, cabinet, false);

    for (const auto& plugin : inputChain)
    {
        AppendPluginParams(dump, plugin, false);
    }

    for (const auto& plugin : fxLoop)
    {
        AppendPluginParams(dump, plugin, false);
    }

    for (const auto& plugin : outputChain)
    {
        AppendPluginParams(dump, plugin, false);
    }

    for (auto& entry : ccMapEntries)
    {
        dump.append("MapController ");
        dump.append(std::to_string(entry.CCNumber));
        dump.append(" ");
        dump.append(entry.Plugin->ID);

        if (entry.Parameter == -1)
        {
            dump.append(" Enabled");
        }
        else
        {
            dump.append(" ");
            dump.append((entry.Plugin->Parameters + entry.Parameter)->Name);
        }

        dump.append("\r\n");
    }

    dump.append("EndProgram\r\n");

    return dump;
}

std::string PluginProcessor::GetPresets()
{
    std::string presetList;

    for (const auto& entry : std::filesystem::directory_iterator(presetPath))
    {
        presetList.append(entry.path().filename().string());
        presetList.append(" ");
    }

    return presetList;
}

// This is pretty gross - should probably read presets into a list and keep it around...
std::string PluginProcessor::GetNextPreset(std::string afterPreset)
{
    bool isFirst = true;
    bool returnNext = false;
    std::string first;

    for (const auto& entry : std::filesystem::directory_iterator(presetPath))
    {
        if (returnNext)
        {
            return entry.path().filename().string();
        }

        if (isFirst)
        {
            first = entry.path().filename().string();

            isFirst = false;
        }

        if (entry.path().filename().string() == afterPreset)
        {
            returnNext = true;
        }
    }

    return first;
}

std::string PluginProcessor::GetPreviousPreset(std::string beforePreset)
{
    bool isFirst = true;
    bool needLast = false;
    std::string last;

    for (const auto& entry : std::filesystem::directory_iterator(presetPath))
    {
        if (entry.path().filename().string() == beforePreset)
        {
            if (isFirst)
            {
                needLast = true;
            }
            else
            {
                return last;
            }
        }

        isFirst = false;

        last = entry.path().filename().string();
    }

    return last;
}


std::string PluginProcessor::HandleCommand(std::string const& line)
{
    std::string err;

    std::cerr << "Got message: " << line << "\n";

    std::vector<std::string> commandWords;
    std::istringstream iss(line);

    for (std::string word; iss >> word; )
    {
        commandWords.push_back(word);
    }

    if (commandWords.size() > 0)
    {
        if ((commandWords[0] == "Ok") || (commandWords[0] == "Error"))
        {
            // Handle responses
        }
        else if (commandWords[0] == "SetParam")
        {
            if (commandWords.size() > 1)
            {
                StompBox* component = pluginFactory.FindPlugin(commandWords[1]);

                if (component == nullptr)
                {
                    err.append("Bad Plugin");

                    std::cerr << err << "\n";
                }
                else
                {
                    std::cerr << "Set on: " << commandWords[1] << "\n";

                    bool gotParam = false;

                    if (commandWords.size() > 2)
                    {
                        if (commandWords[2] == "Enabled")
                        {
                            if (commandWords.size() > 3)
                            {
                                component->Enabled = (atoi(commandWords[3].c_str()) == 1);

                                CheckMidiCommand(component, -1);
                            }

                            gotParam = true;
                        }
                        else
                        {
                            for (int i = 0; i < component->NumParameters; i++)
                            {
                                StompBoxParameter* param = component->GetParameter(i);

                                if (param->Name == commandWords[2])
                                {
                                    std::cerr << "Set on parameter: " << commandWords[2] << "\n";

                                    if (commandWords.size() > 3)
                                    {
                                        if (component->GetParameter(i)->ParameterType == PARAMETER_TYPE_ENUM)
                                        {
                                            std::vector<std::string>* enumValues = param->EnumValues;

                                            bool gotEnum = false;

                                            for (int e = 0; e < enumValues->size(); e++)
                                            {
                                                if ((*enumValues)[e] == commandWords[3])
                                                {
                                                    component->SetParameterValue(i, e);

                                                    gotEnum = true;

                                                    break;
                                                }
                                            }

                                            if (!gotEnum)
                                            {
                                                err.append("Bad Enum Value");
                                            }
                                        }
                                        else if (param->CanSyncToHostBPM && (commandWords[3].find('/') != std::string::npos))
                                        {
                                            size_t pos = commandWords[3].find('/');

                                            param->BPMSyncNumerator = atoi(commandWords[3].substr(0, pos).c_str());
                                            param->BPMSyncDenominator = atoi(commandWords[3].c_str() + pos + 1);

                                            component->UpdateBPM();
                                        }
                                        else
                                        {
                                            if (param->CanSyncToHostBPM)
                                            {
                                                param->BPMSyncNumerator = param->BPMSyncDenominator = 0;
                                            }

                                            component->SetParameterValue(i, atof(commandWords[3].c_str()));

                                            CheckMidiCommand(component, i);
                                        }

                                        gotParam = true;
                                    }

                                    break;
                                }
                            }
                        }
                    }

                    if (!gotParam)
                    {
                        err.append("Bad Parameter");
                    }
                }
            }
        }
        else if (commandWords[0] == "SetChain")
        {
            if (commandWords.size() > 1)
            {
                std::list<StompBox*>* chain = nullptr;

                if (commandWords[1] == "Input")
                {
                    chain = &inputChain;
                }
                else if (commandWords[1] == "FxLoop")
                {
                    chain = &fxLoop;
                }
                else if (commandWords[1] == "Output")
                {
                    chain = &outputChain;
                }
                else
                {
                    err = "Bad Chain";
                }

                if (chain != nullptr)
                {
                    int numPlugins = commandWords.size() - 2;

                    std::cerr << "SetChain " << commandWords[1] << "\n";

                    chain->clear();

                    for (int i = 0; i < numPlugins; i++)
                    {
                        std::cerr << commandWords[i + 2] << "\n";

                        StompBox* newComponent = CreatePlugin(commandWords[i + 2]);

                        if (newComponent != nullptr)
                            chain->push_back(newComponent);
                    }

                    UpdatePlugins();
                }
            }
        }
        else if (commandWords[0] == "ReleasePlugin")
        {
            if (commandWords.size() > 1)
            {
                pluginFactory.ReleasePlugin(commandWords[1]);
            }
        }
        else if (commandWords[0] == "Dump")
        {
            if (commandWords.size() > 1)
            {
                if (commandWords[1] == "Program")
                {
                    SendClientMessage(DumpSettings());
                    SendClientMessage(DumpProgram());
                }
                else if (commandWords[1] == "Config")
                {
                    SendClientMessage(DumpConfig());
                }
            }
        }
        else if (commandWords[0] == "List")
        {
            if (commandWords.size() > 1)
            {
                if (commandWords[1] == "Presets")
                {
                    std::string presetList = "Presets ";
                    
                    presetList.append(GetPresets());

                    presetList.append("\r\n");

                    SendClientMessage(presetList);
                }
            }
        }
        else if (commandWords[0] == "SavePreset")
        {
            if (commandWords.size() > 1)
            {
                currentPreset = commandWords[1];

                std::string dump = DumpProgram();

                std::filesystem::path outPath;
                
                outPath.assign(presetPath);
                outPath.append(currentPreset);

                std::ofstream outFile(outPath, std::ios::binary);

                outFile << dump;

                outFile.close();
            }
        }
        else if (commandWords[0] == "LoadPreset")
        {
            if (commandWords.size() > 1)
            {
                LoadPreset(commandWords[1]);
            }
        }
        else if (commandWords[0] == "SaveSettings")
        {
            std::string settings = DumpSettings();

            std::filesystem::path outPath;

            outPath.assign(dataPath);
            outPath.append(".GuitarSimSettings");

            std::ofstream outFile(outPath, std::ios::binary);

            outFile << settings;

            outFile.close();
        }
        else if (commandWords[0] == "MapController")
        {
            if (commandWords.size() > 2)
            {
                CCMap ccMap;

                ccMap.CCNumber = atoi(commandWords[1].c_str());

                if (commandWords.size() > 3)
                {
                    StompBox* plugin = pluginFactory.FindPlugin(commandWords[2]);

                    ccMap.Plugin = plugin;

                    if (plugin != nullptr)
                    {
                        if (commandWords[3] == "Enabled")
                        {
                            ccMap.Parameter = -1;

                            ccMapEntries.push_back(ccMap);
                        }
                        else
                        {
                            for (int i = 0; i < plugin->NumParameters; i++)
                            {
                                StompBoxParameter* param = plugin->GetParameter(i);

                                if (param->Name == commandWords[3])
                                {
                                    ccMap.Parameter = i;

                                    ccMapEntries.push_back(ccMap);

                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        else if (commandWords[0] == "ClearControllerMap")
        {
            ccMapEntries.clear();
        }
        else if (commandWords[0] == "MapStompController")
        {
            if (commandWords.size() > 2)
            {
                int stompIndex = atoi(commandWords[1].c_str()) - 1;
                int ccNumber = atoi(commandWords[2].c_str());

                stompCC[stompIndex] = ccNumber;
            }
        }
        else if (commandWords[0] == "MapModeController")
        {
            if (commandWords.size() > 1)
            {
                modeChangeCC = atoi(commandWords[1].c_str());
            }
        }
        else if (commandWords[0] == "SendCommand")
        {
            if (commandWords.size() > 1)
            {
                StompBox* component = pluginFactory.FindPlugin(commandWords[1]);

                if (component == nullptr)
                {
                    err.append("Bad Plugin");

                    std::cerr << err << "\n";
                }
                else
                {
                    component->HandleCommand(commandWords);
                }
            }
        }
        else if (commandWords[0] == "PluginOutputOn")
        {
            pluginOutputOn = true;
        }
        else if (commandWords[0] == "PluginOutputOff")
        {
            pluginOutputOn = false;
        }
        else if (commandWords[0] == "HandleMidi")
        {
            if (commandWords.size() > 3)
            {
                int midiCommand = atoi(commandWords[1].c_str());
                int midiData1 = atoi(commandWords[2].c_str());
                int midiData2 = atoi(commandWords[3].c_str());

                HandleMidiCommand(midiCommand, midiData1, midiData2);
            }
        }
        else if (commandWords[0] == "MidiLockOn")
        {
            midiLocked = true;
        }
        else if (commandWords[0] == "MidiLockOff")
        {
            midiLocked = false;
        }
        else
        {
            err.append("Bad Command");
        }
    }

    return err;
}

bool PluginProcessor::HandleMidiCommand(int midiCommand, int midiData1, int midiData2)
{
    if (midiLocked)
        return false;

    if (midiCommand == 0xB0) // Control change
    {        
        bool handled = false;

        if (midiData1 == modeChangeCC)
        {
            switch (currentMidiMode)
            {
                case MIDI_MODE_PRESETCHANGE:
                    if (serialDisplayInterface.IsConnected())
                    {
                        serialDisplayInterface.SetPresetText(currentPreset.c_str(), PRESET_TEXT_COLOR);
                    }
                    break;

                case MIDI_MODE_TUNER:
                    tuner->Enabled = false;
                    tuner->EnabledIsDirty = true;
                    break;
            }

            currentMidiMode++;

            if (currentMidiMode == MIDI_MODE_MAX)
                currentMidiMode = 0;

            switch (currentMidiMode)
            {
                case MIDI_MODE_STOMP:
                    SyncPreset();
                    break;

                case MIDI_MODE_PRESETCHANGE:
                    previewPreset = currentPreset;

                    if (serialDisplayInterface.IsConnected())
                    {
                        serialDisplayInterface.SetPresetText(previewPreset.c_str(), PRESET_PREVIEW_COLOR);
                    }
                    break;

                case MIDI_MODE_TUNER:
                    if (serialDisplayInterface.IsConnected())
                    {
                        serialDisplayInterface.ResetDisplay();
                        serialDisplayInterface.SetPresetText("Tuner", PRESET_TEXT_COLOR);
                        serialDisplayInterface.ResetTuner();
                    }

                    tuner->Enabled = true;
                    tuner->EnabledIsDirty = true;

                    break;

                case MIDI_MODE_RECORDER:
                    if (serialDisplayInterface.IsConnected())
                    {
                        serialDisplayInterface.ResetDisplay();
                        serialDisplayInterface.SetPresetText("Recorder", PRESET_TEXT_COLOR);
                        serialDisplayInterface.ResetTuner();
                    }

                    break;
            }

            handled = true;
        }
        else
        {
            switch (currentMidiMode)
            {
            case MIDI_MODE_STOMP:
                for (auto& ccMap : ccMapEntries)
                {
                    if (ccMap.CCNumber == midiData1)
                    {
                        fprintf(stderr, "Got CC %d\n", midiData1);

                        fprintf(stderr, "Plugin is %s\n", ccMap.Plugin->Name.c_str());

                        if (ccMap.Parameter == -1) // Enabled
                        {
                            ccMap.Plugin->Enabled = !ccMap.Plugin->Enabled; // (midiData2 > 1);

                            CheckMidiCommand(ccMap.Plugin, -1);

                            ccMap.Plugin->EnabledIsDirty = true;
                        }
                        else
                        {
                            StompBoxParameter* parameter = ccMap.Plugin->Parameters + ccMap.Parameter;

                            double value = parameter->MinValue + ((parameter->MaxValue - parameter->MinValue) * ((double)midiData2 / 127.0));

                            ccMap.Plugin->SetParameterValue(ccMap.Parameter, value);

                            CheckMidiCommand(ccMap.Plugin, ccMap.Parameter);

                            ccMap.Plugin->ParamIsDirty = true;
                            parameter->IsDirty = true;
                        }

                        handled = true;
                    }
                }
                break;

            case MIDI_MODE_PRESETCHANGE:
                if ((stompCC[0] != -1) && (midiData1 == stompCC[0]))
                {
                    previewPreset.assign(GetPreviousPreset(previewPreset));
                    serialDisplayInterface.SetPresetText(previewPreset.c_str(), PRESET_PREVIEW_COLOR);
                }
                else if ((stompCC[1] != -1) && (midiData1 == stompCC[1]))
                {
                    previewPreset.assign(GetNextPreset(previewPreset));
                    serialDisplayInterface.SetPresetText(previewPreset.c_str(), PRESET_PREVIEW_COLOR);
                }
                else if ((stompCC[2] != -1) && (midiData1 == stompCC[2]))
                {
                    LoadPreset(previewPreset);
                }
                break;

            case MIDI_MODE_RECORDER:
                if ((stompCC[2] != -1) && (midiData1 == stompCC[2]))
                {
                    if ((audioFileRecorder != nullptr) && audioFileRecorder->Enabled)
                    {
                        std::thread saveThread = std::thread(&AudioFileRecorder::SaveRecording, audioFileRecorder);
                        saveThread.detach();
                    }
                }
                break;
            }
        }

        return handled;
    }
    else if (midiCommand == 0xC0) // Patch change
    {
        fprintf(stderr, "Patch change to %d\n", midiData1);

        for (const auto& entry : std::filesystem::directory_iterator(presetPath))
        {
            std::string preset = entry.path().filename().string();

            if (atoi(preset.substr(0, 2).c_str()) == (midiData1 + 1))
            {
                LoadPreset(preset);

                if (guitarServer.HaveClient())
                {
                    std::string dump = DumpProgram();

                    SendClientMessage(dump);
                }

                break;
            }
        }
    }

    return false;
}

bool PluginProcessor::CheckMidiCommand(StompBox* plugin, int parameter)
{
    for (auto& ccMap : ccMapEntries)
    {
        if ((ccMap.Plugin == plugin) && (ccMap.Parameter == parameter))
        {
            int stomp = -1;

            for (int i = 0; i < 16; i++)
            {
                if (stompCC[i] == ccMap.CCNumber)
                {
                    stomp = i;

                    break;
                }
            }

            if (stomp != -1)
            {
                if (serialDisplayInterface.IsConnected())
                {
                    if (ccMap.Parameter == -1)
                    {
                        serialDisplayInterface.HandleStomp(stomp, ccMap.Plugin->Enabled, ccMap.Plugin->Name.c_str(), "Enabled",
                            ccMap.Plugin->BackgroundColor.c_str(), ccMap.Plugin->ForegroundColor.c_str());
                    }
                    else
                    {
                        serialDisplayInterface.HandleStomp(stomp, (ccMap.Plugin->GetParameterValue(ccMap.Parameter) > 0), ccMap.Plugin->Name.c_str(), ccMap.Plugin->Parameters[ccMap.Parameter].Name.c_str(),
                            ccMap.Plugin->BackgroundColor.c_str(), ccMap.Plugin->ForegroundColor.c_str());
                    }
                }
            }
        }
    }

    return false;
}

void PluginProcessor::SyncPreset()
{
    serialDisplayInterface.ResetDisplay();
    serialDisplayInterface.SetPresetText(currentPreset.c_str(), PRESET_TEXT_COLOR);

    for (auto& ccMap : ccMapEntries)
    {
        CheckMidiCommand(ccMap.Plugin, ccMap.Parameter);
    }
}

void PluginProcessor::LoadPreset(std::string preset)
{
    fprintf(stderr, "Loading preset %s\n", preset.c_str());

    std::filesystem::path inPath;

    inPath.assign(presetPath);
    inPath.append(preset);

    // Clear any MIDI CC mappings we have
    ccMapEntries.clear();

    if (LoadCommandsFromFile(inPath))
    {
        currentPreset = preset;

        if (serialDisplayInterface.IsConnected())
        {
            SyncPreset();
        }

        currentMidiMode = MIDI_MODE_STOMP;
    }
}

void PluginProcessor::LoadSettings()
{
    std::filesystem::path settingsPath;

    settingsPath.assign(dataPath);
    settingsPath.append(".GuitarSimSettings");

    LoadCommandsFromFile(settingsPath);
}

bool PluginProcessor::LoadCommandsFromFile(std::filesystem::path filePath)
{
    fprintf(stderr, "Load commands from file: %s\n", filePath.c_str());

    if (!std::filesystem::exists(filePath))
        return false;

    std::ifstream inFile(filePath);

    std::string line;

    while (std::getline(inFile, line))
    {
        if (line[line.size() - 1] == '\r')
        {
            HandleCommand(line.substr(0, line.size() - 1));
        }
        else
        {
            HandleCommand(line);
        }
    }

    return true;
}

void PluginProcessor::Process(double* input, double* output, int count)
{
    // Disable floating point denormals
    std::fenv_t fe_state;
    std::feholdexcept(&fe_state);
    disable_denormals();

    memcpy(output, input, count * sizeof(double));

    for (const auto& plugin : plugins)
    {
        if (plugin->NeedsInit)
        {
            InitPlugin(plugin);
        }

        if (plugin->Enabled)
        {
            plugin->compute(count, output, output);
        }

        if (plugin->RampSamples != 0)
        {
            for (int i = 0; i < count; i++)
            {
                double gain = abs((double)plugin->RampPos) / (double)plugin->RampSamples;

                output[i] *= gain;

                plugin->RampPos++;

                if (plugin->RampPos >= plugin->RampSamples)
                {
                    plugin->RampPos = 0;
                    plugin->RampSamples = 0;

                    break;
                }
            }
        }

        // restore previous floating point state
        std::feupdateenv(&fe_state);

        if (plugin == monitorPlugin)
        {
            monitorCallback(output, count);
        }
    }
}