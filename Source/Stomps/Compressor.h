#pragma once

#include "GuitarSimComponent.h"

enum
{
	COMPRESSOR_SUSTAIN,
	COMPRESSOR_ATTACK,
	COMPRESSOR_WETDRY,
	COMPRESSOR_NUMPARAMETERS
};

class Compressor : public GuitarSimComponent
{
private:
	int fSamplingFreq;

	FAUSTFLOAT fVslider0;
	FAUSTFLOAT fEntry0;
	FAUSTFLOAT fEntry1;
	FAUSTFLOAT fEntry2;
	double fConst0;
	double fConst1;
	FAUSTFLOAT fHslider0;
	double fConst2;
	double fConst3;
	double fRec1[2];
	FAUSTFLOAT fHslider1;
	double fRec0[2];

public:
	Compressor();
	virtual ~Compressor() {}
	virtual void init(int samplingFreq);
	virtual void instanceConstants(int samplingFreq);
	virtual void instanceClear();
	virtual void compute(int count, FAUSTFLOAT* input, FAUSTFLOAT* output);
};
