#pragma once

#include "StompBox.h"
#include "PeakingFilter.h"

class GraphicEqualizer : public StompBox
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
