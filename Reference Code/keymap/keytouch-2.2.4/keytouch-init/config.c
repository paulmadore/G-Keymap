/*---------------------------------------------------------------------------------
Name               : config.c
Author             : Marvin Raaijmakers
Description        : Reads and writes the configuration files
Date of last change: 19-Mar-2006
History            : 19-Mar-2006 get_current_keyboard() first checks if
                                 current_keyboard.xml exists
                     26-Jan-2006 Modified read_keyboard_file() so that a keyboard
                                 file without a scancode element is still valid
                     11-Dec-2005 read_key_settings() now skips acpi-hotkeys

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
#include <unistd.h>
#include <mxml.h>

#include <string_to_keycode.h>
#include <keytouch-init.h> 

static KTKeySettings *read_key_settings (	XmlContent		*keylist_element,
						int			*index,
						XmlElementType		*key_type,
						XmlElementType		*scancode_type,
						XmlElementType		*keycode_type,
						XmlAttributeName	*key_type_attrname );


KTKeySettings
*read_key_settings (	XmlContent		*keylist_element,
			int			*index,
			XmlElementType		*key_type,
			XmlElementType		*scancode_type,
			XmlElementType		*keycode_type,
			XmlAttributeName	*key_type_attrname )
/*
Input:
	keylist_element		- The element to read from.
	index			- Read the index'th key element
	key_type		- The XmlElementType for the key element
	scancode_type		- The XmlElementType for the scancode element
	keycode_type		- The XmlElementType for the keycode element
	key_type_attrname	- The XmlAttributeName for the key-type attribute
				  name
Output:
	index			- If the value of "key-type" attribute of the
				  *index'th element is "acpi-hotkey". The integer
				  pointed to by index will be the first key element
				  not of key-type "acpi-hotkey" after the *index'th
				  key element.
Returns:
	A pointer to the new KTKeySettings if it was read, otherwise NULL.
Desciption:
	This function reads the key_name and keycode member from the *index'th key
	element that is a child of keylist_element.
*/
{
	XmlContent	*key_element,
			*scancode_element,
			*keycode_element;
	char		*keycode_string,
			*scancode_string,
			*end_pointer,
			*key_type_string;
	KTKeySettings	*key_settings;
	
	(*index)--;
	do {
		(*index)++;
		key_element = XmlGetElementOfType(XmlElementContentList(keylist_element), key_type, *index);
		if (key_element == NULL)
		{
			return (NULL);
		}
		key_type_string = XmlGetAttributeValue(key_type_attrname, key_element);
	} while (key_type_string != NULL && strcmp(key_type_string, "acpi-hotkey") == EQUAL);
	
	if ((scancode_element =
	       XmlGetElementOfType(XmlElementContentList(key_element), scancode_type, 0)) == NULL ||
	    (keycode_element =
	       XmlGetElementOfType(XmlElementContentList(key_element), keycode_type, 0)) == NULL)
	{
		KTError ("The keyboard file contains an incomplete key element.", "");
		exit (EXIT_FAILURE);
	}
	key_settings = keytouch_malloc(sizeof(KTKeySettings));
	/* Read the scancode */
	scancode_string = XmlGetElementString(scancode_element, "");
	/* Convert the string of the scancode to a integer */
	key_settings->scancode = strtol(scancode_string, &end_pointer, 0);
	/* If the scancode string is not a vallid scancode */
	if (*end_pointer != '\0')
	{
		KTError ("%s is an invalid scancode", scancode_string);
		exit (EXIT_FAILURE);
	}
	XmlFree (scancode_string);
	/* Read the string of the keycode */
	keycode_string = XmlGetElementString(keycode_element, "");
	/* Convert the keycode */
	key_settings->keycode = string_to_kernel_keycode(keycode_string);
	/* If the string is not a string for a keycode */
	if (key_settings->keycode == 0)
	{
		KTError ("The keyboard file contains an invalid keycode.", "");
		exit (EXIT_FAILURE);
	}
	XmlFree (keycode_string);
	
	key_settings->next = NULL;
	
	return (key_settings);
}


void
read_keyboard_file (	KTKeySettingsList	*key_list,
			KTKeyboardName		*keyboard_name  )
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
	- keyname
	- key_code
*/
{
	XmlDocument		*document;
	XmlContent		*keyboard_element,
				*keylist_element;
	XmlElementType		*keyboard_type,
				*keylist_type,
				*key_type,
				*scancode_type,
				*keycode_type;
	XmlAttributeName	*key_type_attrname;
	KTKeySettings		*key_settings;
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
	
	if ((keycode_type = XmlGetElementType("keycode", document, FALSE)) == NULL ||
	    (key_type = XmlGetElementType("key", document, FALSE)) == NULL)
	{
		KTError ("The keyboard file '%s' is invalid.", file_name);
		exit (EXIT_FAILURE);
	}
	scancode_type = XmlGetElementType("scancode", document, TRUE);
	key_type_attrname = XmlGetAttributeName ("key-type", document, TRUE);
	
	count = 0;
	/* Read the first key of the key settings list */
	key_list->head = key_list->tail = read_key_settings(keylist_element, &count, key_type,
	                                                    scancode_type, keycode_type,
	                                                    key_type_attrname);
	key_settings = key_list->head;
	/* Read the rest of the key settings list */
	for (count++; key_settings != NULL; count++)
	{
		key_settings = read_key_settings(keylist_element, &count, key_type,
		                                 scancode_type, keycode_type,
		                                 key_type_attrname);
		key_list->tail->next = key_settings;
		if (key_settings != NULL)
		{
			key_list->tail = key_settings;
		}
	}
	
	free (file_name);
	XmlFreeDocument (document);
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
	This function reads the name of the current keyboard from
	CURRENT_KEYBOARD_FILE. If CURRENT_KEYBOARD_FILE this function will exit the
	program quietly.
*/
{
	XmlDocument	*document;
	XmlElementType	*current_keyboard_type,
			*model_type,
			*manufacturer_type;
	XmlContent	*current_keyboard_element,
			*model_element,
			*manufacturer_element;
	
	/* If current_keyboard.xml does not exist */
	if (access( CURRENT_KEYBOARD_FILE, F_OK ) == -1)
	{
		exit (EXIT_SUCCESS);
	}
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
