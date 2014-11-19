/*---------------------------------------------------------------------------------
Name               : check_syntax.c
Author             : Marvin Raaijmakers
Description        : Checks if the syntax of a keyboard file is valid
Date of last change: 29-Sep-2007
History            : 21-Jul-2007 Added check_keyboard_info() and checks for
                                 'usb-code' elements in check_key_settings()
                     20-Jul-2007 Fixed memory leak (key_name) in check_key_settings
                     28-Jun-2007 Fixed compiler warnings
                     17-Sep-2006 Added support for ACPI hotkeys
                     24-Jun-2006 Added checks for keycodes and scancodes

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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define _XOPEN_SOURCE /* glibc2 needs this */
#include <time.h>

#include <mxml.h>
#include <keytouch-editor.h>
#include <config_element_types.h>
#include <check_syntax.h>
#include <string_to_keycode.h>


static Boolean check_action (	XmlContent		*action_element,
				XmlElementType		*pluginname_type,
				XmlElementType		*pluginfunction_type,
				XmlAttributeName	*action_type_attrname );
static Boolean check_key_settings (	XmlContent		*keylist_element,
					int			index,
					XmlElementType		*key_type,
					XmlElementType		*keyname_type,
					XmlElementType		*keycode_type,
					XmlElementType		*scancode_type,
					XmlElementType		*usbcode_type,
					XmlElementType		*acpi_event_descr_type,
					XmlElementType		*default_action_type,
					XmlElementType		*pluginname_type,
					XmlElementType		*pluginfunction_type,
					XmlAttributeName	*action_type_attrname,
					XmlAttributeName	*key_type_attrname,
					unsigned int		*num_keys,
					char			*key_names[MAX_NUM_KEYS],
					Boolean			keycode_used[NUM_KEYCODES],
					Boolean			*valid );
static Boolean syntax_version_ok (	XmlContent	*file_info_element,
					XmlDocument	*document,
					unsigned int	version_major,
					unsigned int	version_minor );
static Boolean check_last_change_date (	XmlContent	*file_info_element,
					XmlDocument	*document );
static Boolean check_keyboard_info (XmlDocument *keyboard_file, XmlContent  *keyboard_element);



Boolean
check_action (	XmlContent		*action_element,
		XmlElementType		*pluginname_type,
		XmlElementType		*pluginfunction_type,
		XmlAttributeName	*action_type_attrname )
/*
Input:
	action_element		- The element to read from.
	pluginname_type		- The XmlElementType for the plugin-name element
	pluginfunction_type	- The XmlElementType for the plugin-function element
	action_type_attrname	- The XmlElementName for the action-type attribute
Returns:
	TRUE if 'action_element' has a correct syntax, otherwise FALSE.
Desciption:
	This function checks if 'action_element' has a correct syntax, otherwise
	FALSE.
*/
{
	char		*action_type_argument;
	Boolean		error;
	
	error = FALSE;
	/* Get the type of the action */
	action_type_argument = XmlGetAttributeValue (action_type_attrname, action_element);
	/* If it is a plugin */
	if (action_type_argument && strcmp(action_type_argument, ATTR_VAL_PLUGIN) == 0)
	{
		if (XmlGetElementOfType(XmlElementContentList(action_element), pluginname_type, 0) == NULL ||
		    XmlGetElementOfType(XmlElementContentList(action_element), pluginfunction_type, 0) == NULL)
		{
			KTSetErrorMsg (_("The keyboard file contains an invalid action "
			                 "element (where the action is a plugin)."), "");
			error = TRUE;
		}
	}
	/* If it is not a program */
	else if (action_type_argument != NULL && strcmp(action_type_argument, ATTR_VAL_PROGRAM) != 0)
	{
		KTSetErrorMsg (_("The keyboard file contains the invalid action-type value '%s'."), action_type_argument);
		error = TRUE;
	}
	
	return !error;
}


