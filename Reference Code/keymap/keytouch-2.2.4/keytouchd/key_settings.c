/*---------------------------------------------------------------------------------
Name               : key_settings.c
Author             : Marvin Raaijmakers
Description        : Performs action for the KTKeySettingsList
Date of last change: 20-Aug-2005

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
#include <string.h>

#include <mxml.h>
#include <keytouchd.h>

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

KTKeySettings
*key_settings_of_keycode (	unsigned int		keycode,
				KTKeySettingsList	*key_list  )
/*
Input:
	keycode		- The keycode of the key to get the key settings from.
	key_list	- The list to search in for the key setting.
Output:
	-
Returns:
	A pointer to the key settings of the key that has keycode 'keycode'. If no
	such key settings exist, NULL is returned.
Description:
	This function searches in key_list the key settings of the key that has
	keycode 'keycode'.
*/
{
	KTKeySettings *key_settings;
	
	for (key_settings = key_list->head;
	     key_settings != NULL && key_settings->keycode != keycode;
	     key_settings = key_settings->next)
		; /* NULL Statement */
	return (key_settings);
}

void
clear_key_settings_list (KTKeySettingsList *key_list)
/*
Input:
	key_list	- The key settings list to clear
Output:
	key_list	- The cleared key settings list
Returns:
	-
Description:
	This function clears key_list.
*/
{
	KTKeySettings	*key_settings,
			*next;
	
	for (key_settings = key_list->head;
	     key_settings != NULL;
	     key_settings = next)
	{
		next = key_settings->next;
		/* Because key_settings->key_name was read with XmlGetElementString()
		 * we should free it with XmlFree()
		 */
		XmlFree (key_settings->key_name);
		if (key_settings->action.type == KTActionTypeProgram)
		{
			/* Because key_settings->action.program.command was read with
			 * XmlGetElementString() we should free it with XmlFree()
			 */
			XmlFree (key_settings->action.program.command);
		}
		free (key_settings);
	}
	key_list->head = key_list->tail = NULL;
}
