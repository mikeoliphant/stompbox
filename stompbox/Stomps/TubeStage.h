#pragma once

#include "StompBox.h"

enum
{
	TUBESTAGE_GAIN,
	TUBESTAGE_NUMPARAMETERS
};

class TubeStage : public StompBox
{
private:
	int fSamplingFreq;
	int tubeTableIndex;

	double vplus;
	double divider;
	double Rp;
	double fck;
	double Rk;
	double Vk0;

	double fConst0;
	double fConst1;
	double fConst2;
	double fConst3;
	double fConst4;
	double fConst5;
	double fConst6;
	double fConst7;
	double fConst8;
	double fVec0[2];
	double fRec3[2];
	FAUSTFLOAT fVslider0;
	double fRec6[2];
	double fVec1[2];
	double fRec5[2];
	double fRec4[3];
	double fRec2[2];
	double fVec2[2];
	double fConst9;
	double fConst10;
	double fRec1[2];
	double fRec0[2];

public:
	TubeStage(int tubeTableIndex, double initialGain, double vplus, double divider, double Rp, double fck, double Rk, double Vk0);
	virtual ~TubeStage() {}
	virtual void init(int samplingFreq);
	virtual void instanceConstants(int samplingFreq);
	virtual void instanceClear();
	virtual void compute(int count, double* input, double* output);
};
