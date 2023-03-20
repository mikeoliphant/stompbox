#pragma once

#include "StompBox.h"

enum
{
	GAIN_GAIN,
	GAIN_NUMPARAMETERS
};

class Gain : public StompBox
{
private:
	double gain;
	double minGain;
	double maxGain;
	double currentLevel = 0;

public:
	Gain(double initialGain, double minGain, double maxGain);
	~Gain() {}
	double GetLevel();

	virtual void compute(int count, double* input, double* output);
};
