#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

#include <psp2/kernel/clib.h>
#include <psp2/kernel/iofilemgr.h>

#include "device.h"
#include "debug.h"
#include "ini.h"
#include "ini_file_processor_c.h"

#define DATA_DIR "savedata0:"
#define DEVICE_FILE "device.ini"

#define BOOL(v) sceClibStrcmp((v), "true") == 0
#define write_bool(fd, key, value) fprintf(fd, "%s = %s\n", key, value ? "true" : "false");
#define write_string(fd, key, value) fprintf(fd, "%s = %s\n", key, value)

device_infos_t known_devices = {0};

device_info_t* find_device(const char *name) {
  // TODO: mutex
  for (int i = 0; i < known_devices.count; i++) {
    if (!sceClibStrcmp(name, known_devices.devices[i].name)) {
      return &known_devices.devices[i];
    }
  }
  return NULL;
}

static void device_file_path(char *out, const char *dir) {
  sceClibSnprintf(out, 512, DATA_DIR "/%s/" DEVICE_FILE, dir);
}

device_info_t* append_device(device_info_t *info) {
  if (find_device(info->name)) {
    vita_debug_log("append_device: device %s is already in the list\n", info->name);
    return NULL;
  }
  // FIXME: need mutex
  if (known_devices.size == 0) {
    vita_debug_log("append_device: allocating memory for the initial device list...\n");
    known_devices.devices = malloc(sizeof(device_info_t) * 4);
    if (known_devices.devices == NULL) {
      vita_debug_log("append_device: failed to allocate memory for the initial device list\n");
      return NULL;
    }
    known_devices.size = 4;
  } else if (known_devices.size == known_devices.count) {
    vita_debug_log("append_device: the device list is full, resizing...\n");
    //if (known_devices.size == 64) {
    //  return false;
    //}
    size_t new_size = sizeof(device_info_t) * (known_devices.size * 2);
    device_info_t *tmp = realloc(known_devices.devices, new_size);
    if (tmp == NULL) {
      vita_debug_log("append_device: failed to resize the device list\n");
      return NULL;
    }
    known_devices.devices = tmp;
    known_devices.size *= 2;
  }
  device_info_t *p = &known_devices.devices[known_devices.count];

  sceClibStrncpy(p->name, info->name, 255);
  p->paired = info->paired;
  sceClibStrncpy(p->internal, info->internal, 255);
  sceClibStrncpy(p->external, info->external, 255);
  p->prefer_external = info->prefer_external;
  vita_debug_log("append_device: device %s is added to the list\n", p->name);

  known_devices.count++;
  return p;
}

bool update_device(device_info_t *info) {
  device_info_t *p = find_device(info->name);
  if (p == NULL) {
    return false;
  }

  //sceClibStrncpy(p->name, info->name, 255);
  p->paired = info->paired;
  sceClibStrncpy(p->internal, info->internal, 255);
  sceClibStrncpy(p->external, info->external, 255);
  p->prefer_external = info->prefer_external;
  return true;
}

void load_all_known_devices() {
  struct stat st;
  device_info_t info;

  SceUID dfd = sceIoDopen(DATA_DIR);
  if (dfd < 0) {
    return;
  }
  do {
    SceIoDirent ent = {0};
    if (sceIoDread(dfd, &ent) <= 0) {
      break;
    }
    if (sceClibStrcmp(".", ent.d_name) == 0 || sceClibStrcmp("..", ent.d_name) == 0) {
      continue;
    }
    if (!SCE_STM_ISDIR(ent.d_stat.st_mode)) {
      continue;
    }

    sceClibMemset(&info, 0, sizeof(device_info_t));
    sceClibStrncpy(info.name, ent.d_name, 255);
    if (!load_device_info(&info)) {
      continue;
    }
    append_device(&info);
  } while(true);

  sceIoDclose(dfd);
  return;
}

bool load_device_info(device_info_t *info) {
  char path[512] = {0};
  device_file_path(path, info->name);
  vita_debug_log("load_device_info: reading %s\n", path);

  char iniProcContext[8];
  SceIniFileProcessorParam iniProcInitParam;
  SceIniFileProcessorMemCallbacks iniAllocCb;
  sceIniFileProcessorCreateContext(iniProcContext);

  sceIniFileProcessorInitializeParam(&iniProcInitParam);
  sceIniFileProcessorCreateInstance(iniProcContext, &iniProcInitParam);

  int ret = sceIniFileProcessorOpenFile(iniProcContext, path, "r", 0);
  if (ret < 0) {
	  sceIniFileProcessorDestroyInstanceForError(iniProcContext);
	  sceClibPrintf("sceIniFileProcessorOpenFile() returned 0x%X", ret);
	  return false;
  }

  int int_value;
  char* string_value;
  if (!iniGetValueByKey(iniProcContext, "paired", INI_VALUE_BOOL, 0, &int_value))
	info->paired = int_value;
  iniGetStringByKey(iniProcContext, "internal", &string_value);
	sceClibStrncpy(info->internal, string_value, 255);
	free(string_value);
  iniGetStringByKey(iniProcContext, "external", &string_value);
	sceClibStrncpy(info->external, string_value, 255);
	free(string_value);
  if (!iniGetValueByKey(iniProcContext, "prefer_external", INI_VALUE_BOOL, 0, &int_value))
	info->prefer_external = int_value;

  vita_debug_log("load_device_info: device found:\n", ret);
  vita_debug_log("load_device_info:   info->name = %s\n", info->name);
  vita_debug_log("load_device_info:   info->paired = %s\n", info->paired ? "true" : "false");
  vita_debug_log("load_device_info:   info->internal = %s\n", info->internal);
  vita_debug_log("load_device_info:   info->external = %s\n", info->external);
  vita_debug_log("load_device_info:   info->prefer_external = %s\n", info->prefer_external ? "true" : "false");

  sceIniFileProcessorFinalize(iniProcContext);

  return true;
}

void save_device_info(const device_info_t *info) {

  char path[512] = {0};
  device_file_path(path, info->name);
  vita_debug_log("save_device_info: device file path: %s\n", path);

  char iniProcContext[8];
  SceIniFileProcessorParam iniProcInitParam;
  SceIniFileProcessorMemCallbacks iniAllocCb;
  sceIniFileProcessorCreateContext(iniProcContext);

  sceIniFileProcessorInitializeParam(&iniProcInitParam);
  sceIniFileProcessorCreateInstance(iniProcContext, &iniProcInitParam);

  int ret = sceIniFileProcessorCreateFile(iniProcContext, path, "rw", 0);
  if (ret < 0) {
	  sceIniFileProcessorDestroyInstanceForError(iniProcContext);
	  sceClibPrintf("sceIniFileProcessorCreateFile() returned 0x%X", ret);
	  return;
  }

  vita_debug_log("save_device_info: paired = %s\n", info->paired ? "true" : "false");
  iniCreateSetKey(iniProcContext, "paired", INI_VALUE_BOOL, info->paired);

  vita_debug_log("save_device_info: internal = %s\n", info->internal);
  sceIniFileProcessorAddKey(iniProcContext, "internal", info->internal);

  vita_debug_log("save_device_info: external = %s\n", info->external);
  sceIniFileProcessorAddKey(iniProcContext, "external", info->external);

  vita_debug_log("save_device_info: prefer_external = %s\n", info->prefer_external ? "true" : "false");
  iniCreateSetKey(iniProcContext, "prefer_external", INI_VALUE_BOOL, info->prefer_external);

  sceIniFileProcessorFinalize(iniProcContext);

  vita_debug_log("save_device_info: file closed\n");
}
