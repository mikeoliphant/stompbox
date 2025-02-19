#pragma once

#include <filesystem>

#ifndef NOMINMAX
#define NOMINMAX  // prevents windows.h from defining min/max macros and messing with juce
#endif

#if _WIN32
#include "windows.h"
#endif

#include <math.h>
#include <algorithm>
#include <memory.h>
#include <sys/stat.h>
#include <fstream>
#include <limits.h>

#include "convoengine.h"

#include "StompBox.h"
#include "AudioFile.h"
#include "FileType.h"

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

struct Convolution
{
	WDL_ImpulseBuffer ImpulseBuffer;
	//WDL_ConvolutionEngine_Div ConvolutionEngine;
	WDL_ConvolutionEngine_Thread ConvolutionEngine;
};

class IRLoader : public FileLoader<Convolution>
{
private:
	int sampleRate = 48000;

protected:
	Convolution* LoadFromFile(const std::filesystem::path& loadPath)
	{
		WaveReader* waveReader = new WaveReader(loadPath.string(), (int)sampleRate);

		float* waveData = waveReader->GetWaveData();

		if (waveData != nullptr)
		{
			float* data = new float[waveReader->NumSamples];

			float gain = (float)pow(10, -18 * 0.05);  // IRs are usually too loud

			for (size_t i = 0; i < waveReader->NumSamples; i++)
			{
				data[i] = waveData[i] * gain;
			}

			Convolution* conv = new Convolution();

			float* newImpulseData[1];

			newImpulseData[0] = data;

			conv->ImpulseBuffer.Set((const float**)newImpulseData, (int)waveReader->NumSamples, 1);
			conv->ConvolutionEngine.SetImpulse(&conv->ImpulseBuffer);

			delete waveReader;

			return conv;
		}

		return nullptr;
	}

public:
	IRLoader(const FileType& fileType) :
		FileLoader<Convolution>(fileType)
	{
	}

	void SetSampleRate(int newSampleRate)
	{
		sampleRate = newSampleRate;
	}
};


class GuitarConvolver : public StompBox
{
private:
	float impulseIndex = -1;
	float wet = 1;
	float dry = 0;
	FileType fileType;
	IRLoader irLoader;

public:

	GuitarConvolver(const std::string folderName, const std::vector<std::string> fileExtensions, const std::filesystem::path& basePath);
	virtual ~GuitarConvolver() {}
	virtual void init(int samplingFreq);
	void SetParameterValue(StompBoxParameter* parameter, float value);
	virtual void compute(int count, float* input, float* output);
};