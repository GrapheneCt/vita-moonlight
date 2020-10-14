/*
 * This file is part of Moonlight Embedded.
 *
 * Copyright (C) 2015, 2016 Iwan Timmer
 *
 * Moonlight is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Moonlight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Moonlight; if not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include "audio.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include "ini.h"
#include "ini_file_processor_c.h"
#include "input/vita.h"

#include <psp2/kernel/clib.h>
#include <psp2/kernel/sysmem.h>

#define MOONLIGHT_PATH "/moonlight"
#define USER_PATHS "."
#define DEFAULT_CONFIG_DIR "/.config"
#define DEFAULT_CACHE_DIR "/.cache"

CONFIGURATION config;
char *config_path;

bool inputAdded = false;
static bool mapped = true;
const char* audio_device = NULL;

bool config_file_parse(char* filename, PCONFIGURATION config) {
  char iniProcContext[8];
  SceIniFileProcessorParam iniProcInitParam;
  SceIniFileProcessorMemCallbacks iniAllocCb;
  sceIniFileProcessorCreateContext(iniProcContext);

  sceIniFileProcessorInitializeParam(&iniProcInitParam);
  sceIniFileProcessorCreateInstance(iniProcContext, &iniProcInitParam);

  int ret = sceIniFileProcessorOpenFile(iniProcContext, filename, "r", 0);
  if (ret < 0) {
	sceIniFileProcessorDestroyInstanceForError(iniProcContext);
	sceClibPrintf("sceIniFileProcessorOpenFile() returned 0x%X", ret);
	return 0;
  }

  int int_value;
  if (!iniGetValueByKey(iniProcContext, "top", INI_VALUE_INT, 0, &int_value))
	config->back_deadzone.top = int_value;
  if (!iniGetValueByKey(iniProcContext, "right", INI_VALUE_INT, 0, &int_value))
	config->back_deadzone.right = int_value;
  if (!iniGetValueByKey(iniProcContext, "bottom", INI_VALUE_INT, 0, &int_value))
	config->back_deadzone.bottom = int_value;
  if (!iniGetValueByKey(iniProcContext, "left", INI_VALUE_INT, 0, &int_value))
	config->back_deadzone.left = int_value;

  if (!iniGetValueByKey(iniProcContext, "nw", INI_VALUE_INT, 16, &int_value))
	config->special_keys.nw = int_value;
  if (!iniGetValueByKey(iniProcContext, "ne", INI_VALUE_INT, 16, &int_value))
	config->special_keys.ne = int_value;
  if (!iniGetValueByKey(iniProcContext, "sw", INI_VALUE_INT, 16, &int_value))
	config->special_keys.sw = int_value;
  if (!iniGetValueByKey(iniProcContext, "se", INI_VALUE_INT, 16, &int_value))
	config->special_keys.se = int_value;
  if (!iniGetValueByKey(iniProcContext, "offset", INI_VALUE_INT, 0, &int_value))
	config->special_keys.offset = int_value;
  if (!iniGetValueByKey(iniProcContext, "size", INI_VALUE_INT, 0, &int_value))
	config->special_keys.size = int_value;

  iniGetStringByKey(iniProcContext, "address", &config->address);
  if (!iniGetValueByKey(iniProcContext, "width", INI_VALUE_INT, 0, &int_value))
	config->stream.width = int_value;
  if (!iniGetValueByKey(iniProcContext, "height", INI_VALUE_INT, 0, &int_value))
	config->stream.height = int_value;
  if (!iniGetValueByKey(iniProcContext, "fps", INI_VALUE_INT, 0, &int_value))
	config->stream.fps = int_value;
  if (!iniGetValueByKey(iniProcContext, "bitrate", INI_VALUE_INT, 0, &int_value))
	config->stream.bitrate = int_value;
  if (!iniGetValueByKey(iniProcContext, "sops", INI_VALUE_BOOL, 0, &int_value))
	config->sops = int_value;
  if (!iniGetValueByKey(iniProcContext, "localaudio", INI_VALUE_BOOL, 0, &int_value))
	config->localaudio = int_value;
  if (!iniGetValueByKey(iniProcContext, "enable_frame_pacer", INI_VALUE_BOOL, 0, &int_value))
	config->enable_frame_pacer = int_value;
  if (!iniGetValueByKey(iniProcContext, "enable_bgm_mode", INI_VALUE_BOOL, 0, &int_value))
	  config->enable_bgm_mode = int_value;
  if (!iniGetValueByKey(iniProcContext, "center_region_only", INI_VALUE_BOOL, 0, &int_value))
	config->center_region_only = int_value;
  if (!iniGetValueByKey(iniProcContext, "disable_powersave", INI_VALUE_BOOL, 0, &int_value))
	config->disable_powersave = int_value;
  if (!iniGetValueByKey(iniProcContext, "disable_dimming", INI_VALUE_BOOL, 0, &int_value))
	  config->disable_dimming = int_value;
  if (!iniGetValueByKey(iniProcContext, "jp_layout", INI_VALUE_BOOL, 0, &int_value))
	config->jp_layout = int_value;
  if (!iniGetValueByKey(iniProcContext, "show_fps", INI_VALUE_BOOL, 0, &int_value))
	config->show_fps = int_value;
  if (!iniGetValueByKey(iniProcContext, "save_debug_log", INI_VALUE_BOOL, 0, &int_value))
	config->save_debug_log = int_value;
  iniGetStringByKey(iniProcContext, "mapping", &config->mapping);
  if (!iniGetValueByKey(iniProcContext, "mouse_acceleration", INI_VALUE_INT, 0, &int_value))
	config->mouse_acceleration = int_value;
  if (!iniGetValueByKey(iniProcContext, "enable_ref_frame_invalidation", INI_VALUE_BOOL, 0, &int_value))
	config->enable_ref_frame_invalidation = int_value;
  if (!iniGetValueByKey(iniProcContext, "enable_remote_stream_optimization", INI_VALUE_BOOL, 0, &int_value))
	config->stream.streamingRemotely = int_value;

  sceIniFileProcessorFinalize(iniProcContext);

  return 1;
}

void config_save(const char* filename, PCONFIGURATION config) {
  char iniProcContext[8];
  SceIniFileProcessorParam iniProcInitParam;
  SceIniFileProcessorMemCallbacks iniAllocCb;
  sceIniFileProcessorCreateContext(iniProcContext);

  sceIniFileProcessorInitializeParam(&iniProcInitParam);
  sceIniFileProcessorCreateInstance(iniProcContext, &iniProcInitParam);

  int ret = sceIniFileProcessorCreateFile(iniProcContext, filename, "rw", 0);
  if (ret < 0) {
	sceIniFileProcessorDestroyInstanceForError(iniProcContext);
	sceClibPrintf("sceIniFileProcessorCreateFile() returned 0x%X", ret);
	return;
  }

  if (config->address)
    sceIniFileProcessorAddKey(iniProcContext, "address", config->address);

  if (config->mapping)
	sceIniFileProcessorAddKey(iniProcContext, "mapping", config->mapping);

  if (config->stream.width != 1280)
	iniCreateSetKey(iniProcContext, "width", INI_VALUE_INT, config->stream.width);
  if (config->stream.height != 720)
	iniCreateSetKey(iniProcContext, "height", INI_VALUE_INT, config->stream.height);
  if (config->stream.fps != 60)
	iniCreateSetKey(iniProcContext, "fps", INI_VALUE_INT, config->stream.fps);
  if (config->stream.bitrate != -1)
	iniCreateSetKey(iniProcContext, "bitrate", INI_VALUE_INT, config->stream.bitrate);
  if (config->stream.packetSize != 1024)
	iniCreateSetKey(iniProcContext, "packetsize", INI_VALUE_INT, config->stream.packetSize);
  if (!config->sops)
	iniCreateSetKey(iniProcContext, "sops", INI_VALUE_BOOL, config->sops);
  if (config->localaudio)
	iniCreateSetKey(iniProcContext, "localaudio", INI_VALUE_BOOL, config->localaudio);

  if (sceClibStrcmp(config->app, "Steam") != 0)
	sceIniFileProcessorAddKey(iniProcContext, "app", config->app);

  iniCreateSetKey(iniProcContext, "enable_frame_pacer", INI_VALUE_BOOL, config->enable_frame_pacer);
  iniCreateSetKey(iniProcContext, "enable_bgm_mode", INI_VALUE_BOOL, config->enable_bgm_mode);
  iniCreateSetKey(iniProcContext, "center_region_only", INI_VALUE_BOOL, config->center_region_only);
  iniCreateSetKey(iniProcContext, "disable_powersave", INI_VALUE_BOOL, config->disable_powersave);
  iniCreateSetKey(iniProcContext, "disable_dimming", INI_VALUE_BOOL, config->disable_dimming);
  iniCreateSetKey(iniProcContext, "jp_layout", INI_VALUE_BOOL, config->jp_layout);
  iniCreateSetKey(iniProcContext, "show_fps", INI_VALUE_BOOL, config->show_fps);
  iniCreateSetKey(iniProcContext, "save_debug_log", INI_VALUE_BOOL, config->save_debug_log);

  iniCreateSetKey(iniProcContext, "mouse_acceleration", INI_VALUE_INT, config->mouse_acceleration);
  iniCreateSetKey(iniProcContext, "enable_ref_frame_invalidation", INI_VALUE_BOOL, config->enable_ref_frame_invalidation);
  iniCreateSetKey(iniProcContext, "enable_remote_stream_optimization", INI_VALUE_BOOL, config->stream.streamingRemotely);

  iniCreateSetKey(iniProcContext, "top", INI_VALUE_INT, config->back_deadzone.top);
  iniCreateSetKey(iniProcContext, "right", INI_VALUE_INT, config->back_deadzone.right);
  iniCreateSetKey(iniProcContext, "bottom", INI_VALUE_INT, config->back_deadzone.bottom);
  iniCreateSetKey(iniProcContext, "left", INI_VALUE_INT, config->back_deadzone.left);

  iniCreateSetKey(iniProcContext, "nw", INI_VALUE_HEX, config->special_keys.nw);
  iniCreateSetKey(iniProcContext, "ne", INI_VALUE_HEX, config->special_keys.ne);
  iniCreateSetKey(iniProcContext, "sw", INI_VALUE_HEX, config->special_keys.sw);
  iniCreateSetKey(iniProcContext, "se", INI_VALUE_HEX, config->special_keys.se);
  iniCreateSetKey(iniProcContext, "offset", INI_VALUE_INT, config->special_keys.offset);
  iniCreateSetKey(iniProcContext, "size", INI_VALUE_INT, config->special_keys.size);

  sceIniFileProcessorFinalize(iniProcContext);
}

void update_layout() {
  if (config.jp_layout) {
    config.btn_confirm = SCE_CTRL_CIRCLE;
    config.btn_cancel = SCE_CTRL_CROSS;
  }
  else {
    config.btn_confirm = SCE_CTRL_CROSS;
    config.btn_cancel = SCE_CTRL_CIRCLE;
  }
}

void config_parse(int argc, char* argv[], PCONFIGURATION config) {
  LiInitializeStreamConfiguration(&config->stream);

  config->stream.width = 1280;
  config->stream.height = 720;
  config->stream.fps = 60;
  config->stream.bitrate = -1;
  config->stream.packetSize = 1024;
  config->stream.streamingRemotely = 0;
  config->stream.audioConfiguration = AUDIO_CONFIGURATION_STEREO;
  config->stream.supportsHevc = false;

  config->platform = "vita";
  config->model = sceKernelGetModelForCDialog();
  config->app = "Steam";
  config->action = NULL;
  config->address = NULL;
  config->config_file = NULL;
  config->sops = true;
  config->localaudio = false;
  config->fullscreen = true;
  config->unsupported_version = false;
  config->save_debug_log = false;
  config->disable_powersave = true;
  config->disable_dimming = true;
  config->jp_layout = false;
  config->show_fps = false;
  config->enable_frame_pacer = true;
  config->enable_bgm_mode = true;
  config->center_region_only = false;

  config->special_keys.nw = INPUT_SPECIAL_KEY_PAUSE | INPUT_TYPE_SPECIAL;
  config->special_keys.ne = INPUT_SPECIAL_KEY_KEYBOARD | INPUT_TYPE_SPECIAL;
  config->special_keys.sw = SPECIAL_FLAG | INPUT_TYPE_GAMEPAD;
  config->special_keys.offset = 0;
  config->special_keys.size = 150;

  config->mouse_acceleration = 150;
  config->enable_ref_frame_invalidation = false;

  config->inputsCount = 0;
  config->mapping = NULL;
  config->key_dir[0] = 0;

  //char* config_file = get_path("moonlight.conf", "ux0:data/moonlight/");
  char* config_file = config_path;
  if (config_file) {
    config_file_parse(config_file, config);
  }

  update_layout();

  if (config->config_file != NULL)
    config_save(config->config_file, config);

  if (config->key_dir[0] == 0x0) {
    const char *xdg_cache_dir = getenv("XDG_CACHE_DIR");
    if (xdg_cache_dir != NULL)
      sprintf(config->key_dir, "%s" MOONLIGHT_PATH, xdg_cache_dir);
    else {
      const char *home_dir = getenv("HOME");
      sprintf(config->key_dir, "%s" DEFAULT_CACHE_DIR MOONLIGHT_PATH, home_dir);
    }
  }

  if (config->stream.fps == -1)
    config->stream.fps = config->stream.height >= 1080 ? 30 : 60;

  if (config->stream.bitrate == -1) {
    if (config->stream.height >= 1080 && config->stream.fps >= 60)
      config->stream.bitrate = 20000;
    else if (config->stream.height >= 1080 || config->stream.fps >= 60)
      config->stream.bitrate = 10000;
    else
      config->stream.bitrate = 5000;
  }

  if (inputAdded) {
    if (!mapped) {
        fprintf(stderr, "Mapping option should be followed by the input to be mapped.\n");
        exit(-1);
    } else if (config->mapping == NULL) {
        fprintf(stderr, "Please specify mapping file as default mapping could not be found.\n");
        exit(-1);
    }
  }
}
