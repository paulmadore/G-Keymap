/*-------------------------------------------------------------------------------
Name               : keyboard_config.c
Author             : Marvin Raaijmakers
Description        : Providing operations for KEYBOARD_CONFIG
Date of last change: 25-December-2004
History            :
                     20-October-2004  Created:
                                      - keyboard_config_set_name()
                                      - key_settings_new()
                                      - key_settings_add()
                     25-December-2004 Modified key_settings_add()

    Copyright (C) 2004 Marvin Raaijmakers

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    You can contact me at: marvin_raaijmakers(at)lycos(dot)nl
    (replace (at) by @ and (dot) by .)
---------------------------------------------------------------------------------*/ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <keytouch.h>


void
kbcfg_init (KEYBOARD_CONFIG *keyboard_config)
/*
input:
	-
output:
	keyboard_config		- A keyboard configuration
returns:
	-
description:
	This function initializes keyboard_config.
*/
{
	keyboard_config->key_settings_list.head = keyboard_config->key_settings_list.tail = NULL;
	keyboard_config->keyboard_name.model[0] = keyboard_config->keyboard_name.manufacture[0] = '\0';
}


void
kbcfg_set_name (	KEYBOARD_CONFIG	*keyboard_config,
			char		model [MAX_NAME_SIZE+1],
			char		manufacture [MAX_NAME_SIZE+1]  )
/*
input:
	keyboard_model		- The name of the keyboard model
	keyboard_manufacture	- The name of the keyboard manufacture
output:
	keyboard_config		- A keyboard configuration
returns:
	-
description:
	This function copies model and manufacture to the name of the keyboard of keyboard_config.
*/
{
	strcpy (keyboard_config->keyboard_name.model, model);
	strcpy (keyboard_config->keyboard_name.manufacture, manufacture);
}


KEY_SETTINGS
*ks_new (void)
/*
input:
	-
output:
	-
returns:
	The address of the new KEY_SETTINGS.
description:
	This function creates a new (initialized) KEY_SETTINGS and returns its address.
*/
{
	KEY_SETTINGS *key_settings;
	
	key_settings = (KEY_SETTINGS *) malloc( sizeof(KEY_SETTINGS) );
	if (key_settings == NULL)
	{
		handle_fatal_error (FERR_MALLOC);
	}
	/* Initialize the elements values */
	*(key_settings->key_sym) = *(key_settings->key_name) = 
		*(key_settings->program_default) = *(key_settings->program_user) = '\0';
	key_settings->next = NULL;
	
	return (key_settings);
}


void
ksl_add (	KEY_SETTINGS_LIST	*key_settings_list,
		KEY_SETTINGS		*new_key_settings       )
/*
input:
	new_key_settings	- The new KEY_SETTING to add to key_settings_list
output:
	key_settings_list	- The list from which new_key_settings was added to
returns:
	-
description:
	This function adds new_key_settings to key_settings_list so that the names of all
	key settings in key_settings_list are in order.
*/
{
	KEY_SETTINGS *key_settings;
	
	if (key_settings_list->head == NULL) /* If key_settings_list is empty */
	{
		key_settings_list->head = key_settings_list->tail = new_key_settings;
	}
	  /* If the first key_settings of key_settings_list comes before new_key_settings */
	else if (strcmp(key_settings_list->head->key_name, new_key_settings->key_name) < 0)
	{
		/* Find the key_settings that will be the predecessor of new_key_settings */
		for (	key_settings = key_settings_list->head;
			key_settings->next && strcmp(key_settings->next->key_name, new_key_settings->key_name) < 0;
			key_settings = key_settings->next  )
			 ; /* NULL Statement */
		/* If new_key_settings has no predecessor */
		if (key_settings->next == NULL)
		{
			key_settings_list->tail->next = new_key_settings;
			key_settings_list->tail = new_key_settings;
		}
		else
		{
			new_key_settings->next = key_settings->next;
			key_settings->next = new_key_settings;
		}
	}
	else /* If new_key_settings will be the first KEY_SETTINGS of key_settings_list */
	{
		new_key_settings->next = key_settings_list->head;
		key_settings_list->head = new_key_settings;
	}
}

KEY_SETTINGS
*ksl_find_key (	KEY_SETTINGS_LIST	*key_settings_list,
		char			*key_name          )
/*
input:
	key_settings_list	- List containing settings of every key of the current
				  keyboard
	key_name		- The name of the key to find
output:
	-
returns:
	The address of the KEY_SETTINGS element the function found or NULL if the key was
	not found.
description:
	The function will search through key_settings_list for the key which name is equal
	to key_name.
*/
{
	KEY_SETTINGS *key_settings;
	
	key_settings = key_settings_list->head;
	/* Find the key with key_name */
	while ( key_settings && (strcmp(key_settings->key_name, key_name) != EQUAL) )
	{
		key_settings = key_settings->next;
	}
	/*if ( key_settings == NULL ) /* If not found *
	{
		handle_error (ERR_KEY_NEXISTS, key_name);
	}*/

	return (key_settings);
}


void
ksl_clear (KEY_SETTINGS_LIST *key_settings_list)
/*
input:
	-
output:
	key_settings_list	- List containing settings of every key (now empty)
returns:
	-
description:
	The function clears key_settings_list
*/
{
	KEY_SETTINGS *key_settings, *temp;
	
	key_settings = key_settings_list->head;
	while (key_settings)
	{
		temp = key_settings;
		key_settings = key_settings->next;
		free (temp);
	}
	key_settings_list->head = key_settings_list->tail = NULL;
}
