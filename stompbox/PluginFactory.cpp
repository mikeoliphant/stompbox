#include <iostream>
#include <filesystem>

#include "PluginFactory.h"

#include "NAM.h"
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

StompBox* CreateInputGainPlugin()
{
	Gain *input = new Gain(0, -40, 40);

	input->IsUserSelectable = false;

	return input;
}

StompBox* CreateMasterVolumePlugin()
{
	Gain* master = new Gain(0, -40, 40);

	master->Parameters[GAIN_GAIN].Name = "Volume";
	master->IsUserSelectable = false;

	return master;
}

std::vector<std::string> preampStages
{
	"12AX7",
	"JJECC83S"
};

std::vector<int> preampStageMap
{
	AMPSTAGE_12AX7,
	AMPSTAGE_JJECC83S
};

StompBox* CreatePreampPlugin()
{
	AmpStage* preamp = new AmpStage();

	preamp->Parameters[AMPSTAGE_STAGETYPE].EnumValues = &preampStages;
	preamp->Parameters[AMPSTAGE_STAGETYPE].MaxValue = preampStages.size();
	preamp->stageMap = &preampStageMap;

	preamp->Parameters[AMPSTAGE_NUMSTAGES].MaxValue = 3;
	preamp->Parameters[AMPSTAGE_NUMSTAGES].DefaultValue = 1;
	preamp->SetParameterValue(AMPSTAGE_NUMSTAGES, 1);

	return preamp;
}

std::vector<std::string> powerampStages
{
	"6V6",
	"EL34",
	"EL84",
	"6L6CG"
};

std::vector<int> powerampStageMap
{
	AMPSTAGE_6V6,
	AMPSTAGE_EL34,
	AMPSTAGE_EL84,
	AMPSTAGE_6L6CG,
};

StompBox* CreatePowerampPlugin()
{
	AmpStage* poweramp = new AmpStage();
	poweramp->Parameters[AMPSTAGE_GAIN].Name = "Volume";
	poweramp->Parameters[AMPSTAGE_STAGETYPE].EnumValues = &powerampStages;
	poweramp->Parameters[AMPSTAGE_STAGETYPE].MaxValue = powerampStages.size();
	poweramp->stageMap = &powerampStageMap;
	poweramp->gainType = AMPSTAGE_GAIN_LOG;

	poweramp->SetParameterValue(AMPSTAGE_NUMSTAGES, 1);

	return poweramp;
}

StompBox* CreateTonestackPlugin()
{
	Tonestack* tonestack = new Tonestack();
	tonestack->SetPreset(0);

	return tonestack;
}

std::filesystem::path cabinetPath;

StompBox* CreateGuitarConvolverPlugin()
{
	GuitarConvolver* convolver = new GuitarConvolver();

	convolver->IndexImpulses(cabinetPath);

	for (int i = 0; i < convolver->Parameters[CONVOLVER_IMPULSE].EnumValues->size(); i++)
	{
		if ((*convolver->Parameters[CONVOLVER_IMPULSE].EnumValues)[i].rfind("Fender", 0) == 0)
		{
			convolver->SetParameterValue(CONVOLVER_IMPULSE, i);

			break;
		}
	}

	return convolver;
}

std::filesystem::path reverbPath;

StompBox* CreateConvolutionReverbPlugin()
{
	GuitarConvolver* reverb = new GuitarConvolver();

	reverb->SetParameterValue(CONVOLVER_DRY, 1);
	reverb->SetParameterValue(CONVOLVER_WET, 0.3);
	reverb->Parameters[CONVOLVER_WET].DefaultValue = 0.3f;

	reverb->IndexImpulses(reverbPath);

	return reverb;
}

std::filesystem::path namModelPath;

StompBox* CreateNAMPlugin()
{
	NAM* nam = new NAM();

	nam->IndexModels(namModelPath);

	return nam;
}

StompBox* CreateBoostPlugin()
{
	Gain *gain = new Gain(10, 0, 20);

	gain->BackgroundColor = "#e31b00";
	gain->ForegroundColor = "#ffffff";

	return gain;
}

StompBox* CreateCompressorPlugin()
{
	Compressor *compressor = new Compressor();

	compressor->BackgroundColor = "#0043db";
	compressor->ForegroundColor = "#ffffff";

	return compressor;
}

StompBox* CreateFuzzPlugin()
{
	Fuzz *fuzz = new Fuzz();

	fuzz->BackgroundColor = "#fabd00";

	return fuzz;
}

StompBox* CreateAutoWahPlugin()
{
	AutoWah *wah = new AutoWah();

	wah->BackgroundColor = "#e00095";
	wah->ForegroundColor = "#ffffff";

	return wah;
}

StompBox* CreateWahPlugin()
{
	Wah* wah = new Wah();

	wah->BackgroundColor = "#111111";
	wah->ForegroundColor = "#ffffff";

	return wah;
}

StompBox* CreateScreamerPlugin()
{
	TS9* ts9 = new TS9();

	ts9->BackgroundColor = "#01b434";

	return ts9;
}

StompBox* CreateDistortionPlugin()
{
	BossDS1 *ds1 = new BossDS1();

	ds1->BackgroundColor = "#d15f00";

	return ds1;
}

StompBox* CreateOverdrivePlugin()
{
	BossSD1* sd1 = new BossSD1();

	sd1->BackgroundColor = "#ece400";

	return sd1;
}

StompBox* CreatePhaserPlugin()
{
	Phaser *phaser = new Phaser();

	phaser->BackgroundColor = "#fa4b00";

	return phaser;
}

StompBox* CreateVibratoPlugin()
{
	Vibrato *vibrato = new Vibrato();

	vibrato->BackgroundColor = "#066800";

	return vibrato;
}

