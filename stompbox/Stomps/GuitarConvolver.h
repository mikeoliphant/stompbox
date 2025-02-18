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
	double sampleRate = 48000;

protected:
	Convolution* LoadFromFile(const std::filesystem::path& loadPath)
	{
		WaveReader* waveReader = new WaveReader(loadPath.string(), (int)sampleRate);

		float* waveData = waveReader->GetWaveData();

		if (waveData != nullptr)
		{
			double* data = new double[waveReader->NumSamples];

			float gain = pow(10, -18 * 0.05);  // IRs are usually too loud

			for (int i = 0; i < waveReader->NumSamples; i++)
			{
				data[i] = waveData[i] * gain;
			}

			Convolution* conv = new Convolution();

			double* newImpulseData[1];

			newImpulseData[0] = data;

			conv->ImpulseBuffer.Set((const double**)newImpulseData, waveReader->NumSamples, 1);
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

	void SetSampleRate(double sampleRate)
	{
		this->sampleRate = sampleRate;
	}
};


class GuitarConvolver : public StompBox
{
private:
	double impulseIndex = -1;
	double wet = 1;
	double dry = 0;
	FileType fileType;
	IRLoader irLoader;

public:

	GuitarConvolver(const std::string folderName, const std::vector<std::string> fileExtensions, const std::filesystem::path& basePath);
	virtual ~GuitarConvolver() {}
	virtual void init(int samplingFreq);
	void SetParameterValue(StompBoxParameter* parameter, double value);
	virtual void compute(int count, double* input, double* output);
};