#include "GuitarConvolver.h"

GuitarConvolver::GuitarConvolver()
{
    Name = "Cabinet";

    newImpulseData[0] = nullptr;

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
}

void GuitarConvolver::IndexImpulses(std::filesystem::path path)
{
    impulseNames.clear();

    struct stat fstat;

    if (stat(path.string().c_str(), &fstat) == 0)
    {
        for (const auto& entry : std::filesystem::directory_iterator(path))
        {
            if (entry.path().filename().extension() == ".cir")
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
        SetImpulse(impulsePaths[(int)impulseIndex]);
    }
}

void GuitarConvolver::SetImpulse(const std::string filename)
{
    struct stat fstat;

    fprintf(stderr, "Loading IR from %s\n", filename.c_str());

    if (stat(filename.c_str(), &fstat) == 0)
    {       
        std::ifstream impulseFile(filename, std::ios::in | std::ios::binary);

        if (!impulseFile.bad())
        {
            while (impulseFile)
            {
                unsigned int numProperties = 0;

                impulseFile.read((char*)&numProperties, sizeof(numProperties));

                unsigned int impulseSampleRate = 0;

                for (int propNum = 0; propNum < numProperties; propNum++)
                {
                    unsigned int property = 0;
                    unsigned int propertyValue = 0;

                    impulseFile.read((char*)&property, sizeof(property));
                    impulseFile.read((char*)&propertyValue, sizeof(propertyValue));

                    switch (property)
                    {
                        case IMPUlSE_PARAM_SAMPLERATE:
                            impulseSampleRate = propertyValue;
                            break;
                        default:
                            break;
                    }
                }

                unsigned int irSize = 0;

                impulseFile.read((char*)&irSize, sizeof(irSize));

                if (impulseSampleRate == (unsigned int)samplingFreq)
                {
                    char* buf = new char[irSize];

                    impulseFile.read(buf, irSize);

                    short* intData = (short*)buf;

                    newImpulseSamples = irSize / 2;

                    double* doubleData = new double[newImpulseSamples];

                    double max = (double)SHRT_MAX;

                    for (int i = 0; i < newImpulseSamples; i++)
                    {
                        doubleData[i] = (double)intData[i] / max;
                    }

                    newImpulseData[0] = doubleData;

                    delete[] buf;

                    break;
                }
                else
                {
                    impulseFile.ignore(irSize);
                }
            }

            impulseFile.close();
        }
    }
}

void GuitarConvolver::SetImpulse(double **impulseData, int numSamples)
{
    float gain = pow(10, -18 * 0.05);  // IRs are usually too loud

    for (int i = 0; i < numSamples; i++)
    {
        impulseData[0][i] = impulseData[0][i] * gain;
    }

    impulseBuffer.Set((const double **)impulseData, numSamples, 1);

    //convolutionEngine.SetImpulse(&impulseBuffer, 0, 0, 0, 0, 1024);
    convolutionEngine.SetImpulse(&impulseBuffer); // , 0, 32);

    haveImpulseData = true;
}


void GuitarConvolver::compute(int count, double* input, double* output)
{
    if (newImpulseData[0] != nullptr)
    {
        SetImpulse(newImpulseData, newImpulseSamples);

        delete[] newImpulseData[0];

        newImpulseData[0] = nullptr;
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

        if ((wet < 1) || (dry < 1))
        {
            for (int sample = copiedSoFar, wetSamp = 0; wetSamp < nAvail; sample++, wetSamp++)
            {
                output[sample] = (convo[wetSamp] * wet) + (input[sample] * dry);
            }
        }
        else
        {
            memcpy(output + copiedSoFar, convo, nAvail * sizeof(double));
        }

        convolutionEngine.Advance(nAvail);
        
        copiedSoFar += nAvail;
    } while (copiedSoFar < count);
}