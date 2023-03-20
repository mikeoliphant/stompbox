#pragma once

#include <filesystem>

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

#include "NAM/dsp.h"
#include "GuitarSimComponent.h"

enum
{
	NAM_MODEL,
	NAM_NUMPARAMETERS
};


class NAM : public GuitarSimComponent
{
private:
	double modelIndex;
	std::vector<std::string> modelNames;
	std::vector<std::string> modelPaths;
	std::unique_ptr<DSP> stagedModel;
	std::unique_ptr<DSP> activeModel;

public:

	NAM();
	virtual ~NAM() {}
	virtual void init(int samplingFreq);
	void IndexModels(std::filesystem::path path);
	void SetParameterValue(int id, double value);
	void SetModel(const std::string filename);
	virtual void compute(int count, double* input, double* output);
};