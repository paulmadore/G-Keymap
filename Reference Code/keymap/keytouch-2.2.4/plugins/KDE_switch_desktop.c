/*---------------------------------------------------------------------------------
Name               : KDE_switch_desktop.c
Author             : Marvin Raaijmakers
Description        : Plugin for keyTouch that switches to another desktop in KDE.
Date of last change: 16-Dec-2005
History            : 16-Dec-2005 Created this plugin

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
#include <stdlib.h>
#include <unistd.h>

#include <plugin.h>

void next_desktop (KTPreferences *preferences);
void previous_desktop (KTPreferences *preferences);
void desktop1 (KTPreferences *preferences);
void desktop2 (KTPreferences *preferences);
void desktop3 (KTPreferences *preferences);
void desktop4 (KTPreferences *preferences);

KeytouchPlugin plugin_struct = {
	{"KDE Switch Desktop", "Marvin Raaijmakers", "GPL 2", "1.0",
	  "Switches to another desktop in KDE."},
	"KDE_switch_desktop.so",
	6,
	{{"Next Desktop",     KTPluginFunctionType_Function, {.function = next_desktop}},
	 {"Previous Desktop", KTPluginFunctionType_Function, {.function = previous_desktop}},
	 {"Desktop 1",        KTPluginFunctionType_Function, {.function = desktop1}},
	 {"Desktop 2",        KTPluginFunctionType_Function, {.function = desktop2}},
	 {"Desktop 3",        KTPluginFunctionType_Function, {.function = desktop3}},
	 {"Desktop 4",        KTPluginFunctionType_Function, {.function = desktop4}}
	}
};


void
next_desktop (KTPreferences *preferences)
{
	if (fork() == 0)
	{
		execlp ("sh", "sh", "-c", "dcop kwin KWinInterface nextDesktop", NULL);
		exit (EXIT_SUCCESS);
	}
}

void
previous_desktop (KTPreferences *preferences)
{
	if (fork() == 0)
	{
		execlp ("sh", "sh", "-c", "dcop kwin KWinInterface previousDesktop", NULL);
		exit (EXIT_SUCCESS);
	}
}

void
desktop1 (KTPreferences *preferences)
{
	if (fork() == 0)
	{
		execlp ("sh", "sh", "-c", "dcop kwin KWinInterface setCurrentDesktop 1", NULL);
		exit (EXIT_SUCCESS);
	}
}

void
desktop2 (KTPreferences *preferences)
{
	if (fork() == 0)
	{
		execlp ("sh", "sh", "-c", "dcop kwin KWinInterface setCurrentDesktop 2", NULL);
		exit (EXIT_SUCCESS);
	}
}

void
desktop3 (KTPreferences *preferences)
{
	if (fork() == 0)
	{
		execlp ("sh", "sh", "-c", "dcop kwin KWinInterface setCurrentDesktop 3", NULL);
		exit (EXIT_SUCCESS);
	}
}

void
desktop4 (KTPreferences *preferences)
{
	if (fork() == 0)
	{
		execlp ("sh", "sh", "-c", "dcop kwin KWinInterface setCurrentDesktop 4", NULL);
		exit (EXIT_SUCCESS);
	}
}
