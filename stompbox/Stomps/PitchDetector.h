#pragma once

#include "StompBox.h"
#include "PitchMPM.h"

enum
{
	PITCHDETECTOR_MUTE,
	PITCHDETECTOR_NUMPARAMETERS
};

class PitchDetector : public StompBox
{
private:
	PitchMPM *pitchMPM;
	int bufferSize;
	int bufferSizeTimesTwo;
	float* buffer;
	int bufIndex;
	double currentPitch;
	int numDetects = 0;
	int numFailures = 0;
	bool noteOn = false;
	int lastNote = 0;
	double muteOutput = 1;
public:
	PitchDetector(int bufferSize);
	~PitchDetector()
	{
		delete pitchMPM;
		delete[] buffer;
	}

	virtual void init(int samplingFreq);
	virtual void compute(int count, double* input, double* output);

	float GetCurrentPitch();
};
