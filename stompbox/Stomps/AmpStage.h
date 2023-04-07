#pragma once

#include "TubeStage.h"
#include "StompBox.h"

enum
{
	AMPSTAGE_12AX7,
	AMPSTAGE_JJECC83S,
	AMPSTAGE_6V6,
	AMPSTAGE_EL34,
	AMPSTAGE_EL84,
	AMPSTAGE_6L6CG,
	AMPSTAGE_NUMTYPES
};

enum
{
	AMPSTAGE_GAIN,
	AMPSTAGE_STAGETYPE,
	AMPSTAGE_NUMSTAGES,
	AMPSTAGE_NUMPARAMETERS
};

enum
{
	AMPSTAGE_GAIN_LINEAR,
	AMPSTAGE_GAIN_LOG
};

class AmpStage : public StompBox
{
private:
	int samplingFreq;
	int newStageType = 0;
	int newNumStages = 0;

public:
	double StageType = 0;
	double NumTubeStages = 0;
	TubeStage* TubeStages[3];
	double Gain;
	int gainType = AMPSTAGE_GAIN_LINEAR;
	double Volume = 1.0;
	bool needUpdate = false;
	std::vector<int>* stageMap = nullptr;

	AmpStage();
	virtual ~AmpStage() {}
	virtual double GetParameterValue(int index);
	virtual void SetParameterValue(int id, double value);
	void UpdateStageType(int stageType, int numTubeStages);
	virtual void init(int samplingFreq);
	virtual void compute(int count, double* input, double* output);
};