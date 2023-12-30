#include "GuitarConvolver.h"

GuitarConvolver::GuitarConvolver()
{
    Name = "Cabinet";

    NumParameters = CONVOLVER_NUMPARAMETERS;
    Parameters = new StompBoxParameter[NumParameters];

    Parameters[CONVOLVER_IMPULSE].Name = "Impulse";
    Parameters[CONVOLVER_IMPULSE].SourceVariable = &impulseIndex;
    Parameters[CONVOLVER_IMPULSE].ParameterType = PARAMETER_TYPE_ENUM;
    Parameters[CONVOLVER_IMPULSE].EnumValues = &impulseNames;
    Parameters[CONVOLVER_IMPULSE].DefaultValue = -1;

    Parameters[CONVOLVER_DRY].Name = "Dry";
    Parameters[CONVOLVER_DRY].SourceVariable = &dry;
    Parameters[CONVOLVER_DRY].DefaultValue = 0;

    Parameters[CONVOLVER_WET].Name = "Wet";
    Parameters[CONVOLVER_WET].SourceVariable = &wet;
    Parameters[CONVOLVER_WET].DefaultValue = 1;
}

void GuitarConvolver::init(int samplingFreq)
{
    StompBox::init(samplingFreq);

    if (impulseIndex != -1)
        SetImpulse(impulsePaths[(int)impulseIndex]);
}

void GuitarConvolver::IndexImpulses(std::filesystem::path path)
{
    impulseNames.clear();

    struct stat fstat;

    if (stat(path.string().c_str(), &fstat) == 0)
    {
        for (const auto& entry : std::filesystem::directory_iterator(path))
        {
            if (entry.path().filename().extension() == ".wav")
            {
                auto filename = entry.path().filename().replace_extension();

                impulseNames.push_back(filename.string());
                impulsePaths.push_back(entry.path().string());
            }
        }
    }

    Parameters[CONVOLVER_IMPULSE].MaxValue = (int)impulseNames.size() - 1;
}

void GuitarConvolver::SetParameterValue(int id, double value)
{
    StompBox::SetParameterValue(id, value);

    if (id == CONVOLVER_IMPULSE)
    {
        if (!NeedsInit)
            SetImpulse(impulsePaths[(int)impulseIndex]);
    }
}

void GuitarConvolver::SetImpulse(const std::string filename)
{
    struct stat fstat;

    fprintf(stderr, "Loading IR from %s\n", filename.c_str());

    WaveReader *waveReader = new WaveReader(filename, samplingFreq);

    float* waveData = waveReader->GetWaveData();

    if (waveData != nullptr)
    {
        if (impulseData != nullptr)
            delete[] impulseData;

        impulseSamples = waveReader->NumSamples;

        impulseData = new double[impulseSamples];

        float gain = pow(10, -18 * 0.05);  // IRs are usually too loud

        for (int i = 0; i < impulseSamples; i++)
        {
            impulseData[i] = waveData[i] * gain;
        }

        haveNewImpulseData = true;
    }

    delete waveReader;
    waveReader = nullptr;
}

void GuitarConvolver::SetImpulse()
{
    double* newImpulseData[1];

    newImpulseData[0] = impulseData;

    impulseBuffer.Set((const double **)newImpulseData, impulseSamples, 1);

    //convolutionEngine.SetImpulse(&impulseBuffer, 0, 0, 0, 0, 1024);
    convolutionEngine.SetImpulse(&impulseBuffer); // , 0, 32);

    haveImpulseData = true;
}

void GuitarConvolver::compute(int count, double* input, double* output)
{
    if (haveNewImpulseData)
    {
        SetImpulse();

        haveNewImpulseData = false;
    }

    if (!haveImpulseData)
    {
        memcpy(output, input, count * sizeof(double));

        return;
    }

    convolutionEngine.Add((WDL_FFT_REAL**)&input, count, 1);

    int copiedSoFar = 0;

    do
    {
        int nAvail = convolutionEngine.Avail(count - copiedSoFar); //  MIN(mEngine.Avail(nFrames), nFrames);

        WDL_FFT_REAL* convo = convolutionEngine.Get()[0];

        if ((wet < 1) || (dry > 1))
        {
            double* dest = output + copiedSoFar;
            double* src = input + copiedSoFar;
                
            for (int sample = 0; sample < nAvail; sample++)
            {
                dest[sample] = (convo[sample] * wet) + (src[sample] * dry);
            }
        }
        else
        {
            memcpy(output + copiedSoFar, convo, nAvail * sizeof(double));
        }

        convolutionEngine.Advance(nAvail);
        
        copiedSoFar += nAvail;
    }
    while (copiedSoFar < count);
}