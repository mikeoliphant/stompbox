#pragma once

#include <list>

#include "StompBox.h"
#include "Gain.h"
#include "AmpStage.h"
#include "GuitarConvolver.h"
#include "ToneStack.h"
#include "Tremolo.h"
#include "Screamer.h"
#include "Distortion.h"
#include "Overdrive.h"
#include "AutoWah.h"
#include "Wah.h"
#include "Delay.h"
#include "Reverb.h"
#include "Compressor.h"
#include "Phaser.h"
#include "Vibrato.h"
#include "Flanger.h"
#include "Chorus.h"
#include "Fuzz.h"
#include "GraphicEqualizer.h"
#include "PitchDetector.h"
#include "AudioFilePlayer.h"
#include "AudioFileRecorder.h"
#include "NAM.h"

#include <unordered_map>

typedef StompBox * (*CreatePluginFunction)(void);

extern std::filesystem::path cabinetPath;
extern std::filesystem::path reverbPath;
extern std::filesystem::path musicPath;
extern std::filesystem::path namModelPath;

class PluginFactory
{
protected:
	std::unordered_map<std::string, CreatePluginFunction> pluginMap;
	std::unordered_map<std::string, StompBox*> loadedPlugins;
	std::list<std::string> userPluginList;
	std::list<std::string> allPluginList;

	void AddPlugin(std::string name, CreatePluginFunction function, bool isUserSelectable);

public:
	PluginFactory();
	~PluginFactory() {}
	void SetDataPath(std::filesystem::path path)
	{
		cabinetPath.assign(path);
		cabinetPath.append("Cabinets");

		reverbPath.assign(path);
		reverbPath.append("Reverb");

		musicPath.assign(path);
		musicPath.append("Music");

		namModelPath.assign(path);
		namModelPath.append("NAM");
	}
	std::list<std::string> GetUserPlugins();
	std::list<std::string> GetAllPlugins();
	void LoadAllPlugins();
	StompBox* CreatePlugin(std::string const& name, std::string const& id);
	StompBox* CreatePlugin(std::string const& id)
	{
		std::size_t found = id.find('_', 0);

		if (found != std::string::npos)
		{
			return CreatePlugin(id.substr(0, found), id);
		}

		return CreatePlugin(id, id);
	}
	void ReleasePlugin(std::string id);
	StompBox* FindPlugin(std::string id);
};