Boolean
check_key_settings (	XmlContent		*keylist_element,
			int			index,
			XmlElementType		*key_type,
			XmlElementType		*keyname_type,
			XmlElementType		*keycode_type,
			XmlElementType		*scancode_type,
			XmlElementType		*usbcode_type,
			XmlElementType		*acpi_event_descr_type,
			XmlElementType		*default_action_type,
			XmlElementType		*pluginname_type,
			XmlElementType		*pluginfunction_type,
			XmlAttributeName	*action_type_attrname,
			XmlAttributeName	*key_type_attrname,
			unsigned int		*num_keys,
			char			*key_names[MAX_NUM_KEYS],
			Boolean			keycode_used[NUM_KEYCODES],
			Boolean			*valid )
/*
Input:
	keylist_element		- The parent of the element to check.
	index			- Read the index'th key element
	key_type		- The XmlElementType for the key element
	keyname_type		- The XmlElementType for the key-name element
	keycode_type		- The XmlElementType for the keycode element
	scancode_type		- The XmlElementType for the scancode element
	usbcode_type		- The XmlElementType for the usb-code element
	acpi_event_descr_type	- The XmlElementType for the event-descr element
	default_action_type	- The XmlElementType for the default-action element
	pluginname_type		- The XmlElementType for the plugin-name element
	pluginfunction_type	- The XmlElementType for the plugin-function element
	action_type_attrname	- The XmlElementName for the action-type attribute
	key_type_attrname	- The XmlElementName for the key-type attribute
	num_keys		- The number of elements used in 'key_names'
	key_names		- Array of pointers to key names that are in the
				  keyboard file.
	keycode_used		- For keycode i, keycode_used[i] indicates if it is
				  used already in the keyboard file.
Output:
	valid			- Will be TRUE if the index'th element of 'key_type'
				  in 'keylist_element', is valid (otherwise FALSE).
	num_keys		- If '*valid' is set to TRUE then 'num_keys' will be
				  increased with 1 if it is smaller than
				  MAX_NUM_KEYS-1.
	key_names		- If '*valid' is set to TRUE then the (*num_key-1)'th
				  (if *num_keys < MAX_NUM_KEYS) element of this array,
				  will point to the key name of the checked key
				  setting. The memory of this name is allocated and
				  should be freed if it is no longer needed.
	keycode_used		- For keycode i, keycode_used[i] indicates if it is
				  used already in the keyboard file.
Returns:
	TRUE if the index'th element of 'key_type' in 'keylist_element' exist.
Desciption:
	This function checks if the key_name and default_action member, of the
	index'th key element of 'keylist_element', are valid.
*/
{
	XmlContent	*key_element,
			*event_descr_element,
			*keyname_element,
			*keycode_element,
			*scancode_element,
			*usbcode_element,
			*default_action_element;
	unsigned int	i, keycode;
	char		*key_name,
			*key_type_str,
			*end_pointer,
			*keycode_string,
			*scancode_string;
	Boolean		is_acpi_hotkey;
	
	*valid = TRUE;
	key_element = XmlGetElementOfType(XmlElementContentList(keylist_element), key_type, index);
	if (key_element)
	{
		key_type_str = XmlGetAttributeValue (key_type_attrname, key_element);
		is_acpi_hotkey = (key_type_str && !strcmp(key_type_str, ATTR_VAL_ACPIHOTKEY));
		
		keyname_element = XmlGetElementOfType(XmlElementContentList(key_element), keyname_type, 0);
		default_action_element = XmlGetElementOfType(XmlElementContentList(key_element), default_action_type, 0);
		keycode_element = XmlGetElementOfType(XmlElementContentList(key_element), keycode_type, 0);
		event_descr_element = XmlGetElementOfType(XmlElementContentList(key_element), acpi_event_descr_type, 0);
		scancode_element = XmlGetElementOfType(XmlElementContentList(key_element), scancode_type, 0);
		usbcode_element = XmlGetElementOfType(XmlElementContentList(key_element), usbcode_type, 0);
		
		if (keyname_element == NULL || default_action_element == NULL || keycode_element == NULL ||
		    (is_acpi_hotkey ?
		      event_descr_element == NULL :
		      (scancode_element == NULL && usbcode_element == NULL) ))
		{
			KTSetErrorMsg (_("The keyboard file contains an incomplete key element."), "");
			*valid = FALSE;
		}
		else
		{
			key_name = XmlGetElementString (keyname_element, "");
			if (*key_name == '\0')
			{
				KTSetErrorMsg (_("The keyboard file contains an empty key-name element."), "");
				*valid = FALSE;
			}
			else
			{
				for (i = 0; i < *num_keys && strcmp (key_name, key_names[i]) != 0; i++)
					; /* NULL Statement */
				if (i < *num_keys)
				{
					KTSetErrorMsg (_("The keyboard file contains more than once a key named '%s'."), key_name);
					*valid = FALSE;
				}
				else if ( !check_action (default_action_element, pluginname_type,
					                 pluginfunction_type, action_type_attrname) )
				{
					*valid = FALSE;
				}
				
			}
			if (*valid)
			{
				if (!is_acpi_hotkey)
				{
					if (scancode_element)
					{
						/** Check the scancode **/
						scancode_string = XmlGetElementString(scancode_element, "");
						/* Convert the string of the scancode to a integer */
						strtol (scancode_string, &end_pointer, 0);
						/* If the scancode string is not a valid scancode */
						if (*end_pointer != '\0')
						{
							KTSetErrorMsg (_("'%s' is an invalid scancode."), scancode_string);
							*valid = FALSE;
						}
						XmlFree (scancode_string);
					}
					if (usbcode_element)
					{
						/** Check the USB code **/
						scancode_string = XmlGetElementString(usbcode_element, "");
						/* Convert the string of the USB code to a integer */
						strtol (scancode_string, &end_pointer, 16);
						/* If the string is not a valid USB code */
						if (*end_pointer != '\0')
						{
							KTSetErrorMsg (_("'%s' is an invalid USB code."), scancode_string);
							*valid = FALSE;
						}
						XmlFree (scancode_string);
					}
				}
				
				if (*valid)
				{
					/** Check the keycode **/
					keycode_string = XmlGetElementString (keycode_element, "");
					keycode = string_to_kernel_keycode (keycode_string);
					if (keycode)
					{
						if (keycode_used[keycode])
						{
							KTSetErrorMsg (_("Keycode %s is used more than once."), keycode_string);
							*valid = FALSE;
						}
						else
						{
							keycode_used[keycode] = TRUE;
						}
					}
					else
					{
						if (keycode_string[0] == '\0')
						{
							KTSetErrorMsg (_("The keyboard file contains an empty keycode."), "");
						}
						else
						{
							KTSetErrorMsg (_("'%s' is an invalid keycode."), keycode_string);
						}
						*valid = FALSE;
					}
					XmlFree (keycode_string);
					
					if (*valid && *num_keys < MAX_NUM_KEYS)
					{
						key_names[*num_keys] = key_name;
						(*num_keys)++;
					}
				}
			}
			/* If the key is invalid or the name could
			 * not have been added to the name list */
			if (!*valid || *num_keys >= MAX_NUM_KEYS)
			{
				XmlFree (key_name);
			}
		}
	}
	
	return (key_element != NULL);
}


