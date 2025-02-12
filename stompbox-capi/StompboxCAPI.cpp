#include "PluginProcessor.h"
#include "StompboxCAPI.h"

struct ProcessorState
{
    PluginProcessor* processor;
};

struct StringVector
{
    std::vector<std::string>* vec;
};

const size_t GetSize(StringVector strVec)
{
    return (*(strVec.vec)).size();
}

const char* GetValue(StringVector strVec, int index)
{
    return (*(strVec.vec))[index].c_str();
}

const ProcessorState* CreateProcessor(const wchar_t* dataPath, bool dawMode)
{
    ProcessorState* state = new ProcessorState();

    state->processor = new PluginProcessor(dataPath, dawMode);

    return state;
}

void DeleteProcessor(ProcessorState* state)
{
    delete state->processor;
    delete state;
}

void Init(ProcessorState* state, double sampleRate)
{
    state->processor->Init(sampleRate);
}

void StartServer(ProcessorState* state)
{
    state->processor->StartServer();
}

void SetBPM(ProcessorState* state, double bpm)
{
    state->processor->SetBPM(bpm);
}

bool IsPresetLoading(ProcessorState* state)
{
	return state->processor->IsPresetLoading();
}

const std::filesystem::path::value_type* GetDataPath(ProcessorState* state)
{
    return state->processor->GetDataPath().c_str();
}

const StringVector GetAllPlugins(ProcessorState* state)
{
    StringVector allPlugins;

    allPlugins.vec = &(state->processor->GetPluginFactory()->GetAllPlugins());

    return allPlugins;
}

