/*
 * This file is part of Moonlight Embedded.
 *
 * Copyright (C) 2015,2020 Iwan Timmer, Graphene
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

#include "mapping.h"
#include "../ini.h"
#include "../ini_file_processor_c.h"

#include <psp2/kernel/clib.h>

void mapping_load(char* fileName, struct mapping* map) {

  char iniProcContext[8];
  SceIniFileProcessorParam iniProcInitParam;
  SceIniFileProcessorMemCallbacks iniAllocCb;
  sceIniFileProcessorCreateContext(iniProcContext);

  sceIniFileProcessorInitializeParam(&iniProcInitParam);
  sceIniFileProcessorCreateInstance(iniProcContext, &iniProcInitParam);

  int ret = sceIniFileProcessorOpenFile(iniProcContext, fileName, "r", 0);
  if (ret < 0) {
	sceIniFileProcessorDestroyInstanceForError(iniProcContext);
	sceClibPrintf("sceIniFileProcessorOpenFile() returned 0x%X", ret);
	return;
  }

  int int_value;
  if (!iniGetValueByKey(iniProcContext, "abs_x", INI_VALUE_INT, 16, &int_value))
	map->abs_x = int_value;
  if (!iniGetValueByKey(iniProcContext, "abs_y", INI_VALUE_INT, 16, &int_value))
	map->abs_y = int_value;
  if (!iniGetValueByKey(iniProcContext, "abs_z", INI_VALUE_INT, 16, &int_value))
	map->abs_z = int_value;
  if (!iniGetValueByKey(iniProcContext, "abs_rx", INI_VALUE_INT, 16, &int_value))
	map->abs_rx = int_value;
  if (!iniGetValueByKey(iniProcContext, "abs_ry", INI_VALUE_INT, 16, &int_value))
	map->abs_ry = int_value;
  if (!iniGetValueByKey(iniProcContext, "abs_rz", INI_VALUE_INT, 16, &int_value))
	map->abs_rz = int_value;
  if (!iniGetValueByKey(iniProcContext, "abs_deadzone", INI_VALUE_INT, 16, &int_value))
	map->abs_deadzone = int_value;
  if (!iniGetValueByKey(iniProcContext, "abs_dpad_x", INI_VALUE_INT, 16, &int_value))
	map->abs_dpad_x = int_value;
  if (!iniGetValueByKey(iniProcContext, "abs_dpad_y", INI_VALUE_INT, 16, &int_value))
	map->abs_dpad_y = int_value;
  if (!iniGetValueByKey(iniProcContext, "btn_south", INI_VALUE_INT, 16, &int_value))
	map->btn_south = int_value;
  if (!iniGetValueByKey(iniProcContext, "btn_north", INI_VALUE_INT, 16, &int_value))
    map->btn_north = int_value;
  if (!iniGetValueByKey(iniProcContext, "btn_east", INI_VALUE_INT, 16, &int_value))
	map->btn_east = int_value;
  if (!iniGetValueByKey(iniProcContext, "btn_west", INI_VALUE_INT, 16, &int_value))
	map->btn_west = int_value;
  if (!iniGetValueByKey(iniProcContext, "btn_select", INI_VALUE_INT, 16, &int_value))
	map->btn_select = int_value;
  if (!iniGetValueByKey(iniProcContext, "btn_start", INI_VALUE_INT, 16, &int_value))
	map->btn_start = int_value;
  if (!iniGetValueByKey(iniProcContext, "btn_mode", INI_VALUE_INT, 16, &int_value))
	map->btn_mode = int_value;
  if (!iniGetValueByKey(iniProcContext, "btn_thumbl", INI_VALUE_INT, 16, &int_value))
	map->btn_thumbl = int_value;
  if (!iniGetValueByKey(iniProcContext, "btn_thumbr", INI_VALUE_INT, 16, &int_value))
	map->btn_thumbr = int_value;
  if (!iniGetValueByKey(iniProcContext, "btn_tl", INI_VALUE_INT, 16, &int_value))
	map->btn_tl = int_value;
  if (!iniGetValueByKey(iniProcContext, "btn_tr", INI_VALUE_INT, 16, &int_value))
	map->btn_tr = int_value;
  if (!iniGetValueByKey(iniProcContext, "btn_tl2", INI_VALUE_INT, 16, &int_value))
	map->btn_tl2 = int_value;
  if (!iniGetValueByKey(iniProcContext, "btn_tr2", INI_VALUE_INT, 16, &int_value))
	map->btn_tr2 = int_value;
  if (!iniGetValueByKey(iniProcContext, "btn_dpad_up", INI_VALUE_INT, 16, &int_value))
	map->btn_dpad_up = int_value;
  if (!iniGetValueByKey(iniProcContext, "btn_dpad_down", INI_VALUE_INT, 16, &int_value))
	map->btn_dpad_down = int_value;
  if (!iniGetValueByKey(iniProcContext, "btn_dpad_left", INI_VALUE_INT, 16, &int_value))
	map->btn_dpad_left = int_value;
  if (!iniGetValueByKey(iniProcContext, "btn_dpad_right", INI_VALUE_INT, 16, &int_value))
	map->btn_dpad_right = int_value;
  if (!iniGetValueByKey(iniProcContext, "reverse_x", INI_VALUE_BOOL, 16, &int_value))
	map->reverse_x = int_value;
  if (!iniGetValueByKey(iniProcContext, "reverse_y", INI_VALUE_BOOL, 16, &int_value))
	map->reverse_y = int_value;
  if (!iniGetValueByKey(iniProcContext, "reverse_rx", INI_VALUE_BOOL, 16, &int_value))
	map->reverse_rx = int_value;
  if (!iniGetValueByKey(iniProcContext, "reverse_ry", INI_VALUE_BOOL, 16, &int_value))
	map->reverse_ry = int_value;
  if (!iniGetValueByKey(iniProcContext, "reverse_dpad_x", INI_VALUE_BOOL, 16, &int_value))
	  map->reverse_dpad_x = int_value;
  if (!iniGetValueByKey(iniProcContext, "reverse_dpad_y", INI_VALUE_BOOL, 16, &int_value))
	  map->reverse_dpad_y = int_value;

  sceIniFileProcessorFinalize(iniProcContext);
}

void mapping_save(char* fileName, struct mapping* map) {

  char iniProcContext[8];
  SceIniFileProcessorParam iniProcInitParam;
  SceIniFileProcessorMemCallbacks iniAllocCb;
  sceIniFileProcessorCreateContext(iniProcContext);

  sceIniFileProcessorInitializeParam(&iniProcInitParam);
  sceIniFileProcessorCreateInstance(iniProcContext, &iniProcInitParam);

  int ret = sceIniFileProcessorCreateFile(iniProcContext, fileName, "rw", 0);
  if (ret < 0) {
	 sceIniFileProcessorDestroyInstanceForError(iniProcContext);
	 sceClibPrintf("sceIniFileProcessorCreateFile() returned 0x%X", ret);
	 return;
  }

  iniCreateSetKey(iniProcContext, "abs_x", INI_VALUE_HEX, map->abs_x);
  iniCreateSetKey(iniProcContext, "abs_y", INI_VALUE_HEX, map->abs_y);
  iniCreateSetKey(iniProcContext, "abs_z", INI_VALUE_HEX, map->abs_z);

  iniCreateSetKey(iniProcContext, "reverse_x", INI_VALUE_BOOL, map->reverse_x);
  iniCreateSetKey(iniProcContext, "reverse_y", INI_VALUE_BOOL, map->reverse_y);

  iniCreateSetKey(iniProcContext, "abs_rx", INI_VALUE_HEX, map->abs_rx);
  iniCreateSetKey(iniProcContext, "abs_ry", INI_VALUE_HEX, map->abs_ry);
  iniCreateSetKey(iniProcContext, "abs_rz", INI_VALUE_HEX, map->abs_rz);

  iniCreateSetKey(iniProcContext, "reverse_rx", INI_VALUE_BOOL, map->reverse_rx);
  iniCreateSetKey(iniProcContext, "reverse_ry", INI_VALUE_BOOL, map->reverse_ry);

  iniCreateSetKey(iniProcContext, "abs_deadzone", INI_VALUE_HEX, map->abs_deadzone);

  iniCreateSetKey(iniProcContext, "abs_dpad_x", INI_VALUE_HEX, map->abs_dpad_x);
  iniCreateSetKey(iniProcContext, "abs_dpad_y", INI_VALUE_HEX, map->abs_dpad_y);

  iniCreateSetKey(iniProcContext, "reverse_dpad_x", INI_VALUE_BOOL, map->reverse_dpad_x);
  iniCreateSetKey(iniProcContext, "reverse_dpad_y", INI_VALUE_BOOL, map->reverse_dpad_y);

  iniCreateSetKey(iniProcContext, "btn_north", INI_VALUE_HEX, map->btn_north);
  iniCreateSetKey(iniProcContext, "btn_east", INI_VALUE_HEX, map->btn_east);
  iniCreateSetKey(iniProcContext, "btn_south", INI_VALUE_HEX, map->btn_south);
  iniCreateSetKey(iniProcContext, "btn_west", INI_VALUE_HEX, map->btn_west);

  iniCreateSetKey(iniProcContext, "btn_select", INI_VALUE_HEX, map->btn_select);
  iniCreateSetKey(iniProcContext, "btn_start", INI_VALUE_HEX, map->btn_start);
  iniCreateSetKey(iniProcContext, "btn_mode", INI_VALUE_HEX, map->btn_mode);

  iniCreateSetKey(iniProcContext, "btn_thumbl", INI_VALUE_HEX, map->btn_thumbl);
  iniCreateSetKey(iniProcContext, "btn_thumbr", INI_VALUE_HEX, map->btn_thumbr);

  iniCreateSetKey(iniProcContext, "btn_tl", INI_VALUE_HEX, map->btn_tl);
  iniCreateSetKey(iniProcContext, "btn_tr", INI_VALUE_HEX, map->btn_tr);
  iniCreateSetKey(iniProcContext, "btn_tl2", INI_VALUE_HEX, map->btn_tl2);
  iniCreateSetKey(iniProcContext, "btn_tr2", INI_VALUE_HEX, map->btn_tr2);

  iniCreateSetKey(iniProcContext, "btn_dpad_up", INI_VALUE_HEX, map->btn_dpad_up);
  iniCreateSetKey(iniProcContext, "btn_dpad_down", INI_VALUE_HEX, map->btn_dpad_down);
  iniCreateSetKey(iniProcContext, "btn_dpad_left", INI_VALUE_HEX, map->btn_dpad_left);
  iniCreateSetKey(iniProcContext, "btn_dpad_right", INI_VALUE_HEX, map->btn_dpad_right);

  sceIniFileProcessorFinalize(iniProcContext);
}
