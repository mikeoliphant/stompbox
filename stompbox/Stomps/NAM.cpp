#include "NAM.h"

NAM::NAM(const std::string folderName, const std::vector<std::string> fileExtensions, const std::filesystem::path& basePath) :
    fileType(folderName, fileExtensions, basePath),
    namLoader(fileType)
{
    Name = "NAM";
    Description = "Neural Amp Modeler capture playback";

    NumParameters = NAM_NUMPARAMETERS;
    CreateParameters(NumParameters);

    Parameters[NAM_MODEL].Name = "Model";
    Parameters[NAM_MODEL].SourceVariable = &modelIndex;
    Parameters[NAM_MODEL].ParameterType = PARAMETER_TYPE_FILE;
    Parameters[NAM_MODEL].FilePath = "NAM";
    Parameters[NAM_MODEL].EnumValues = &fileType.GetFileNames();
    Parameters[NAM_MODEL].DefaultValue = -1;
    Parameters[NAM_MODEL].Description = "Selected NAM model";
    Parameters[NAM_MODEL].MinValue = -1;
    Parameters[NAM_MODEL].MaxValue = (int)(fileType.GetFileNames().size()) - 1;
}

void NAM::init(int samplingFreq)
{
    StompBox::init(samplingFreq);
}

void NAM::SetParameterValue(StompBoxParameter* parameter, double value)
{
    StompBox::SetParameterValue(parameter, value);

    if (parameter == &Parameters[NAM_MODEL])
    {
        namLoader.LoadIndex((int)modelIndex);
    }
}

void NAM::compute(int count, double* input, double* output)
{
    auto activeModel = namLoader.GetCurrentData();

    if (activeModel == nullptr)
    {
        memcpy(output, input, count * sizeof(double));

        return;
    }

    if (namBuffer.size() != count)
    {
        namBuffer.resize(count);
    }

    for (int i = 0; i < count; i++)
    {
        namBuffer[i] = (float)input[i];
    }

    activeModel->Process(namBuffer.data(), namBuffer.data(), count);

    float adjustDB = activeModel->GetRecommendedOutputDBAdjustment();

    double modelLoudnessAdjustmentGain = pow(10.0, adjustDB / 20.0);

    for (int i = 0; i < count; i++)
    {
        output[i] = (double)namBuffer[i] * modelLoudnessAdjustmentGain;
    }
}