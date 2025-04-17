#pragma once

#include <math.h>
#include <list>
#include "StompBox.h"
#include "PluginFactory.h"
#include "StompboxServer.h"
#include "SerialDisplayInterface.h"

enum
{
	MIDI_MODE_STOMP,
	MIDI_MODE_PRESETCHANGE,
	MIDI_MODE_TUNER,
	MIDI_MODE_RECORDER,
	MIDI_MODE_MAX
};

#define PRESET_TEXT_COLOR "#FFFFFF"
#define PRESET_PREVIEW_COLOR "#FFAA00"

struct CCMap
{
	int CCNumber;
	StompBox* Plugin;
	std::string Parameter;
};

class ChainElement
{
public:
	std::string Name;
	bool IsChain;
	bool IsMaster;
	std::vector<StompBox*> Plugins;
};

class PluginProcessor
{
protected:
	void UpdateClient();
	void SendClientMessage(const std::string message);
	void AppendParamDefs(std::string& dump, StompBox* plugin);
	void AppendPluginParams(std::string& dump, StompBox* plugin, bool dirtyOnly);
	void AppendParams(std::string& dump, StompBox* plugin, bool dirtyOnly);
	void ThreadLoadPreset();

	float sampleRate;
	float bpm = 120;
	std::list<StompBox*> pluginList1;
	std::list<StompBox*> pluginList2;
	std::list<StompBox*>& plugins = pluginList1;
	int tmpBufSize = 64;
	float* tmpBuf1 = nullptr;
	float* tmpBuf2 = nullptr;
	bool initialized = false;
	PluginFactory pluginFactory;
	StompboxServer stompboxServer;
	SerialDisplayInterface serialDisplayInterface;
	std::unordered_map<std::string, ChainElement*> chainLookup;
	std::list<ChainElement*> chainList;
	std::vector<std::string> presets;
	StompBox* monitorPlugin = nullptr;
	std::function<void(float*, int)> monitorCallback = nullptr;
	std::function<void(int, int, int)> midiCallback = nullptr;
	std::function<void(const std::string)> messageCallback = nullptr;
	bool serverIsRunning = false;
	std::thread* clientUpdateThread = nullptr;
	std::filesystem::path dataPath;
	std::filesystem::path presetPath;
	std::string currentPreset;
	std::string loadPreset;
	std::string previewPreset;
	std::string clientCommand;
	bool pluginOutputOn = false;
	bool midiLocked = false;
	int currentMidiMode = MIDI_MODE_STOMP;
	std::list<CCMap> ccMapEntries;
	int modeChangeCC = -1;
	int stompCC[16];
	float minDSPLoad = 1;
	float maxDSPLoad = 0;
	int clientUpdateFrame = 0;
	std::thread* presetLoadThread = nullptr;
	int ramp = 0;
	int rampSamplesSoFar;
	int rampSamples;
	float defaultRampMS = 50;
	bool needPluginUpdate = false;

public:
	PluginProcessor(std::filesystem::path dataPath, bool dawMode);
	~PluginProcessor();
	static std::string GetVersion();
	void ScanPresets();
	void Init(float sampleRate);
	void SetBPM(float bpm);
	void StartServer();
	StompBox* CreatePlugin(std::string const& id);
	void UpdatePlugins();
	void InitPlugin(StompBox* plugin);

	bool IsPresetLoading()
	{
		return (ramp != 0);
	}

	float GetBPM()
	{
		return bpm;
	}

	std::filesystem::path& GetDataPath()
	{
		return dataPath;
	}

	bool GetServerIsRunning() {
		return serverIsRunning;
	}

	bool HaveClient()
	{
		if (!serverIsRunning)
			return false;

		return stompboxServer.HaveClient();
	}

	PluginFactory* GetPluginFactory()
	{
		return &pluginFactory;
	}

	StompBox* GetPluginSlot(const std::string& slotName)
	{
		auto slot = chainLookup.find(slotName);

		if (slot != chainLookup.end())
		{
			if (slot->second->IsChain)
				return nullptr;

			if (slot->second->Plugins.size() == 0)
				return nullptr;

			return slot->second->Plugins[0];
		}

		return nullptr;
	}

	void SetPluginSlot(const std::string& slotName, const std::string& pluginID)
	{
		std::cout << "Set Plugin Slot: " << slotName << " to " << pluginID << std::endl;

		auto slot = chainLookup.find(slotName);

		if (slot != chainLookup.end())
		{
			if (!slot->second->IsChain)
			{
				slot->second->Plugins.clear();
				slot->second->Plugins.push_back(CreatePlugin(pluginID));
			}
		}
	}

	std::vector<StompBox*> *GetChain(const std::string& chainName)
	{
		auto chain = chainLookup.find(chainName);

		if (chain != chainLookup.end())
		{
			return &chain->second->Plugins;
		}

		return nullptr;
	}

	std::vector<std::string>& GetPresets()
	{
		return presets;
	}

	std::string& GetCurrentPreset()
	{
		return currentPreset;
	}

	std::string GetNextPreset(std::string afterPreset);
	std::string GetPreviousPreset(std::string beforePreset);

	void SetMonitorPlugin(std::string const& pluginID, std::function<void(float *, int)> callback)
	{
		monitorCallback = callback;
		monitorPlugin = pluginFactory.FindPlugin(pluginID);
	}

	void SetMidiCallback(std::function<void(int, int, int)> callback)
	{
		midiCallback = callback;
	}

	void SetMessageCallback(std::function<void(const std::string)> callback);
	void ReportDSPLoad(float currentLoad);
	std::string HandleCommand(const std::string& line);
	bool HandleMidiCommand(int midiCommand, int midiData1, int midiData2);
	bool CheckMidiCommand(StompBox* plugin, StompBoxParameter* parameter);
	void SyncPreset();
	void LoadPreset(std::string preset);
	void LoadSettings();
	void SaveSettings();
	bool LoadCommandsFromFile(std::filesystem::path filePath);
	std::string DumpSettings();
	std::string DumpProgram();
	std::string DumpConfig();
	void Process(float* input, float* output, size_t count);
	void StartRamp(int rampDirection, float rampMS);
};