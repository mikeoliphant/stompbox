#pragma once

#ifdef __cplusplus
extern "C" {
#endif


#ifdef _MSC_VER
#define STOMPBOX_EXTERN extern __declspec(dllexport)
#define PATH_STR wchar_t*
#else
#define STOMPBOX_EXTERN extern
#define PATH_STR char*
#endif

struct ProcessorState;
struct StringVector;

STOMPBOX_EXTERN const char* GetNextValue(StringVector strEnum, int index);

STOMPBOX_EXTERN const ProcessorState* CreateProcessor(const wchar_t* dataPath, bool dawMode);

STOMPBOX_EXTERN void DeleteProcessor(ProcessorState* state);

STOMPBOX_EXTERN void Init(ProcessorState* state, double sampleRate);

STOMPBOX_EXTERN void StartServer(ProcessorState* state);

STOMPBOX_EXTERN void SetBPM(ProcessorState* state, double bpm);

STOMPBOX_EXTERN bool IsPresetLoading(ProcessorState* state);

STOMPBOX_EXTERN const PATH_STR GetDataPath(ProcessorState* state);

STOMPBOX_EXTERN const StringVector GetAllPlugins(ProcessorState* state);

#ifdef __cplusplus
}
#endif