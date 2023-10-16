#pragma once

#include <math.h>
#include <list>
#include "StompBox.h"
#include "PluginFactory.h"
#include "GuitarServer.h"
#include "GuitarClient.h"
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
	GuitarServer guitarServer;
	SerialDisplayInterface serialDisplayInterface;
	Gain* inputGain;
	StompBox* preamp;
	StompBox* tonestack;
	//AmpStage* poweramp;
	Gain* masterVolume;
	GuitarConvolver* cabinet;
	PitchDetector* tuner;
	AudioFilePlayer* audioFilePlayer = nullptr;
	AudioFileRecorder* audioFileRecorder = nullptr;
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
#if _WIN32
	PluginProcessor(std::filesystem::path dataPath, bool dawMode);
#else
	PluginProcessor(bool dawMode);
#endif
	~PluginProcessor();
	void Init(double sampleRate);
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

		return guitarServer.HaveClient();
	}
	PluginFactory* GetPluginFactory()
	{
		return &pluginFactory;
	}
	Gain* GetInputGain()
	{
		return inputGain;
	}
	Gain* GetMasterVolume()
	{
		return masterVolume;
	}
	StompBox* GetPreamp()
	{
		return preamp;
	}
	StompBox* GetTonestack()
	{
		return tonestack;
	}
	GuitarConvolver* GetCabinet()
	{
		return cabinet;
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
	std::string HandleCommand(std::string const& line);
	bool HandleMidiCommand(int midiCommand, int midiData1, int midiData2);
	bool CheckMidiCommand(StompBox* plugin, int parameter);
	void SyncPreset();
	void LoadPreset(std::string preset);
	void LoadSettings();
	bool LoadCommandsFromFile(std::filesystem::path filePath);
	void AppendPluginParams(std::string& dump, StompBox* plugin, bool dirtyOnly);
	std::string DumpSettings();
	std::string DumpProgram();
	std::string DumpConfig();
	void Process(double* input, double* output, int count);
};