#include <math.h>
#include <algorithm>

#include "Compressor.h"

static double mydsp_faustpower2_f(double value) {
	return (value * value);

}

Compressor::Compressor()
{
	Name = "Compressor";
	Description = "Audio level compressor";

	fVslider0 = FAUSTFLOAT(1.0);
	fEntry0 = FAUSTFLOAT(20.0);
	fEntry1 = FAUSTFLOAT(3.0);
	fEntry2 = FAUSTFLOAT(3.0);
	fHslider0 = FAUSTFLOAT(0.02);
	fHslider1 = FAUSTFLOAT(0.2);

	NumParameters = COMPRESSOR_NUMPARAMETERS;
	CreateParameters(NumParameters);
	Parameters[COMPRESSOR_SUSTAIN].Name = "Sustain";
	Parameters[COMPRESSOR_SUSTAIN].SourceVariable = &fEntry0;
	Parameters[COMPRESSOR_SUSTAIN].MinValue = 0;
	Parameters[COMPRESSOR_SUSTAIN].MaxValue = 40;
	Parameters[COMPRESSOR_SUSTAIN].DefaultValue = fEntry0;
	Parameters[COMPRESSOR_SUSTAIN].DisplayFormat = "{0:0.0}";
	Parameters[COMPRESSOR_SUSTAIN].Description = "Compression threshold (dB below 0)";

	Parameters[COMPRESSOR_ATTACK].Name = "Attack";
	Parameters[COMPRESSOR_ATTACK].MinValue = 0.002;
	Parameters[COMPRESSOR_ATTACK].MaxValue = 0.05;
	Parameters[COMPRESSOR_ATTACK].Step = 0.001;
	Parameters[COMPRESSOR_ATTACK].DefaultValue = fHslider0;
	Parameters[COMPRESSOR_ATTACK].SourceVariable = &fHslider0;
	Parameters[COMPRESSOR_ATTACK].DisplayFormat = "{0:.000}s";
	Parameters[COMPRESSOR_ATTACK].Description = "Compression attack time";

	Parameters[COMPRESSOR_WETDRY].Name = "Blend";
	Parameters[COMPRESSOR_WETDRY].DefaultValue = fVslider0;
	Parameters[COMPRESSOR_WETDRY].SourceVariable = &fVslider0;
	Parameters[COMPRESSOR_WETDRY].Description = "Wet/dry effect blend";
}

void Compressor::init(int samplingFreq)
{
	StompBox::init(samplingFreq);

	instanceConstants(samplingFreq);

	instanceClear();
}

void Compressor::instanceConstants(int samplingFreq)
{
	fSamplingFreq = samplingFreq;

	fConst0 = std::min<double>(192000.0, std::max<double>(1.0, double(fSamplingFreq)));
	fConst1 = (1.0 / fConst0);
	fConst2 = exp((0.0 - (10.0 / fConst0)));
	fConst3 = (1.0 - fConst2);
}

void Compressor::instanceClear() {
	for (int l0 = 0; (l0 < 2); l0 = (l0 + 1)) {
		fRec1[l0] = 0.0;

	}
	for (int l1 = 0; (l1 < 2); l1 = (l1 + 1)) {
		fRec0[l1] = 0.0;

	}
}

void Compressor::compute(int count, FAUSTFLOAT* input0, FAUSTFLOAT* output0) {
	double fSlow0 = double(fVslider0);
	double fSlow1 = (1.0 - fSlow0);
	double fSlow2 = double(fEntry0);
	double fSlow3 = double(fEntry1);
	double fSlow4 = (fSlow0 * pow(10.0, (0.025000000000000001 * (fSlow2 * (1.0 - (1.0 / fSlow3))))));
	double fSlow5 = (1.0 - fSlow3);
	double fSlow6 = (0.050000000000000003 * fSlow5);
	double fSlow7 = double(fEntry2);
	double fSlow8 = (1.0 / (fSlow7 + 0.001));
	double fSlow9 = (fSlow2 + fSlow7);
	double fSlow10 = exp((0.0 - (fConst1 / std::max<double>(fConst1, double(fHslider0)))));
	double fSlow11 = exp((0.0 - (fConst1 / std::max<double>(fConst1, double(fHslider1)))));
	for (int i = 0; (i < count); i = (i + 1)) {
		double fTemp0 = double(input0[i]);
		fRec1[0] = ((fConst3 * fabs(((fSlow0 * fTemp0) + 9.9999999999999995e-21))) + (fConst2 * fRec1[1]));
		double fTemp1 = ((fSlow10 * double((fRec0[1] < fRec1[0]))) + (fSlow11 * double((fRec0[1] >= fRec1[0]))));
		fRec0[0] = ((fRec0[1] * fTemp1) + (fRec1[0] * (1.0 - fTemp1)));
		double fTemp2 = std::max<double>(0.0, (fSlow9 + (20.0 * log10(fRec0[0]))));
		double fTemp3 = std::min<double>(1.0, std::max<double>(0.0, (fSlow8 * fTemp2)));
		output0[i] = FAUSTFLOAT((fTemp0 * (fSlow1 + (fSlow4 * pow(10.0, (fSlow6 * ((fTemp3 * fTemp2) / (1.0 - (fSlow5 * fTemp3)))))))));
		fRec1[1] = fRec1[0];
		fRec0[1] = fRec0[0];

	}
}
