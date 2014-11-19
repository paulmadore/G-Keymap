/*---------------------------------------------------------------------------------
Name               : check_desktop.c
Author             : Marvin Raaijmakers
Description        : Identifies the currently running desktop environment
Date of last change: 24-Sep-2006
History            : 24-Sep-2006 is_gnome_desktop() now also returns TRUE when
                                 the application "gnome-panel" is running.
                     03-Jun-2006 is_gnome_desktop() now checks if the
                                 GNOME_DESKTOP_SESSION_ID env variable exists
                     26-Jan-2006 Changed "dcop kdesktop >> /dev/null" to
                                 "dcop kdesktop &> /dev/null"

    Copyright (C) 2004-2006 Marvin Raaijmakers

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
#include <stdlib.h>
#include <wait.h>
#include <check_desktop.h>

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
	if ( WEXITSTATUS(system("dcop kdesktop &> /dev/null")) ) /* If KDE is NOT running */
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
	return (getenv("GNOME_DESKTOP_SESSION_ID") != NULL || !WEXITSTATUS( system("ps -C \"gnome-panel\" &> /dev/null") ));
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
