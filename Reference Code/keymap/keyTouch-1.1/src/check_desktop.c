/*-------------------------------------------------------------------------------
Name               : check_desktop.c
Author             : Marvin Raaijmakers
Description        : Finding out the currently running desktop environment
Date of last change: 25-September-2004

    Copyright (C) 2004 Marvin Raaijmakers

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    You can contact me at: marvin_raaijmakers(at)lycos(dot)nl
    (replace (at) by @ and (dot) by .)
---------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <check_desktop.h>
#include <keytouch.h>

static char is_kde_desktop(void);
static char is_gnome_desktop(void);


char
is_kde_desktop (void)
/*
input:
	-
output:
	-
returns:
	TRUE if KDE is running, otherwise FALSE
description:
	This function checks if KDE is running.
*/
{
	if ( WEXITSTATUS(system("dcop kdesktop >> /dev/null")) ) /* If KDE is NOT running */
	{
		return (FALSE);
	}
	return (TRUE);
}

char
is_gnome_desktop (void)
/*
input:
	-
output:
	-
returns:
	TRUE if GNOME is running, otherwise FALSE
description:
	This function checks if GNOME is running.
*/
{
	Display *display;
	Atom gnome_is_running;
	
	display = XOpenDisplay(NULL);
	if (display == NULL)
	{
		return (FALSE); /* Because X is not running, GNOME isn't either */
	}
	gnome_is_running = XInternAtom(display, "GNOME_SM_PROXY", True);
	XCloseDisplay (display);
	if (gnome_is_running) /* If GNOME is running */
	{
		return (TRUE);
	}
	return (FALSE);
}

DESKTOP
check_desktop (void)
/*
input:
	-
output:
	-
returns:
	The currently running desktop environment or UNKNOWN_DESKTOP is no recognized
	DE is running
description:
	This function finds out which desktop environment is running.
*/
{
	if (is_kde_desktop())
	{
		return (KDE);
	}
	else if (is_gnome_desktop())
	{
		return (GNOME);
	}
	else
	{
		return (UNKNOWN_DESKTOP);
	}
}
