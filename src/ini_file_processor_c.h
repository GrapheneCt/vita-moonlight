#ifndef INI_FILE_PROCESSOR_C
#define INI_FILE_PROCESSOR_C

#include <psp2/types.h>

/**
 * Errors
 */
#define SCE_INI_FILE_PROCESSOR_ERROR_MODE              0x80840002
#define SCE_INI_FILE_PROCESSOR_ERROR_EOF               0x80840005
#define SCE_INI_FILE_PROCESSOR_ERROR_KEY_NOT_FOUND     0x80840010

#define SCE_INI_FILE_PROCESSOR_PARSE_COMPLETED         0x00840001

typedef void*(*SceIniFileProcessorAllocFunc)(SceSize size);

typedef void(*SceIniFileProcessorFreeFunc)(void* ptr);

typedef struct SceIniFileProcessorMemCallbacks {
	SceIniFileProcessorAllocFunc allocFunc;
	SceIniFileProcessorFreeFunc freeFunc;
} SceIniFileProcessorMemCallbacks;

typedef struct SceIniFileProcessorParam {
	int a1; //size or mode? usually 0, seen: 0x1000, 0x2000
	int a2; //size or mode? usually 0, seen: 0x1000, 0x2000
	SceIniFileProcessorMemCallbacks* pMemCallbacks; //can be NULL
	int a4;
} SceIniFileProcessorParam;

void* sceIniFileProcessorCreateContext(void* context);
void sceIniFileProcessorInitializeParam(SceIniFileProcessorParam* param);
int sceIniFileProcessorCreateInstance(void* context, SceIniFileProcessorParam* param);
int sceIniFileProcessorDeserialize(void* context, const char* ini, SceSize size);
int sceIniFileProcessorOpenFile(void* context, const char* filePath, const char* mode, int unk);
int sceIniFileProcessorCreateFile(void* context, const char* filePath, const char* mode, int unk);
int sceIniFileProcessorSerialize(void* context, const char** ini, SceSize* size);
int sceIniFileProcessorParse(void* context, char* outKeyBuf, char* outValueBuf, SceSize outBufSize);
void* sceIniFileProcessorResetParser(void* context);
int sceIniFileProcessorGetKeyNum(void* context);
int sceIniFileProcessorFindValueByKey(void* context, const char* key, char* outValueBuf, SceSize outValueBufSize, int keyNum);
int sceIniFileProcessorSetValueForKey(void* context, const char* key, const char* value);
int sceIniFileProcessorAddKey(void* context, const char* key, const char* value);
int sceIniFileProcessorDeleteKey(void* context, const char* key);
int sceIniFileProcessorCleanup(void* context);
void sceIniFileProcessorCloseFile(void* context);
void sceIniFileProcessorDestroyInstance(void* context);
void sceIniFileProcessorDestroyInstanceForError(void* context);
void sceIniFileProcessorDestroyContext(void* context);
void sceIniFileProcessorDestroyFreeContext(void* context);

/**
 * Finalize INI file processor library instance
 *
 * @param[in] context - context buffer
 *
 */
inline
void sceIniFileProcessorFinalize(void* context)
{
	sceIniFileProcessorCleanup(context);
	sceIniFileProcessorCloseFile(context);
	sceIniFileProcessorDestroyInstance(context);
	sceIniFileProcessorDestroyContext(context);
}

#endif /* INI_FILE_PROCESSOR_C */
