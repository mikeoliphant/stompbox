#include <math.h>
#include <algorithm>

#include "tube.h"
#include "AmpStage.h"

AmpStage::AmpStage()
{
	Name = "AmpStage";

	RampTime = 0.1;

	Gain = 3;

	NumParameters = AMPSTAGE_NUMPARAMETERS;
	Parameters = new StompBoxParameter[NumParameters];

	Parameters[AMPSTAGE_GAIN].Name = "Gain";
	Parameters[AMPSTAGE_GAIN].SourceVariable = &Gain;
	Parameters[AMPSTAGE_GAIN].MinValue = 0;
	Parameters[AMPSTAGE_GAIN].MaxValue = 10;
	Parameters[AMPSTAGE_GAIN].DefaultValue = 3;
	Parameters[AMPSTAGE_GAIN].Step = 0.1;
	Parameters[AMPSTAGE_GAIN].DisplayFormat = "{0:0.0}";

	Parameters[AMPSTAGE_STAGETYPE].Name = "StageType";
	Parameters[AMPSTAGE_STAGETYPE].SourceVariable = &StageType;
	Parameters[AMPSTAGE_STAGETYPE].ParameterType = PARAMETER_TYPE_ENUM;

	Parameters[AMPSTAGE_NUMSTAGES].Name = "NumStages";
	Parameters[AMPSTAGE_NUMSTAGES].SourceVariable = &NumTubeStages;
	Parameters[AMPSTAGE_NUMSTAGES].ParameterType = PARAMETER_TYPE_INT;
	Parameters[AMPSTAGE_NUMSTAGES].MinValue = 1;

	for (int i = 0; i < 3; i++)
	{
		TubeStages[i] = nullptr;
	}

	NumTubeStages = 0;
}

double AmpStage::GetParameterValue(int index)
{
	if (needUpdate)
	{
		// Make sure we return the updated value

		if (index == AMPSTAGE_STAGETYPE)
		{
			return newStageType;
		}
		else if (index == AMPSTAGE_NUMSTAGES)
		{
			return newNumStages;
		}
	}

	return *(Parameters[index].SourceVariable);
}


void AmpStage::SetParameterValue(int id, double value)
{
	if (id == AMPSTAGE_STAGETYPE)
	{
		newStageType = (int)value;

		needUpdate = true;
	}
	else if (id == AMPSTAGE_NUMSTAGES)
	{
		newNumStages = (int)value;

		needUpdate = true;
	}
	else
	{
		StompBox::SetParameterValue(id, value);
	}
}


void AmpStage::UpdateStageType(int stageType, int numTubeStages)
{
	StageType = stageType;
	NumTubeStages = numTubeStages;

	for (int i = 0; i < 3; i++)
	{
		if (TubeStages[i] != nullptr)
		{
			delete TubeStages[i];
			TubeStages[i] = nullptr;
		}
	}

	// Have to map it, since pre and power amp have different enum lists
	switch ((int)(*stageMap)[(int)StageType])
	{
		// fck = 1 / (2 * pi * Rk * Ck)
		// Ck is uF / 1000000
		case AMPSTAGE_12AX7:
			TubeStages[0] = new TubeStage(int(TUBE_TABLE_12AX7_68k), 0, 250.0, 40.0, 100000, 86.0, 2700.0, 1.581656);
			if (NumTubeStages > 1)
				TubeStages[1] = new TubeStage(int(TUBE_TABLE_12AX7_250k), 0, 250.0, 40.0, 100000, 132.0, 1500.0, 1.204285);
			if (NumTubeStages > 2)
				TubeStages[2] = new TubeStage(int(TUBE_TABLE_12AX7_250k), 0, 250.0, 40.0, 100000, 194.0, 820.0, 0.840703);
			Volume = 1;
			break;
		case AMPSTAGE_JJECC83S:
			TubeStages[0] = new TubeStage(int(TUBE_TABLE_JJECC83S_68k), 0, 250.0, 40.0, 100000, 86.0, 2700.0, 1.581656);
			if (NumTubeStages > 1)
				TubeStages[1] = new TubeStage(int(TUBE_TABLE_JJECC83S_250k), 0, 250.0, 40.0, 100000, 132.0, 1500.0, 1.204285);
			if (NumTubeStages > 2)
				TubeStages[2] = new TubeStage(int(TUBE_TABLE_JJECC83S_250k), 0, 250.0, 40.0, 100000, 194.0, 820.0, 0.840703);
			Volume = 1;
			break;
		case AMPSTAGE_6V6:
			TubeStages[0] = new TubeStage(int(TUBE_TABLE_6V6_250k), 0, 250.0, 40.0, 100000, 6531.0, 820.0, 1.130462);
			Volume = 1;
			break;
		case AMPSTAGE_EL34:
			TubeStages[0] = new TubeStage(int(TUBE_TABLE_EL34_250k), 0, 495.0, 10.0, 35000, 256.0, 230.0, 15.743945);
			Volume = 0.15;
			break;
		case AMPSTAGE_EL84:
			TubeStages[0] = new TubeStage(int(TUBE_TABLE_EL84_250k), 0, 350.0, 10.0, 35000, 256.0, 230.0, 8.324292);
			Volume = 0.15;
			break;
		case AMPSTAGE_6L6CG:
			TubeStages[0] = new TubeStage(int(TUBE_TABLE_6L6CG_250k), 0, 450.0, 10.0, 35000, 256.0, 230.0, 11.784307);
			Volume = 0.15;
			break;
	}

	NeedsInit = true;
	needUpdate = false;
}

void AmpStage::init(int samplingFreq)
{	
	if (needUpdate)
		UpdateStageType(newStageType, newNumStages);

	StompBox::init(samplingFreq);
	//samplingFreq *= 8;

	this->samplingFreq = samplingFreq;

	for (int i = 0; i < NumTubeStages; i++)
	{
		TubeStages[i]->init(samplingFreq);
	}
}

void AmpStage::compute(int count, double* input, double* output)
{
	double linearGain = 1;

	switch (gainType)
	{
		case AMPSTAGE_GAIN_LINEAR:
			linearGain = Gain;
			break;

		case AMPSTAGE_GAIN_LOG:
			linearGain = (pow(10, (Gain * 0.3)) - 1.0) / 9.0;
			break;
	}

	double logGain = 20.0 * log10(linearGain * 0.5);

	if (NumTubeStages == 0)
	{
		for (int i = 0; i < count; i++)
		{
			output[i] = input[i];
		}

		return;
	}

	for (int i = 0; i < NumTubeStages; i++)
	{
		TubeStages[i]->SetParameterValue(TUBESTAGE_GAIN, logGain);
	}

	TubeStages[0]->compute(count, input, output);

	for (int stageNum = 1; stageNum < NumTubeStages; stageNum++)
	{
		TubeStages[stageNum]->compute(count, output, output);
	}

	if (Volume != 1.0)
	{
		for (int i = 0; i < count; i++)
		{
			output[i] *= Volume;
		}
	}

	if (needUpdate)  // Do this here so we get init/rampup next processing round
	{
		UpdateStageType(newStageType, newNumStages);
	}
}

