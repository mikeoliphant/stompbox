#pragma once

#include <math.h>
#include <algorithm>
#include <sys/stat.h>
#include <fstream>
#include <filesystem>
#include <limits.h>

#include "StompBox.h"
#include "AudioFile.h"
#include "FileType.h"

enum
{
	AUDIOFILEPLAYER_FILE,
	AUDIOFILEPLAYER_LEVEL,
	AUDIOFILEPLAYER_PLAYING,
	AUDIOFILEPLAYER_POSITION,
	AUDIOFILEPLAYER_NUMPARAMETERS
};

class AudioFilePlayer : public StompBox
{
private:
	double fileIndex = -1;
	double level = 0.5;
	double playing = 0;
	double position = 0;
	size_t audioBufferSize = 0;
	bool needUpdate = false;
	WaveReader* waveReader = nullptr;
	float* waveBuffer = nullptr;
	double* recordBuffer = nullptr;
	bool recordArmed = false;
	bool recording = false;
	bool haveRecording = false;
	size_t readPosition;
	bool needWaveLoad = false;
	FileType fileType;
public:
	AudioFilePlayer(const std::string folderName, const std::vector<std::string> fileExtensions, const std::filesystem::path& basePath);
	~AudioFilePlayer() {}
	void SetFile();
	void SetParameterValue(StompBoxParameter* parameter, double value);
	virtual void HandleCommand(std::vector<std::string> commandWords);

	virtual void init(int samplingFreq);
	virtual void compute(int count, double* input, double* output);
};
