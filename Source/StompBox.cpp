#include "StompBox.h"
#include <algorithm>

StompBoxParameter* StompBox::GetParameter(int id)
{
	return &Parameters[id];
}

StompBox::StompBox()
{
	doubleBuffer[0] = nullptr;
}

void StompBox::SetParameterValue(int index, double value)
{
	*(Parameters[index].SourceVariable) = std::clamp(value, Parameters[index].MinValue, Parameters[index].MaxValue);
}

double StompBox::GetParameterValue(int index)
{
	return *(Parameters[index].SourceVariable);
}

void StompBox::SetBPM(double bpm)
{
	this->bpm = bpm;

	UpdateBPM();
}

void StompBox::UpdateBPM()
{
	for (int i = 0; i < NumParameters; i++)
	{
		if ((Parameters[i].BPMSyncNumerator != 0) && (Parameters[i].BPMSyncDenominator != 0))
		{
			(*Parameters[i].SourceVariable) = ((60.0 / bpm) * ((double)Parameters[i].BPMSyncNumerator / (double)Parameters[i].BPMSyncDenominator)) * 1000;
		}
	}
}

//void StompBox::doComputeDouble(int count, FAUSTFLOAT* input, FAUSTFLOAT* output, EFactor oversample)
//{
//	if (count != bufferSize)
//	{
//		bufferSize = count;
//
//		if (doubleBuffer[0] != nullptr)
//			delete doubleBuffer[0];
//
//		doubleBuffer[0] = new double[bufferSize];
//
//		if (oversample != kNone)
//		{
//			if (overSampler == nullptr)
//			{
//				overSampler = new OverSampler<double>(oversample, true, 1);
//				overSampler->Reset(bufferSize);
//			}
//		}
//	}
//
//	for (int i = 0; i < count; i++)
//	{
//		doubleBuffer[0][i] = input[i];
//	}
//
//	double* resultBuffer;
//
//	if (oversample != kNone)
//	{
//		overSampler->ProcessBlock(doubleBuffer, doubleBuffer, bufferSize, 1, [this](double** inputs, double** outputs, int count2)
//		{
//			computeDouble(count2, inputs[0], outputs[0]);
//		});
//
//		resultBuffer = doubleBuffer[0];
//	}
//	else
//	{
//		computeDouble(count, doubleBuffer[0], doubleBuffer[0]);
//
//		resultBuffer = doubleBuffer[0];
//	}
//
//	for (int i = 0; i < count; i++)
//	{
//		output[i] = (float)resultBuffer[i];
//	}
//}
