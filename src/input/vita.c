/*
 * This file is part of Moonlight Embedded.
 *
 * Copyright (C) 2016 Ilya Zhuravlev, Sunguk Lee, Vasyl Horbachenko
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

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <ctype.h>

#include "../config.h"
#include "../connection.h"
#include "../platform.h"
#include "vita.h"
#include "mapping.h"

#include <Limelight.h>

#include <psp2/kernel/clib.h>
#include <psp2/net/net.h>
#include <psp2/sysmodule.h>
#include <psp2/kernel/sysmem.h>
#include <psp2/kernel/threadmgr.h>

#include <psp2/ctrl.h>
#include <psp2/touch.h>
#include <psp2/ime.h>
#include <psp2/systemgesture.h>
#include <psp2/rtc.h>

#define WIDTH 960
#define HEIGHT 544

#define TOUCH_WIDTH 1920
#define TOUCH_HEIGHT 1088

struct mapping map = {0};

typedef struct input_data {
    short button;
    short lx;
    short ly;
    short rx;
    short ry;
    char  lt;
    char  rt;
} input_data;

typedef struct Point {
  short x;
  short y;
} Point;

typedef struct Section {
  Point left;
  Point right;
} Section;

typedef struct BackTouchState {
	int nw;
	int ne;
	int sw;
	int se;
} BackTouchState;

static SceSystemGestureTouchRecognizer *touch_recognizers;
static bool lhold_rec = false;
static bool scroll_rec = false;

extern SceUID state_evf;
static unsigned int libime_work[SCE_IME_WORK_BUFFER_SIZE / sizeof(unsigned int)];
static char libime_initval[8] = { 1 };
static char libime_out[SCE_IME_MAX_PREEDIT_LENGTH * 2 + 8];
static char libime_out_old[SCE_IME_MAX_PREEDIT_LENGTH * 2 + 8];
static int caret_index_old = 0;
static SceImeCaret caret_rev;

double mouse_multiplier;

static void mouse_lhold(unsigned int reportNum) {
	SceSystemGestureTouchEvent hold_event;
	SceUInt32 event_num_buffer = 0;
	sceSystemGestureGetTouchEvents(&touch_recognizers[TOUCHREC_MOUSE_LHOLD], &hold_event, 1, &event_num_buffer);
	if (event_num_buffer > 0 && reportNum == 1 && hold_event.eventState == SCE_SYSTEM_GESTURE_TOUCH_STATE_BEGIN) {
		LiSendMouseButtonEvent(BUTTON_ACTION_PRESS, BUTTON_LEFT);
		lhold_rec = true;
	}
	else if (lhold_rec && reportNum == 0) {
		LiSendMouseButtonEvent(BUTTON_ACTION_RELEASE, BUTTON_LEFT);
		lhold_rec = false;
	}
}

static void mouse_click(unsigned int reportNum) {
	SceSystemGestureTouchEvent tap_event;
	SceUInt32 event_num_buffer = 0;
	sceSystemGestureGetTouchEvents(&touch_recognizers[TOUCHREC_MOUSE_CLICK], &tap_event, 1, &event_num_buffer);
	if (event_num_buffer > 0)
	if (event_num_buffer > 0 && reportNum == 0) {
		LiSendMouseButtonEvent(BUTTON_ACTION_PRESS, BUTTON_LEFT);
		LiSendMouseButtonEvent(BUTTON_ACTION_RELEASE, BUTTON_LEFT);
	}
	else if (event_num_buffer > 0 && reportNum == 1) {
		LiSendMouseButtonEvent(BUTTON_ACTION_PRESS, BUTTON_RIGHT);
		LiSendMouseButtonEvent(BUTTON_ACTION_RELEASE, BUTTON_RIGHT);
	}
}

static void mouse_move(unsigned int reportNum) {
	SceSystemGestureTouchEvent drag_event;
	SceUInt32 mouse_drag_touching = 0;
	sceSystemGestureGetTouchEvents(&touch_recognizers[TOUCHREC_MOUSE_DRAG], &drag_event, 1, &mouse_drag_touching);
	if (mouse_drag_touching > 0 && reportNum == 1) {

		int x = lround(drag_event.property.drag.deltaVector.x * mouse_multiplier);
		int y = lround(drag_event.property.drag.deltaVector.y * mouse_multiplier);

		LiSendMouseMoveEvent(x, y);
	}
	else if (mouse_drag_touching > 0 && reportNum == 2) {
		int delta = lround(drag_event.property.drag.deltaVector.y * 0.1f);
		LiSendScrollEvent(delta);
		if (lhold_rec) {
			LiSendMouseButtonEvent(BUTTON_ACTION_RELEASE, BUTTON_LEFT);
			lhold_rec = false;
		}
		scroll_rec = true;
	}
	else if (scroll_rec && reportNum != 2)
		scroll_rec = false;
}

SceCtrlData pad, pad_old;
SceTouchData front, back;

static int special_status;

input_data curr, old;
BackTouchState back_touch_state;
int controller_port;

static int VERTICAL;
static int HORIZONTAL;

//Virtual IME string: |wchar0(dummy)||wchar1(dummy)|<-caret always returned to that position>|wchar2(value out)||wchar3(dummy)|
//Caret is always set to middle position so we can detect special keys like backspace
void vitainput_ime_event_handler(void *arg, const SceImeEvent *e)
{
	switch (e->id) {
	case SCE_IME_EVENT_UPDATE_TEXT:;
		if (e->param.text.caretIndex == 0) {
			LiSendKeyboardEvent(0x08, KEY_ACTION_DOWN, 0);
			LiSendKeyboardEvent(0x08, KEY_ACTION_UP, 0);
			sceImeSetText((SceWChar16 *)libime_initval, 4);
		}
		else {
			short upperchar = sceClibToupper(*(short *)&libime_out[2]);
			LiSendKeyboardEvent(upperchar, KEY_ACTION_DOWN, 0);
			LiSendKeyboardEvent(upperchar, KEY_ACTION_UP, 0);
			sceClibMemset(&caret_rev, 0, sizeof(SceImeCaret));
			caret_rev.index = 1;
			sceImeSetCaret(&caret_rev);
			sceImeSetText((SceWChar16 *)libime_initval, 4);
		}
		break;
	case SCE_IME_EVENT_PRESS_ENTER:
		LiSendKeyboardEvent(0x0D, KEY_ACTION_DOWN, 0);
		LiSendKeyboardEvent(0x0D, KEY_ACTION_UP, 0);
	case SCE_IME_EVENT_PRESS_CLOSE:
		sceImeClose();
		sceKernelClearEventFlag(state_evf, ~FLAG_MOONLIGHT_IS_IME);
		break;
	}
}

void open_keyboard(void) {

	if (!sceKernelPollEventFlag(state_evf, FLAG_MOONLIGHT_IS_IME, SCE_KERNEL_EVF_WAITMODE_AND, NULL)) {
		sceImeClose();
		sceKernelClearEventFlag(state_evf, ~FLAG_MOONLIGHT_IS_IME);
		return;
	}

	sceClibMemset(libime_out, 0, (SCE_IME_MAX_PREEDIT_LENGTH * 2 + 6));

	SceImeParam param;
	sceImeParamInit(&param);
	//param.supportedLanguages = 0;
	param.languagesForced = false;
	param.option = SCE_IME_OPTION_NO_ASSISTANCE | SCE_IME_OPTION_NO_AUTO_CAPITALIZATION;
	param.filter = NULL;
	param.work = libime_work;
	param.arg = NULL;
	param.type = SCE_IME_TYPE_DEFAULT;
	param.inputTextBuffer = (SceWChar16 *)libime_out;
	param.maxTextLength = 4;
	param.handler = vitainput_ime_event_handler;
	param.initialText = (SceWChar16 *)libime_initval;
	sceImeOpen(&param);
	sceKernelSetEventFlag(state_evf, FLAG_MOONLIGHT_IS_IME);
}

static void update_back_touch_state() {
	SceSystemGestureTouchEvent drag_event;
	sceSystemGestureGetTouchEvents(&touch_recognizers[TOUCHREC_BACK_NW], &drag_event, 1, &back_touch_state.nw);
	sceSystemGestureGetTouchEvents(&touch_recognizers[TOUCHREC_BACK_NE], &drag_event, 1, &back_touch_state.ne);
	sceSystemGestureGetTouchEvents(&touch_recognizers[TOUCHREC_BACK_SW], &drag_event, 1, &back_touch_state.sw);
	sceSystemGestureGetTouchEvents(&touch_recognizers[TOUCHREC_BACK_SE], &drag_event, 1, &back_touch_state.se);
}

inline uint32_t is_pressed(uint32_t defined) {
  uint32_t dev_type = defined & INPUT_TYPE_MASK;
  uint32_t dev_val  = defined & INPUT_VALUE_MASK;

  switch(dev_type) {
    case INPUT_TYPE_GAMEPAD:
      return pad.buttons & dev_val;
    case INPUT_TYPE_BACK_TOUCHSCREEN:
		switch (dev_val) {
		case TOUCHSEC_BACK_NORTHWEST:
			if (back_touch_state.nw > 0) {
				return 1;
			}
		case TOUCHSEC_BACK_NORTHEAST:
			if (back_touch_state.ne > 0) {
				return 1;
			}
		case TOUCHSEC_BACK_SOUTHWEST:
			if (back_touch_state.sw > 0) {
				return 1;
			}
		case TOUCHSEC_BACK_SOUTHEAST:
			if (back_touch_state.se > 0) {
				return 1;
			}
	  }
  }
  return 0;
}

inline short read_analog(uint32_t defined) {
  uint32_t dev_type = defined & INPUT_TYPE_MASK;
  uint32_t dev_val  = defined & INPUT_VALUE_MASK;

  if (dev_type == INPUT_TYPE_ANALOG) {
    int v;
    switch(dev_val) {
      case LEFTX:
        v = pad.lx;
        break;
      case LEFTY:
        v = pad.ly;
        break;
      case RIGHTX:
        v = pad.rx;
        break;
      case RIGHTY:
        v = pad.ry;
        break;
      case LEFT_TRIGGER:
        return pad.lt;
      case RIGHT_TRIGGER:
        return pad.rt;
      default:
        return 0;
    }
    v = v * 256 - (1 << 15) + 128;
    return (short)(v);
  }
  return is_pressed(defined) ? 0xff : 0;
}

static void special(uint32_t defined, int touchrec) {
	SceSystemGestureTouchEvent tap_event;
	SceUInt32 event_num_buffer = 0;
	uint32_t dev_type;
	uint32_t dev_val;

	sceSystemGestureGetTouchEvents(&touch_recognizers[touchrec], &tap_event, 1, &event_num_buffer);
	if (event_num_buffer > 0) {
		dev_type = defined & INPUT_TYPE_MASK;
		dev_val = defined & INPUT_VALUE_MASK;
		switch (dev_type) {
		case INPUT_TYPE_SPECIAL:
			switch (dev_val) {
			case INPUT_SPECIAL_KEY_PAUSE:
				connection_minimize();
				return;
			case INPUT_SPECIAL_KEY_KEYBOARD:
				open_keyboard();
				return;
			}
		case INPUT_TYPE_GAMEPAD:
			curr.button |= dev_val;
			return;
		case INPUT_TYPE_ANALOG:
			switch (dev_val) {
			case LEFT_TRIGGER:
				curr.lt = 0xff;
				return;
			case RIGHT_TRIGGER:
				curr.rt = 0xff;
				return;
			}
			return;
		case INPUT_TYPE_MOUSE:
			LiSendMouseButtonEvent(BUTTON_ACTION_PRESS, dev_val);
			LiSendMouseButtonEvent(BUTTON_ACTION_RELEASE, dev_val);
			return;
		case INPUT_TYPE_KEYBOARD:
			LiSendKeyboardEvent(dev_val, KEY_ACTION_DOWN, 0);
			LiSendKeyboardEvent(dev_val, KEY_ACTION_UP, 0);
			return;
		}
	}
}

static inline void vitainput_process(void) {
  sceClibMemset(&pad, 0, sizeof(pad));
  sceClibMemset(&curr, 0, sizeof(input_data));

  sceCtrlSetSamplingModeExt(SCE_CTRL_MODE_ANALOG_WIDE);
  sceCtrlPeekBufferPositiveExt2(controller_port, &pad, 1);

  sceTouchPeek(SCE_TOUCH_PORT_FRONT, &front, 1);
  sceTouchPeek(SCE_TOUCH_PORT_BACK, &back, 1);

  sceSystemGestureUpdatePrimitiveTouchRecognizer(&front, &back);
  for (int i = 0; i < TOUCH_RECOGNIZERS_NUM; i++)
	  sceSystemGestureUpdateTouchRecognizer(&touch_recognizers[i]);

  update_back_touch_state();

  // buttons
  curr.button |= is_pressed(map.btn_dpad_up)    ? UP_FLAG     : 0;
  curr.button |= is_pressed(map.btn_dpad_left)  ? LEFT_FLAG   : 0;
  curr.button |= is_pressed(map.btn_dpad_down)  ? DOWN_FLAG   : 0;
  curr.button |= is_pressed(map.btn_dpad_right) ? RIGHT_FLAG  : 0;
  curr.button |= is_pressed(map.btn_start)      ? PLAY_FLAG   : 0;
  curr.button |= is_pressed(map.btn_select)     ? BACK_FLAG   : 0;
  curr.button |= is_pressed(map.btn_north)      ? Y_FLAG      : 0;
  curr.button |= is_pressed(map.btn_east)       ? B_FLAG      : 0;
  curr.button |= is_pressed(map.btn_south)      ? A_FLAG      : 0;
  curr.button |= is_pressed(map.btn_west)       ? X_FLAG      : 0;
  curr.button |= is_pressed(map.btn_thumbl)     ? LB_FLAG     : 0; // l1
  curr.button |= is_pressed(map.btn_thumbr)     ? RB_FLAG     : 0; // r1
  curr.button |= is_pressed(map.btn_tl2)        ? LS_CLK_FLAG : 0; // l3
  curr.button |= is_pressed(map.btn_tr2)        ? RS_CLK_FLAG : 0; // r3

  // analogs
  curr.lt = read_analog(map.btn_tl); // l2
  curr.rt = read_analog(map.btn_tr); // r2
  curr.lx = read_analog(map.abs_x);
  curr.ly = read_analog(map.abs_y);
  curr.rx = read_analog(map.abs_rx);
  curr.ry = read_analog(map.abs_ry);

  // special touchscreen buttons
  special(config.special_keys.nw, TOUCHREC_SPECIAL_NW);
  special(config.special_keys.ne, TOUCHREC_SPECIAL_NE);
  special(config.special_keys.sw, TOUCHREC_SPECIAL_SW);
  special(config.special_keys.se, TOUCHREC_SPECIAL_SE);

  if ((sceClibMemcmp(&curr, &old, sizeof(input_data)) != 0) &&
	  sceKernelPollEventFlag(state_evf, FLAG_MOONLIGHT_IS_IME, SCE_KERNEL_EVF_WAITMODE_AND, NULL)) {
    LiSendControllerEvent(curr.button, curr.lt, curr.rt,
                          curr.lx, -1 * curr.ly, curr.rx, -1 * curr.ry);
    sceClibMemcpy(&old, &curr, sizeof(input_data));
    sceClibMemcpy(&pad_old, &pad, sizeof(SceCtrlData));
  }

  mouse_click(front.reportNum);
  mouse_move(front.reportNum);
  if (!scroll_rec)
	  mouse_lhold(front.reportNum);

}

int vitainput_thread(SceSize args, void *argp) {
  while (1) {
    
	sceKernelWaitEventFlag(state_evf, FLAG_MOONLIGHT_IS_FG | FLAG_MOONLIGHT_ACTIVE_INPUT_THREAD, SCE_KERNEL_EVF_WAITMODE_AND, NULL, NULL);

    vitainput_process();

    sceKernelDelayThread(5000); // 5 ms
  }

  return 0;
}

void vitainput_init_touch() {
	sceSysmoduleLoadModule(SCE_SYSMODULE_SYSTEM_GESTURE);
	sceSystemGestureInitializePrimitiveTouchRecognizer(NULL);
	touch_recognizers = (SceSystemGestureTouchRecognizer *)calloc(TOUCH_RECOGNIZERS_NUM, sizeof(SceSystemGestureTouchRecognizer));
	//Mouse events recognizers
	SceSystemGestureRectangle rect;
	rect.x = 0;
	rect.y = 0;
	rect.width = 1920;
	rect.height = 1088;
	sceSystemGestureCreateTouchRecognizer(&touch_recognizers[TOUCHREC_MOUSE_DRAG], SCE_SYSTEM_GESTURE_TYPE_DRAG, SCE_TOUCH_PORT_FRONT, &rect, NULL);
	sceSystemGestureCreateTouchRecognizer(&touch_recognizers[TOUCHREC_MOUSE_CLICK], SCE_SYSTEM_GESTURE_TYPE_TAP, SCE_TOUCH_PORT_FRONT, &rect, NULL);
	sceSystemGestureCreateTouchRecognizer(&touch_recognizers[TOUCHREC_MOUSE_LHOLD], SCE_SYSTEM_GESTURE_TYPE_TAP_AND_HOLD, SCE_TOUCH_PORT_FRONT, &rect, NULL);
	//Special zones
	sceClibMemset(&rect, 0, sizeof(SceSystemGestureRectangle));
	sceSystemGestureCreateTouchRecognizer(&touch_recognizers[TOUCHREC_SPECIAL_NW], SCE_SYSTEM_GESTURE_TYPE_TAP, SCE_TOUCH_PORT_FRONT, &rect, NULL);
	sceSystemGestureCreateTouchRecognizer(&touch_recognizers[TOUCHREC_SPECIAL_NE], SCE_SYSTEM_GESTURE_TYPE_TAP, SCE_TOUCH_PORT_FRONT, &rect, NULL);
	sceSystemGestureCreateTouchRecognizer(&touch_recognizers[TOUCHREC_SPECIAL_SW], SCE_SYSTEM_GESTURE_TYPE_TAP, SCE_TOUCH_PORT_FRONT, &rect, NULL);
	sceSystemGestureCreateTouchRecognizer(&touch_recognizers[TOUCHREC_SPECIAL_SE], SCE_SYSTEM_GESTURE_TYPE_TAP, SCE_TOUCH_PORT_FRONT, &rect, NULL);
	//Back touchpad
	sceSystemGestureCreateTouchRecognizer(&touch_recognizers[TOUCHREC_BACK_NW], SCE_SYSTEM_GESTURE_TYPE_DRAG, SCE_TOUCH_PORT_BACK, &rect, NULL);
	sceSystemGestureCreateTouchRecognizer(&touch_recognizers[TOUCHREC_BACK_NE], SCE_SYSTEM_GESTURE_TYPE_DRAG, SCE_TOUCH_PORT_BACK, &rect, NULL);
	sceSystemGestureCreateTouchRecognizer(&touch_recognizers[TOUCHREC_BACK_SW], SCE_SYSTEM_GESTURE_TYPE_DRAG, SCE_TOUCH_PORT_BACK, &rect, NULL);
	sceSystemGestureCreateTouchRecognizer(&touch_recognizers[TOUCHREC_BACK_SE], SCE_SYSTEM_GESTURE_TYPE_DRAG, SCE_TOUCH_PORT_BACK, &rect, NULL);
}

bool vitainput_init() {
  sceCtrlSetSamplingModeExt(SCE_CTRL_MODE_ANALOG_WIDE);
  sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
  sceTouchSetSamplingState(SCE_TOUCH_PORT_BACK, SCE_TOUCH_SAMPLING_STATE_START);

  vitainput_init_touch();

  SceUID thid = sceKernelCreateThread("vitainput_thread", vitainput_thread, 70, 0x1000, 0, SCE_KERNEL_CPU_MASK_USER_2, NULL);
  if (thid >= 0) {
    sceKernelStartThread(thid, 0, NULL);
    return true;
  }

  return false;
}

void vitainput_config(CONFIGURATION config) {
  map.abs_x           = LEFTX               | INPUT_TYPE_ANALOG;
  map.abs_y           = LEFTY               | INPUT_TYPE_ANALOG;
  map.abs_rx          = RIGHTX              | INPUT_TYPE_ANALOG;
  map.abs_ry          = RIGHTY              | INPUT_TYPE_ANALOG;

  map.btn_dpad_up     = SCE_CTRL_UP         | INPUT_TYPE_GAMEPAD;
  map.btn_dpad_down   = SCE_CTRL_DOWN       | INPUT_TYPE_GAMEPAD;
  map.btn_dpad_left   = SCE_CTRL_LEFT       | INPUT_TYPE_GAMEPAD;
  map.btn_dpad_right  = SCE_CTRL_RIGHT      | INPUT_TYPE_GAMEPAD;
  map.btn_south       = SCE_CTRL_CROSS      | INPUT_TYPE_GAMEPAD;
  map.btn_east        = SCE_CTRL_CIRCLE     | INPUT_TYPE_GAMEPAD;
  map.btn_north       = SCE_CTRL_TRIANGLE   | INPUT_TYPE_GAMEPAD;
  map.btn_west        = SCE_CTRL_SQUARE     | INPUT_TYPE_GAMEPAD;

  map.btn_select      = SCE_CTRL_SELECT     | INPUT_TYPE_GAMEPAD;
  map.btn_start       = SCE_CTRL_START      | INPUT_TYPE_GAMEPAD;

  map.btn_thumbl      = SCE_CTRL_L1         | INPUT_TYPE_GAMEPAD;
  map.btn_thumbr      = SCE_CTRL_R1         | INPUT_TYPE_GAMEPAD;

  if (config.model == SCE_KERNEL_MODEL_VITATV) {
    map.btn_tl        = LEFT_TRIGGER        | INPUT_TYPE_ANALOG;
    map.btn_tr        = RIGHT_TRIGGER       | INPUT_TYPE_ANALOG;
    map.btn_tl2       = SCE_CTRL_L3         | INPUT_TYPE_GAMEPAD;
    map.btn_tr2       = SCE_CTRL_R3         | INPUT_TYPE_GAMEPAD;
  } else {
    map.btn_tl        = TOUCHSEC_BACK_NORTHWEST  | INPUT_TYPE_BACK_TOUCHSCREEN;
    map.btn_tr        = TOUCHSEC_BACK_NORTHEAST  | INPUT_TYPE_BACK_TOUCHSCREEN;
    map.btn_tl2       = TOUCHSEC_BACK_SOUTHWEST  | INPUT_TYPE_BACK_TOUCHSCREEN;
    map.btn_tr2       = TOUCHSEC_BACK_SOUTHEAST  | INPUT_TYPE_BACK_TOUCHSCREEN;
  }

  if (config.mapping) {
    char mapping_file_path[256];
    sprintf(mapping_file_path, "savedata0:%s", config.mapping);
    sceClibPrintf("Loading mapping at %s\n", mapping_file_path);
    mapping_load(mapping_file_path, &map);
  }

  controller_port = config.model == SCE_KERNEL_MODEL_VITATV ? 1 : 0;

  int real_offset = config.special_keys.offset * 2;
  int real_size = config.special_keys.size * 2;

  //Update special touch areas
  SceSystemGestureRectangle rect;
  rect.x = real_offset;
  rect.y = real_offset;
  rect.width = real_size;
  rect.height = real_size;
  sceSystemGestureUpdateTouchRecognizerRectangle(&touch_recognizers[TOUCHREC_SPECIAL_NW], &rect);
  rect.x = TOUCH_WIDTH - real_offset - real_size;
  rect.y = real_offset;
  sceSystemGestureUpdateTouchRecognizerRectangle(&touch_recognizers[TOUCHREC_SPECIAL_NE], &rect);
  rect.x = real_offset;
  rect.y = TOUCH_HEIGHT - real_offset - real_size;
  sceSystemGestureUpdateTouchRecognizerRectangle(&touch_recognizers[TOUCHREC_SPECIAL_SW], &rect);
  rect.x = TOUCH_WIDTH - real_offset - real_size;
  rect.y = TOUCH_HEIGHT - real_offset - real_size;
  sceSystemGestureUpdateTouchRecognizerRectangle(&touch_recognizers[TOUCHREC_SPECIAL_SE], &rect);

  //Update mouse click area
  rect.x = real_offset + real_size;
  rect.y = real_offset + real_size;
  rect.width = TOUCH_WIDTH - real_offset - real_size;
  rect.height = TOUCH_HEIGHT - real_offset - real_size;
  sceSystemGestureUpdateTouchRecognizerRectangle(&touch_recognizers[TOUCHREC_MOUSE_CLICK], &rect);

  //Update back touch areas
  int back_real_deadzone_left = config.back_deadzone.left * 2;
  int back_real_deadzone_right = config.back_deadzone.right * 2;
  int back_real_deadzone_top = config.back_deadzone.top * 2;
  int back_real_deadzone_bottom = config.back_deadzone.bottom * 2;

  HORIZONTAL   = ((WIDTH - config.back_deadzone.left - config.back_deadzone.right) / 2
             + config.back_deadzone.left) * 2;
  VERTICAL = ((HEIGHT - config.back_deadzone.top - config.back_deadzone.bottom) / 2
             + config.back_deadzone.top) * 2;

  rect.x = back_real_deadzone_left;
  rect.y = back_real_deadzone_top;
  rect.width = HORIZONTAL;
  rect.height = VERTICAL;
  sceSystemGestureUpdateTouchRecognizerRectangle(&touch_recognizers[TOUCHREC_BACK_NW], &rect);
  rect.x = HORIZONTAL;
  rect.y = back_real_deadzone_top;
  sceSystemGestureUpdateTouchRecognizerRectangle(&touch_recognizers[TOUCHREC_BACK_NE], &rect);
  rect.x = back_real_deadzone_left;
  rect.y = VERTICAL;
  sceSystemGestureUpdateTouchRecognizerRectangle(&touch_recognizers[TOUCHREC_BACK_SW], &rect);
  rect.x = HORIZONTAL;
  rect.y = VERTICAL;
  sceSystemGestureUpdateTouchRecognizerRectangle(&touch_recognizers[TOUCHREC_BACK_SE], &rect);

  mouse_multiplier = 0.003 * config.mouse_acceleration;
}

void vitainput_start(void) {
  sceKernelSetEventFlag(state_evf, FLAG_MOONLIGHT_ACTIVE_INPUT_THREAD);
}

void vitainput_stop(void) {
  sceKernelClearEventFlag(state_evf, ~FLAG_MOONLIGHT_ACTIVE_INPUT_THREAD);
}
