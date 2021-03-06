/*---------------------------------------------------------------------------------
Name               : xmms.c
Author             : Marvin Raaijmakers
Description        : Plugin for keyTouch that provides functions for XMMS.
Date of last change: 31-Aug-2005

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


static void run (char *command);

void play_pause (KTPreferences *preferences);
void stop (KTPreferences *preferences);
void next (KTPreferences *preferences);
void previous (KTPreferences *preferences);

KeytouchPlugin plugin_struct = {
	{"XMMS", "Marvin Raaijmakers", "GPL 2", "1.0",
	 "Provides functions to control XMMS."},
	"xmms.so",
	4,
	{{"Play/Pause", KTPluginFunctionType_Function, {.function = play_pause}},
	 {"Stop",       KTPluginFunctionType_Function, {.function = stop}},
	 {"Previous",   KTPluginFunctionType_Function, {.function = previous}},
	 {"Next",       KTPluginFunctionType_Function, {.function = next}},
	}
};


void
run (char *command)
{
	if (fork() == 0)
	{
		execlp ("sh", "sh", "-c", command, NULL);
		exit (EXIT_SUCCESS);
	}
}

void
play_pause (KTPreferences *preferences)
{
	run ("xmms -t");
}

void
stop (KTPreferences *preferences)
{
	run ("xmms --stop");
}

void
next (KTPreferences *preferences)
{
	run ("xmms --fwd");
}

void
previous (KTPreferences *preferences)
{
	run ("xmms --rew");
}
