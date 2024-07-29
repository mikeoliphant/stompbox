#pragma once

#include "StompBox.h"
#include "NAM.h"
#include "Crossover.h"

enum
{
	NAMMULTIBAND_CROSSOVER_FREQ,
	NAMMULTIBAND_MODEL_GAIN,
	NAMMULTIBAND_MODEL_VOLUME,
	NAMMULTIBAND_MODEL,
	NAMMULTIBAND_NUMPARAMETERS
};

class NAMMultiBand : public StompBox
{
private:
	NAM nam;
	Crossover crossover;

	double crossoverFreq = 300;
	// hacky - won't work for buffer sizes > 4096
	double splitBuf[2][4096];

public:

	NAMMultiBand();
	virtual ~NAMMultiBand() {}
	virtual void init(int samplingFreq);
	void IndexModels(std::filesystem::path path);
	double GetParameterValue(StompBoxParameter* parameter) override;
	void SetParameterValue(StompBoxParameter* parameter, double value) override;
	virtual void compute(int count, double* input, double* output);
};