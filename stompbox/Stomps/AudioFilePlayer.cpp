#include "AudioFilePlayer.h"

AudioFilePlayer::AudioFilePlayer()
{
	Name = "AudioFilePlayer";

	NumParameters = AUDIOFILEPLAYER_NUMPARAMETERS;
	CreateParameters(NumParameters);

	Parameters[AUDIOFILEPLAYER_FILE].Name = "File";
	Parameters[AUDIOFILEPLAYER_FILE].SourceVariable = &fileIndex;
	Parameters[AUDIOFILEPLAYER_FILE].ParameterType = PARAMETER_TYPE_FILE;
	Parameters[AUDIOFILEPLAYER_FILE].FilePath = "Music";
	Parameters[AUDIOFILEPLAYER_FILE].EnumValues = &fileNames;

	Parameters[AUDIOFILEPLAYER_LEVEL].Name = "Level";
	Parameters[AUDIOFILEPLAYER_LEVEL].SourceVariable = &level;
	Parameters[AUDIOFILEPLAYER_LEVEL].MaxValue = 1.5;
	Parameters[AUDIOFILEPLAYER_LEVEL].DefaultValue = level;

	Parameters[AUDIOFILEPLAYER_PLAYING].Name = "Playing";
	Parameters[AUDIOFILEPLAYER_PLAYING].ParameterType = PARAMETER_TYPE_BOOL;
	Parameters[AUDIOFILEPLAYER_PLAYING].SourceVariable = &playing;
	Parameters[AUDIOFILEPLAYER_PLAYING].DefaultValue = playing;
	Parameters[AUDIOFILEPLAYER_PLAYING].SuppressSave = true;

	Parameters[AUDIOFILEPLAYER_POSITION].Name = "Position";
	Parameters[AUDIOFILEPLAYER_POSITION].SourceVariable = &position;
	Parameters[AUDIOFILEPLAYER_POSITION].DefaultValue = position;
	Parameters[AUDIOFILEPLAYER_POSITION].SuppressSave = true;
}

void AudioFilePlayer::IndexFiles(std::filesystem::path path)
{
	fileNames.clear();
	filePaths.clear();

	struct stat fstat;

	if (stat(path.string().c_str(), &fstat) == 0)
	{
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			if (entry.path().filename().extension() == ".wav")
			{
				filePaths.push_back(entry.path());
			}
		}
	}

	std::sort(filePaths.begin(), filePaths.end());

	for (const auto& entry : filePaths)
	{
		auto filename = entry.filename().replace_extension();

		fileNames.push_back(filename.string());
	}

	Parameters[AUDIOFILEPLAYER_FILE].MaxValue = (int)fileNames.size() - 1;
}

void AudioFilePlayer::SetParameterValue(StompBoxParameter* param, double value)
{
	StompBox::SetParameterValue(param, value);

	if (param == &Parameters[AUDIOFILEPLAYER_FILE])
	{
		needWaveLoad = true;
	}
	else if (param == &Parameters[AUDIOFILEPLAYER_POSITION])
	{
		if (waveReader)
		{
			readPosition = (int)(waveReader->NumSamples * position);
		}
	}
}

void AudioFilePlayer::HandleCommand(std::vector<std::string> commandWords)
{
	if (commandWords.size() > 2)
	{
		if (commandWords[2] == "ArmRecord")
		{
			if (playing == 1)
			{
				recordArmed = true;
				haveRecording = false;
				recording = false;

				fprintf(stderr, "AudioFilePlayer record armed\n");
			}
		}
	}
}

void AudioFilePlayer::SetFile(const std::string filename)
{
	recordArmed = false;
	recording = false;
	haveRecording = false;

	if (waveReader)
	{
		delete waveReader;
		waveReader = nullptr;

		delete recordBuffer;
		recordBuffer = nullptr;
	}

	waveReader = new WaveReader(filename, samplingFreq);

	if (waveReader->NumSamples == 0)
	{
		delete waveReader;
		waveReader = nullptr;
	}
	else
	{
		waveBuffer = waveReader->GetWaveData();
		readPosition = 0;

		recordBuffer = new double[waveReader->NumSamples];
	}
}

void AudioFilePlayer::init(int samplingFreq)
{
	StompBox::init(samplingFreq);
}

void AudioFilePlayer::compute(int count, double* input, double* output)
{
	if (needWaveLoad)
	{
		needWaveLoad = false;

		if (filePaths.size() >= (int)fileIndex)
		{
			SetFile(filePaths[(int)fileIndex].string());
		}
	}

	if ((playing == 1) && (waveReader))
	{
		double linearLevel = (pow(10, level) - 1) / 9;

		unsigned int leftToRead = count;
		unsigned int outputPos = 0;

		double* recordInput = input;

		while (leftToRead > 0)
		{			
			int toRead = std::min(leftToRead, waveReader->NumSamples - readPosition);

			if (recording)
			{
				memcpy(recordBuffer + readPosition, recordInput, toRead * sizeof(double));

				recordInput += toRead;
			}

			float* readPtr = waveBuffer + (readPosition * waveReader->NumChannels);

			for (int i = 0; i < toRead; i++)
			{
				double outputVal = 0;

				for (int channel = 0; channel < waveReader->NumChannels; channel++)
				{
					outputVal += *readPtr;

					readPtr++;
				}

				output[outputPos] = input[outputPos] + (outputVal * linearLevel);

				if (haveRecording)
				{
					output[outputPos] += recordBuffer[readPosition + i];
				}

				outputPos++;
			}

			leftToRead -= toRead;
			readPosition += toRead;

			if (leftToRead > 0)
			{
				// Wrap back to beginning of file

				readPosition = 0;

				if (recording)
				{
					// If we are recording, stop

					recordArmed = recording = false;
					haveRecording = true;

					fprintf(stderr, "AudioFilePlayer record finished\n");
				}
				else
				{
					// If recording is armed, turn it on when the file loops around
					if (recordArmed)
					{
						recording = true;

						fprintf(stderr, "AudioFilePlayer record started\n");
					}
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < count; i++)
		{
			output[i] = input[i];
		}
	}
}