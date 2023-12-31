#pragma once

#include <filesystem>

#if _WIN32
#include "windows.h"
#endif

#include <math.h>
#include <algorithm>
#include <memory.h>
#include <sys/stat.h>
#include <fstream>
#include <limits.h>

#define WDL_FFT_REALSIZE 8
#define WDL_CONVO_THREAD
#include "convoengine.h"

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