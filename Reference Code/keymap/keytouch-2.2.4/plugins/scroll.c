/*---------------------------------------------------------------------------------
Name               : scroll.c
Author             : Marvin Raaijmakers
Description        : Plugin for keyTouch that sends scroll events.
Date of last change: 17-Dec-2005
History            : 17-Dec-2005 Created this plugin

    Copyright (C) 2005 Marvin Raaijmakers

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
#include <X11/extensions/XTest.h>

#include <plugin.h>

void scroll_up (KTPreferences *preferences);
void scroll_down (KTPreferences *preferences);
void scroll_left (KTPreferences *preferences);
void scroll_right (KTPreferences *preferences);

KeytouchPlugin plugin_struct = {
	{"Scroll", "Marvin Raaijmakers", "GPL 2", "1.0",
	  "Sends scroll events."},
	"scroll.so",
	4,
	{{"Up",    KTPluginFunctionType_Function, {.function = scroll_up}},
	 {"Down",  KTPluginFunctionType_Function, {.function = scroll_down}},
	 {"Left",  KTPluginFunctionType_Function, {.function = scroll_left}},
	 {"Right", KTPluginFunctionType_Function, {.function = scroll_right}}
	}
};

static void
emulate_button (unsigned int button)
/*
Input:
	button	- The button event to send
Description:
	This function sends a button press and release event to the the default X
	display.
*/
{
	Display	*display;
	int	i;
	
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
		XCloseDisplay (display);
		return;
	}
	/* Send the button press event */
	XTestFakeButtonEvent (display, button, True, CurrentTime);
	/* Send the button release event */
        XTestFakeButtonEvent (display, button, False, CurrentTime);
	XCloseDisplay (display);
}


void
scroll_up (KTPreferences *preferences)
{
	emulate_button (4);
}

void
scroll_down (KTPreferences *preferences)
{
	emulate_button (5);
}

void
scroll_left (KTPreferences *preferences)
{
	emulate_button (6);
}

void
scroll_right (KTPreferences *preferences)
{
	emulate_button (7);
}
