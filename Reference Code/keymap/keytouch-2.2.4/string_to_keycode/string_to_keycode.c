/*---------------------------------------------------------------------------------
Name               : string_to_keycode.c
Author             : Marvin Raaijmakers
Description        : Converts a string to a kernel- or X keycode.
Date of last change: 09-Dec-2006
Changes            :
                     09-Dec-2006 Removed "BASSBOOST" from key_name, because the
                                 corresponding kernel keycode does not produce a
                                 keycode under X
                     05-May-2006 Added to key_name: "NEW", "REDO", "SEND", "REPLY",
                                 "FORWARDMAIL", "SAVE", "DOCUMENTS"
                     16-Nov-2005 Changed element 126 of keycode_table[] to 133

    Copyright (C) 2005-2006 Marvin Raaijmakers

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

    You can contact me at: marvinr(at)users(dot)sf(dot)net
    (replace (at) by @ and (dot) by .)
-----------------------------------------------------------------------------------*/
#include <strings.h>
#include <stdlib.h>

#include <string_to_keycode.h>
#include <keys.h>

/* The names that belong to the kernel keycodes of extra function keys */
char *key_name[KEY_MAX + 1] = {
	[0 ... KEY_MAX] = NULL,
	[KEY_MUTE] = "MUTE",				[KEY_VOLUMEDOWN] = "VOLUMEDOWN",
	[KEY_VOLUMEUP] = "VOLUMEUP",			[KEY_POWER] = "POWER",
	[KEY_KPEQUAL] = "KPEQUAL",			[KEY_KPPLUSMINUS] = "KPPLUSMINUS",
	[KEY_PAUSE] = "PAUSE",				[KEY_KPCOMMA] = "KPCOMMA",
	[KEY_HANGUEL] = "HANGUEL",			[KEY_HANJA] = "HANJA",
	[KEY_YEN] = "YEN",				[KEY_LEFTMETA] = "LEFTMETA",
	[KEY_RIGHTMETA] = "RIGHTMETA",			[KEY_COMPOSE] = "COMPOSE",
	[KEY_STOP] = "STOP",				[KEY_AGAIN] = "AGAIN",
	[KEY_PROPS] = "PROPS",				[KEY_UNDO] = "UNDO",
	[KEY_FRONT] = "FRONT",				[KEY_COPY] = "COPY",
	[KEY_OPEN] = "OPEN",				[KEY_PASTE] = "PASTE",
	[KEY_FIND] = "FIND",				[KEY_CUT] = "CUT",
	[KEY_HELP] = "HELP",				[KEY_MENU] = "MENU",
	[KEY_CALC] = "CALC",				[KEY_SETUP] = "SETUP",
	[KEY_SLEEP] = "SLEEP",				[KEY_WAKEUP] = "WAKEUP",
	[KEY_FILE] = "FILE",				[KEY_SENDFILE] = "SENDFILE",
	[KEY_DELETEFILE] = "DELETEFILE",		[KEY_XFER] = "XFER",
	[KEY_PROG1] = "PROG1",				[KEY_PROG2] = "PROG2",
	[KEY_WWW] = "WWW",				[KEY_MSDOS] = "MSDOS",
	[KEY_COFFEE] = "COFFEE",			[KEY_DIRECTION] = "DIRECTION",
	[KEY_CYCLEWINDOWS] = "CYCLEWINDOWS",		[KEY_MAIL] = "MAIL",
	[KEY_BOOKMARKS] = "BOOKMARKS",			[KEY_COMPUTER] = "COMPUTER",
	[KEY_BACK] = "BACK",				[KEY_FORWARD] = "FORWARD",
	[KEY_CLOSECD] = "CLOSECD",			[KEY_EJECTCD] = "EJECTCD",
	[KEY_EJECTCLOSECD] = "EJECTCLOSECD",		[KEY_NEXTSONG] = "NEXTSONG",
	[KEY_PLAYPAUSE] = "PLAYPAUSE",			[KEY_PREVIOUSSONG] = "PREVIOUSSONG",
	[KEY_STOPCD] = "STOPCD",			[KEY_RECORD] = "RECORD",
	[KEY_REWIND] = "REWIND",			[KEY_PHONE] = "PHONE",
	[KEY_ISO] = "ISO",				[KEY_CONFIG] = "CONFIG",
	[KEY_HOMEPAGE] = "HOMEPAGE",			[KEY_REFRESH] = "REFRESH",
	[KEY_EXIT] = "EXIT",				[KEY_MOVE] = "MOVE",
	[KEY_EDIT] = "EDIT",				[KEY_SCROLLUP] = "SCROLLUP",
	[KEY_SCROLLDOWN] = "SCROLLDOWN",		[KEY_KPLEFTPAREN] = "KPLEFTPAREN",
	[KEY_KPRIGHTPAREN] = "KPRIGHTPAREN",		[KEY_PLAYCD] = "PLAYCD",
	[KEY_PAUSECD] = "PAUSECD",			[KEY_PROG3] = "PROG3",
	[KEY_PROG4] = "PROG4",				[KEY_SUSPEND] = "SUSPEND",
	[KEY_CLOSE] = "CLOSE",				[KEY_PLAY] = "PLAY",
	[KEY_FASTFORWARD] = "FASTFORWARD",		[KEY_DOCUMENTS] = "DOCUMENTS",
	[KEY_PRINT] = "PRINT",				[KEY_HP] = "HP",
	[KEY_CAMERA] = "CAMERA",			[KEY_SOUND] = "SOUND",
	[KEY_QUESTION] = "QUESTION",			[KEY_EMAIL] = "EMAIL",
	[KEY_CHAT] = "CHAT",				[KEY_SEARCH] = "SEARCH",
	[KEY_CONNECT] = "CONNECT",			[KEY_FINANCE] = "FINANCE",
	[KEY_SPORT] = "SPORT",				[KEY_SHOP] = "SHOP",
	[KEY_ALTERASE] = "ALTERASE",			[KEY_CANCEL] = "CANCEL",
	[KEY_BRIGHTNESSDOWN] = "BRIGHTNESSDOWN",	[KEY_BRIGHTNESSUP] = "BRIGHTNESSUP",
	[KEY_MEDIA] = "MEDIA",				[KEY_SWITCHVIDEOMODE] = "SWITCHVIDEOMODE",
	[KEY_KBDILLUMTOGGLE] = "KBDILLUMTOGGLE",	[KEY_KBDILLUMDOWN] = "KBDILLUMDOWN",
	[KEY_KBDILLUMUP] = "KBDILLUMUP",		[KEY_MACRO] = "MACRO",
	[KEY_NEW] = "NEW",				[KEY_REDO] = "REDO",
	[KEY_SEND] = "SEND",				[KEY_REPLY] = "REPLY",
	[KEY_FORWARDMAIL] = "FORWARDMAIL",		[KEY_SAVE] = "SAVE"
};

