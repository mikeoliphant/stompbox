#pragma once

#include <math.h>
#include <algorithm>
#include <sys/stat.h>
#include <fstream>
#include <filesystem>
#include <limits.h>

#include "GuitarSimComponent.h"
#include "WaveReader.h"

enum
{
	AUDIOFILEPLAYER_FILE,
	AUDIOFILEPLAYER_LEVEL,
	AUDIOFILEPLAYER_PLAYING,
	AUDIOFILEPLAYER_POSITION,
	AUDIOFILEPLAYER_NUMPARAMETERS
};

class AudioFilePlayer : public GuitarSimComponent
{
private:
	std::vector<std::string> fileNames;
	std::vector<std::filesystem::path> filePaths;
	double fileIndex = 0;
	double level = 0.5;
	double playing = 0;
	double position = 0;
	int audioBufferSize = 0;
	bool needUpdate = false;
	WaveReader* waveReader = nullptr;
	char* waveBuffer = nullptr;
	double* recordBuffer = nullptr;
	bool recordArmed = false;
	bool recording = false;
	bool haveRecording = false;
	int readPosition;
	bool needWaveLoad = false;
public:
	AudioFilePlayer();
	~AudioFilePlayer() {}
	void IndexFiles(std::filesystem::path path);
	void SetFile(const std::string filename);
	void SetParameterValue(int id, double value);
	virtual void HandleCommand(std::vector<std::string> commandWords);

	virtual void init(int samplingFreq);
	virtual void compute(int count, double* input, double* output);
};
