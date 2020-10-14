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

#include <Limelight.h>

// #include <dlfcn.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

typedef enum MoonlightVitaStateEVF {
	FLAG_MOONLIGHT_IS_FG = 1,
	FLAG_MOONLIGHT_ACTIVE_VIDEO_THREAD = 2,
	FLAG_MOONLIGHT_ACTIVE_AUDIO_THREAD = 4,
	FLAG_MOONLIGHT_ACTIVE_INPUT_THREAD = 8,
	FLAG_MOONLIGHT_ACTIVE_PACER_THREAD = 16,
	FLAG_MOONLIGHT_ACTIVE_POWER_THREAD = 32,
	FLAG_MOONLIGHT_IS_IME = 64
} MoonlightVitaStateEVF;

enum platform { VITA };

enum platform platform_check(char*);
PDECODER_RENDERER_CALLBACKS platform_get_video(enum platform system);
PAUDIO_RENDERER_CALLBACKS platform_get_audio(enum platform system);
bool platform_supports_hevc(enum platform system);

extern DECODER_RENDERER_CALLBACKS decoder_callbacks_vita;