Boolean
check_last_change_date (	XmlContent	*file_info_element,
				XmlDocument	*document )
/*
Input:
	file_info_element	- The last-change element will be read from this
				  element
	document		- file_info_element must located in this XmlDocument
Returns:
	TRUE if the date was succesfully read, otherwise FALSE.
Description:
	This function reads the last-change element that is located in
	file_info_element and checks if it is OK.
*/
{
	XmlElementType		*last_change_type;
	XmlContent		*last_change_element;
	XmlAttributeName	*format_attrname;
	char			*date_string,
				*format;
	Boolean			return_val;
	struct tm		date;
	
	/* Get the last-change element */
	if ((last_change_type = XmlGetElementType(ELEM_TYPE_LASTCHANGE, document, FALSE)) == NULL ||
	    (last_change_element =
	      XmlGetElementOfType(XmlElementContentList(file_info_element), last_change_type, 0)) == NULL)
	{
		return (FALSE);
	}
	/* Get the value of the format attribute */
	if ((format_attrname = XmlGetAttributeName(ATTR_NAME_FORMAT, document, FALSE)) == NULL ||
	    (format = XmlGetAttributeValue(format_attrname, last_change_element)) == NULL)
	{
		return (FALSE);
	}
	
	date_string = XmlGetElementString(last_change_element, "");
	/* Convert string to a time tm structure */
	return_val = (strptime(date_string, format, &date) != NULL);
	if (!return_val)
	{
		KTSetErrorMsg (_("The last-change-date \"%s\" of the keyboard file is not a valid date."), date_string);
	}
	XmlFree (date_string);
	
	return (return_val);
}


