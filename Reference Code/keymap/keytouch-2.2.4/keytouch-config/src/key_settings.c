/*---------------------------------------------------------------------------------
Name               : key_settings.c
Author             : Marvin Raaijmakers
Description        : Contains functions for managing the key settings
Date of last change: 20-May-2006
History            : 20-May-2006 Added clear_key_setting_list()

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
#include <string.h>

#include <mxml.h>
#include <keytouch.h>
#include <key_settings.h>


static void clear_action (KTAction *action);


KTKeySettings
*find_key_settings (	char			*key_name,
			KTKeySettingsList	*key_list  )
/*
Input:
	key_name	- The name of the key to get the key settings from.
	key_list	- The list to search in for the key setting.
Output:
	-
Returns:
	A pointer to the key settings of the key that has the name key_name. If no
	such key settings exist, NULL is returned.
Description:
	This function searches in key_list the key settings of the key that has
	the name key_name.
*/
{
	KTKeySettings *key_settings;
	
	for (key_settings = key_list->head;
	      key_settings != NULL &&
	      strcmp(key_settings->key_name, key_name) != EQUAL;
	     key_settings = key_settings->next)
		; /* NULL Statement */
	return (key_settings);
}


void
clear_action (KTAction *action)
{
	if (action->type == KTActionTypePlugin)
	{
		free (action->plugin.plugin_name);
		free (action->plugin.function_name);
	}
	else if (action->type == KTActionTypeProgram)
	{
		free (action->program.command);
	}
}


void
clear_key_settings_action (KTKeySettings *key)
/*
Input:
	key	- The key to clear
Output:
	key	- The cleared key
Returns:
	-
Description:
	This function frees all allocated memory in the action member of key.
*/
{
	if (memcmp(&key->action, &key->default_action, sizeof(KTAction)))
	{
		clear_action (&(key->action));
	}
}


void
ks_set_program (	KTKeySettings	*key,
			char		*command )
/*
Input:
	command	- The command of the program to use as the action the key
Output:
	key	- The action of the key will be set to 'command'
Returns:
	-
Description:
	This function changes the type of the action of the key to
	KTActionTypeProgram and sets the action program to 'command'. Note that a
	copy of 'command' will be stored in 'key'.
*/
{
	clear_key_settings_action (key);
	key->action.type = KTActionTypeProgram;
	key->action.program.command = keytouch_strdup (command);
}


void
ks_set_plugin (	KTKeySettings	*key,
		char		*plugin_name,
		char		*function_name )
/*
Input:
	plugin_name	- The name of the plugin to use as the action of the key
	function_name	- The name of the plugin function to use as the action of
			  the key
Output:
	key		- The action of the key will be set to the plugin named
			  'plugin_name' and the function named 'function_name'
Returns:
	-
Description:
	This function changes the type of the action of the key to
	KTActionTypePlugin and sets the action plugin to 'plugin_name' and the
	function to 'plugin_function'. Note that copies of 'plugin_name' and
	'plugin_name' will be stored in 'key'.
*/
{
	clear_key_settings_action (key);
	key->action.type = KTActionTypePlugin;
	key->action.plugin.plugin_name = keytouch_strdup (plugin_name);
	key->action.plugin.function_name = keytouch_strdup (function_name);
}


void
clear_key_setting_list (KTKeySettingsList *key_list)
/*
Output:
	key_list	- This list will be cleared
*/
{
	KTKeySettings *key, *next;
	
	for (key = key_list->head; key; key = next)
	{
		next = key->next;
		clear_key_settings_action (key);
		clear_action (&(key->default_action));
		free (key->key_name);
		free (key);
	}
	key_list->head = key_list->tail = NULL;
}
