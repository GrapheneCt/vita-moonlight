/*
 * This file is part of Moonlight Embedded.
 *
 * Copyright (C) 2015 Iwan Timmer, Sunguk Lee
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
#include <stdbool.h>

#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/clib.h>
#include <psp2/power.h>
#include <taihen.h>
#include "../config.h"
#include "../platform.h"
#include "../video/vita.h"

#define SCE_APP_EVENT_UNK0					(0x00000003)
#define SCE_APP_EVENT_ON_ACTIVATE			(0x10000001)
#define SCE_APP_EVENT_ON_DEACTIVATE			(0x10000002)
#define SCE_APP_EVENT_UNK1					(0x10000300)
#define SCE_APP_EVENT_REQUEST_QUIT			(0x20000001)
#define SCE_APP_EVENT_UNK2					(0x30000003)

typedef struct SceAppMgrEvent { // size is 0x64
	int event;				/* Event ID */
	SceUID appId;			/* Application ID. Added when required by the event */
	char  param[56];		/* Parameters to pass with the event */
} SceAppMgrEvent;

int _sceAppMgrReceiveEvent(SceAppMgrEvent *appEvent);
int sceAppMgrQuitForNonSuspendableApp(void);

enum {
  DISABLE_DIMMING = 1,
  DISABLE_SUSPEND = 2,
};

extern SceUID state_evf;

static int powermode = 0;

void vitapower_termninate() {
	SceUID modid = taiLoadStartKernelModule("ux0:app/GRVA00010/module/kernel/exit_module.skprx", 0, NULL, 0);
	sceKernelSetEventFlag(state_evf, FLAG_MOONLIGHT_IS_FG);
	int ret = sceAppMgrQuitForNonSuspendableApp();
	taiStopUnloadKernelModule(modid, 0, NULL, 0, NULL, NULL);
	sceClibPrintf("Moonlight quit: 0x%X\n", ret);
}

int vitapower_thread(SceSize args, void *argp) {

  SceAppMgrEvent appEvent;

  while (1) {
	  //Not sure if flushing is better on activation or deactivation
	  _sceAppMgrReceiveEvent(&appEvent);
	  switch (appEvent.event) {
	  case SCE_APP_EVENT_ON_ACTIVATE:
		  vitavideo_flush_decoder();
		  sceKernelSetEventFlag(state_evf, FLAG_MOONLIGHT_IS_FG);
		  break;
	  case SCE_APP_EVENT_ON_DEACTIVATE:
		  sceKernelClearEventFlag(state_evf, ~FLAG_MOONLIGHT_IS_FG);
		  break;
	  case SCE_APP_EVENT_REQUEST_QUIT:;
		  vitapower_termninate();
		  break;
	  }

    if (sceKernelPollEventFlag(state_evf, FLAG_MOONLIGHT_ACTIVE_POWER_THREAD, SCE_KERNEL_EVF_WAITMODE_AND, NULL)) {
      sceKernelDelayThread(10 * 1000);
      continue;
    }
    if (powermode & DISABLE_SUSPEND) {
      sceKernelPowerTick(SCE_KERNEL_POWER_TICK_DISABLE_AUTO_SUSPEND);
    }
	if (powermode & DISABLE_DIMMING) {
	  sceKernelPowerTick(SCE_KERNEL_POWER_TICK_DISABLE_OLED_OFF);
	  sceKernelPowerTick(SCE_KERNEL_POWER_TICK_DISABLE_OLED_DIMMING);
	}
    sceKernelDelayThread(10 * 1000);
  }

  return 0;
}

bool vitapower_init() {
  SceUID thid = sceKernelCreateThread("vitapower_thread", vitapower_thread, 127, 0x1000, 0, SCE_KERNEL_CPU_MASK_USER_2, NULL);
  if (thid >= 0)
    sceKernelStartThread(thid, 0, NULL);

  return true;
}

void vitapower_config(CONFIGURATION config) {
  powermode = 0;

  if (config.disable_powersave) {
    powermode |= DISABLE_SUSPEND;
  }
  if (config.disable_dimming) {
	powermode |= DISABLE_DIMMING;
  }
}

void vitapower_start() {
  sceKernelSetEventFlag(state_evf, FLAG_MOONLIGHT_ACTIVE_POWER_THREAD);
}

void vitapower_stop() {
  sceKernelClearEventFlag(state_evf, ~FLAG_MOONLIGHT_ACTIVE_POWER_THREAD);
}
