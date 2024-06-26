#pragma once

#include "StompBox.h"

enum
{
	FUZZ_FUZZ,
	FUZZ_OCTAVE,
	FUZZ_LEVEL,
	FUZZ_ASYMMETRY,
	FUZZ_NUMPARAMETERS
};

class Fuzz : public StompBox
{
private:
	double level;
	double fuzz;
	double octave;
	double asymmetry;

	double fb;
	double env;
public:
	Fuzz();
	~Fuzz() {}

	virtual void init(int samplingFreq);
	virtual void compute(int count, double* input, double* output);
};
