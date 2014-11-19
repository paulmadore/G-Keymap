/*---------------------------------------------------------------------------------
Name               : config.c
Author             : Marvin Raaijmakers
Description        : Reads configuration files
Date of last change: 27-Nov-2005
Changes            : 27-Nov-2005 Modified the original config.c for keytouchd so
                                 that it works for keytouch-acpid

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
#include <mxml.h>

#include <string_to_keycode.h>
#include <keytouch-acpid.h> 

static KTAcpiKey *read_acpi_key (	XmlContent		*keylist_element,
					int			*index,
					XmlElementType		*key_type,
					XmlElementType		*event_descr_type,
					XmlElementType		*keycode_type,
					XmlAttributeName	*key_type_attrname      );
static void read_keyboard_file (	KTAcpiKeyList	*key_list,
					KTKeyboardName	*keyboard_name    );


KTAcpiKey
*read_acpi_key (	XmlContent		*keylist_element,
			int			*index,
			XmlElementType		*key_type,
			XmlElementType		*event_descr_type,
			XmlElementType		*keycode_type,
			XmlAttributeName	*key_type_attrname       )
/*
Input:
	keylist_element		- The element to read from.
	index			- Read the *index'th key element
	key_type		- The XmlElementType for the key element
	event_decr_type		- The XmlElementType for the event-descr element
	keycode_type		- The XmlElementType for the keycode element
	key_type_attrname	- The XmlAttributeName for the key-type attribute
				  name
Output:
	index			- If the value of "key-type" attribute of the
				  *index'th element is not "acpi-hotkey". The
				  integer pointed to by index will be the first key
				  element of key-type "acpi-hotkey" after the
				  *index'th key element.
Returns:
	A pointer to the new KTAcpiKey if it was read, otherwise NULL.
Desciption:
	This function reads the event-descr and keycode member from the *index'th key
	element that is a child of keylist_element.
*/
{
	XmlContent	*key_element,
			*keycode_element,
			*event_descr_element;
	char		*keycode_string,
			*key_type_string;
	KTAcpiKey	*acpi_key;
	
	(*index)--;
	/* Find the first key element whery the value of the key_type attribute
	 * is equal to "acpi-hotkey"
	 */
	do {
		(*index)++;
		key_element = XmlGetElementOfType(XmlElementContentList(keylist_element), key_type, *index);
		if (key_element == NULL)
		{
			return (NULL);
		}
		key_type_string = XmlGetAttributeValue(key_type_attrname, key_element);
	} while (key_type_string == NULL || strcmp(key_type_string, "acpi-hotkey") != EQUAL);
	
	if ((event_descr_element =
	       XmlGetElementOfType(XmlElementContentList(key_element), event_descr_type, 0)) == NULL ||
	    (keycode_element =
	       XmlGetElementOfType(XmlElementContentList(key_element), keycode_type, 0)) == NULL)
	{
		KTError ("The keyboard file contains an incomplete key element.", "");
		exit (EXIT_FAILURE);
	}
	acpi_key = keytouch_malloc(sizeof(KTAcpiKey));
	/* Read the event description */
	acpi_key->event_descr = XmlGetElementString(event_descr_element, "");
	/* Read the string of the keycode */
	keycode_string = XmlGetElementString(keycode_element, "");
	/* Convert the keycode */
	acpi_key->keycode = string_to_xkeycode(keycode_string);
	/* If the string is not a string for a keycode */
	if (acpi_key->keycode == 0)
	{
		KTError ("The keyboard file contains an invalid keycode: %s.", keycode_string);
		exit (EXIT_FAILURE);
	}
	XmlFree (keycode_string);
	acpi_key->next = NULL;
	
	return (acpi_key);
}


void
read_keyboard_file (	KTAcpiKeyList	*key_list,
			KTKeyboardName	*keyboard_name  )
