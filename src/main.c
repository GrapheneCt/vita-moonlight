/*
 * This file is part of Moonlight Embedded.
 *
 * Copyright (C) 2015, 2016, 2020 Iwan Timmer, Graphene
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

#include "loop.h"
#include "client.h"
#include "connection.h"
#include "configuration.h"
#include "audio.h"
#include "video.h"
#include "discover.h"
#include "config.h"
#include "platform.h"

#include "input/vita.h"

#include <Limelight.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <ctype.h>

#include <psp2/kernel/clib.h>
#include <psp2/kernel/rng.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/kernel/modulemgr.h>
#include <psp2/libdbg.h>

#include <psp2/net.h>
#include <psp2/libnetctl.h>

#include <psp2/kernel/iofilemgr.h>

#include <psp2/sysmodule.h>
#include <psp2/ctrl.h>
#include <psp2/touch.h>
#include <psp2/rtc.h>

#include "device.h"
#include "gui/ui.h"
#include "power/vita.h"

typedef struct SceKernelModuleLoadStartParam {
	SceUInt32 flags;
	int *status;
	void *option;
	int a4; // not used
} SceKernelModuleLoadStartParam;

SceUID _sceKernelLoadStartModule(const char *moduleFileName, SceSize args, const void *argp, const SceKernelModuleLoadStartParam *pParam);

typedef struct SceSysmoduleOpt {
	int flags;
	int *result;
	int unused[2];
} SceSysmoduleOpt;

typedef struct ScePafInit {
	unsigned int global_heap_size;
	int a2;
	int a3;
	int cdlg_mode;
	int heap_opt_param1;
	int heap_opt_param2;
} ScePafInit;

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern int LINE_SIZE;

// We mostly use SceLibc heap
int _newlib_heap_size_user = 1 * 1024 * 1024;
SceUID state_evf = 0;

static void vita_init() {

  int ret;

  sceDbgSetMinimumLogLevel(SCE_DBG_LOG_LEVEL_ERROR);

  //Global state EVF
  state_evf = sceKernelCreateEventFlag(
	  "moonlight_event_flag", 
	  SCE_KERNEL_ATTR_MULTI, 
	  FLAG_MOONLIGHT_IS_FG | FLAG_MOONLIGHT_ACTIVE_VIDEO_THREAD | FLAG_MOONLIGHT_ACTIVE_AUDIO_THREAD, 
	  NULL);

  //Configure main thread
  sceKernelChangeThreadCpuAffinityMask(SCE_KERNEL_THREAD_ID_SELF, SCE_KERNEL_CPU_MASK_USER_0);
  sceKernelChangeThreadPriority(SCE_KERNEL_THREAD_ID_SELF, 80);

  //Load stuff

  SceKernelModuleLoadStartParam param = { 0 };

  sceSysmoduleLoadModule(SCE_SYSMODULE_IME);
  sceSysmoduleLoadModule(SCE_SYSMODULE_NET);
  //We use direct SceModulemgr function because normal one doesn't always work for ScePsp2Compat for some reason
  _sceKernelLoadStartModule("vs0:data/external/webcore/ScePsp2Compat.suprx", 0, NULL, &param);
  sceKernelLoadStartModule("app0:module/user/mdns.suprx", 0, NULL, 0, NULL, 0);
  sceKernelLoadStartModule("app0:module/user/h264bitstream.suprx", 0, NULL, 0, NULL, 0);
  sceKernelLoadStartModule("app0:module/user/uuid.suprx", 0, NULL, 0, NULL, 0);
  sceKernelLoadStartModule("app0:module/user/libexpat.suprx", 0, NULL, 0, NULL, 0);
  sceKernelLoadStartModule("app0:module/user/vita2d_sys.suprx", 0, NULL, 0, NULL, 0);

  //PAF needed for SceIniFileProcessor
  SceSysmoduleOpt sysmodule_opt;
  ScePafInit init_param;
  init_param.global_heap_size = 6 * 1024 * 1024;
  init_param.a2 = 0x0000EA60;
  init_param.a3 = 0x00040000;
  init_param.cdlg_mode = 0;
  init_param.heap_opt_param1 = 0;
  init_param.heap_opt_param2 = 0;

  sysmodule_opt.flags = 0;
  sysmodule_opt.result = &ret;

  sceSysmoduleLoadModuleInternalWithArg(SCE_SYSMODULE_INTERNAL_PAF, sizeof(init_param), &init_param, &sysmodule_opt);
  sceSysmoduleLoadModuleInternal(SCE_SYSMODULE_INTERNAL_INI_FILE_PROCESSOR);

  // Seed OpenSSL with Sony-grade random number generator
  char random_seed[0x40] = {0};
  sceKernelGetRandomNumber(random_seed, sizeof(random_seed));
  RAND_seed(random_seed, sizeof(random_seed));
  OpenSSL_add_all_algorithms();

  // This is only used for PIN codes, doesn't really matter
  srand(time(NULL));

  sceClibPrintf("Vita Moonlight %d.%d.%d (%s)\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, COMPILE_OPTIONS);

  size_t net_mem_sz = 100 * 1024;
  SceNetInitParam net_param = {0};
  net_param.memory = calloc(net_mem_sz, 1);
  net_param.size = net_mem_sz;
  ret = sceNetInit(&net_param);

  ret = sceNetCtlInit();
  // TODO(xyz): cURL breaks when socket FD is too big, very hacky workaround below!
  int s = sceNetSocket("", SCE_NET_AF_INET, SCE_NET_SOCK_STREAM, 0);
  sceNetSocketClose(s);
  if (s >= 20) {
    sceClibPrintf("Cycling sockets...\n");
    int c = 0;
    do {
      c = sceNetSocket("", SCE_NET_AF_INET, SCE_NET_SOCK_STREAM, 0);
      sceNetSocketClose(c);
    } while (c >= 5);
  }
}

void loop_forever(void) {
  while (connection_is_ready()) {
    sceKernelDelayThread(100 * 1000);
  }
}

int main(int argc, char* argv[]) {
  vita_init();

  if (!vitapower_init()) {
    sceClibPrintf("Failed to init power!");
    loop_forever();
  }

  if (!vitainput_init()) {
    sceClibPrintf("Failed to init input!");
    loop_forever();
  }

  config_path = "savedata0:moonlight.conf";
  config_parse(argc, argv, &config);
  strcpy(config.key_dir, "savedata0:");

  switch (config.stream.width) {
  case 1920:
	  SCREEN_WIDTH = 1920;
	  SCREEN_HEIGHT = 1088;
	  LINE_SIZE = 1920;
	  break;
  case 1280:
	  SCREEN_WIDTH = 1280;
	  SCREEN_HEIGHT = 725;
	  LINE_SIZE = 1280;
	  break;
  default:
	  break;
  }

  vitapower_config(config);
  vitainput_config(config);

  config.log_file = fopen("savedata0:moonlight.log", "w");

  load_all_known_devices();

  gui_loop();
}
