#pragma once

#include "GuitarSimComponent.h"
#include "PeakingFilter.h"

class GraphicEqualizer : public GuitarSimComponent
{
private:
	int fSamplingFreq;

	double volume;
	int numBands;
	PeakingFilter** peakingFilters = nullptr;

public:
	GraphicEqualizer(int numBands, int startingFreqency, double q);
	virtual ~GraphicEqualizer();
	virtual void init(int samplingFreq);
	virtual void instanceConstants(int samplingFreq);
	virtual void instanceClear();
	virtual void compute(int count, FAUSTFLOAT* input, FAUSTFLOAT* output);
};
