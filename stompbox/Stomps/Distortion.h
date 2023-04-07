#pragma once

#include "StompBox.h"

enum
{
	DISTORTION_DISTORTION,
	DISTORTION_LEVEL,
	DISTORTION_TONE,
	DISTORTION_NUMPARAMETERS
};

class BossDS1 : public StompBox
{
private:
	int fSamplingFreq;

	double fConst0;
	double fConst1;
	double fConst2;
	FAUSTFLOAT fHslider0;
	double fRec0[2];
	double fConst3;
	double fConst4;
	double fConst5;
	double fConst6;
	double fConst7;
	double fConst8;
	double fConst9;
	double fConst10;
	double fConst11;
	double fConst12;
	double fConst13;
	double fConst14;
	double fConst15;
	double fConst16;
	double fConst17;
	double fConst18;
	double fConst19;
	double fConst20;
	double fConst21;
	double fRec7[2];
	double fConst22;
	double fConst23;
	double fConst24;
	double fRec6[4];
	double fConst25;
	FAUSTFLOAT fHslider1;
	double fRec8[2];
	double fConst26;
	double fConst27;
	double fConst28;
	double fConst29;
	double fConst30;
	double fConst31;
	double fRec5[3];
	double fConst32;
	double fConst33;
	double fConst34;
	double fConst35;
	double fConst36;
	double fConst37;
	double fRec4[3];
	double fConst38;
	double fConst39;
	double fVec0[2];
	double fRec9[2];
	double fConst40;
	double fConst41;
	double fRec3[3];
	FAUSTFLOAT fVslider0;
	double fRec10[2];
	double fConst42;
	double fConst43;
	double fConst44;
	double fConst45;
	double fConst46;
	double fConst47;
	double fConst48;
	double fConst49;
	double fConst50;
	double fConst51;
	double fConst52;
	double fConst53;
	double fRec2[3];
	double fConst54;
	double fConst55;
	double fConst56;
	double fConst57;
	double fConst58;
	double fConst59;
	double fConst60;
	double fConst61;
	double fConst62;
	double fRec1[3];
	double fConst63;

public:
	BossDS1();
	virtual ~BossDS1() {}
	virtual void init(int samplingFreq);
	virtual void instanceConstants(int samplingFreq);
	virtual void instanceClear();
	virtual void compute(int count, FAUSTFLOAT* input, FAUSTFLOAT* output);
};
