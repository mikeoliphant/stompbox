#pragma once

#include "GuitarSimComponent.h"

enum
{
	TREMOLO_SPEED,
	TREMOLO_DEPTH,
	TREMOLO_SHAPE,
	TREMOLO_NUMPARAMETERS
};

class Tremolo : public GuitarSimComponent
{
private:
	double speed;
	double depth;
	double shape;
	double time;
	double tfrac;
public:
	Tremolo();
	~Tremolo() {}

	virtual void init(int samplingFreq);
	virtual void compute(int count, double* input, double* output);
};
