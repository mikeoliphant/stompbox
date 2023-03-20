#include <math.h>
#include <algorithm>
#include "PitchDetector.h"

PitchDetector::PitchDetector(int bufferSize)
{
	this->bufferSize = bufferSize;
	this->bufferSizeTimesTwo = bufferSize * 2;

	Name = "PitchDetector";

	NumParameters = 0;
	Parameters = new StompBoxParameter[0];

	pitchMPM = new PitchMPM(bufferSize);
	buffer = new float[bufferSizeTimesTwo];

	bufIndex = 0;

	OutputValue = &currentPitch;
}

float PitchDetector::GetCurrentPitch()
{
	return currentPitch;
}

void PitchDetector::init(int samplingFreq)
{
	StompBox::init(samplingFreq);

	pitchMPM->setSampleRate(samplingFreq);
}

void PitchDetector::compute(int count, double* input0, double* output0)
{
	//for (int i = 0; i < count; i++)
	//{
	//	output0[i] = input0[i];
	//}

	//return;

	for (int i = 0; i < count; i++)
	{
		float val = (float)input0[i];

		buffer[(bufIndex + bufferSize) % bufferSizeTimesTwo] = val;
		buffer[bufIndex++] = val;

		output0[i] = input0[i];

		if (bufIndex == bufferSize)
			bufIndex = 0;
	}

	int offset = bufIndex - bufferSize;
	
	if (offset < 0)
		offset += bufferSize;

	currentPitch = pitchMPM->getPitch(buffer + offset);


	//if (bufIndex == bufferSize)
	//{
	//	currentPitch = pitchMPM->getPitch(buffer);

		//if (currentPitch > 0)
		//{
		//	float note = (log(currentPitch / 440.0) / log(2) * 12 + 69) + 12;

		//	note = round(note);

		//	if (!noteOn || (lastNote != note))
		//	{
		//		if (noteOn)
		//			MidiCallback(0x80, lastNote, 0);

		//		MidiCallback(0x90, note, 127);

		//		noteOn = true;
		//		lastNote = note;
		//	}
		//}
		//else
		//{
		//	if (noteOn)
		//	{
		//		MidiCallback(0x80, lastNote, 0);

		//		noteOn = false;
		//	}
		//}

		//float pitch = pitchMPM->getPitch(buffer);

		//if (pitch > 0)
		//{
		//	int factor = std::min(10, numDetects);

		//	currentPitch = ((factor * currentPitch) + pitch) / (double)(factor + 1);

		//	numDetects++;
		//}
		//else
		//{
		//	numFailures++;

		//	if (numFailures > 10)
		//	{
		//		numDetects = 0;
		//		currentPitch = -1;
		//	}
		//}

	//	bufIndex = 0;
	//}
}