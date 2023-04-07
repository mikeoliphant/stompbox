#pragma once

#include <limits.h>
#include <string.h>
#include <string>
#include <fstream>

typedef struct WAVE_HEADER
{
	/* RIFF Chunk Descriptor */
	uint8_t         RIFF[4];        // RIFF Header Magic header
	uint32_t        ChunkSize;      // RIFF Chunk Size
	uint8_t         WAVE[4];        // WAVE Header
	/* "fmt" sub-chunk */
	uint8_t         fmt[4];         // FMT header
	uint32_t        Subchunk1Size;  // Size of the fmt chunk
	uint16_t        AudioFormat;    // Audio format 1=PCM,6=mulaw,7=alaw,     257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
	uint16_t        NumOfChan;      // Number of channels 1=Mono 2=Sterio
	uint32_t        SamplesPerSec;  // Sampling Frequency in Hz
	uint32_t        bytesPerSec;    // bytes per second
	uint16_t        blockAlign;     // 2=16-bit mono, 4=16-bit stereo
	uint16_t        bitsPerSample;  // Number of bits per sample
} WaveHeader;

class WaveReader
{
public:
	WaveReader(std::string filename)
	{
		memset(&waveHeader, 0, sizeof(waveHeader));

		waveFile.open(filename, std::ios::in | std::ios::binary);

		if (waveFile)
		{
			if (waveFile.read((char*)&waveHeader, sizeof(waveHeader)))
			{
				int fmtRemainder = waveHeader.Subchunk1Size - 16;

				if (fmtRemainder > 0)
				{
					waveFile.ignore(fmtRemainder);
				}

				if (waveFile)
				{
					char header[4];

					if (waveFile.read(header, 4))
					{
						while (strncmp(header, "data", 4) != 0)
						{
							uint32_t chunkSize;

							waveFile.read((char*)&chunkSize, sizeof(chunkSize));

							waveFile.ignore(chunkSize);

							waveFile.read(header, 4);

							if (!waveFile)
							{
								break;
							}
						}

						if (waveFile)
						{
							uint32_t dataSize;

							if (waveFile.read((char*)&dataSize, sizeof(dataSize)))
							{
								waveData = new char[dataSize];

								if (waveFile.read(waveData, dataSize))
								{
									NumChannels = waveHeader.NumOfChan;
									NumSamples = dataSize / ((waveHeader.bitsPerSample / 8) * NumChannels);
								}
								else
								{
									delete[] waveData;
									waveData = nullptr;
								}
							}
						}
					}
				}
			}
		}

		waveFile.close();
	};

	~WaveReader()
	{
		if (waveData != nullptr)
		{
			delete[] waveData;
		}
	};

	char* GetWaveData()
	{
		return waveData;
	}

	uint16_t NumChannels = 0;
	uint32_t NumSamples = 0;

private:
	std::ifstream waveFile;
	WaveHeader waveHeader;
	char* waveData = nullptr;
};

class WaveWriter
{
public:
	WaveWriter(int sampleRate, int recordSeconds)
	{
		this->sampleRate = sampleRate;
		this->recordSeconds = recordSeconds;

		waveDataSize = (uint32_t)recordSeconds * (uint32_t)sampleRate * 3;

		waveData = new char[waveDataSize];

		ResetRecording();
	};

	~WaveWriter()
	{
		delete[] waveData;
	}

	float GetRecordSeconds()
	{
		if (recordIsFull)
			return recordSeconds;

		return ((float)recordPos / (float)waveDataSize) * recordSeconds;
	}

	void ResetRecording()
	{
		needReset = true;
	}

	void AddSamples(double* samples, int numSamples)
	{
		if (needReset)
		{
			recordPos = 0;
			recordIsFull = false;

			needReset = false;
		}

		long pos = recordPos;	// Make a copy in case recordPos gets changed while we're writing

		for (int i = 0; i < numSamples; i++)
		{
			int32_t intSample = samples[i] * INT32_MAX;

			waveData[pos++] = ((intSample >> 8) & 0xff);
			waveData[pos++] = ((intSample >> 16) & 0xff);
			waveData[pos++] = ((intSample >> 24) & 0xff);

			if (pos == waveDataSize)
			{
				recordIsFull = true;

				pos = 0;
			}
		}

		recordPos = pos;
	};

	void WriteToFile(std::string filename)
	{
		WaveHeader waveHeader;

		uint32_t recordDataSize;

		if (recordIsFull)
			recordDataSize = waveDataSize;
		else
		{
			recordDataSize = recordPos;
		}

		fprintf(stderr, "AudioFileRecorder saving %d bytes\n", recordDataSize);

		memcpy((char*)waveHeader.fmt, "fmt ", 4);
		waveHeader.Subchunk1Size = 16;
		waveHeader.AudioFormat = 1;
		waveHeader.NumOfChan = 1;
		waveHeader.SamplesPerSec = sampleRate;
		waveHeader.blockAlign = 3;
		waveHeader.bytesPerSec = sampleRate * waveHeader.blockAlign;
		waveHeader.bitsPerSample = 3 * 8;


		memcpy((char *)waveHeader.RIFF, "RIFF", 4);
		memcpy((char*)waveHeader.WAVE, "WAVE", 4);

		waveHeader.ChunkSize = 36 + recordDataSize;

		std::ofstream waveFile;

		waveFile.open(filename, std::ios::trunc | std::ios::binary);

		waveFile.write((char *)&waveHeader, sizeof(waveHeader));

		waveFile.write("data", 4);
		waveFile.write((char*)&recordDataSize, 4);

		if (recordIsFull)
		{
			waveFile.write(waveData + recordPos, (std::streamsize)waveDataSize - recordPos);
		}

		waveFile.write(waveData, recordPos);

		waveFile.close();

		fprintf(stderr, "AudioFileRecorder save finished\n");
	}

private:
	int sampleRate;
	int recordSeconds;
	char* waveData = nullptr;
	uint32_t waveDataSize;
	long recordPos;
	bool recordIsFull = false;
	bool needReset = false;
};