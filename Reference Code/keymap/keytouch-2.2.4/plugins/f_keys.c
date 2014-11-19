/*---------------------------------------------------------------------------------
Name               : f_keys.c
Author             : Marvin Raaijmakers
Description        : Plugin for keyTouch that emulates key press events of function
                     keys F1 - F12.
Date of last change: 07-May-2006
History            : 17-Dec-2005 Created this plugin

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
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>

#include <plugin.h>


static void emulate_key (KeySym keysym);

#define declare_F_key(n) void F ## n (KTPreferences *preferences) { emulate_key (XK_F ## n); }
declare_F_key(1)
declare_F_key(2)
declare_F_key(3)
declare_F_key(4)
declare_F_key(5)
declare_F_key(6)
declare_F_key(7)
declare_F_key(8)
declare_F_key(9)
declare_F_key(10)
declare_F_key(11)
declare_F_key(12)

KeytouchPlugin plugin_struct = {
	{"F-keys", "Marvin Raaijmakers", "GPL 2", "1.0",
	  "Emulates key press events of function keys F1 - F12."},
	"f_keys.so",
	12,
	{{"F1",  KTPluginFunctionType_Function, {.function = F1}},
	 {"F2",  KTPluginFunctionType_Function, {.function = F2}},
	 {"F3",  KTPluginFunctionType_Function, {.function = F3}},
	 {"F4",  KTPluginFunctionType_Function, {.function = F4}},
	 {"F5",  KTPluginFunctionType_Function, {.function = F5}},
	 {"F6",  KTPluginFunctionType_Function, {.function = F6}},
	 {"F7",  KTPluginFunctionType_Function, {.function = F7}},
	 {"F8",  KTPluginFunctionType_Function, {.function = F8}},
	 {"F9",  KTPluginFunctionType_Function, {.function = F9}},
	 {"F10", KTPluginFunctionType_Function, {.function = F10}},
	 {"F11", KTPluginFunctionType_Function, {.function = F11}},
	 {"F12", KTPluginFunctionType_Function, {.function = F12}}
	}
};


void
emulate_key (KeySym keysym)
/*
Input:
	keysym	- The keysymbol of the key to emulate
Description:
	This function sends a key press and release event of the key with symbol
	keysym to the the default X display.
*/
{
	Display	*display;
	int	i;
	KeyCode	keycode;
	
	/* Connect to the server specified in the DISPLAY evironment variable */
	display = XOpenDisplay(NULL);
	if (display == NULL)
	{
		fprintf (stderr, "Cannot connect to X server %s.\n", XDisplayName(NULL));
		return;
	}
	if (!XQueryExtension (display, "XTEST", &i, &i, &i))
	{
		fprintf (stderr, "Extension XTest unavailable on display '%s'.\n", XDisplayName(NULL));
	}
	else
	{
		keycode = XKeysymToKeycode(display, keysym);
		/* Send the key press event */
		XTestFakeKeyEvent(display, keycode, True, 0);
		/* Send the key release event */
		XTestFakeKeyEvent(display, keycode, False, 0);
		XSync (display, False);
	}
	XCloseDisplay (display);
}
