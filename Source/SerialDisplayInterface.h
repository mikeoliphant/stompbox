#pragma once

#include <stdint.h>
#include <math.h>
#include "Serial.h"
#include "SerialTFT.h"


#if _WIN32
#define SERIAL_INTERFACE_PORT "\\\\.\\COM4"
#else
#define SERIAL_INTERFACE_PORT "/dev/ttyACM0"
#endif

#define BACKGROUND_COLOR 0

class SerialDisplayInterface
{
public:
	bool Connect();
	void Close();

	bool IsConnected()
	{
		return isConnected;
	}
	void ResetDisplay();
	void SetPresetText(const char* presetName, const char* textColor);
	void HandleStomp(int stomp, bool enabled, const char* pluginName, const char* parameterName, const char *pluginBGColor, const char* pluginFGColor);
	void UpdateTuner(double frequency);
	void ResetTuner();
	void UpdateRecordSeconds(double recordSeconds);

protected:
	Serial* serial;
	SerialTFT* serialTFT;
	bool isConnected = false;

	char const *noteNames[12] =
	{
		"C","C#","D","Eb","E","F","F#","G","Ab","A", "Bb","B"
	};

	double runningFrequency;
	int histSize;
	int screenWidth = 320;
	int screenHeight = 240;
	const int maxHistSize = 10;
};