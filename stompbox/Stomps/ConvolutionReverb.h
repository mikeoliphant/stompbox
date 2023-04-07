#pragma once

#include <filesystem>

#define NOMINMAX  // prevents windows.h from defining min/max macros and messing with juce

#if _WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#include <math.h>
#include <algorithm>
#include <memory.h>
#include <sys/stat.h>
#include <fstream>
#include <limits.h>

#include "WDL/convoengine.h"

#include "StompBox.h"

enum
{
	CONVOREVERB_IMPULSE,
	CONVOREVERB_NUMPARAMETERS
};

enum
{
	IMPUlSE_PARAM_SAMPLERATE
};


class GuitarConvolver : public StompBox
{
private:
	WDL_ImpulseBuffer impulseBuffer;
	WDL_ConvolutionEngine_Div convolutionEngine;

	double* newImpulseData[1];
	int newImpulseSamples;
	bool haveImpulseData = false;
	double impulseIndex;
	std::vector<std::string> impulseNames;
	std::vector<std::string> impulsePaths;

public:

	GuitarConvolver();
	virtual ~GuitarConvolver() {}
	virtual void init(int samplingFreq);
	void IndexImpulses(std::filesystem::path path);
	void SetParameterValue(int id, double value);
	void SetImpulse(const std::string filename);
	void SetImpulse(double** impulseData, int numSamples);
	virtual void compute(int count, double* input, double* output);
};