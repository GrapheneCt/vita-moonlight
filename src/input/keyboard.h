/*
 * This file is part of Moonlight Embedded.
 *
 * Copyright (C) 2015 Iwan Timmer
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

#define KEY_NONE 0x00 // No key pressed
#define KEY_ERR_OVF 0x01 //  Keyboard Error Roll Over - used for all slots if too many keys are pressed ("Phantom key")
 // 0x02 //  Keyboard POST Fail
 // 0x03 //  Keyboard Error Undefined
#define KEY_A 0x04 // Keyboard a and A
#define KEY_B 0x05 // Keyboard b and B
#define KEY_C 0x06 // Keyboard c and C
#define KEY_D 0x07 // Keyboard d and D
#define KEY_E 0x08 // Keyboard e and E
#define KEY_F 0x09 // Keyboard f and F
#define KEY_G 0x0a // Keyboard g and G
#define KEY_H 0x0b // Keyboard h and H
#define KEY_I 0x0c // Keyboard i and I
#define KEY_J 0x0d // Keyboard j and J
#define KEY_K 0x0e // Keyboard k and K
#define KEY_L 0x0f // Keyboard l and L
#define KEY_M 0x10 // Keyboard m and M
#define KEY_N 0x11 // Keyboard n and N
#define KEY_O 0x12 // Keyboard o and O
#define KEY_P 0x13 // Keyboard p and P
#define KEY_Q 0x14 // Keyboard q and Q
#define KEY_R 0x15 // Keyboard r and R
#define KEY_S 0x16 // Keyboard s and S
#define KEY_T 0x17 // Keyboard t and T
#define KEY_U 0x18 // Keyboard u and U
#define KEY_V 0x19 // Keyboard v and V
#define KEY_W 0x1a // Keyboard w and W
#define KEY_X 0x1b // Keyboard x and X
#define KEY_Y 0x1c // Keyboard y and Y
#define KEY_Z 0x1d // Keyboard z and Z

#define KEY_1 0x1e // Keyboard 1 and !
#define KEY_2 0x1f // Keyboard 2 and @
#define KEY_3 0x20 // Keyboard 3 and #
#define KEY_4 0x21 // Keyboard 4 and $
#define KEY_5 0x22 // Keyboard 5 and %
#define KEY_6 0x23 // Keyboard 6 and ^
#define KEY_7 0x24 // Keyboard 7 and &
#define KEY_8 0x25 // Keyboard 8 and *
#define KEY_9 0x26 // Keyboard 9 and (
#define KEY_0 0x27 // Keyboard 0 and )

#define KEY_ENTER 0x28 // Keyboard Return (ENTER)
#define KEY_ESC 0x29 // Keyboard ESCAPE
#define KEY_BACKSPACE 0x2a // Keyboard DELETE (Backspace)
#define KEY_TAB 0x2b // Keyboard Tab
#define KEY_SPACE 0x2c // Keyboard Spacebar
#define KEY_MINUS 0x2d // Keyboard - and _
#define KEY_EQUAL 0x2e // Keyboard = and +
#define KEY_LEFTBRACE 0x2f // Keyboard [ and {
#define KEY_RIGHTBRACE 0x30 // Keyboard ] and }
#define KEY_BACKSLASH 0x31 // Keyboard \ and |
#define KEY_HASHTILDE 0x32 // Keyboard Non-US # and ~
#define KEY_SEMICOLON 0x33 // Keyboard ; and :
#define KEY_APOSTROPHE 0x34 // Keyboard ' and "
#define KEY_GRAVE 0x35 // Keyboard ` and ~
#define KEY_COMMA 0x36 // Keyboard , and <
#define KEY_DOT 0x37 // Keyboard . and >
#define KEY_SLASH 0x38 // Keyboard / and ?
#define KEY_CAPSLOCK 0x39 // Keyboard Caps Lock

#define KEY_F1 0x3a // Keyboard F1
#define KEY_F2 0x3b // Keyboard F2
#define KEY_F3 0x3c // Keyboard F3
#define KEY_F4 0x3d // Keyboard F4
#define KEY_F5 0x3e // Keyboard F5
#define KEY_F6 0x3f // Keyboard F6
#define KEY_F7 0x40 // Keyboard F7
#define KEY_F8 0x41 // Keyboard F8
#define KEY_F9 0x42 // Keyboard F9
#define KEY_F10 0x43 // Keyboard F10
#define KEY_F11 0x44 // Keyboard F11
#define KEY_F12 0x45 // Keyboard F12

#define KEY_SYSRQ 0x46 // Keyboard Print Screen
#define KEY_SCROLLLOCK 0x47 // Keyboard Scroll Lock
#define KEY_PAUSE 0x48 // Keyboard Pause
#define KEY_INSERT 0x49 // Keyboard Insert
#define KEY_HOME 0x4a // Keyboard Home
#define KEY_PAGEUP 0x4b // Keyboard Page Up
#define KEY_DELETE 0x4c // Keyboard Delete Forward
#define KEY_END 0x4d // Keyboard End
#define KEY_PAGEDOWN 0x4e // Keyboard Page Down
#define KEY_RIGHT 0x4f // Keyboard Right Arrow
#define KEY_LEFT 0x50 // Keyboard Left Arrow
#define KEY_DOWN 0x51 // Keyboard Down Arrow
#define KEY_UP 0x52 // Keyboard Up Arrow

#define KEY_NUMLOCK 0x53 // Keyboard Num Lock and Clear
#define KEY_KPSLASH 0x54 // Keypad /
#define KEY_KPASTERISK 0x55 // Keypad *
#define KEY_KPMINUS 0x56 // Keypad -
#define KEY_KPPLUS 0x57 // Keypad +
#define KEY_KPENTER 0x58 // Keypad ENTER
#define KEY_KP1 0x59 // Keypad 1 and End
#define KEY_KP2 0x5a // Keypad 2 and Down Arrow
#define KEY_KP3 0x5b // Keypad 3 and PageDn
#define KEY_KP4 0x5c // Keypad 4 and Left Arrow
#define KEY_KP5 0x5d // Keypad 5
#define KEY_KP6 0x5e // Keypad 6 and Right Arrow
#define KEY_KP7 0x5f // Keypad 7 and Home
#define KEY_KP8 0x60 // Keypad 8 and Up Arrow
#define KEY_KP9 0x61 // Keypad 9 and Page Up
#define KEY_KP0 0x62 // Keypad 0 and Insert
#define KEY_KPDOT 0x63 // Keypad . and Delete

#define KEY_102ND 0x64 // Keyboard Non-US \ and |
#define KEY_COMPOSE 0x65 // Keyboard Application
#define KEY_POWER 0x66 // Keyboard Power
#define KEY_KPEQUAL 0x67 // Keypad =

#define KEY_F13 0x68 // Keyboard F13
#define KEY_F14 0x69 // Keyboard F14
#define KEY_F15 0x6a // Keyboard F15
#define KEY_F16 0x6b // Keyboard F16
#define KEY_F17 0x6c // Keyboard F17
#define KEY_F18 0x6d // Keyboard F18
#define KEY_F19 0x6e // Keyboard F19
#define KEY_F20 0x6f // Keyboard F20
#define KEY_F21 0x70 // Keyboard F21
#define KEY_F22 0x71 // Keyboard F22
#define KEY_F23 0x72 // Keyboard F23
#define KEY_F24 0x73 // Keyboard F24

#define KEY_OPEN 0x74 // Keyboard Execute
#define KEY_HELP 0x75 // Keyboard Help
#define KEY_PROPS 0x76 // Keyboard Menu
#define KEY_FRONT 0x77 // Keyboard Select
#define KEY_STOP 0x78 // Keyboard Stop
#define KEY_AGAIN 0x79 // Keyboard Again
#define KEY_UNDO 0x7a // Keyboard Undo
#define KEY_CUT 0x7b // Keyboard Cut
#define KEY_COPY 0x7c // Keyboard Copy
#define KEY_PASTE 0x7d // Keyboard Paste
#define KEY_FIND 0x7e // Keyboard Find
#define KEY_MUTE 0x7f // Keyboard Mute
#define KEY_VOLUMEUP 0x80 // Keyboard Volume Up
#define KEY_VOLUMEDOWN 0x81 // Keyboard Volume Down
// 0x82  Keyboard Locking Caps Lock
// 0x83  Keyboard Locking Num Lock
// 0x84  Keyboard Locking Scroll Lock
#define KEY_KPCOMMA 0x85 // Keypad Comma
// 0x86  Keypad Equal Sign
#define KEY_RO 0x87 // Keyboard International1
#define KEY_KATAKANAHIRAGANA 0x88 // Keyboard International2
#define KEY_YEN 0x89 // Keyboard International3
#define KEY_HENKAN 0x8a // Keyboard International4
#define KEY_MUHENKAN 0x8b // Keyboard International5
#define KEY_KPJPCOMMA 0x8c // Keyboard International6
// 0x8d  Keyboard International7
// 0x8e  Keyboard International8
// 0x8f  Keyboard International9
#define KEY_HANGEUL 0x90 // Keyboard LANG1
#define KEY_HANJA 0x91 // Keyboard LANG2
#define KEY_KATAKANA 0x92 // Keyboard LANG3
#define KEY_HIRAGANA 0x93 // Keyboard LANG4
#define KEY_ZENKAKUHANKAKU 0x94 // Keyboard LANG5

static SceUInt16 kbNames[256];

static const short keyCodes[] = {
  0, //VK_RESERVED
  0x1B, //VK_ESCAPE
  0x31, //VK_1
  0x32, //VK_2
  0x33, //VK_3
  0x34, //VK_4
  0x35, //VK_5
  0x36, //VK_6
  0x37, //VK_7
  0x38, //VK_8
  0x39, //VK_9
  0x30, //VK_0
  0xBD, //VK_MINUS
  0xBB, //VK_EQUALS
  0x08, //VK_BACK_SPACE
  0x09, //VK_TAB
  0x51, //VK_Q
  0x57, //VK_W
  0x45, //VK_E
  0x52, //VK_R
  0x54, //VK_T
  0x59, //VK_Y
  0x55, //VK_U
  0x49, //VK_I
  0x4F, //VK_O
  0x50, //VK_P
  0xDB, //VK_BRACELEFT
  0xDD, //VK_BRACERIGHT
  0x0D, //VK_ENTER
  0x11, //VK_CONTROL Left control
  0x41, //VK_A
  0x53, //VK_S
  0x44, //VK_D
  0x46, //VK_F
  0x47, //VK_G
  0x48, //VK_H
  0x4A, //VK_J
  0x4B, //VK_K
  0x4C, //VK_L
  0xBA, //VK_SEMICOLON
  0xDE, //VK_APOSTROPHE
  0xC0, //VK_GRAVE
  0x10, //VK_SHIFT Left shift
  0xDC, //VK_BACK_SLASH
  0x5A, //VK_Z
  0x58, //VK_X
  0x43, //VK_C
  0x56, //VK_V
  0x42, //VK_B
  0x4E, //VK_N
  0x4D, //VK_M
  0xBC, //VK_COMMA
  0xBE, //VK_DOT
  0xBF, //VK_SLASH
  0x10, //VK_SHIFT Right shift
  0, //VK_KPASTERISK
  0x11, //VK_ALT Left alt
  0x20, //VK_SPACE
  0x14, //VK_CAPS_LOCK
  0x70, //VK_F1
  0x71, //VK_F2
  0x72, //VK_F3
  0x73, //VK_F4
  0x74, //VK_F5
  0x75, //VK_F6
  0x76, //VK_F7
  0x77, //VK_F8
  0x78, //VK_F9
  0x79, //VK_F10
  0x90, //VK_NUM_LOCK
  0x91, //VK_SCROLL_LOCK
  0x67, //VK_NUMPAD7
  0x68, //VK_NUMPAD8
  0x69, //VK_NUMPAD9
  0, //VK_NUMPAD_MINUS
  0x64, //VK_NUMPAD4
  0x65, //VK_NUMPAD5
  0x66, //VK_NUMPAD6
  0, //VK_NUMPADPLUS
  0x61, //VK_NUMPAD1
  0x62, //VK_NUMPAD2
  0x63, //VK_NUMPAD3
  0x60, //VK_NUMPAD0
  0, //KeyEvent.VK_NUMPADDOT
  0,
  0, //KeyEvent.VK_ZENKAKUHANKAKU
  0, //KeyEvent.VK_102ND
  0x7A, //VK_F11
  0x7B, //VK_F12
  0, //KeyEvent.VK_RO
  0xF1, //VK_KATAKANA
  0xF2, //VK_HIRAGANA
  0, //VK_HENKAN
  0, //VK_KATAKANAHIRAGANA
  0, //VK_MUHENKAN
  0, //VK_KPJPCOMMA
  0, //VK_KPENTER
  0x11, //VK_CONTROL Right ctrl
  0, //VK_KPSLASH
  0, //VK_SYSRQ
  0x11, //VK_ALT Right alt
  0, //KeyEvent.VK_LINEFEED
  0x24, //VK_HOME
  0x26, //VK_UP
  0x21, //VK_PAGE_UP
  0x25, //VK_LEFT
  0x27, //VK_RIGHT
  0x23, //VK_END
  0x28, //VK_DOWN
  0x22, //VK_PAGE_DOWN
  0x9B, //VK_INSERT
  0x2E, //VK_DELETE
  0, //VK_MACRO
  0, //VK_MUTE
  0, //VK_VOLUMEDOWN
  0, //VK_VOLUMEUP
  0, //VK_POWER SC System Power Down
  0, //VK_KPEQUAL
  0, //VK_KPPLUSMINUS
  0x13, //VK_PAUSE
  0, //VK_SCALE AL Compiz Scale (Expose)
};

void vitainput_kb_init_names();
