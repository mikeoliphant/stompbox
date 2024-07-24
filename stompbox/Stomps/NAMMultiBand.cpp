#include "NAMMultiBand.h"

NAMMultiBand::NAMMultiBand()
{
    Name = "NAMMulti";

    NumParameters = NAMMULTIBAND_NUMPARAMETERS;
    CreateParameters(NumParameters);

	Parameters[NAMMULTIBAND_CROSSOVER_FREQ].Name = "Freq";
	Parameters[NAMMULTIBAND_CROSSOVER_FREQ].MinValue = 10;
	Parameters[NAMMULTIBAND_CROSSOVER_FREQ].MaxValue = 1000;
	Parameters[NAMMULTIBAND_CROSSOVER_FREQ].SourceVariable = &crossoverFreq;
	Parameters[NAMMULTIBAND_CROSSOVER_FREQ].DefaultValue = crossoverFreq;
	Parameters[NAMMULTIBAND_CROSSOVER_FREQ].DisplayFormat = "{0:0}hz";

	memcpy(&Parameters[NAMMULTIBAND_MODEL], &nam.Parameters[NAM_MODEL], sizeof(StompBoxParameter));
}

void NAMMultiBand::init(int samplingFreq)
{
    nam.init(samplingFreq);
	crossover.init(samplingFreq);
}

void NAMMultiBand::IndexModels(std::filesystem::path path)
{
	nam.IndexModels(path);
}

void NAMMultiBand::SetParameterValue(StompBoxParameter *parameter, double value)
{
	StompBox::SetParameterValue(parameter, value);

	if (parameter == &Parameters[NAMMULTIBAND_MODEL])
	{
		nam.GetParameter(NAM_MODEL)->SetValue(value);
	}
}

void NAMMultiBand::compute(int count, double* input, double* output)
{
	crossover.SetCrossoverFrequency(crossoverFreq);
	crossover.compute(count, input, splitBuf[0], splitBuf[1]);

	// Run nam on high frequencies
	nam.compute(count, splitBuf[1], output);

	// Blend in passthrough of low frequencies
	for (int i = 0; i < count; i++)
	{
		output[i] += splitBuf[0][i];
	}
}
