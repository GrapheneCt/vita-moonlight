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

#include "ini.h"
#include "ini_file_processor_c.h"

#include <stdlib.h>

#include <psp2/kernel/clib.h>
#include <psp2/sysmodule.h>

int iniCreateSetKey(void* context, const char* key, IniValueType valueType, int inVal)
{
	int ret = -1;
	char value[64];

	switch (valueType) {
	case INI_VALUE_INT:
		sceClibMemset(value, 0, 64);
		sceClibSnprintf(value, 64, "%d", inVal);
		ret = sceIniFileProcessorAddKey(context, key, value);
		break;
	case INI_VALUE_HEX:
		sceClibMemset(value, 0, 64);
		sceClibSnprintf(value, 64, "%x", inVal);
		ret = sceIniFileProcessorAddKey(context, key, value);
		break;
	case INI_VALUE_BOOL:
		if (inVal)
			ret = sceIniFileProcessorAddKey(context, key, "true");
		else
			ret = sceIniFileProcessorAddKey(context, key, "false");
		break;
	}

	return 0;
}

int iniGetStringByKey(void* context, const char* key, char** string)
{
	void* outBuf = malloc(1024);
	sceClibMemset(outBuf, 0, 1024);

	SceInt32 ret = sceIniFileProcessorFindValueByKey(context, key, outBuf, 1024, 0);
	if (ret < 0)
		return ret;

	*string = (char*)outBuf;

	return 0;
}

int iniGetValueByKey(void* context, const char* key, IniValueType valueType, int base, void* out)
{
	int ret = 0;
	float fret = 0;
	char outBuf[256];
	sceClibMemset(outBuf, 0, 256);

	ret = sceIniFileProcessorFindValueByKey(context, key, outBuf, 256, 0);
	if (ret < 0) {
		*(int *)out = 0;
		return ret;
	}

	char* retPtr;

	switch (valueType) {
	case INI_VALUE_INT:
		ret = (int)strtol(outBuf, &retPtr, base);
		*(int *)out = ret;
		break;
	case INI_VALUE_FLOAT:
		fret = (float)strtof(outBuf, &retPtr);
		*(float *)out = fret;
		break;
	case INI_VALUE_BOOL:
		if (!sceClibStrncmp(outBuf, "true", 4))
			*(int *)out = 1;
		else
			*(int *)out = 0;
		break;
	}

	return 0;
}