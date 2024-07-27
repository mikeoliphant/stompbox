#include <math.h>
#include <algorithm>

#include "HighLowFilter.h"

HighLowFilter::HighLowFilter()
{
	Name = "HighPass";

	NumParameters = HIGHLOWFILTER_NUMPARAMETERS;
	CreateParameters(NumParameters);

	Parameters[HIGHLOWFILTER_LOW_FREQUENCY].Name = "Low";
	Parameters[HIGHLOWFILTER_LOW_FREQUENCY].MinValue = 1;
	Parameters[HIGHLOWFILTER_LOW_FREQUENCY].MaxValue = 500;
	Parameters[HIGHLOWFILTER_LOW_FREQUENCY].SourceVariable = &lowFreq;
	Parameters[HIGHLOWFILTER_LOW_FREQUENCY].DefaultValue = lowFreq;
	Parameters[HIGHLOWFILTER_LOW_FREQUENCY].RangePower = 3;
	Parameters[HIGHLOWFILTER_LOW_FREQUENCY].DisplayFormat = "{0:0}hz";

	Parameters[HIGHLOWFILTER_HIGH_FREQUENCY].Name = "High";
	Parameters[HIGHLOWFILTER_HIGH_FREQUENCY].MinValue = 5000;
	Parameters[HIGHLOWFILTER_HIGH_FREQUENCY].MaxValue = 20000;
	Parameters[HIGHLOWFILTER_HIGH_FREQUENCY].SourceVariable = &highFreq;
	Parameters[HIGHLOWFILTER_HIGH_FREQUENCY].DefaultValue = highFreq;
	Parameters[HIGHLOWFILTER_HIGH_FREQUENCY].RangePower = 2;
	Parameters[HIGHLOWFILTER_HIGH_FREQUENCY].DisplayFormat = "{0:0}hz";
}

void HighLowFilter::init(int samplingFreq)
{
	StompBox::init(samplingFreq);

	instanceConstants(samplingFreq);

	instanceClear();
}

void HighLowFilter::instanceConstants(int samplingFreq)
{
	fSampleRate = samplingFreq;
	fConst0 = std::min<double>(1.92e+05, std::max<double>(1.0, double(fSampleRate)));
	fConst1 = 44.1 / fConst0;
	fConst2 = 1.0 - fConst1;
	fConst3 = 3.141592653589793 / fConst0;
}

void HighLowFilter::instanceClear() {
	for (int l0 = 0; l0 < 2; l0 = l0 + 1) {
		fRec1[l0] = 0.0;
	}
	for (int l1 = 0; l1 < 2; l1 = l1 + 1) {
		fRec3[l1] = 0.0;
	}
	for (int l2 = 0; l2 < 2; l2 = l2 + 1) {
		fVec0[l2] = 0.0;
	}
	for (int l3 = 0; l3 < 2; l3 = l3 + 1) {
		fRec2[l3] = 0.0;
	}
	for (int l4 = 0; l4 < 2; l4 = l4 + 1) {
		fRec0[l4] = 0.0;
	}
}

void HighLowFilter::compute(int count, FAUSTFLOAT* input0, FAUSTFLOAT* output0) {
	double fSlow0 = fConst1 * highFreq;
	double fSlow1 = fConst1 * lowFreq;
	for (int i0 = 0; i0 < count; i0 = i0 + 1) {
		fRec1[0] = fSlow0 + fConst2 * fRec1[1];
		double fTemp0 = 1.0 / std::tan(fConst3 * fRec1[0]);
		fRec3[0] = fSlow1 + fConst2 * fRec3[1];
		double fTemp1 = std::tan(fConst3 * fRec3[0]);
		double fTemp2 = 1.0 / fTemp1;
		double fTemp3 = double(input0[i0]);
		fVec0[0] = fTemp3;
		fRec2[0] = -((fRec2[1] * (1.0 - fTemp2) - (fTemp3 - fVec0[1]) / fTemp1) / (fTemp2 + 1.0));
		fRec0[0] = -((fRec0[1] * (1.0 - fTemp0) - (fRec2[0] + fRec2[1])) / (fTemp0 + 1.0));
		output0[i0] = FAUSTFLOAT(fRec0[0]);
		fRec1[1] = fRec1[0];
		fRec3[1] = fRec3[0];
		fVec0[1] = fVec0[0];
		fRec2[1] = fRec2[0];
		fRec0[1] = fRec0[0];
	}
}

