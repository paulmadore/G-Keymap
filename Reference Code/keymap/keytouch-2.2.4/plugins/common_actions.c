/*---------------------------------------------------------------------------------
Name               : common_actions.c
Author             : Marvin Raaijmakers
Description        : Plugin for keyTouch that provides common actions.
Date of last change: 07-May-2006
History            :
                     07-May-2006 Added function "Close Window" (close_window)
                     16-Dec-2005 Added key functions Cut, Copy, Paste and Select
                                 All

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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>

#include <plugin.h>


static void close_window (KTPreferences *preferences);


KTPluginKey key_new_file[] = {
	{{NULL, TRUE}, ControlMask, "N"}
};

KTPluginKey key_open[] = {
	{{NULL, TRUE}, ControlMask, "O"}
};

KTPluginKey key_save[] = {
	{{NULL, TRUE}, ControlMask, "S"}
};

KTPluginKey key_print[] = {
	{{NULL, TRUE}, ControlMask, "P"}
};

KTPluginKey key_undo[] = {
	{{NULL, TRUE}, ControlMask, "Z"}
};

KTPluginKey key_redo[] = {
	{{NULL, TRUE}, ControlMask|ShiftMask, "Z"}
};

KTPluginKey key_find[] = {
	{{NULL, TRUE}, ControlMask, "F"}
};

KTPluginKey key_copy[] = {
	{{NULL, TRUE}, ControlMask, "C"}
};

KTPluginKey key_paste[] = {
	{{NULL, TRUE}, ControlMask, "V"}
};

KTPluginKey key_cut[] = {
	{{NULL, TRUE}, ControlMask, "X"}
};

KTPluginKey key_select_all[] = {
	{{NULL, TRUE}, ControlMask, "A"}
};


KeytouchPlugin plugin_struct = {
	{"Common actions", "Marvin Raaijmakers", "GPL 2", "1.2",
	  "Preforms common actions."},
	"common_actions.so",
	12,
	{{"New (file)",   KTPluginFunctionType_Key, {.key_function = {1, key_new_file}}},
	 {"Open",         KTPluginFunctionType_Key, {.key_function = {1, key_open}}},
	 {"Save",         KTPluginFunctionType_Key, {.key_function = {1, key_save}}},
	 {"Print",        KTPluginFunctionType_Key, {.key_function = {1, key_print}}},
	 {"Undo",         KTPluginFunctionType_Key, {.key_function = {1, key_undo}}},
	 {"Redo",         KTPluginFunctionType_Key, {.key_function = {1, key_redo}}},
	 {"Find",         KTPluginFunctionType_Key, {.key_function = {1, key_find}}},
	 {"Cut",          KTPluginFunctionType_Key, {.key_function = {1, key_cut}}},
	 {"Copy",         KTPluginFunctionType_Key, {.key_function = {1, key_copy}}},
	 {"Paste",        KTPluginFunctionType_Key, {.key_function = {1, key_paste}}},
	 {"Select All",   KTPluginFunctionType_Key, {.key_function = {1, key_select_all}}},
	 {"Close Window", KTPluginFunctionType_Function, {.function = close_window}}
	}
};


void
close_window (KTPreferences *preferences)
{
	Display	*display;
	int	i;
	KeyCode	alt_keycode,
		f4_keycode;
	
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
		alt_keycode = XKeysymToKeycode(display, XK_Alt_L);
		f4_keycode = XKeysymToKeycode(display, XK_F4);
		
		/* Send the key press events: */
		XTestFakeKeyEvent(display, alt_keycode, True, 0);
		XTestFakeKeyEvent(display, f4_keycode, True, 0);
		/* Send the key release events: */
		XTestFakeKeyEvent(display, f4_keycode, False, 0);
		XTestFakeKeyEvent(display, alt_keycode, False, 0);
		
		XSync (display, False);
	}
	XCloseDisplay (display);
}
