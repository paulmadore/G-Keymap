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
#include <keytouch-acpid.h>


unsigned int
get_keycode (	char		*event_descr,
		KTAcpiKeyList	*key_list  )
/*
Input:
	keycode		- The event description of the key to get the keycode from.
	key_list	- The list to search in for the keycode.
Output:
	-
Returns:
	The keycode of the KTAcpiKey in key_list for which the event description is
	equal to event_descr. If no such KTAcpiKey exists 0 will be returned.
Description:
	See above.
*/
{
	KTAcpiKey *key;
	
	for (key= key_list->head; key != NULL; key = key->next)
	{
		if (strcmp(key->event_descr, event_descr) == EQUAL)
		{
			return (key->keycode);
		}
	}
	return (0);
}

void
clear_acpi_key_list (KTAcpiKeyList *key_list)
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
	KTAcpiKey	*key,
			*next;
	
	for (key = key_list->head; key != NULL; key = next)
	{
		next = key->next;
		/* Because key->event_descr was read with XmlGetElementString()
		 * we should free it with XmlFree()
		 */
		XmlFree (key->event_descr);
		free (key);
	}
	key_list->head = key_list->tail = NULL;
}
