/*-----------------------------------------------------------------------------------
Name               : key_list.c
Author             : Marvin Raaijmakers
Description        : Provides functions for the KTKey and KTKeyList type
Date of last change: 22-Aug-2007
History            : 22-Aug-2007 Added support for USB keys
                     30-Aug-2006 Added code for new acpi_event_descr member of KTKey
                     07-Apr-2006 Added keycode parameter to KTKey_new()

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
-------------------------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>

#include <keytouch-editor.h>

void
KTKeyList_init (KTKeyList *list)
/*
Output:
	list	- Will be initialized
*/
{
	list->head = list->tail = NULL;
}


void
KTKeyList_clear (KTKeyList *list)
/*
Output:
	list	- Will be empty
Description:
	All elements in 'list' will be deleted.
*/
{
	KTKey *next_key;
	list->head = list->tail = NULL;
	
	while (list->head)
	{
		next_key = list->head->next;
		KTKey_delete (list->head);
		list->head = next_key;
	}
	list->tail = NULL;
}


KTKey
*KTKeyList_find_key (	const KTKeyList	*list,
			const char	*name )
/*
Input:
	list	- The list to search in for the key
	name	- The name of the key to search for
Returns:
	The address of the KTKey in 'list' with name 'name'. NULL will be returned
	if no such key appears in 'list'.
*/
{
	KTKey *key;
	
	for (key = list->head; key != NULL && strcmp(key->name, name) != EQUAL; key = key->next)
		; /* NULL Statement */
	
	return (key);
}


void
KTKeyList_append (	KTKeyList	*list,
			KTKey	 	*key  )
/*
Input:
	key	- The address of the KTKey to append to list
Output:
	list	- The KTKey will be appended to this list
Description:
	This functions appends the key pointed to by 'key' to 'list'.
*/
{
	if (list->head)
	{
		list->tail->next = key;
	}
	else
	{
		list->head = key;
	}
	list->tail = key;
}


void
KTKeyList_remove_key (	KTKeyList	*list,
			KTKey	 	*key  )
/*
Input:
	list	- The list to remove the KTKey from
	key	- The address of the key to remove
Output:
	list	- 'key' will be removed from it
Description:
	This functions frees the KTKey pointed to by 'key' and removes it from
	'list'. Note that the KTKey to remove MUST appear in 'list'.
*/
{
	KTKey *predecessor;
	
	if (list->head == key) /* If key appears at the beginning of list */
	{
		list->head = key->next;
		if (list->head == NULL)
		{
			list->tail = NULL;
		}
	}
	else
	{
		/* Unfortunately we will have to walk trough the linked list to
		 * find the predecessor of key
		 */
		for (predecessor = list->head; predecessor->next != key; predecessor = predecessor->next)
			; /* NULL Statement */
		predecessor->next = key->next;
		if (predecessor->next == NULL)
		{
			list->tail = predecessor;
		}
	}
	KTKey_delete (key);
}


Boolean
KTKeyList_contains_usb_key (KTKeyList *list)
/*
Input:
	list - The list to search through
Returns:
	TRUE if there is a key in 'list' that has a non-zero 'usagecode' member.
*/
{
	KTKey *key;
	
	for (key = list->head; key; key = key->next)
	{
		if (key->usagecode)
		{
			return TRUE;
		}
	}
	return FALSE;
}


void
KTKey_free_default_action (KTKey *key)
/*
Input:
	key	- The address of the KTKey to free the default action of
Description:
	See the code
*/ 
{
	if (key->default_action.type == KTActionTypeProgram)
	{
		free (key->default_action.program.command);
	}
	else if (key->default_action.type == KTActionTypePlugin)
	{
		free (key->default_action.plugin.plugin_name);
		free (key->default_action.plugin.function_name);
	}
}


KTKey
*KTKey_new (const char  *name,
            int         scancode,
            int         usagecode,
            int         keycode,
            char        *acpi_event_descr )
