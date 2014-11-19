/*---------------------------------------------------------------------------------
Name               : rhythmbox.c
Author             : Marvin Raaijmakers
Description        : Plugin for keyTouch that provides functions for Rhythmbox.
Date of last change: 31-July-2006

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

#include <plugin.h>


static void control_rhythmbox (char *rhythmbox_command, char *rhythmbox_client_command);

void play_pause (KTPreferences *preferences);
void pause_song (KTPreferences *preferences);
void play (KTPreferences *preferences);
void next (KTPreferences *preferences);
void previous (KTPreferences *preferences);

KeytouchPlugin plugin_struct = {
	{"Rhythmbox", "Marvin Raaijmakers", "GPL 2", "1.0",
	 "Provides functions to control Rhythmbox."},
	"rhythmbox.so",
	5,
	{{"Play/Pause", KTPluginFunctionType_Function, {.function = play_pause}},
	 {"Pause",      KTPluginFunctionType_Function, {.function = pause_song}},
	 {"Play",       KTPluginFunctionType_Function, {.function = play}},
	 {"Previous",   KTPluginFunctionType_Function, {.function = previous}},
	 {"Next",       KTPluginFunctionType_Function, {.function = next}},
	}
};


void
control_rhythmbox (	char	*rhythmbox_command,
			char	*rhythmbox_client_command )
/*
Description:
	This functions creates a client process that runs 'rhythmbox_client_command' when the
	program "rhythmbox-client" is installed and otherwise 'rhythmbox_command'.
*/
{
	static Boolean use_client;
	static Boolean initialized = FALSE;
	
	if (!initialized)
	{
		initialized = TRUE;
		use_client = (system( "which rhythmbox-client" ) == 0);
	}
	if (fork() == 0)
	{
		execlp ("sh", "sh", "-c", use_client ? rhythmbox_client_command : rhythmbox_command, NULL);
		exit (EXIT_SUCCESS);
	}
}

void
play_pause (KTPreferences *preferences)
{
	control_rhythmbox ("rhythmbox --play-pause", "rhythmbox-client --play-pause");
}

void
pause_song (KTPreferences *preferences)
{
	control_rhythmbox ("rhythmbox --pause", "rhythmbox-client --pause");
}

void
play (KTPreferences *preferences)
{
	control_rhythmbox ("rhythmbox --play", "rhythmbox-client --play");
}

void
next (KTPreferences *preferences)
{
	control_rhythmbox ("rhythmbox --next", "rhythmbox-client --next");
}

void
previous (KTPreferences *preferences)
{
	control_rhythmbox ("rhythmbox --previous", "rhythmbox-client --previous");
}
