#include <math.h>
#include <algorithm>
#include "Gain.h"

Gain::Gain(double initialGain, double minGain, double maxGain)
{
	Name = "Gain";

	gain = initialGain;
	this->minGain = minGain;
	this->maxGain = maxGain;

	NumParameters = GAIN_NUMPARAMETERS;
	Parameters = new GuitarSimParameter[NumParameters];

	Parameters[GAIN_GAIN].Name = "Gain";
	Parameters[GAIN_GAIN].SourceVariable = &gain;
	Parameters[GAIN_GAIN].MinValue = minGain;
	Parameters[GAIN_GAIN].MaxValue = maxGain;
	Parameters[GAIN_GAIN].DefaultValue = initialGain;
	Parameters[GAIN_GAIN].DisplayFormat = "{0:0.0}dB";

	OutputValue = &currentLevel;
}

double Gain::GetLevel()
{
	return currentLevel;
}

void Gain::compute(int count, double* input0, double* output0)
{
	float linearGain = pow(10.0, (0.050000000000000003 * double(gain)));

	double level = 0;

	for (int i = 0; i < count; i++)
	{
		output0[i] = input0[i] * linearGain;

		level = std::max(level, abs(output0[i]));
	}

	currentLevel -= ((float)count / (float)samplingFreq) * 10 * currentLevel;

	if (currentLevel < 0)
		currentLevel = 0;

	currentLevel = std::max(currentLevel, level);
}