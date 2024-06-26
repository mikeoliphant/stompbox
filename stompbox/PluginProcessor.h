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
	int Parameter;
};

class PluginProcessor
{
protected:
	void UpdateClient();
	void SendClientMessage(const std::string message);

	std::string stompboxVersion = "stompbox v0.1.4 - Copyright (c) Mike Oliphant";
	double sampleRate;
	double bpm = 120;
	std::list<StompBox *> pluginList1;
	std::list<StompBox*> pluginList2;
	std::list<StompBox*>& plugins = pluginList1;
	int tmpBufSize = 64;
	double* tmpBuf1 = nullptr;
	double* tmpBuf2 = nullptr;
	bool initialized = false;
	PluginFactory pluginFactory;
	StompboxServer stompboxServer;
	SerialDisplayInterface serialDisplayInterface;
	StompBox* inputGain = nullptr;
	StompBox* amp = nullptr;
	StompBox* tonestack = nullptr;
	StompBox* masterVolume = nullptr;
	StompBox* cabinet = nullptr;
	StompBox* tuner = nullptr;
	StompBox* audioFilePlayer = nullptr;
	StompBox* audioFileRecorder = nullptr;
	std::list<StompBox*> inputChain;
	std::list<StompBox*> fxLoop;
	std::list<StompBox*> outputChain;
	StompBox* monitorPlugin = nullptr;
	std::function<void(double*, int)> monitorCallback = nullptr;
	std::function<void(int, int, int)> midiCallback = nullptr;
	std::function<void(const std::string)> messageCallback = nullptr;
	bool serverIsRunning = false;
	std::thread* clientUpdateThread = nullptr;
	std::filesystem::path dataPath;
	std::filesystem::path presetPath;
	std::string currentPreset;
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

public:
	PluginProcessor(std::filesystem::path dataPath, bool dawMode);
	~PluginProcessor();
	void Init(double sampleRate);
	std::string GetVersion()
	{
		return stompboxVersion;
	};
	void SetBPM(double bpm);
	void StartServer();
	StompBox* CreatePlugin(std::string const& id);
	void UpdatePlugins();
	void InitPlugin(StompBox* plugin);
	double GetBPM()
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
	StompBox* GetInputGain()
	{
		return inputGain;
	}
	StompBox* GetMasterVolume()
	{
		return masterVolume;
	}
	StompBox* GetPluginSlot(const std::string& slotName)
	{
		if (slotName == "Amp")
		{
			return amp;
		}
		else if (slotName == "Tonestack")
		{
			return tonestack;
		}
		else if (slotName == "Cabinet")
		{
			return cabinet;
		}

		return nullptr;
	}
	std::list<StompBox*> GetInputChain()
	{
		return inputChain;
	}
	std::list<StompBox*> GetFxLoop()
	{
		return fxLoop;
	}
	std::list<StompBox*> GetOutputChain()
	{
		return outputChain;
	}
	std::string GetPresets();
	std::string GetCurrentPreset()
	{
		return currentPreset;
	}
	std::string GetNextPreset(std::string afterPreset);
	std::string GetPreviousPreset(std::string beforePreset);
	void SetMonitorPlugin(std::string const& pluginID, std::function<void(double *, int)> callback)
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
	bool CheckMidiCommand(StompBox* plugin, int parameter);
	void SyncPreset();
	void LoadPreset(std::string preset, bool updateClient);
	void LoadSettings();
	bool LoadCommandsFromFile(std::filesystem::path filePath);
	void AppendPluginParams(std::string& dump, StompBox* plugin, bool dirtyOnly);
	std::string DumpSettings();
	std::string DumpProgram();
	std::string DumpConfig();
	void Process(double* input, double* output, int count);
};