/*
Input:
	name             - The name of the KTKey to create
	scancode         - The scancode to assign to the new KTKey
	usagecode        - The usagecode to assign to the new KTKey
	keycode          - The keycode to assign to the new KTKey
	acpi_event_descr - The ACPI event description to assign to the new
	                   KTKey. It must point to NULL or an allocated
	                   string containing the event description.
Returns:
	The address of the created KTKey.
Description:
	This function creates a new KTKey with initialized with name, keycode,
	scancode and, acpi_event_descr. The default action will be a program
	with command "".
*/
{
	KTKey *new_key;
	
	new_key = keytouch_malloc(sizeof(KTKey));
	new_key->name = strdup(name);
	new_key->scancode = scancode;
	new_key->usagecode = usagecode;
	new_key->keycode = keycode;
	new_key->default_action.type = KTActionTypeProgram;
	new_key->default_action.program.command = strdup ("");
	new_key->acpi_event_descr = acpi_event_descr;
	new_key->next = NULL;
	if (new_key->name == NULL || new_key->default_action.program.command == NULL)
	{
		KTError (_("Could not allocate memory."), "");
		exit (EXIT_FAILURE);
	}
	return (new_key);
}


void
KTKey_delete (KTKey *key)
/*
Input:
	key	- The address of the key to remove
Description:
	This function frees the KTKey pointed to by 'key'.
*/
{
	free (key->name);
	KTKey_free_default_action (key);
	if (key->acpi_event_descr)
	{
		free (key->acpi_event_descr);
	}
	free (key);
}


void
KTKey_set_name (	KTKey		*key,
			const char	*name )
/*
Input:
	name	- The new name of the key
Output:
	key	- The key containing the new name
Description:
	This function copies 'name' to the name element of 'key'.
*/
{
	free (key->name);
	key->name = strdup(name);
	if (key->name == NULL)
	{
		KTError (_("Could not allocate memory."), "");
		exit (EXIT_FAILURE);
	}
}


void
KTKey_set_program (	KTKey		*key,
			const char	*command )
/*
Input:
	command	- The command of the program
Output:
	key	- The default action will be changed to KTActionTypeProgram with
	          'command' as the command.
*/
{
	KTKey_free_default_action (key);
	key->default_action.type = KTActionTypeProgram;
	key->default_action.program.command = strdup (command);
	if (key->default_action.program.command == NULL)
	{
		KTError (_("Could not allocate memory."), "");
		exit (EXIT_FAILURE);
	}
}


void
KTKey_set_plugin (	KTKey		*key,
			const char	*plugin,
			const char	*function )
/*
Input:
	plugin		- The name of the plugin
	function	- The name of the plugin function
Output:
	key	- The default action will be changed to KTActionTypePlugin with
	          plugin name 'plugin' and function name 'function'
*/
{
	KTKey_free_default_action (key);
	key->default_action.type = KTActionTypePlugin;
	key->default_action.plugin.plugin_name = strdup (plugin);
	key->default_action.plugin.function_name = strdup (function);
	if (key->default_action.plugin.plugin_name == NULL ||
	    key->default_action.plugin.function_name == NULL)
	{
		KTError (_("Could not allocate memory."), "");
		exit (EXIT_FAILURE);
	}
}


void
KTKey_set_scancode (KTKey *key,
                    int   scancode )
{
	key->scancode = scancode;
	key->acpi_event_descr = NULL;
	if (key->acpi_event_descr)
	{
		free (key->acpi_event_descr);
	}
}


void
KTKey_set_usagecode (KTKey *key,
                     int usagecode )
{
	key->usagecode = usagecode;
	key->acpi_event_descr = NULL;
	if (key->acpi_event_descr)
	{
		free (key->acpi_event_descr);
	}
}


void
KTKey_set_acpi_event_descr (	KTKey	*key,
				char	*acpi_event_descr )
{
	if (key->acpi_event_descr)
	{
		free (key->acpi_event_descr);
	}
	key->scancode = 0;
	key->usagecode = 0;
	key->acpi_event_descr = acpi_event_descr;
}
