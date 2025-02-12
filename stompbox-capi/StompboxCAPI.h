#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct ProcessorState;
struct StringVector;

static const char* GetNextValue(StringVector strEnum, int index);

static const ProcessorState* CreateProcessor(const wchar_t* dataPath, bool dawMode);

static void DeleteProcessor(ProcessorState* state);

static void Init(ProcessorState* state, double sampleRate);

static void StartServer(ProcessorState* state);

static void SetBPM(ProcessorState* state, double bpm);

static bool IsPresetLoading(ProcessorState* state);

static const wchar_t* GetDataPath(ProcessorState* state);

static const StringVector GetAllPlugins(ProcessorState* state);

#ifdef __cplusplus
}
#endif