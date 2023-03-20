#pragma once

#include "GuitarSimComponent.h"

enum
{
	REVERB_ROOMSIZE,
	REVERB_DECAY,
	REVERB_WETDRY,
	REVERB_NUMPARAMETERS
};


class FreeVerb : public GuitarSimComponent
{
private:
	FAUSTFLOAT fVslider0;
	FAUSTFLOAT fVslider1;
	FAUSTFLOAT fVslider2;
	double fRec9_0; double fRec9_1;
	int IOTA;
	double fVec0[8192];
	int fSampleRate;
	int iConst1;
	double fRec8_0; double fRec8_1;
	double fRec11_0; double fRec11_1;
	double fVec1[8192];
	int iConst2;
	double fRec10_0; double fRec10_1;
	double fRec13_0; double fRec13_1;
	double fVec2[8192];
	int iConst3;
	double fRec12_0; double fRec12_1;
	double fRec15_0; double fRec15_1;
	double fVec3[8192];
	int iConst4;
	double fRec14_0; double fRec14_1;
	double fRec17_0; double fRec17_1;
	double fVec4[8192];
	int iConst5;
	double fRec16_0; double fRec16_1;
	double fRec19_0; double fRec19_1;
	double fVec5[8192];
	int iConst6;
	double fRec18_0; double fRec18_1;
	double fRec21_0; double fRec21_1;
	double fVec6[8192];
	int iConst7;
	double fRec20_0; double fRec20_1;
	double fRec23_0; double fRec23_1;
	double fVec7[8192];
	int iConst8;
	double fRec22_0; double fRec22_1;
	double fVec8[4096];
	int iConst9;
	double fRec6_0; double fRec6_1;
	double fVec9[2048];
	int iConst10;
	double fRec4_0; double fRec4_1;
	double fVec10[2048];
	int iConst11;
	double fRec2_0; double fRec2_1;
	double fVec11[1024];
	int iConst12;
	double fRec0_0; double fRec0_1;

public:
	FreeVerb();
	virtual ~FreeVerb() {}
	virtual void init(int samplingFreq);
	virtual void instanceConstants(int samplingFreq);
	virtual void instanceClear();
	virtual void compute(int count, FAUSTFLOAT* input, FAUSTFLOAT* output);
};
