#include "StompBox.h"
#include <algorithm>

StompBox::StompBox()
{
	doubleBuffer[0] = nullptr;
}

StompBox::~StompBox()
{
	if (Parameters != nullptr)
	{
		delete[] Parameters;
	}
}

void StompBox::CreateParameters(int numParameters)
{
	Parameters = new StompBoxParameter[numParameters];

	for (int i = 0; i < NumParameters; i++)
	{
		Parameters[i].Stomp = this;
	}
}

StompBoxParameter* StompBox::GetParameter(int id)
{
	return &Parameters[id];
}

StompBoxParameter* StompBox::GetParameter(std::string name)
{
	for (int i = 0; i < NumParameters; i++)
	{
		if (Parameters[i].Name == name)
			return &Parameters[i];
	}

	StompBoxParameter* parameter = nullptr;

	if (InputGain != nullptr)
		parameter = InputGain->GetParameter(name);

	if ((parameter == nullptr) && (OutputVolume != nullptr))
		parameter = OutputVolume->GetParameter(name);

	return parameter;
}

void StompBox::init(int newSamplingFreq)
{
	samplingFreq = newSamplingFreq;
}


void StompBox::SetParameterValue(int id, double value)
{
	SetParameterValue(&(Parameters[id]), value);
}

void StompBox::SetParameterValue(StompBoxParameter *param, double value)
{
	*(param->SourceVariable) = std::clamp(value, param->MinValue, param->MaxValue);
}

double StompBox::GetParameterValue(int id)
{
	return GetParameterValue(&(Parameters[id]));
}

double StompBox::GetParameterValue(StompBoxParameter* param)
{
	return *(param->SourceVariable);
}

void StompBox::SetBPM(double newBpm)
{
	bpm = newBpm;

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
