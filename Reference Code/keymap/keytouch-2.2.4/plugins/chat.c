/*---------------------------------------------------------------------------------
Name               : chat.c
Author             : Marvin Raaijmakers
Description        : Plugin for keyTouch that launches the chat program specified
                     in the users preferences.
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

void chat (KTPreferences *preferences);


KeytouchPlugin plugin_struct = {
	{"Chat", "Marvin Raaijmakers", "GPL 2", "1.0", "Starts the chat program."},
	"chat.so",
	1,
	{{"Chat", KTPluginFunctionType_Function, {.function = chat}}}
};


void
chat (KTPreferences *preferences)
{

	if (fork() == 0)
	{
		execlp ("sh", "sh", "-c", preferences->chat_program, NULL);
		exit (EXIT_SUCCESS);
	}
}
