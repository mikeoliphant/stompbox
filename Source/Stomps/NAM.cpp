#include "NAM.h"

NAM::NAM()
{
    Name = "NAM";

    NumParameters = NAM_NUMPARAMETERS;
    Parameters = new StompBoxParameter[NumParameters];

    Parameters[NAM_MODEL].Name = "Model";
    Parameters[NAM_MODEL].SourceVariable = &modelIndex;
    Parameters[NAM_MODEL].ParameterType = PARAMETER_TYPE_ENUM;
    Parameters[NAM_MODEL].EnumValues = &modelNames;
}

void NAM::init(int samplingFreq)
{
    StompBox::init(samplingFreq);

    SetModel((int)modelIndex);
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

void NAM::SetParameterValue(int id, double value)
{
    StompBox::SetParameterValue(id, value);

    if (id == NAM_MODEL)
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
            stagedModel = get_dsp(filename);
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
        activeModel = std::move(stagedModel);
        stagedModel = nullptr;
    }

    if (activeModel == nullptr)
    {
        memcpy(output, input, count * sizeof(double));

        return;
    }

    activeModel->process(&input, &output, 1, count, 1.0, 1.0, {});
    activeModel->finalize_(count);
}