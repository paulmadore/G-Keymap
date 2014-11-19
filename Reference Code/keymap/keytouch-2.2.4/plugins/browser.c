/*---------------------------------------------------------------------------------
Name               : browser.c
Author             : Marvin Raaijmakers
Description        : Plugin for keyTouch that provides functions for web browsers.
Date of last change: 25-Jun-2006
History            : 25-Jun-2006 Added tab-handling functions
                     19-Jun-2006 run_browser() uses a default browser (depending on
                                 the desktop environment) if no browser was set
                     16-Dec-2005 Added key function Stop

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
#include <string.h>
#include <X11/Xlib.h>
#include <unistd.h>

#include <plugin.h>
#include <check_desktop.h>

static void run_browser (char *browser, char *url);

void home (KTPreferences *preferences);
void search (KTPreferences *preferences);

KTPluginKey key_back[] = {
	{{NULL, TRUE}, Mod1Mask, "Left"}
};

KTPluginKey key_forward[] = {
	{{NULL, TRUE}, Mod1Mask, "Right"}
};

KTPluginKey key_history[] = {
	{{NULL, TRUE}, ControlMask, "H"}
};

KTPluginKey key_add_bookmark[] = {
	{{"Konqueror", TRUE}, ControlMask, "B"},
	{{NULL,        TRUE}, ControlMask, "D"}
};

KTPluginKey key_reload[] = {
	{{"Konqueror", TRUE}, 0,           "F5"},
	{{NULL,        TRUE}, ControlMask, "R"}
};

KTPluginKey key_stop[] = {
	{{NULL, TRUE}, 0, "Escape"}
};


KTPluginKey key_close_tab[] = {
	{{NULL, TRUE}, ControlMask, "W"}
};

KTPluginKey key_new_tab[] = {
	{{NULL, TRUE}, ControlMask, "T"}
};

KTPluginKey key_next_tab[] = {
	{{"Konqueror", TRUE}, ControlMask, "period"},
	{{NULL,        TRUE}, ControlMask, "Next"}
};

KTPluginKey key_previous_tab[] = {
	{{"Konqueror", TRUE}, ControlMask, "comma"},
	{{NULL,        TRUE}, ControlMask, "Prior"}
};


KeytouchPlugin plugin_struct = {
	{"WWW Browser", "Marvin Raaijmakers", "GPL 2", "1.2", "Provides functions for web browsers"},
	"browser.so",
	12,
	{{"Home",           KTPluginFunctionType_Function, {.function = home}},
	 {"Search",         KTPluginFunctionType_Function, {.function = search}},
	 {"Back",           KTPluginFunctionType_Key,      {.key_function = {1, key_back}}},
	 {"Forward",        KTPluginFunctionType_Key,      {.key_function = {1, key_forward}}},
	 {"Reload",         KTPluginFunctionType_Key,      {.key_function = {2, key_reload}}},
	 {"Stop",           KTPluginFunctionType_Key,      {.key_function = {1, key_stop}}},
	 {"History",        KTPluginFunctionType_Key,      {.key_function = {1, key_history}}},
	 {"Add Bookmark",   KTPluginFunctionType_Key,      {.key_function = {2, key_add_bookmark}}},
	 {"Close Tab",      KTPluginFunctionType_Key,      {.key_function = {1, key_close_tab}}},
	 {"New Tab",        KTPluginFunctionType_Key,      {.key_function = {1, key_new_tab}}},
	 {"Next Tab",       KTPluginFunctionType_Key,      {.key_function = {2, key_next_tab}}},
	 {"Previous Tab",   KTPluginFunctionType_Key,      {.key_function = {2, key_previous_tab}}}
	}
};


void
run_browser (	char	*browser,
		char	*url  )
{
	char *command;
	
	if (browser[0] == '\0') /* If the browser was not set */
	{
		switch (check_desktop())
		{
			case KDE:
				browser = "kfmclient openURL";
				break;
			case GNOME:
				browser = "gnome-open";
				break;
			default:
				/* An unknown desktop environment is running,
				 * so we just try to run firefox
				 */
				browser = "firefox";
				break;
		}
	}
	command = malloc(strlen(browser) + strlen(url) + strlen(" \"\"") + 1);
	if (command != NULL)
	{
		strcpy (command, browser);
		strcat (command, " \"");
		strcat (command, url);
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
home (KTPreferences *preferences)
{
	run_browser (preferences->browser, preferences->home_page);
}

void
search (KTPreferences *preferences)
{
	run_browser (preferences->browser, preferences->search_page);
}
