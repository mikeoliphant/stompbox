#include <math.h>
#include <algorithm>

#include "Tube.h"
#include "TubeStage.h"

TubeStage::TubeStage(int tubeTableIndex, double initialGain, double vplus, double divider, double Rp, double fck, double Rk, double Vk0)

{
	Name = "TubeStage";

	this->tubeTableIndex = tubeTableIndex;
	this->vplus = vplus;
	this->divider = divider;
	this->Rp = Rp;
	this->fck = fck;
	this->Rk = Rk;
	this->Vk0 = Vk0;

	fVslider0 = FAUSTFLOAT(initialGain);

	NumParameters = TUBESTAGE_NUMPARAMETERS;
	Parameters = new GuitarSimParameter[NumParameters];

	Parameters[TUBESTAGE_GAIN].Name = "Gain";
	Parameters[TUBESTAGE_GAIN].SourceVariable = &fVslider0;
	Parameters[TUBESTAGE_GAIN].MinValue = -20;
	Parameters[TUBESTAGE_GAIN].MaxValue = 20;
	Parameters[TUBESTAGE_GAIN].Step = 0.1;
}

void TubeStage::init(int samplingFreq)
{
	GuitarSimComponent::init(samplingFreq);

	instanceConstants(samplingFreq);

	instanceClear();
}

void TubeStage::instanceConstants(int samplingFreq)
{
	fSamplingFreq = samplingFreq;

	fConst0 = std::min<double>(192000.0, std::max<double>(1.0, double(fSamplingFreq)));
	fConst1 = (1.0 / tan((20517.741620594938 / fConst0)));
	fConst2 = (1.0 / (fConst1 + 1.0));
	fConst3 = (1.0 - fConst1);
	fConst4 = tan((97.389372261283583 / fConst0));
	fConst5 = (1.0 / fConst4);
	fConst6 = (fConst5 + 1.0);
	fConst7 = (0.0 - (1.0 / (fConst4 * fConst6)));
	fConst8 = (3.1415926535897931 / fConst0);
	fConst9 = (1.0 / fConst6);
	fConst10 = (1.0 - fConst5);
}

void TubeStage::instanceClear() {
	for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
		fVec0[l0] = 0.0;

	}
	for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
		fRec3[l1] = 0.0;

	}
	for (int l2 = 0; (l2 < 2); l2 = (l2 + 1)) {
		fRec6[l2] = 0.0;

	}
	for (int l3 = 0; (l3 < 2); l3 = (l3 + 1)) {
		fVec1[l3] = 0.0;

	}
	for (int l4 = 0; (l4 < 2); l4 = (l4 + 1)) {
		fRec5[l4] = 0.0;

	}
	for (int l5 = 0; (l5 < 3); l5 = (l5 + 1)) {
		fRec4[l5] = 0.0;

	}
	for (int l6 = 0; (l6 < 2); l6 = (l6 + 1)) {
		fRec2[l6] = 0.0;

	}
	for (int l7 = 0; (l7 < 2); l7 = (l7 + 1)) {
		fVec2[l7] = 0.0;

	}
	for (int l8 = 0; (l8 < 2); l8 = (l8 + 1)) {
		fRec1[l8] = 0.0;

	}
	for (int l9 = 0; (l9 < 2); l9 = (l9 + 1)) {
		fRec0[l9] = 0.0;

	}
}

void TubeStage::compute(int count, double* input0, double* output0) {
	double fSlow0 = (1.0 / divider);
	double fSlow1 = ((Rp * Vk0) / Rk);
	double fSlow2 = (1.0 / tan((fConst8 * fck)));
	double fSlow3 = (1.0 / (fSlow2 + 1.0));
	double fSlow4 = (Rk / Rp);
	double fSlow5 = (1.0 - fSlow2);
	double fSlow6 = (0.0010000000000000009 * pow(10.0, (0.050000000000000003 * double(fVslider0))));
	double fSlow7 = (fConst5 / divider);
	for (int i = 0; (i < count); i = (i + 1)) {
		double fTemp0 = (fSlow4 * fRec2[1]);
		fVec0[0] = fTemp0;
		fRec3[0] = (fSlow3 * ((fTemp0 + fVec0[1]) - (fSlow5 * fRec3[1])));
		fRec6[0] = (fSlow6 + (0.999 * fRec6[1]));
		double fTemp1 = (double(input0[i]) * fRec6[0]);
		fVec1[0] = fTemp1;
		fRec5[0] = ((0.93028479253239138 * (fTemp1 + fVec1[1])) - (0.86056958506478287 * fRec5[1]));
		fRec4[0] = (fRec5[0] - ((0.86129424393186271 * fRec4[2]) + (1.8405051250752198 * fRec4[1])));
		fRec2[0] = (fSlow1 + (double(Ftube(int(TUBE_TABLE_12AX7_68k), double((((fRec3[0] + (1.8508996845035413 * fRec4[1])) + (0.92544984225177063 * (fRec4[0] + fRec4[2]))) - Vk0)))) - vplus));
		fVec2[0] = (fSlow0 * fRec2[0]);
		fRec1[0] = ((fConst7 * fVec2[1]) - (fConst9 * ((fConst10 * fRec1[1]) - (fSlow7 * fRec2[0]))));
		fRec0[0] = (0.0 - (fConst2 * ((fConst3 * fRec0[1]) - (fRec1[0] + fRec1[1]))));
		output0[i] = FAUSTFLOAT(fRec0[0]);
		fVec0[1] = fVec0[0];
		fRec3[1] = fRec3[0];
		fRec6[1] = fRec6[0];
		fVec1[1] = fVec1[0];
		fRec5[1] = fRec5[0];
		fRec4[2] = fRec4[1];
		fRec4[1] = fRec4[0];
		fRec2[1] = fRec2[0];
		fVec2[1] = fVec2[0];
		fRec1[1] = fRec1[0];
		fRec0[1] = fRec0[0];

	}
}
