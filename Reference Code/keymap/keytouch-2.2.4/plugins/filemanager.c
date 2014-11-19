/*---------------------------------------------------------------------------------
Name               : filemanager.c
Author             : Marvin Raaijmakers
Description        : Plugin for keyTouch that provides functions for file managers.
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
#include <string.h>
#include <X11/Xlib.h>
#include <unistd.h>

#include <check_desktop.h>
#include <plugin.h>

void run_filemanager (char *location);
void documents (KTPreferences *preferences);
void filemanager (KTPreferences *preferences);


KTPluginKey key_reload[] = {
	{{"Konqueror", TRUE}, 0,           "F5"}, /* Konqueror */
	{{NULL,        TRUE}, ControlMask, "R"}   /* Others */
};

KTPluginKey key_back[] = {
	{{NULL, FALSE}, Mod1Mask, "Left"}
};

KTPluginKey key_forward[] = {
	{{NULL, FALSE}, Mod1Mask, "Right"}
};

KTPluginKey key_up[] = {
	{{NULL, FALSE}, Mod1Mask, "Up"}
};

KeytouchPlugin plugin_struct = {
	{"Filemanager", "Marvin Raaijmakers", "GPL 2", "1.0", "Provides filemanager functions."},
	"filemanager.so",
	6,
	{{"Filemanager",      KTPluginFunctionType_Function, {.function = filemanager}},
	 {"Documents",        KTPluginFunctionType_Function, {.function = documents}},
	 {"Reload",           KTPluginFunctionType_Key,      {.key_function = {2, key_reload}}},
	 {"Back",             KTPluginFunctionType_Key,      {.key_function = {1, key_back}}},
	 {"Forward",          KTPluginFunctionType_Key,      {.key_function = {1, key_forward}}},
	 {"Parent directory", KTPluginFunctionType_Key,      {.key_function = {1, key_up}}}
	}
};


void
run_filemanager (char *location)
{
	char	*filemanager,
		*command;
	
	switch (check_desktop())
	{
		case KDE:
			filemanager = "konqueror";
			break;
		case GNOME:
			filemanager = "nautilus";
			break;
		default:
			return;
	}
	
	command = malloc(strlen(filemanager) + strlen(location) + strlen(" \"\"") + 1);
	if (command != NULL)
	{
		strcpy (command, filemanager);
		strcat (command, " \"");
		strcat (command, location);
		strcat (command, "\"");
		if (fork() == 0)
		{
			execlp ("sh", "sh", "-c", command, NULL);
			exit (EXIT_SUCCESS);
		}
		free (command);
	}
}

void
documents (KTPreferences *preferences)
{
	run_filemanager (preferences->documents_dir);
}

void
filemanager (KTPreferences *preferences)
{
	run_filemanager ("~");
}
