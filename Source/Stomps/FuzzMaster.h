#pragma once

#include "GuitarSimComponent.h"
#define __rt_data
#include <cstdint>

#include "Tube.h"

enum {
	TRANY_TABLE_KT88_68k,
	TRANY_TABLE_KT88_250k,
	TRANY_TABLE_7199P_68k,
	TRANY_TABLE_7199P_250k,
	TRANY_TABLE_SIZE
};

enum
{
	FUZZMASTER_TONE,
	FUZZMASTER_VOLUME,
	FUZZMASTER_NUMPARAMETERS
};

class FuzzMaster : public GuitarSimComponent
{
private:
	int fSamplingFreq;

	double fConst0;
	double fConst1;
	double fConst2;
	double fConst3;
	double fConst4;
	double fConst5;
	double fConst6;
	double fConst7;
	double fRec6[2];
	double fRec11[2];
	double fConst8;
	double fConst9;
	double fConst10;
	double fConst11;
	FAUSTFLOAT fVslider0;
	double fRec15[2];
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
	double fConst22;
	double fConst23;
	double fConst24;
	double fConst25;
	double fConst26;
	double fConst27;
	double fConst28;
	double fConst29;
	double fConst30;
	double fConst31;
	double fConst32;
	double fConst33;
	double fConst34;
	double fConst35;
	double fConst36;
	double fConst37;
	double fConst38;
	double fConst39;
	double fRec14[7];
	double fConst40;
	double fConst41;
	double fConst42;
	double fConst43;
	FAUSTFLOAT fVslider1;
	double fRec16[2];
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
	double fConst54;
	double fConst55;
	double fConst56;
	double fConst57;
	double fConst58;
	double fConst59;
	double fConst60;
	double fConst61;
	double fConst62;
	double fConst63;
	double fConst64;
	double fConst65;
	double fConst66;
	double fConst67;
	double fConst68;
	double fConst69;
	double fConst70;
	double fConst71;
	double fVec0[2];
	double fRec13[2];
	double fRec12[3];
	double fRec10[3];
	double fConst72;
	double fConst73;
	double fRec9[2];
	double fRec8[2];
	double fRec7[3];
	double fRec5[3];
	double fRec4[2];
	double fRec3[2];
	double fRec2[3];
	double fRec17[2];
	double fRec1[3];
	double fRec0[2];
public:
	FuzzMaster();
	virtual ~FuzzMaster() {}
	virtual void init(int samplingFreq);
	virtual void instanceConstants(int samplingFreq);
	virtual void instanceClear();
	virtual void compute(int count, FAUSTFLOAT* input, FAUSTFLOAT* output);
};
