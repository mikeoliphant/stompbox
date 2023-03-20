#pragma once

#define NOMINMAX

//#include "WDLConvolver/Oversampler.h"

#ifndef FAUSTFLOAT
#define FAUSTFLOAT double
#endif

#include <string>
#include <vector>
#include <functional>

#ifdef __linux__
#define __forceinline __attribute__((always_inline))
#endif

enum
{
	PARAMETER_TYPE_KNOB,
	PARAMETER_TYPE_BOOL,
	PARAMETER_TYPE_INT,
	PARAMETER_TYPE_VSLIDER,
	PARAMETER_TYPE_ENUM
};

struct GuitarSimParameter
{
	std::string Name = "Unnamed";
	double* SourceVariable;
	double MinValue = 0;
	double MaxValue = 1;
	double DefaultValue = 0.5;
	const char* DisplayFormat = "{0:0.00}";
	double Step = 0.01;
	int ParameterType = PARAMETER_TYPE_KNOB;
	bool CanSyncToHostBPM = false;
	int BPMSyncNumerator = 0;
	int BPMSyncDenominator = 0;
	bool IsAdvanced = false;
	std::vector<std::string>* EnumValues = nullptr;
	bool IsDirty = false;
	bool SuppressSave = false;
};

class GuitarSimComponent
{
protected:
	double samplingFreq = 0;
	double bpm;
	double* doubleBuffer[1];
	int bufferSize;
	//void doComputeDouble(int count, FAUSTFLOAT* input, FAUSTFLOAT* output, EFactor oversample);
	//OverSampler<double>* overSampler = nullptr;

public:
	bool Enabled = false;
	GuitarSimParameter* Parameters = nullptr;
	double* OutputValue = nullptr;
	int NumParameters;
	std::string Name;
	std::string ID;
	std::string BackgroundColor;
	std::string ForegroundColor;
	bool IsUserSelectable = true;
	bool NeedsInit = true;
	double RampTime = 0;
	int RampSamples = 0;
	int RampPos;
	bool EnabledIsDirty = false;
	bool ParamIsDirty = false;
	std::function<void(int, int, int)> MidiCallback = nullptr;

	GuitarSimComponent();
	virtual ~GuitarSimComponent()
	{
		if (Parameters != nullptr)
		{
			delete[] Parameters;
		}
	}
	virtual GuitarSimParameter* GetParameter(int id);
	virtual void SetParameterValue(int id, double value);
	virtual double GetParameterValue(int index);
	virtual void HandleCommand(std::vector<std::string> commandWords) { };
	virtual void SetBPM(double bpm);
	virtual void UpdateBPM();
	virtual void init(int samplingFreq)
	{
		this->samplingFreq = samplingFreq;
	}
	virtual void instanceConstants(int samplingFreq) {};
	virtual void instanceClear() {};
	virtual void compute(int count, double* input, double* output) {};
};