Boolean
syntax_version_ok (	XmlContent	*file_info_element,
			XmlDocument	*document,
			unsigned int	version_major,
			unsigned int	version_minor )
/*
Input:
	file_info_element	- The syntax version will be read from this element
	document		- file_info_element must located in this XmlDocument
	version_major		- The compatible major syntax version number
	version_minor		- The latest compatible minor syntax version number
Output:
	-
Returns:
	TRUE if the syntax version was succesfully read, the major version number
	equals version_major and the minor version number is less than or equal to
	version_minor. Otherwise FALSE is returned.
Description:
	This function reads the syntax-version element that is located in 
	file_info_element. This function will return TRUE if this version is
	compatible with the version specified in the input variables (version_major
	and version_minor).
*/
{
	XmlElementType	*syntax_version_type;
	XmlContent	*syntax_version_element;
	char		*version_string;
	unsigned int	maj, min;
	
	if ((syntax_version_type = XmlGetElementType (ELEM_TYPE_SYNTAXVERSION, document, FALSE)) == NULL ||
	    (syntax_version_element =
	      XmlGetElementOfType (XmlElementContentList(file_info_element), syntax_version_type, 0)) == NULL)
	{
		return (FALSE);
	}
	
	version_string = XmlGetElementString (syntax_version_element, "");
	/* Convert the string to version numbers */
	if (sscanf(version_string, "%d.%d", &maj, &min) != 2)
	{
		maj = min = 0;
	}
	XmlFree (version_string);
	
	/* Return TRUE if the version is compatible */
	return (maj == version_major && min <= version_minor);
}


