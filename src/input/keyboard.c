/*
 * This file is part of Moonlight Embedded for PS Vita.
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

#include <psp2/types.h>
#include "keyboard.h"

void vitainput_kb_init_names()
{
	kbNames[KEY_A] = 0x41; //0x04  Keyboard a and A
	kbNames[KEY_B] = 0x42; //0x05 // Keyboard b and B
	kbNames[KEY_C] = 0x43; //0x06 // Keyboard c and C
	kbNames[KEY_D] = 0x44; //0x07 // Keyboard d and D
	kbNames[KEY_E] = 0x45; //0x08 // Keyboard e and E
	kbNames[KEY_F] = 0x46; //0x09 // Keyboard f and F
	kbNames[KEY_G] = 0x47; //0x0a // Keyboard g and G
	kbNames[KEY_H] = 0x48; //0x0b // Keyboard h and H
	kbNames[KEY_I] = 0x49; //0x0c // Keyboard i and I
	kbNames[KEY_J] = 0x4A; //0x0d // Keyboard j and J
	kbNames[KEY_K] = 0x4B; //0x0e // Keyboard k and K
	kbNames[KEY_L] = 0x4C; //0x0f // Keyboard l and L
	kbNames[KEY_M] = 0x4D; //0x10 // Keyboard m and M
	kbNames[KEY_N] = 0x4E; //0x11 // Keyboard n and N
	kbNames[KEY_O] = 0x4F; //0x12 // Keyboard o and O
	kbNames[KEY_P] = 0x50; //0x13 // Keyboard p and P
	kbNames[KEY_Q] = 0x51; //0x14 // Keyboard q and Q
	kbNames[KEY_R] = 0x52; //0x15 // Keyboard r and R
	kbNames[KEY_S] = 0x53; //0x16 // Keyboard s and S
	kbNames[KEY_T] = 0x54; //0x17 // Keyboard t and T
	kbNames[KEY_U] = 0x55; //0x18 // Keyboard u and U
	kbNames[KEY_V] = 0x56; //0x19 // Keyboard v and V
	kbNames[KEY_W] = 0x57; //0x1a // Keyboard w and W
	kbNames[KEY_X] = 0x58; //0x1b // Keyboard x and X
	kbNames[KEY_Y] = 0x59; //0x1c // Keyboard y and Y
	kbNames[KEY_Z] = 0x60; //0x1d // Keyboard z and Z

	kbNames[KEY_1] = 0x31; //0x1e // Keyboard 1 and !
	kbNames[KEY_2] = 0x32; //0x1f // Keyboard 2 and @
	kbNames[KEY_3] = 0x33; //0x20 // Keyboard 3 and #
	kbNames[KEY_4] = 0x34; //0x21 // Keyboard 4 and $
	kbNames[KEY_5] = 0x35; //0x22 // Keyboard 5 and %
	kbNames[KEY_6] = 0x36; //0x23 // Keyboard 6 and ^
	kbNames[KEY_7] = 0x37; //0x24 // Keyboard 7 and &
	kbNames[KEY_8] = 0x38; //0x25 // Keyboard 8 and *
	kbNames[KEY_9] = 0x39; //0x26 // Keyboard 9 and (
	kbNames[KEY_0] = 0x30; //0x27 // Keyboard 0 and )

	kbNames[KEY_ENTER] = 0xA; //0x28 // Keyboard Return (ENTER)
	kbNames[KEY_ESC] = 0x1B; //0x29 // Keyboard ESCAPE
	kbNames[KEY_BACKSPACE] = 0x8; //0x2a // Keyboard DELETE (Backspace)
	kbNames[KEY_TAB] = 0x9; //0x2b // Keyboard Tab
	kbNames[KEY_SPACE] = 0x20; //0x2c // Keyboard Spacebar
	kbNames[KEY_MINUS] = 0x2D; //0x2d // Keyboard - and _
	kbNames[KEY_EQUAL] = 0xBB; //0x2e // Keyboard = and +

	kbNames[KEY_LEFTBRACE] = 0x5B; //0x2f // Keyboard [ and {
	kbNames[KEY_RIGHTBRACE] = 0x5D; //0x30 // Keyboard ] and }

	kbNames[KEY_BACKSLASH] = 0x5C; //0x31 // Keyboard \ and | not in spanish

	kbNames[KEY_HASHTILDE] = 0x23; //0x32 // Keyboard Non-US # and ~ in spanish ?

	kbNames[KEY_SEMICOLON] = 0x3B; //0x33 // Keyboard ; and :

	kbNames[KEY_APOSTROPHE] = 0xDE; //0x34 // Keyboard ' and "

	kbNames[KEY_GRAVE] = 0x60; //0x35 // Keyboard ` and ~ in spanish ?

	kbNames[KEY_COMMA] = 0x2C; //0x36 // Keyboard , and <

	kbNames[KEY_DOT] = 0x2E; //0x37 // Keyboard . and >

	kbNames[KEY_SLASH] = 0x2F; //0x38 // Keyboard / and ? in spanish -
}