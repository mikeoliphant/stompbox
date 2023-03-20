#pragma once

#include "GuitarSimComponent.h"

enum
{
	VIBRATO_SPEED,
	VIBRATO_DEPTH,
	VIBRATO_FEEDBACK,
	VIBRATO_RATIO,
	VIBRATO_NUMPARAMETERS
};

class Vibrato : public GuitarSimComponent
{
private:
	int fSamplingFreq;

	double freq;
	double depth;
	double fb;
	double width;
	double frqmin;
	double frqmax;
	double fratio;

	double fConst0;
	double fConst1;
	int iVec0[2];
	double fConst2;
	double fRec3[2];
	double fRec4[2];
	double fRec2[3];
	double fRec1[3];
double fRec0[2]; public:
	Vibrato();
	virtual ~Vibrato() {}
	virtual void init(int samplingFreq);
	virtual void instanceConstants(int samplingFreq);
	virtual void instanceClear();
	virtual void compute(int count, FAUSTFLOAT* input, FAUSTFLOAT* output);
};