Boolean
check_keyboard_info (XmlDocument *keyboard_file,
                     XmlContent  *keyboard_element)
{
	XmlContent *keyboard_name_element,
	           *keyboard_info_element,
	           *usb_info_element,
	           *vendor_id_element,
	           *product_id_element;
	XmlElementType *keyboard_name_type,
	               *keyboard_info_type,
	               *model_type,
	               *manufacturer_type,
	               *usb_info_type,
	               *vendor_id_type,
	               *product_id_type;
	char *string, *end_pointer;
	Boolean error;
	
	error = FALSE;
	if ((keyboard_info_type = XmlGetElementType(ELEM_TYPE_KEYBOARDINFO, keyboard_file, FALSE)) == NULL ||
	    (keyboard_name_type = XmlGetElementType(ELEM_TYPE_KEYBOARDNAME, keyboard_file, FALSE)) == NULL ||
	    (model_type = XmlGetElementType(ELEM_TYPE_MODEL, keyboard_file, FALSE)) == NULL ||
	    (manufacturer_type = XmlGetElementType(ELEM_TYPE_MANUFACTURER, keyboard_file, FALSE)) == NULL ||
	    (keyboard_info_element =
	      XmlGetElementOfType (XmlElementContentList(keyboard_element), keyboard_info_type, 0)) == NULL ||
	    (keyboard_name_element =
	      XmlGetElementOfType (XmlElementContentList(keyboard_info_element), keyboard_name_type, 0)) == NULL ||
	    XmlGetElementOfType (XmlElementContentList(keyboard_name_element), model_type, 0) == NULL ||
	    XmlGetElementOfType (XmlElementContentList(keyboard_name_element), manufacturer_type, 0) == NULL)
	{
		KTSetErrorMsg (_("The keyboard file does not provide enough information about the keyboard model."), "");
		error = TRUE;
	}
	else if ((usb_info_type = XmlGetElementType(ELEM_TYPE_USBINFO, keyboard_file, FALSE)) != NULL &&
	         (usb_info_element =
	           XmlGetElementOfType (XmlElementContentList(keyboard_info_element), usb_info_type, 0)) != NULL)
	{
		if ((vendor_id_type = XmlGetElementType(ELEM_TYPE_VENDORID, keyboard_file, FALSE)) == NULL ||
		    (vendor_id_element =
		      XmlGetElementOfType (XmlElementContentList(usb_info_element), vendor_id_type, 0)) == NULL)
		{
			KTSetErrorMsg (_("The 'usb-info' element does not contain a '%s' element."), ELEM_TYPE_VENDORID);
			error = TRUE;
		}
		else if ((product_id_type = XmlGetElementType(ELEM_TYPE_PRODUCTID, keyboard_file, FALSE)) == NULL ||
		         (product_id_element =
		           XmlGetElementOfType (XmlElementContentList(usb_info_element), product_id_type, 0)) == NULL)
		{
			KTSetErrorMsg (_("The 'usb-info' element does not contain a '%s' element."), ELEM_TYPE_PRODUCTID);
			error = TRUE;
		}
		else
		{
			string = XmlGetElementString(vendor_id_element, "");
			/* Convert the string of the vendor ID to a integer */
			strtol (string, &end_pointer, 16);
			/* If the string is not a valid vendor ID */
			if (*end_pointer != '\0')
			{
				KTSetErrorMsg (_("'%s' is an invalid vendor ID."), string);
				error = TRUE;
			}
			XmlFree (string);
			
			if (!error)
			{
				string = XmlGetElementString(product_id_element, "");
				/* Convert the string of the product ID to a integer */
				strtol (string, &end_pointer, 16);
				/* If the string is not a valid product ID */
				if (*end_pointer != '\0')
				{
					KTSetErrorMsg (_("'%s' is an invalid product ID."), string);
					error = TRUE;
				}
				XmlFree (string);
			}
		}
	}
	
	return !error;
}