StompBox* CreateFlangerPlugin()
{
	Flanger *flanger = new Flanger();

	flanger->BackgroundColor = "#94008c";
	flanger->ForegroundColor = "#ffffff";

	return flanger;
}

StompBox* CreateChorusPlugin()
{
	Chorus *chorus = new Chorus();

	chorus->BackgroundColor = "#00c5c5";

	return chorus;
}

StompBox* CreateTremoloPlugin()
{
	Tremolo *tremolo = new Tremolo();

	tremolo->BackgroundColor = "#008a60";
	tremolo->ForegroundColor = "#ffffff";

	return tremolo;
}

StompBox* CreateDelayPlugin()
{
	Delay*delay = new Delay();

	delay->BackgroundColor = "#ecdac6";

	return delay;
}

StompBox* CreateReverbPlugin()
{
	FreeVerb *reverb = new FreeVerb();

	reverb->BackgroundColor = "#006b99";
	reverb->ForegroundColor = "#ffffff";

	return reverb;
}

StompBox* CreateEQ7Plugin()
{
	GraphicEqualizer *eq7 = new GraphicEqualizer(7, 100, 1);

	eq7->BackgroundColor = "#d6d3c1";

	return eq7;
}

StompBox* CreateTunerPlugin()
{
	PitchDetector *tuner = new PitchDetector(4096);

	tuner->IsUserSelectable = false;

	return tuner;
}

std::filesystem::path musicPath;

StompBox* CreateAudioFilePlayerPlugin()
{
	AudioFilePlayer* player = new AudioFilePlayer();

	player->IndexFiles(musicPath);

	player->IsUserSelectable = false;

	return player;
}

StompBox* CreateAudioFileRecorderPlugin()
{
	AudioFileRecorder* recorder = new AudioFileRecorder(musicPath);

	//recorder->IsUserSelectable = false;

	return recorder;
}


PluginFactory::PluginFactory()
{
	AddPlugin("Boost", &CreateBoostPlugin, true);
	AddPlugin("Compressor", &CreateCompressorPlugin, true);
	AddPlugin("Fuzz", &CreateFuzzPlugin, true);
	AddPlugin("AutoWah", &CreateAutoWahPlugin, true);
	AddPlugin("Wah", &CreateWahPlugin, true);
	AddPlugin("Screamer", &CreateScreamerPlugin, true);
	AddPlugin("Distortion", &CreateDistortionPlugin, true);
	AddPlugin("Overdrive", &CreateOverdrivePlugin, true);
	AddPlugin("Phaser", &CreatePhaserPlugin, true);
	AddPlugin("Vibrato", &CreateVibratoPlugin, true);
	AddPlugin("Flanger", &CreateFlangerPlugin, true);
	AddPlugin("Chorus", &CreateChorusPlugin, true);
	AddPlugin("Tremolo", &CreateTremoloPlugin, true);
	AddPlugin("Delay", &CreateDelayPlugin, true);
	AddPlugin("Reverb", &CreateReverbPlugin, true);
	AddPlugin("ConvoReverb", &CreateConvolutionReverbPlugin, true);
	AddPlugin("NAM", &CreateNAMPlugin, true);
	AddPlugin("EQ-7", &CreateEQ7Plugin, true);
	AddPlugin("Preamp", &CreatePreampPlugin, true);
	AddPlugin("Poweramp", &CreatePowerampPlugin, true);
	AddPlugin("Tonestack", &CreateTonestackPlugin, true);
	AddPlugin("Cabinet", &CreateGuitarConvolverPlugin, true);
	AddPlugin("Input", &CreateInputGainPlugin, false);
	AddPlugin("Master", &CreateMasterVolumePlugin, false);
	AddPlugin("Tuner", &CreateTunerPlugin, false);
	AddPlugin("AudioFilePlayer", &CreateAudioFilePlayerPlugin, false);
	AddPlugin("AudioFileRecorder", &CreateAudioFileRecorderPlugin, false);
}

void PluginFactory::AddPlugin(std::string name, CreatePluginFunction function, bool isUserSelectable)
{
	pluginMap[name] = function;

	allPluginList.push_back(name);

	if (isUserSelectable)
	{
		userPluginList.push_back(name);
	}
}

std::list<std::string> PluginFactory::GetUserPlugins()
{
	return userPluginList;
}

std::list<std::string> PluginFactory::GetAllPlugins()
{
	return allPluginList;
}


void PluginFactory::LoadAllPlugins()
{
	for (auto kv : pluginMap)
	{
		CreatePlugin(kv.first, kv.first);
	}
}


StompBox* PluginFactory::CreatePlugin(std::string const& name, std::string const& id)
{
	StompBox* component = FindPlugin(id);

	if (component != nullptr)
	{
		fprintf(stderr, "Plugin %s already loaded\n", id.c_str());

		return component;
	}

	auto iter = pluginMap.find(name);
	if (iter != pluginMap.end())
	{
		CreatePluginFunction func = iter->second;

		StompBox* component = func();

		component->Name.assign(name);
		component->ID.assign(id);

		loadedPlugins[component->ID] = component;

		fprintf(stderr, "Loaded Plugin %s\n", id.c_str());

		return component;
	}

	return nullptr;
}

void PluginFactory::ReleasePlugin(std::string id)
{
	StompBox* component = FindPlugin(id);

	if (component != nullptr)
	{
		loadedPlugins.erase(id);

		fprintf(stderr, "Releasing Plugin %s\n", id.c_str());
	}
}

StompBox* PluginFactory::FindPlugin(std::string id)
{
	auto iter = loadedPlugins.find(id);
	if (iter != loadedPlugins.end())
	{
		return iter->second;
	}

	return nullptr;
}

