#pragma once

#include <filesystem>

#define NOMINMAX  // prevents windows.h from defining min/max macros and messing with juce

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
#include "AudioFile.h"

enum
{
	CONVOLVER_IMPULSE,
	CONVOLVER_DRY,
	CONVOLVER_WET,
	CONVOLVER_NUMPARAMETERS
};

enum
{
	IMPUlSE_PARAM_SAMPLERATE
};


class GuitarConvolver : public StompBox
{
private:
	WDL_ImpulseBuffer impulseBuffer;
//#ifdef _WIN32
	WDL_ConvolutionEngine_Thread convolutionEngine;
//#else
	//WDL_ConvolutionEngine_Div convolutionEngine;
//#endif

	double* impulseData = nullptr;
	bool haveNewImpulseData = false;
	bool haveImpulseData = false;
	uint32_t impulseSamples;
	double impulseIndex = -1;
	double wet = 1;
	double dry = 0;
	std::vector<std::string> impulseNames;
	std::vector<std::string> impulsePaths;

public:

	GuitarConvolver();
	virtual ~GuitarConvolver() {}
	virtual void init(int samplingFreq);
	void IndexImpulses(std::filesystem::path path);
	void SetParameterValue(StompBoxParameter* parameter, double value);
	void SetImpulse(const std::string filename);
	void SetImpulse();
	virtual void compute(int count, double* input, double* output);
};