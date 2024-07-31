#pragma once

#include "StompBox.h"

enum
{
	MULTIGATE_THRESHOLD,
	MULTIGATE_ATTACK,
	MULTIGATE_HOLD,
	MULTIGATE_RELEASE,
	MULTIGATE_NUMPARAMETERS
};

class MultiGate : public StompBox
{
private:
	int fSampleRate;

	double threshold;
	double attackMS;
	double holdMS;
	double releaseMS;

	double fConst0;
	double fConst1;
	double fConst2;
	double fConst3;
	double fRec7[2];
	double fConst4;
	double fRec8[2];
	double fConst5;
	double fRec3[2];
	double fRec4[2];
	double fConst6;
	double fConst7;
	double fConst8;
	double fRec0[2];
	double fConst9;
	double fRec1[2];
	double fConst10;
	double fConst11;
	double fConst12;
	double fConst13;
	double fRec11[2];
	double fConst14;
	double fRec12[2];
	double fConst15;
	double fRec15[2];
	double fRec16[2];
	double fConst16;
	double fRec20[2];
	int iVec0[2];
	double fConst17;
	int iRec21[2];
	double fRec19[2];
	double fRec24[2];
	double fRec25[2];
	double fRec23[2];
	int iVec1[2];
	int iRec27[2];
	double fRec22[2];
	double fRec31[2];
	double fRec32[2];
	double fRec28[2];
	double fRec29[2];
	double fRec34[2];
	double fRec35[2];
	double fRec38[2];
	double fRec39[2];
	double fRec43[2];
	int iVec2[2];
	int iRec44[2];
	double fRec42[2];
	double fRec47[2];
	double fRec48[2];
	double fRec46[2];
	int iVec3[2];
	int iRec50[2];
	double fRec45[2];

public:
	MultiGate();
	virtual ~MultiGate() {}
	virtual void init(int samplingFreq);
	virtual void instanceConstants(int samplingFreq);
	virtual void instanceClear();
	virtual void compute(int count, FAUSTFLOAT* input, FAUSTFLOAT* output);
};
