#pragma once

#include "GuitarSimComponent.h"

enum
{
	OVERDRIVE_DRIVE,
	OVERDRIVE_TONE,
	OVERDRIVE_LEVEL,
	OVERDRIVE_NUMPARAMETERS
};

class BossSD1 : public GuitarSimComponent
{
private:
	int fSamplingFreq;

	FAUSTFLOAT 	fslider0;
	double 	fRec0[2];
	int 	iConst0;
	double 	fConst1;
	double 	fConst2;
	double 	fConst3;
	double 	fConst4;
	double 	fRec2[2];
	double 	fConst5;
	double 	fConst6;
	double 	fVec0[2];
	double 	fConst7;
	FAUSTFLOAT 	fslider1;
	double 	fRec4[2];
	double 	fConst8;
	double 	fConst9;
	double 	fConst10;
	double 	fConst11;
	double 	fConst12;
	double 	fRec3[2];
	double 	fVec1[2];
	FAUSTFLOAT 	fslider2;
	double 	fConst13;
	double 	fRec1[2];

public:
	BossSD1();
	virtual ~BossSD1() {}
	virtual void init(int samplingFreq);
	virtual void instanceConstants(int samplingFreq);
	virtual void instanceClear();
	virtual void compute(int count, FAUSTFLOAT* input, FAUSTFLOAT* output);
};