Boolean
check_keyboard_file (XmlDocument *keyboard_file)
/*
Input:
	keyboard_file	- The contents of the keyboard file to check
Returns:
	TRUE if 'keyboard_file' is valid, otherwise FALSE.
Desciption:
	This function checks if the contents of 'keyboard_file' are valid according
	the "keyTouch 2.1 file documentation". If 'keyboard_file' is not valid then
	an error message describing why it is invalid can be retrieved using
	KTGetErrorMsg().
*/
{
	XmlContent		*keyboard_element,
				*file_info_element,
				*keylist_element;
	XmlElementType		*keyboard_type,
				*file_info_type,
				*keylist_type,
				*key_type,
				*keyname_type,
				*scancode_type,
				*usbcode_type,
				*event_descr_type,
				*keycode_type,
				*default_action_type,
				*pluginname_type,
				*pluginfunction_type;
	XmlAttributeName	*action_type_attrname,
				*key_type_attrname;
	Boolean			error, valid;
	unsigned int		index, num_keys;
	char			*key_names[MAX_NUM_KEYS];
	Boolean			keycode_used[NUM_KEYCODES] = {[0 ... NUM_KEYCODES-1] = 0};
	
	error = FALSE;
	
	event_descr_type = XmlGetElementType (ELEM_TYPE_ACPI_EVENT_DESCR, keyboard_file, TRUE);
	scancode_type = XmlGetElementType (ELEM_TYPE_SCANCODE, keyboard_file, TRUE);
	usbcode_type = XmlGetElementType (ELEM_TYPE_USBCODE, keyboard_file, TRUE);
	
	if ((keyboard_type = XmlGetElementType (ELEM_TYPE_KEYBOARD, keyboard_file, FALSE)) == NULL ||
	    (keyboard_element = XmlGetElementOfType (XmlDocumentContentList(keyboard_file), keyboard_type, 0)) == NULL)
	{
		KTSetErrorMsg (_("The keyboard file does not contain a keyboard element."), "");
		error = TRUE;
	}
	else if (!check_keyboard_info (keyboard_file, keyboard_element))
	{
		/* NOTE: KTSetErrorMsg() is already called by check_keyboard_info() */
		error = TRUE;
	}
	else if ((keylist_type = XmlGetElementType(ELEM_TYPE_KEYLIST, keyboard_file, FALSE)) == NULL ||
		 (keylist_element =
		   XmlGetElementOfType(XmlElementContentList(keyboard_element), keylist_type, 0)) == NULL)
	{
		KTSetErrorMsg (_("The keyboard file does not contain a key-list element."), "");
		error = TRUE;
	}
	else if ((file_info_type = XmlGetElementType(ELEM_TYPE_FILEINFO, keyboard_file, FALSE)) == NULL ||
	         (file_info_element =
		     XmlGetElementOfType(XmlElementContentList(keyboard_element), file_info_type, 0)) == NULL)
	{
		KTSetErrorMsg (_("The \"keyboard\" element in the keyboard file does not contain a %s element."), ELEM_TYPE_FILEINFO);
		error = TRUE;
	}
	else if (!syntax_version_ok (file_info_element, keyboard_file, SYNTAX_VERSION_MAJOR, SYNTAX_VERSION_MINOR))
	{
		KTSetErrorMsg (_("The syntax version of the keyboard file is not compatible with this version of keyTouch."), SYNTAX_VERSION_STRING);
		error = TRUE;
	}
	else if (!check_last_change_date (file_info_element, keyboard_file))
	{
		/* NOTE: KTSetErrorMsg() is already called by check_last_change_date() */
		error = TRUE;
	}
	else if ((default_action_type = XmlGetElementType(ELEM_TYPE_DEFAULTACTION, keyboard_file, FALSE)) == NULL)
	{
		KTSetErrorMsg (_("The keyboard file does not contain any %s element."), ELEM_TYPE_DEFAULTACTION);
		error = TRUE;
	}
	else if ((key_type = XmlGetElementType(ELEM_TYPE_KEY, keyboard_file, FALSE)) == NULL)
	{
		KTSetErrorMsg (_("The keyboard file does not contain any %s element."), ELEM_TYPE_KEY);
		error = TRUE;
	}
	else if ((keyname_type = XmlGetElementType(ELEM_TYPE_KEYNAME, keyboard_file, FALSE)) == NULL)
	{
		KTSetErrorMsg (_("The keyboard file does not contain any %s element."), ELEM_TYPE_KEYNAME);
		error = TRUE;
	}
	else if ((keycode_type = XmlGetElementType(ELEM_TYPE_KEYCODE, keyboard_file, FALSE)) == NULL)
	{
		KTSetErrorMsg (_("The keyboard file does not contain any %s element."), ELEM_TYPE_KEYCODE);
		error = TRUE;
	}
	else
	{
		action_type_attrname = XmlGetAttributeName (ATTR_NAME_ACTIONTYPE, keyboard_file, TRUE);
		key_type_attrname = XmlGetAttributeName (ATTR_NAME_KEYTYPE, keyboard_file, TRUE);
		/* Create the element types "plugin-name" and "plugin-function" if they
		 * do not appear in the document, because these element do not necessary
		 * appear in the document */
		pluginname_type = XmlGetElementType (ELEM_TYPE_PLUGINNAME, keyboard_file, TRUE);
		pluginfunction_type = XmlGetElementType (ELEM_TYPE_PLUGINFUNCTION, keyboard_file, TRUE);
		
		num_keys = 0;
		for (index = 0;
		     check_key_settings (keylist_element, index, key_type, keyname_type, keycode_type,
		                         scancode_type, usbcode_type, event_descr_type, default_action_type,
		                         pluginname_type, pluginfunction_type, action_type_attrname,
		                         key_type_attrname, &num_keys, key_names, keycode_used, &valid);
		     index++)
		{
			if (!valid)
			{
				error = TRUE;
				break;
			}
		}
		while (num_keys--)
		{
			XmlFree (key_names[num_keys]);
		}
	}
	
	return !error;
}

