/*---------------------------------------------------------------------------------
Name               : grab_keys.c
Author             : Marvin Raaijmakers
Description        : Grabs the extra function keys
Date of last change: 26-Feb-2006
History            : 26-Feb-2006 grab_keys() turns auto repeat mode on for keys
                                 with auto_repeat_on equal to TRUE
                     07-Dec-2005 Added x_error_handler()

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

#include <keytouchd.h> 


static int
*x_error_handler (	Display		*display,
			XErrorEvent	*error_event )
{
	static Boolean first_time = TRUE;
	
	if (first_time)
	{
		printf ("Warning: Not all keys can be grabbed by this program. This\n"
		        "         can be caused by another program which is already\n"
			"         grabbing these keys.\n");
		first_time = FALSE;
	}
	return (NULL);
}


void
grab_keys (	KTKeySettingsList	*key_list,
		Display			*display    )
/*
Input:
	key_list	- The list of keys to grab
	display		- Specifies the connection to the X server
Output:
	-
Returns:
	-
Desciption:
	This function establishes a passive grab on the keyboard for every key in
	key_list. For each key in key_list auto repeat mode is turned on for those
	keys with auto_repeat_on member equal to TRUE.
*/
{
	KTKeySettings		*key;
	XKeyboardControl	keyboard_control;
	
	/* Set our own error handler so that the program continues after grabbing
	 * a key failed.
	 */
	XSetErrorHandler ((XErrorHandler) x_error_handler);
	for (key = key_list->head; key != NULL; key = key->next)
	{
		XGrabKey (display, key->keycode, AnyModifier, DefaultRootWindow(display),
		          False, GrabModeAsync, GrabModeAsync);
		keyboard_control.key = key->keycode;
		keyboard_control.auto_repeat_mode = key->auto_repeat_on ? AutoRepeatModeOn : AutoRepeatModeOff;
		/* Turn for the current key auto repeat on/off */
		XChangeKeyboardControl (display, KBKey|KBAutoRepeatMode, &keyboard_control);
	}
}
