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

typedef enum {
  NO_TOUCH_ACTION = 0,
  ON_SCREEN_TOUCH,
  SCREEN_TAP,
  SWIPE_START,
  ON_SCREEN_SWIPE
} TouchScreenState;

typedef enum {
  TOUCHSEC_BACK_NORTHWEST  = 0x1,
  TOUCHSEC_BACK_NORTHEAST  = 0x2,
  TOUCHSEC_BACK_SOUTHWEST  = 0x4,
  TOUCHSEC_BACK_SOUTHEAST  = 0x8,
  TOUCHSEC_SPECIAL_NW = 0x10,
  TOUCHSEC_SPECIAL_NE = 0x20,
  TOUCHSEC_SPECIAL_SW = 0x40,
  TOUCHSEC_SPECIAL_SE = 0x80,
} TouchScreenSection;

typedef enum {
  LEFTX,
  LEFTY,
  RIGHTX,
  RIGHTY,
  LEFT_TRIGGER,
  RIGHT_TRIGGER
} PadSection;

typedef enum {
	TOUCHREC_MOUSE_DRAG,
	TOUCHREC_MOUSE_CLICK,
	TOUCHREC_MOUSE_LHOLD,
	TOUCHREC_SPECIAL_NW,
	TOUCHREC_SPECIAL_NE,
	TOUCHREC_SPECIAL_SW,
	TOUCHREC_SPECIAL_SE,
	TOUCHREC_BACK_NW,
	TOUCHREC_BACK_NE,
	TOUCHREC_BACK_SW,
	TOUCHREC_BACK_SE
} TouchScreenRecognizer;

#define TOUCH_RECOGNIZERS_NUM   11

#define INPUT_TYPE_MASK         0xfff00000
#define INPUT_VALUE_MASK        0x000fffff

#define INPUT_TYPE_KEYBOARD     0x00000000
#define INPUT_TYPE_SPECIAL      0x00100000
#define INPUT_TYPE_MOUSE        0x00200000
#define INPUT_TYPE_GAMEPAD      0x00300000
#define INPUT_TYPE_ANALOG       0x00400000
#define INPUT_TYPE_BACK_TOUCHSCREEN  0x00500000
#define INPUT_TYPE_DEF_NAME     0xf0000000

enum {
  INPUT_SPECIAL_KEY_PAUSE,
  INPUT_SPECIAL_KEY_KEYBOARD
};

bool vitainput_init();
void vitainput_config(CONFIGURATION config);

void vitainput_start(void);
void vitainput_stop(void);