/*
Input:
	keyboard_name	- The name of the keyboard to read the configuration from.
Output:
	key_list	- List containing information for every key.
Returns:
	-
Desciption:
	This function reads the keyboard file of the keyboard named keyboard_name,
	which means that it for every special function key an entry in key_list and
	fills in the following elements:
	- event_descr
	- key_code
*/
{
	XmlDocument		*document;
	XmlContent		*keyboard_element,
				*keylist_element;
	XmlElementType		*keyboard_type,
				*keylist_type,
				*key_type,
				*event_descr_type,
				*keycode_type;
	XmlAttributeName	*key_type_attrname;
	KTAcpiKey		*key;
	char			*file_name;
	int			count;
	
	file_name = (char *)keytouch_malloc(	strlen(KEYBOARD_FILES_DIR "/.")+
						strlen(keyboard_name->manufacturer)+
						strlen(keyboard_name->model)+1 );
	strcpy (file_name, KEYBOARD_FILES_DIR "/");
	strcat (file_name, keyboard_name->model);
	strcat (file_name, ".");
	strcat (file_name, keyboard_name->manufacturer);
	
	document = XmlParseDocument(file_name);
	if (document == NULL)
	{
		KTError ("'%s' is an invalid keyboard file.", file_name);
		exit (EXIT_FAILURE);
	}
	
	keyboard_type = XmlGetElementType("keyboard", document, FALSE);
	if (keyboard_type == NULL ||
	    (keyboard_element = XmlGetElementOfType(XmlDocumentContentList(document), keyboard_type, 0)) == NULL)
	{
		KTError ("The keyboard file '%s' does not contain a keyboard element.", file_name);
		exit (EXIT_FAILURE);
	}
	keylist_type = XmlGetElementType("key-list", document, FALSE);
	if (keylist_type == NULL ||
	    (keylist_element =
	       XmlGetElementOfType(XmlElementContentList(keyboard_element), keylist_type, 0)) == NULL)
	{
		KTError ("The keyboard file '%s' does not contain a key-list element.", file_name);
		exit (EXIT_FAILURE);
	}
	
	if ((key_type = XmlGetElementType("key", document, FALSE)) == NULL ||
	    (keycode_type = XmlGetElementType("keycode", document, FALSE)) == NULL)
	{
		KTError ("The keyboard file '%s' is invalid.", file_name);
		exit (EXIT_FAILURE);
	}
	key_type_attrname = XmlGetAttributeName ("key-type", document, TRUE);
	event_descr_type = XmlGetElementType ("event-descr", document, TRUE);
	
	count = 0;
	/* Read the first key of the key list */
	key_list->head = key_list->tail = read_acpi_key(keylist_element, &count, key_type,
	                                                event_descr_type, keycode_type, key_type_attrname);
	key = key_list->head;
	/* Read the rest of the key list */
	for (count++; key != NULL; count++)
	{
		key = read_acpi_key(keylist_element, &count, key_type,
		                    event_descr_type, keycode_type, key_type_attrname);
		key_list->tail->next = key;
		if (key != NULL)
		{
			key_list->tail = key;
		}
	}
	free (file_name);
	XmlFreeDocument (document);
}


void
read_configuration (	KTAcpiKeyList	*key_list,
			KTKeyboardName	*keyboard_name  )
/*
Input:
	keyboard_name	- The name of the keyboard to read the configuration from.
Output:
	key_list	- List containing the settings for every key.
Desciption:
	This function reads the configuration from the configuration files of the
	keyboard named keyboard_name.
*/
{
	/* Create the list and fill in for every KTAcpiKey the following members:
	 * - event-descr
	 * - keycode
	 */
	read_keyboard_file (key_list, keyboard_name);
}


void
get_current_keyboard (KTKeyboardName *keyboard_name)
/*
Input:
	-
Output:
	keyboard_name	- The readen keyboard name
Returns:
	-
Desciption:
	This function reads the name of the current keyboard. If the model and/or
	the manufacturer member of keyboard_name are no longer needed, they should
	be freed with XmlFree().
*/
{
	XmlDocument	*document;
	XmlElementType	*current_keyboard_type,
			*model_type,
			*manufacturer_type;
	XmlContent	*current_keyboard_element,
			*model_element,
			*manufacturer_element;
	
	document = XmlParseDocument(CURRENT_KEYBOARD_FILE);
	if (document == NULL)
	{
		KTError ("'%s' is invalid", CURRENT_KEYBOARD_FILE);
		exit (EXIT_FAILURE);
	}
	if ((current_keyboard_type = XmlGetElementType("current-keyboard", document, FALSE)) == NULL ||
	    (model_type = XmlGetElementType("model", document, FALSE)) == NULL ||
	    (manufacturer_type = XmlGetElementType("manufacturer", document, FALSE)) == NULL)
	{
		KTError ("'%s' is invalid", CURRENT_KEYBOARD_FILE);
		exit (EXIT_FAILURE);
	}
	if ((current_keyboard_element =
	      XmlGetElementOfType(XmlDocumentContentList(document), current_keyboard_type, 0)) == NULL)
	{
		KTError ("'%s' is invalid because it has no current-keyboard "
		         "element as the root element", CURRENT_KEYBOARD_FILE);
		exit (EXIT_FAILURE);
	}
	if ((model_element =
	      XmlGetElementOfType(XmlElementContentList(current_keyboard_element), model_type, 0)) == NULL ||
	    (manufacturer_element = 
	      XmlGetElementOfType(XmlElementContentList(current_keyboard_element), manufacturer_type, 0)) == NULL)
	{
		KTError ("'%s' is invalid", CURRENT_KEYBOARD_FILE);
		exit (EXIT_FAILURE);
	}
	keyboard_name->model = XmlGetElementString(model_element, "");
	keyboard_name->manufacturer = XmlGetElementString(manufacturer_element, "");
	
	XmlFreeDocument (document);
}
