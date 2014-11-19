/*---------------------------------------------------------------------------------
Name               : plugin.c
Author             : Marvin Raaijmakers
Description        : Contains a list of all plugins and a function to find the
                     plugins.
Date of last change: 24-Jun-2007
History            : 24-Jun-2007 Compiler warnings fixes

    Copyright (C) 2006-2007 Marvin Raaijmakers

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
#include <keytouch-editor.h>
#include <stdlib.h>
#include <string.h>

char *amarok_plugin_functions[4] =		{"Play/Pause", "Stop", "Previous", "Next"};
char *amixer_plugin_functions[3] =		{"Volume increase", "Volume decrease", "Mute"};
char *chat_plugin_functions[1] =		{"Chat"};
char *common_actions_plugin_functions[12] =	{"Close/Quit", "New (file)", "Open", "Save", "Print", "Undo",
						 "Redo", "Find", "Cut", "Copy", "Paste", "Select All"};
char *email_plugin_functions[5] =		{"E-mail", "New mail message", "Send/Receive", "Reply", "Forward"};
char *filemanager_plugin_functions[6] =		{"Filemanager", "Documents", "Reload", "Back", "Forward", "Parent directory"};
char *kde_sw_desktop_plugin_functions[6] =	{"Next Desktop", "Previous Desktop", "Desktop 1", "Desktop 2",
						 "Desktop 3", "Desktop 4"};
char *lock_screen_plugin_functions[1] =		{"Lock Screen"};
char *scroll_plugin_functions[4] =		{"Up", "Down", "Left", "Right"};
char *www_browser_plugin_functions[8] =		{"Home", "Search", "Back", "Forward", "Reload",
						 "Stop", "History", "Add Bookmark"};
char *xmms_plugin_functions[4] =		{"Play/Pause", "Stop", "Previous", "Next"};

const KTPlugin plugin_list[NUM_PLUGINS] = {
	{"Amarok",		4,  amarok_plugin_functions},
	{"Amixer",		3,  amixer_plugin_functions},
	{"Chat",		1,  chat_plugin_functions},
	{"Common actions",	12, common_actions_plugin_functions},
	{"E-mail",		5,  email_plugin_functions},
	{"Filemanager",		6,  filemanager_plugin_functions},
	{"KDE Switch Desktop",	6,  kde_sw_desktop_plugin_functions},
	{"Lock Screen",		1,  lock_screen_plugin_functions},
	{"Scroll",		4,  scroll_plugin_functions},
	{"WWW Browser",		8,  www_browser_plugin_functions},
	{"XMMS",		4,  xmms_plugin_functions}
};


KTPlugin
*find_plugin (	char		*name,
		const KTPlugin	list[],
		unsigned int	list_size )
/*
Input:
	name		- The name of the plugin to find
	list		- The list to search in for the plugin
	list_size	- The number of elements of 'list'
Returns:
	The address of the KTPlugin in 'list', named 'name'. If no such plugin
	exists NULL is returned.
*/
{
	unsigned int i;
	
	for (i = 0; i < list_size; i++)
	{
		if (!strcmp (list[i].name, name))
		{
			return (KTPlugin *) &(list[i]);
		}
	}
	return NULL;
}
