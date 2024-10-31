#include "NAM.h"

NAM::NAM()
{
    Name = "NAM";
    Description = "Neural Amp Modeler capture playback";

    NumParameters = NAM_NUMPARAMETERS;
    CreateParameters(NumParameters);

    Parameters[NAM_MODEL].Name = "Model";
    Parameters[NAM_MODEL].SourceVariable = &modelIndex;
    Parameters[NAM_MODEL].ParameterType = PARAMETER_TYPE_ENUM;
    Parameters[NAM_MODEL].EnumValues = &modelNames;
    Parameters[NAM_MODEL].DefaultValue = -1;
    Parameters[NAM_MODEL].Description = "Selected NAM model";
}

void NAM::init(int samplingFreq)
{
    StompBox::init(samplingFreq);
}

void NAM::IndexModels(std::filesystem::path path)
{
    modelNames.clear();

    struct stat fstat;

    if (stat(path.string().c_str(), &fstat) == 0)
    {
        for (const auto& entry : std::filesystem::directory_iterator(path))
        {
            if (entry.path().filename().extension() == ".nam")
            {
                auto filename = entry.path().filename().replace_extension();

                modelNames.push_back(filename.string());
                modelPaths.push_back(entry.path().string());
            }
        }
    }

    Parameters[NAM_MODEL].MaxValue = (int)modelNames.size() - 1;
}

void NAM::SetParameterValue(StompBoxParameter* parameter, double value)
{
    StompBox::SetParameterValue(parameter, value);

    if (parameter == &Parameters[NAM_MODEL])
    {
        SetModel((int)modelIndex);
    }
}

void NAM::SetModel(int index)
{
    if (loadedModelIndex != index)
    {
        SetModel(modelPaths[index]);

        loadedModelIndex = (int)index;
    }
}

void NAM::SetModel(const std::string filename)
{
    struct stat fstat;

    fprintf(stderr, "Loading model from %s\n", filename.c_str());

    if (stat(filename.c_str(), &fstat) == 0)
    {
        try
        {
            stagedModel = NeuralAudio::NeuralModel::CreateFromFile(filename);
        }
        catch (std::exception& e)
        {
            stagedModel = nullptr;
        }
    }
}

void NAM::compute(int count, double* input, double* output)
{
    if (stagedModel != nullptr)
    {
        if (activeModel != nullptr)
            delete activeModel;

        activeModel = stagedModel;
        stagedModel = nullptr;
    }

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

    double modelLoudnessAdjustmentGain = pow(10.0, (-18 - activeModel->GetRecommendedOutputDBAdjustment()) / 20.0);

    for (int i = 0; i < count; i++)
    {
        output[i] = (double)namBuffer[i];
    }
}