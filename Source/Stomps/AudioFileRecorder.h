#pragma once

#include <math.h>
#include <algorithm>
#include <sys/stat.h>
#include <fstream>
#include <filesystem>
#include <limits.h>

#include "StompBox.h"
#include "WaveReader.h"

enum
{
	AUDIOFILERECORDER_SECONDSRECORDED,
	AUDIOFILERECORDER_NUMPARAMETERS
};

class AudioFileRecorder : public StompBox
{
private:
	WaveWriter* waveWriter = nullptr;
	std::filesystem::path savePath;
	double recordSeconds;
public:
	AudioFileRecorder(std::filesystem::path savePath);
	~AudioFileRecorder();

	virtual void HandleCommand(std::vector<std::string> commandWords);

	void SaveRecording();

	virtual void init(int samplingFreq);
	virtual void compute(int count, double* input, double* output);
};
