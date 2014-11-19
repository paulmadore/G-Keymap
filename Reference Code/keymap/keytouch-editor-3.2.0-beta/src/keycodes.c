/*---------------------------------------------------------------------------------
Name               : keycodes.c
Author             : Marvin Raaijmakers
Description        : Contains an array of all keycodes and a function to find a
                     keycode in the array.
Date of last change: 24-Jun-2007
History            : 24-Jun-2007 Removed KEY_BASSBOOST from visible_keycodes[],
                                 because it was also removed from the key_name[]
                                 array in string_to_keycode.c
                     01-Jul-2006 - Removed array keycode_string[], because
                                   kernel_keycode_to_string() in
                                   string_to_keycode.h will now be used.
                                 - Added to visible_keycodes: KEY_NEW, KEY_REDO,
                                   KEY_SEND, KEY_REPLY, KEY_FORWARDMAIL, KEY_SAVE,
                                   KEY_DOCUMENTS
                     05-Apr-2006 Changed array of "strings" keycode[] to an array
                                 of keycodes (unsigned ints) visible_keycodes[].


    Copyright (C) 2006-2007 Marvin Raaijmakers

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
#include <stdlib.h>
#include <string.h>

#include <string_to_keycode.h>


const unsigned int visible_keycodes[] = {
	KEY_AGAIN,		KEY_ALTERASE,
	KEY_BACK,
	KEY_BOOKMARKS,		KEY_BRIGHTNESSDOWN,
	KEY_BRIGHTNESSUP,	KEY_CALC,
	KEY_CAMERA,		KEY_CANCEL,
	KEY_CHAT,		KEY_CLOSE,
	KEY_CLOSECD,		KEY_COFFEE,
	KEY_COMPOSE,		KEY_COMPUTER,
	KEY_CONFIG,		KEY_CONNECT,
	KEY_COPY,		KEY_CUT,
	KEY_CYCLEWINDOWS,	KEY_DELETEFILE,
	KEY_DIRECTION,		KEY_DOCUMENTS,
	KEY_EDIT,		KEY_EJECTCD,
	KEY_EJECTCLOSECD,	KEY_EMAIL,
	KEY_EXIT,		KEY_FASTFORWARD,
	KEY_FILE,		KEY_FINANCE,
	KEY_FIND,		KEY_FORWARD,
	KEY_FORWARDMAIL,	KEY_FRONT,
	KEY_HANGUEL,		KEY_HANJA,
	KEY_HELP,		KEY_HOMEPAGE,
	KEY_HP,			KEY_ISO,
	KEY_KBDILLUMDOWN,	KEY_KBDILLUMTOGGLE,
	KEY_KBDILLUMUP,		KEY_KPCOMMA,
	KEY_KPEQUAL,		KEY_KPLEFTPAREN,
	KEY_KPPLUSMINUS,	KEY_KPRIGHTPAREN,
	KEY_LEFTMETA,		KEY_MACRO,
	KEY_MAIL,		KEY_MEDIA,
	KEY_MENU,		KEY_MOVE,
	KEY_MSDOS,		KEY_MUTE,
	KEY_NEW,		KEY_NEXTSONG,
	KEY_OPEN,		KEY_PASTE,
	KEY_PAUSE,		KEY_PAUSECD,
	KEY_PHONE,		KEY_PLAY,
	KEY_PLAYCD,		KEY_PLAYPAUSE,
	KEY_POWER,		KEY_PREVIOUSSONG,
	KEY_PRINT,		KEY_PROG1,
	KEY_PROG2,		KEY_PROG3,
	KEY_PROG4,		KEY_PROPS,
	KEY_QUESTION,		KEY_RECORD,
	KEY_REDO,		KEY_REFRESH,
	KEY_REPLY,		KEY_REWIND,
	KEY_RIGHTMETA,		KEY_SAVE,
	KEY_SCROLLDOWN,		KEY_SCROLLUP,
	KEY_SEARCH,		KEY_SEND,
	KEY_SENDFILE,		KEY_SETUP,
	KEY_SHOP,		KEY_SLEEP,
	KEY_SOUND,		KEY_SPORT,
	KEY_STOP,		KEY_STOPCD,
	KEY_SUSPEND,		KEY_SWITCHVIDEOMODE,
	KEY_UNDO,		KEY_VOLUMEDOWN,
	KEY_VOLUMEUP,		KEY_WAKEUP,
	KEY_WWW,		KEY_XFER,
	KEY_YEN,		0
};


unsigned int
string2keycode (const char *keycode_str)
/*
Input:
	keycode_str	- The keycode string to find
Global:
	visible_keycodes
Returns:
	The keycode of that belongs to the keycode string 'keycode_str'. If no such
	keycode exists, 0 will be returned.
*/
{
	int keycode;
	
	for (keycode = 1; keycode < KEY_MAX; keycode++)
	{
		if (kernel_keycode_to_string( keycode ) && !strcmp(kernel_keycode_to_string( keycode ), keycode_str))
		{
			return (keycode);
		}
	}
	return 0;
}


int
string2keycode_index (const char *keycode_str)
/*
Input:
	keycode_str	- The keycode to find
Global:
	visible_keycodes
Returns:
	The index in the visible_keycodes array of the keycode whose string is equal
	to 'keycode_str'. -1 will be returned if no keycode string equal to
	'keycode_str' exists.
*/
{
	int index;
	
	for (index = 0;
	     visible_keycodes[index] && (kernel_keycode_to_string( visible_keycodes[index] ) == NULL ||
	      strcmp(kernel_keycode_to_string( visible_keycodes[index] ), keycode_str));
	     index++)
		; /* NULL Statement */
	if (kernel_keycode_to_string( visible_keycodes[index] ))
	{
		return index;
	}
	return -1;
}


int
keycode2index (unsigned int keycode)
/*
Input:
	keycode			- The keycode to find
Global:
	visible_keycodes	- The array to search through for the keycode
Returns:
	The index of the entry in the visible_keycodes array whose keycode is equal
	to 'keycode'. -1 is returned if no such entry exists.
*/
{
	int index;
	
	if (keycode)
	{
		for (index = 0;
		     visible_keycodes[index] && visible_keycodes[index] != keycode;
		     index++)
			; /* NULL Statement */
		if (visible_keycodes[index] == keycode)
		{
			return index;
		}
	}
	return -1;
}
