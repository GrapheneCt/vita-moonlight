/*
 * This file is part of Moonlight Embedded.
 *
 * Copyright (C) 2020 Graphene
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

typedef enum IniValueType {
	INI_VALUE_INT,
	INI_VALUE_HEX,
	INI_VALUE_FLOAT,
	INI_VALUE_BOOL
} IniValueType;

int iniCreateSetKey(void* context, const char* key, IniValueType valueType, int inVal);
int iniGetStringByKey(void* context, const char* key, char** string);
int iniGetValueByKey(void* context, const char* key, IniValueType valueType, int base, void* out);