/* Kernel keycode -> X keycode table */
const unsigned int keycode_table[NUM_KEYCODES] = {
	  0,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,
	 24,  25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,
	 40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  52,  53,  54,  55,
	 56,  57,  58,  59,  60,  61,  62,  63,  64,  65,  66,  67,  68,  69,  70,  71,
	 72,  73,  74,  75,  76,  77,  76,  79,  80,  81,  82,  83,  84,  85,  86,  87,
	 88,  89,  90,  91, 111,  221, 94,  95,  96, 211, 128, 127, 129, 208, 131, 126,
	108, 109, 112, 111, 113, 181,  97,  98,  99, 100, 102, 103, 104, 105, 106, 107,
	239, 160, 174, 176, 222, 157, 123, 110, 139, 134, 209, 210, 133, 115, 116, 117,
	232, 133, 134, 135, 140, 248, 191, 192, 122, 188, 245, 158, 161, 193, 223, 227,
	198, 199, 200, 147, 159, 151, 178, 201, 146, 203, 166, 236, 230, 235, 234, 233,
	163, 204, 253, 153, 162, 144, 164, 177, 152, 190, 208, 129, 130, 231, 209, 210,
	136, 220, 143, 246, 251, 137, 138, 182, 183, 184,  93, 184, 247, 132, 170, 219,
	249, 205, 207, 149, 150, 154, 155, 167, 168, 169, 171, 172, 173, 165, 175, 179,
	180,   0, 185, 186, 187, 118, 119, 120, 121, 229, 194, 195, 196, 197, 148, 202,
	101, 212, 237, 214, 215, 216, 217, 218, 228, 142, 213, 240, 241, 242, 243, 244,
	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};


unsigned int
string_to_kernel_keycode (char *string)
/*
Input:
	string		- The name of the keycode
Output:
	-
Global:
	key_name[]	- The list of key names
Returns:
	The kernel keycode of the key named string. If no key named string was
	found 0 is returned.
Desciption:
	This function returns the kernel keycode of the key named string. While
	comparing string to the names of keycodes the case of the characters are
	ignored.
*/
{
	unsigned int keycode;
	
	for (keycode = 0; keycode < KEY_MAX; keycode++)
	{
		if (key_name[keycode] != NULL && !strcasecmp(key_name[keycode], string))
		{
			return (keycode);
		}
	}
	return (0);
}


unsigned int
string_to_xkeycode (char *string)
/*
Input:
	string		- The name of the keycode
Output:
	-
Global:
	keycode_table	- Kernel keycode -> X keycode table
Returns:
	The X keycode of the key named string. If no key named string was
	found 0 is returned.
Desciption:
	This function returns the kernel X of the key named string. While comparing
	string to the names of keycodes the case of the characters are ignored.
*/
{
	unsigned int kernel_keycode;
	
	kernel_keycode = string_to_kernel_keycode (string);
	if (kernel_keycode < NUM_KEYCODES)
	{
		return (keycode_table[kernel_keycode]);
	}
	return (0);
}
