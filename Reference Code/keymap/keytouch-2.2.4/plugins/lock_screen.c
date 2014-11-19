/*---------------------------------------------------------------------------------
Name               : lock_screen.c
Author             : Marvin Raaijmakers
Description        : Plugin for keyTouch that locks the screen.
Date of last change: 05-Aug-2006
History            : 05-Aug-2006 Added support for gnome-screensaver

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
#include <unistd.h>

#include <check_desktop.h>
#include <plugin.h>

void lock_screen (KTPreferences *preferences);


KeytouchPlugin plugin_struct = {
	{"Lock Screen", "Marvin Raaijmakers", "GPL 2", "1.1", "Locks the screen."},
	"lock_screen.so",
	1,
	{{"Lock Screen", KTPluginFunctionType_Function, {.function = lock_screen}}}
};


void
lock_screen (KTPreferences *preferences)
{
	char *lockscreen;
	DESKTOP desktop;
	
	desktop = check_desktop();
	if (desktop == KDE)
	{
		lockscreen = "kdesktop_lock --forcelock &";
	}
	else if (desktop == GNOME && !system( "which gnome-screensaver-command" ))
	{
		lockscreen = "gnome-screensaver-command --lock &";
	}
	else
	{
		lockscreen = "(xscreensaver &) && (xscreensaver-command -lock &)";
	}
	if (fork() == 0)
	{
		execlp ("sh", "sh", "-c", lockscreen, NULL);
		exit (EXIT_SUCCESS);
	}
}
