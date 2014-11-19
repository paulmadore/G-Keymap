/*----------------------------------------------------------------------------------

Name               : config.c
Author             : Marvin Raaijmakers
Description        : Reads and writes the keyboard files
Date of last change: 25-Aug-2007
History            : 25-Aug-2007 Added support for syntax version 1.2
                     24-Jun-2007 Compiler warnings fixes
                     ??-Sep-2006 Added support for ACPI hotkeys
                     01-Jul-2006 Added keyboard_file_is_valid()

    Copyright (C) 2005-2007 Marvin Raaijmakers

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
------------------------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include <gtk/gtk.h>

#include <mxml.h>
#include <config_element_types.h>
#include <keytouch-editor.h>
#include <set_data.h>
#include <check_syntax.h>


static KTAction read_action (	XmlContent		*action_element,
				XmlElementType		*pluginname_type,
				XmlElementType		*pluginfunction_type,
				XmlAttributeName	*action_type_attrname     );
static KTKey *read_key_settings (	XmlContent		*keylist_element,
					int			index,
					XmlElementType		*key_type,
					XmlElementType		*keyname_type,
					XmlElementType		*default_action_type,
					XmlElementType		*keycode_type,
					XmlElementType		*scancode_type,
					XmlElementType		*usbcode_type,
					XmlElementType		*event_descr_type,
					XmlElementType		*pluginname_type,
					XmlElementType		*pluginfunction_type,
					XmlAttributeName	*action_type_attrname,
					XmlAttributeName	*key_type_attrname     );
static void xml_add_string_element (	XmlContent	*parent,
					XmlElementType	*element_type,
					const char	*string );
static void add_file_info (	char		*author,
				XmlContent	*keyboard_element,
				XmlDocument	*document);
static void add_keyboard_info (char        *manufacturer,
                               char        *model,
                               XmlContent  *keyboard_element,
                               KTKeyList   *key_list,
                               int         usb_vendor_id,
                               int         usb_product_id,
                               XmlDocument *document);
static void add_key_list (	KTKeyList	*key_list,
				XmlContent	*keyboard_element,
				XmlDocument	*document );
static Boolean read_keyboard_info (XmlContent  *keyboard_element,
                                   XmlDocument *document,
                                   char        **manufacturer,
                                   char        **model,
                                   int         *usb_vendor_id,
                                   int         *usb_product_id);


/*********** The read part: ***********/


KTAction
read_action (	XmlContent		*action_element,
		XmlElementType		*pluginname_type,
		XmlElementType		*pluginfunction_type,
		XmlAttributeName	*action_type_attrname     )
/*
Input:
	action_element		- The element to read from.
	pluginname_type		- The XmlElementType for the plugin-name element
	pluginfunction_type	- The XmlElementType for the plugin-function element
	action_type_attrname	- The XmlElementName for the action-type attribute
Output:
	-
Returns:
	The readen action.
Desciption:
	This function reads the KTAction from action_element and returns it.
*/
{
	XmlContent	*pluginname_element,
			*pluginfunction_element;
	char		*action_type_argument,
			*plugin_name,
			*plugin_function;
	KTAction	action;
	
	/* Get the type of the action */
	action_type_argument = XmlGetAttributeValue (action_type_attrname, action_element);
	/* If it is a program */
	if (action_type_argument == NULL || strcmp(action_type_argument, ATTR_VAL_PROGRAM) == EQUAL)
	{
		action.type = KTActionTypeProgram;
		action.program.command = XmlGetElementString(action_element, "");
	}
	/* If it is a plugin */
	else if (strcmp(action_type_argument, ATTR_VAL_PLUGIN) == EQUAL)
	{
		action.type = KTActionTypePlugin;
		if ((pluginname_element =
		       XmlGetElementOfType(XmlElementContentList(action_element), pluginname_type, 0)) == NULL ||
		    (pluginfunction_element =
		       XmlGetElementOfType(XmlElementContentList(action_element), pluginfunction_type, 0)) == NULL)
		{
			KTError (_("The keyboard file contains an invalid action element."), "");
			action.type = KTActionTypeProgram;
			action.program.command = keytouch_strdup ("");
		}
		else
		{
			plugin_name = XmlGetElementString(pluginname_element, "");
			plugin_function = XmlGetElementString(pluginfunction_element, "");
			action.plugin.plugin_name = plugin_name;
			action.plugin.function_name = plugin_function;
		}
	}
	else
	{
		KTError (_("The keyboard file contains "
		           "an invalid action-type value."), "");
		action.type = KTActionTypeProgram;
		action.program.command = keytouch_strdup ("");
	}
	
	return (action);
}


KTKey
*read_key_settings (	XmlContent		*keylist_element,
			int			index,
			XmlElementType		*key_type,
			XmlElementType		*keyname_type,
			XmlElementType		*default_action_type,
			XmlElementType		*keycode_type,
			XmlElementType		*scancode_type,
			XmlElementType		*usbcode_type,
			XmlElementType		*event_descr_type,
			XmlElementType		*pluginname_type,
			XmlElementType		*pluginfunction_type,
			XmlAttributeName	*action_type_attrname,
			XmlAttributeName	*key_type_attrname     )
/*
Input:
	keylist_element		- The element to read from.
	index			- Read the index'th key element
	key_type		- The XmlElementType for the key element
	keyname_type		- The XmlElementType for the key-name element
	keycode_type		- The XmlElementType for the keycode element
	scancode_type		- The XmlElementType for the scancode element
	usbcode_type		- The XmlElementType for the usb-code element
	event_descr_type	- The XmlElementType for the event-descr element
	default_action_type	- The XmlElementType for the default-action element
	pluginname_type		- The XmlElementType for the plugin-name element
	pluginfunction_type	- The XmlElementType for the plugin-function element
	action_type_attrname	- The XmlAttributeName for the action-type attribute
	key_type_attrname	- The XmlAttributeName for the key-type attribute
Output:
	-
Returns:
	A pointer to the new KTKey if it was read, otherwise NULL.
Desciption:
	This function reads the key_name and default_action member from the index'th
	key element that is a child of keylist_element.
*/
{
	XmlContent	*key_element,
			*keycode_element,
			*scancode_element,
			*usbcode_element,
			*event_descr_element,
			*keyname_element,
			*default_action_element;
	char *tmp_string, *key_type_string,
	     *key_name,
	     *acpi_event_descr;
	int scancode,
	    usagecode,
	    keycode;
	KTKey *key;
	
	key_element = XmlGetElementOfType(XmlElementContentList(keylist_element), key_type, index);
	if (key_element == NULL)
	{
		return (NULL);
	}
	if ((keyname_element =
	       XmlGetElementOfType(XmlElementContentList(key_element), keyname_type, 0)) == NULL ||
	    (keycode_element =
	       XmlGetElementOfType(XmlElementContentList(key_element), keycode_type, 0)) == NULL ||
	    (default_action_element =
	       XmlGetElementOfType(XmlElementContentList(key_element), default_action_type, 0)) == NULL)
	{
		KTError (_("The keyboard file contains an incomplete key element."), "");
		return (NULL);
	}
	/* Read the key name */
	key_name = XmlGetElementString(keyname_element, "");
	
	/* Read the keycode... */
	tmp_string = XmlGetElementString(keycode_element, "");
	/* ... and convert it a real (= unsigned int) keycode */
	keycode = string2keycode(tmp_string);
	XmlFree (tmp_string);
	
	key_type_string = XmlGetAttributeValue(key_type_attrname, key_element);
	if (key_type_string && !strcmp (key_type_string, ATTR_VAL_ACPIHOTKEY) &&
	    (event_descr_element = XmlGetElementOfType(XmlElementContentList(key_element), event_descr_type, 0)) != NULL)
	{
		acpi_event_descr = XmlGetElementString (event_descr_element, "");
		scancode = 0;
		usagecode = 0;
	}
	else
	{
		acpi_event_descr = NULL;
		
		scancode_element = XmlGetElementOfType(XmlElementContentList(key_element), scancode_type, 0);
		if (scancode_element != NULL)
		{
			/* Read the scancode... */
			tmp_string = XmlGetElementString(scancode_element, "");
			/* ... and convert it to an integer */
			scancode = atoi(tmp_string);
			XmlFree (tmp_string);
		}
		else
		{
			scancode = 0;
		}
		usbcode_element = XmlGetElementOfType(XmlElementContentList(key_element), usbcode_type, 0);
		if (usbcode_element != NULL)
		{
			/* Read the usage code... */
			tmp_string = XmlGetElementString(usbcode_element, "");
			/* ... and convert it to an integer */
			usagecode = strtol(tmp_string, NULL, 0);
			XmlFree (tmp_string);
		}
		else
		{
			usagecode = 0;
		}
		if (usbcode_element == NULL && scancode_element == NULL)
		{
			KTError (_("The keyboard file contains an incomplete key element."), "");
			XmlFree (key_name);
			return (NULL);
		}
	}
	
	key = KTKey_new (key_name, scancode, usagecode, keycode, acpi_event_descr);
	XmlFree (key_name);
	
	KTKey_free_default_action (key);
	/* Read the default action */
	key->default_action = read_action (default_action_element,
	                                   pluginname_type,
	                                   pluginfunction_type,
	                                   action_type_attrname);
	
	return (key);
}


Boolean
read_keyboard_info (XmlContent  *keyboard_element,
                    XmlDocument *document,
                    char        **manufacturer,
                    char        **model,
                    int         *usb_vendor_id,
                    int         *usb_product_id)
/*
Input:
	keyboard_element - The element that to read the keyboard-info element from
	document         - This must be the document 'keyboard_element' is in
Output:
	manufacturer   - The manufacturer name that was read
	model          - The model name that was read
	usb_vendor_id  - The USB vendor ID if it was read
	usb_product_id - The USB product ID if it was read
Returns:
	TRUE if the keyboard name was succesfully read, otherwise FALSE.
Description:
	This function reads the name of the keyboard from 'keyboard_element' and puts
	it in 'manufacturer' and 'model'. If available in 'keyboard_element' then the
	values for 'usb_vendor_id' and 'usb_product_id' will also be read.
*/
{
	XmlElementType *keyboard_name_type,
	               *keyboard_info_type,
	               *usb_info_type,
	               *model_type,
	               *manufacturer_type,
	               *vendor_id_type,
	               *product_id_type;
	XmlContent *keyboard_name_element,
	           *keyboard_info_element,
	           *usb_info_element,
	           *model_element,
	           *manufacturer_element,
	           *vendor_id_element,
	           *product_id_element;
	char *tmp_string;
	
	if ((keyboard_info_type = XmlGetElementType(ELEM_TYPE_KEYBOARDINFO, document, FALSE)) == NULL ||
	    (keyboard_name_type = XmlGetElementType(ELEM_TYPE_KEYBOARDNAME, document, FALSE)) == NULL ||
	    (model_type = XmlGetElementType(ELEM_TYPE_MODEL, document, FALSE)) == NULL ||
	    (manufacturer_type = XmlGetElementType(ELEM_TYPE_MANUFACTURER, document, FALSE)) == NULL)
	{
		return (FALSE);
	}
	if ((keyboard_info_element =
	      XmlGetElementOfType(XmlElementContentList(keyboard_element), keyboard_info_type, 0)) == NULL ||
	    (keyboard_name_element =
	      XmlGetElementOfType(XmlElementContentList(keyboard_info_element), keyboard_name_type, 0)) == NULL ||
	    (model_element =
	      XmlGetElementOfType(XmlElementContentList(keyboard_name_element), model_type, 0)) == NULL ||
	    (manufacturer_element =
	      XmlGetElementOfType(XmlElementContentList(keyboard_name_element), manufacturer_type, 0)) == NULL)
	{
		return (FALSE);
	}
	
	*model = XmlGetElementString (model_element, "");
	*manufacturer = XmlGetElementString (manufacturer_element, "");
	
	/** Read the usb-info element **/
	usb_info_type = XmlGetElementType(ELEM_TYPE_USBINFO, document, TRUE);
	usb_info_element = XmlGetElementOfType(XmlElementContentList(keyboard_info_element), usb_info_type, 0);
	if (usb_info_element)
	{
		vendor_id_type = XmlGetElementType(ELEM_TYPE_VENDORID, document, TRUE);
		product_id_type = XmlGetElementType(ELEM_TYPE_PRODUCTID, document, TRUE);
		vendor_id_element = XmlGetElementOfType(XmlElementContentList(usb_info_element), vendor_id_type, 0);
		product_id_element = XmlGetElementOfType(XmlElementContentList(usb_info_element), product_id_type, 0);
		if (vendor_id_element && product_id_element)
		{
			tmp_string = XmlGetElementString (vendor_id_element, "");
			*usb_vendor_id = strtol (tmp_string, NULL, 0);
			XmlFree (tmp_string);
			
			tmp_string = XmlGetElementString (product_id_element, "");
			*usb_product_id = strtol (tmp_string, NULL, 0);
			XmlFree (tmp_string);
		}
	}
	
	return (TRUE);
}


Boolean
read_keyboard_file (KTKeyList *key_list,
                    char      **manufacturer,
                    char      **model,
                    int       *usb_vendor_id,
                    int       *usb_product_id,
                    char      *file_name)
/*
Input:
	file_name - The name of the keyboard file to read from
Output:
	key_list       - List containing information for every key
	manufacturer   -
	model          -
	usb_vendor_id  - If the keyboard file contains one
	usb_product_id - If the keyboard file contains one
Returns:
	TRUE if the file was successfully read, otherwise FALSE.
Desciption:
	This function reads the keyboard file, named file_name, which means that it
	creates for every special function key an entry in key_list and fills in the
	following elements:
	- name
	- keycode
	- scancode
	- usagecode
	- default action
*/
{
	XmlDocument		*document;
	XmlContent		*keyboard_element,
				*keylist_element;
	XmlElementType		*keyboard_type,
				*keylist_type,
				*key_type,
				*keyname_type,
				*default_action_type,
				*scancode_type,
				*usbcode_type,
				*keycode_type,
				*pluginname_type,
				*pluginfunction_type,
				*event_descr_type;
	XmlAttributeName	*action_type_attrname,
				*key_type_attrname;
	KTKey			*key;
	int			count;
	
	document = XmlParseDocument(file_name);
	if (document == NULL)
	{
		KTError (_("'%s' is an invalid keyboard file."), file_name);
		return FALSE;
	}
	
	keyboard_type = XmlGetElementType(ELEM_TYPE_KEYBOARD, document, FALSE);
	if (keyboard_type == NULL ||
	    (keyboard_element = XmlGetElementOfType(XmlDocumentContentList(document), keyboard_type, 0)) == NULL)
	{
		KTError (_("The keyboard file '%s' does not contain a keyboard element."), file_name);
		XmlFreeDocument (document);
		return FALSE;
	}
	
	keylist_type = XmlGetElementType(ELEM_TYPE_KEYLIST, document, FALSE);
	if (keylist_type == NULL ||
	    (keylist_element =
	      XmlGetElementOfType(XmlElementContentList(keyboard_element), keylist_type, 0)) == NULL)
	{
		KTError (_("The keyboard file '%s' does not contain a key-list element."), file_name);
		XmlFreeDocument (document);
		return FALSE;
	}
	
	if ((key_type = XmlGetElementType(ELEM_TYPE_KEY, document, FALSE)) == NULL ||
	    (default_action_type = XmlGetElementType(ELEM_TYPE_DEFAULTACTION, document, FALSE)) == NULL ||
	    (keyname_type = XmlGetElementType(ELEM_TYPE_KEYNAME, document, FALSE)) == NULL ||
	    (keycode_type = XmlGetElementType(ELEM_TYPE_KEYCODE, document, FALSE)) == NULL)
	{
		KTError (_("The keyboard file '%s' is invalid."), file_name);
		XmlFreeDocument (document);
		return FALSE;
	}
	scancode_type = XmlGetElementType(ELEM_TYPE_SCANCODE, document, TRUE);
	usbcode_type = XmlGetElementType(ELEM_TYPE_USBCODE, document, TRUE);
	event_descr_type = XmlGetElementType(ELEM_TYPE_ACPI_EVENT_DESCR, document, TRUE);
	action_type_attrname = XmlGetAttributeName(ATTR_NAME_ACTIONTYPE, document, TRUE);
	key_type_attrname = XmlGetAttributeName(ATTR_NAME_KEYTYPE, document, TRUE);
	/* Create the element types "plugin-name" and "plugin-function" if they
	 * do not appear in the document, because these element do not necessary
	 * appear in the document */
	pluginname_type = XmlGetElementType(ELEM_TYPE_PLUGINNAME, document, TRUE);
	pluginfunction_type = XmlGetElementType(ELEM_TYPE_PLUGINFUNCTION, document, TRUE);
	
	/* If reading the keyboard name failed */
	if (read_keyboard_info (keyboard_element, document, manufacturer,
	                        model, usb_vendor_id, usb_product_id) == FALSE)
	{
		KTError (_("Failed to read the keyboard name from %s."), file_name);
		XmlFreeDocument (document);
		return FALSE;
	}
	
	/* Now read the key settings list */
	KTKeyList_head(key_list) = read_key_settings(keylist_element, 0, key_type,
	                                             keyname_type, default_action_type,
	                                             keycode_type, scancode_type, usbcode_type,
	                                             event_descr_type, pluginname_type,
	                                             pluginfunction_type, action_type_attrname,
	                                             key_type_attrname);
	KTKeyList_tail(key_list) = KTKeyList_head(key_list);
	key = KTKeyList_head(key_list);
	for (count = 1; key != NULL; count++)
	{
		key = read_key_settings(keylist_element, count, key_type,
		                        keyname_type, default_action_type,
		                        keycode_type, scancode_type, usbcode_type,
		                        event_descr_type, pluginname_type,
		                        pluginfunction_type, action_type_attrname,
		                        key_type_attrname);
		KTKeyList_tail(key_list)->next = key;
		if (key != NULL)
		{
			KTKeyList_tail(key_list) = key;
		}
	}
	XmlFreeDocument (document);
	return TRUE;
}


/*********** The write part: ***********/


void
xml_add_string_element (	XmlContent	*parent,
				XmlElementType	*element_type,
				const char	*string )
/*
Input:
	element_type	- The type of the new element
	string		- The string the new element will contain
Output:
	parent		- The parent element where the new element will be add to
Returns:
	-
Description:
	This functions creates a new element of type element_type and adds it to parent.
	The new element will contain the string 'string'.
*/
{
	XmlContent	*new_element,
			*xml_string;
	
	new_element = XmlCreateElement(element_type, FALSE);
	XmlContentAppend (new_element, XmlElementContentList(parent));
	xml_string = XmlCreateContent(XmlCType_String);
	XmlStringSetString (xml_string, (char *) string);
	XmlContentAppend (xml_string, XmlElementContentList(new_element));
}


void
add_file_info (	char		*author,
		XmlContent	*keyboard_element,
		XmlDocument	*document)
/*
Input:
	author			- The author of the keyboard file
	keyboard_element	- The file-info element will be add to this element.
	document		- The document that contains keyboard_element.
Output:
	keyboard_element	- The file-info element will be add to this element.
	document		- The document that contains keyboard_element. New
				  element types and attribute names will be add to
				  this document.
Returns:
	-
Description:
	This functions creates a file-info element and adds it to keyboard_element.
	The file-info element will contain the following sub-elements:
	- syntax-version: Contains the SYNTAX_VERSION.
	- last-change: Contains todays date.
	- author: Will contain the contents of 'author'.
*/
{
	XmlContent	*file_info_element,
			*last_change_element,
			*last_change_string;
	XmlElementType	*element_type;
	char		date[MAX_DATE_LEN+1];
	time_t		current_time;
	
	element_type = XmlGetElementType(ELEM_TYPE_FILEINFO, document, TRUE);
	file_info_element = XmlCreateElement(element_type, FALSE);
	XmlContentAppend (file_info_element, XmlElementContentList(keyboard_element));
	
	/** Create the syntax-version element containing SYNTAX_VERSION and add it to
	 ** the file info element  **/
	xml_add_string_element (file_info_element,
				XmlGetElementType(ELEM_TYPE_SYNTAXVERSION, document, TRUE),
				SYNTAX_VERSION);
	
	/** Create the last change element and add it to the file info element **/
	element_type = XmlGetElementType(ELEM_TYPE_LASTCHANGE, document, TRUE);
	last_change_element = XmlCreateElement(element_type, FALSE);
	XmlContentAppend (last_change_element, XmlElementContentList(file_info_element));
	/** Get the current time, convert it to a string and put it in date **/
	time (&current_time);
	strftime (date, MAX_DATE_LEN, DATE_FORMAT, localtime(&current_time));
	/* Create the XmlContent string that will contain date */
	last_change_string = XmlCreateContent(XmlCType_String);
	/* Let last_change_string contain date */
	XmlStringSetString (last_change_string, date);
	/* Add last_change_string to last_change_element */
	XmlContentAppend (last_change_string, XmlElementContentList(last_change_element));
	/* Set the date format we used */
	XmlSetAttributeValue (	XmlGetAttributeName (ATTR_NAME_FORMAT, document, TRUE),
				DATE_FORMAT,
				last_change_element );
	/** Create an author element, in the file info element, containing the 'author'
	 ** string. **/
	xml_add_string_element (file_info_element,
				XmlGetElementType(ELEM_TYPE_AUTHOR, document, TRUE),
				author);
}


void
add_keyboard_info (char        *manufacturer,
                   char        *model,
                   XmlContent  *keyboard_element,
                   KTKeyList   *key_list,
                   int         usb_vendor_id,
                   int         usb_product_id,
                   XmlDocument *document)
/*
Input:
	manufacturer     - The keyboard manufacturer
	model            - The keyboard model
	keyboard_element - The keyboard-info element will be add to this
	                   element.
	document         - The document that contains keyboard_element.
	key_list         - The list of keys that will be written to the
	                   document
Output:
	keyboard_element - The keyboard-info element will be add to this
	                   element.
	document         - The document that contains keyboard_element. New
	                   element types and attribute names will be add to
	                   this document.
Returns:
	-
Description:
	This functions creates a keyboard-info element and adds it to
	keyboard_element. The keyboard-info element contains a keyboard-name sub-
	element which contains the following sub elements:
	- manufacturer: Will contain the value of 'manufacturer'.
	- model: Will contain the value of 'model'.
	If 'key_list' contains at least 1 key with an usage code, then an
	usb-info element will be added to the keyboard-info element. The usb-info
	element will have two sub elements:
	- vendor-id: Will contain a hexadecimal representation (preceded by "0x"
	             of 'usb_vendor_id'.
	- product-id: Will contain a hexadecimal representation (preceded by "0x"
	              of 'usb_product_id'.
*/
{
	XmlContent *keyboard_info_element,
	           *keyboard_name_element,
	           *usb_info_element;
	XmlElementType *element_type;
	char str_usb_id[64];
	
	element_type = XmlGetElementType(ELEM_TYPE_KEYBOARDINFO, document, TRUE);
	keyboard_info_element = XmlCreateElement(element_type, FALSE);
	XmlContentAppend (keyboard_info_element, XmlElementContentList(keyboard_element));
	element_type = XmlGetElementType(ELEM_TYPE_KEYBOARDNAME, document, TRUE);
	keyboard_name_element = XmlCreateElement(element_type, FALSE);
	XmlContentAppend (keyboard_name_element, XmlElementContentList(keyboard_info_element));
	
	xml_add_string_element (keyboard_name_element,
	                        XmlGetElementType(ELEM_TYPE_MANUFACTURER, document, TRUE),
	                        manufacturer);
	xml_add_string_element (keyboard_name_element,
	                        XmlGetElementType(ELEM_TYPE_MODEL, document, TRUE),
	                        model);
	
	if (KTKeyList_contains_usb_key (key_list))
	{
		element_type = XmlGetElementType(ELEM_TYPE_USBINFO, document, TRUE);
		usb_info_element = XmlCreateElement(element_type, FALSE);
		XmlContentAppend (usb_info_element, XmlElementContentList(keyboard_info_element));
		
		snprintf (str_usb_id, sizeof(str_usb_id), "0x%x", usb_vendor_id);
		str_usb_id[sizeof(str_usb_id)-1] = '\0';
		xml_add_string_element (usb_info_element,
		                        XmlGetElementType(ELEM_TYPE_VENDORID, document, TRUE),
		                        str_usb_id);
		
		snprintf (str_usb_id, sizeof(str_usb_id), "0x%x", usb_product_id);
		str_usb_id[sizeof(str_usb_id)-1] = '\0';
		xml_add_string_element (usb_info_element,
		                        XmlGetElementType(ELEM_TYPE_PRODUCTID, document, TRUE),
		                        str_usb_id);
	}
}


void
add_key_list (	KTKeyList	*key_list,
		XmlContent	*keyboard_element,
		XmlDocument	*document )
/*
Input:
	key_list		- The list containg information for every extra
				  function key.
	keyboard_element	- The key list will be added to this element
	document		- The document containing keyboard_element
Output:
	keyboard_element	- The key list will be added to this element
	document		- The document containing keyboard_element
				  and after calling this function the key list.
Returns:
	-
Description:
	This function creates a key list that will be added to keyboard_element. The
	list will contain the information of 'key_list'.
*/
{
	XmlContent *key_list_element,
	           *key_element,
	           *default_action_element;
	XmlElementType *key_list_etype,
	               *key_etype,
	               *scancode_etype,
	               *usbcode_etype,
	               *keycode_etype,
	               *key_name_etype,
	               *default_action_etype,
	               *plugin_name_etype,
	               *plugin_function_etype,
	               *event_descr_etype;
	XmlAttributeName *action_type_attrname,
	                 *key_type_attrname;
	char scancode_string[MAX_SCANCODE_LEN+1],
	     usbcode_string[MAX_USBCODE_LEN+1],
	     *acpi_event_descr;
	int scancode, usagecode;
	KTKey *key;
	
	key_list_etype = XmlGetElementType(ELEM_TYPE_KEYLIST, document, TRUE);
	key_list_element = XmlCreateElement(key_list_etype, FALSE);
	XmlContentAppend (key_list_element, XmlElementContentList(keyboard_element));
	
	key_list_etype = XmlGetElementType(ELEM_TYPE_KEYLIST, document, TRUE);
	key_etype = XmlGetElementType(ELEM_TYPE_KEY, document, TRUE);
	scancode_etype = XmlGetElementType(ELEM_TYPE_SCANCODE, document, TRUE);
	usbcode_etype = XmlGetElementType(ELEM_TYPE_USBCODE, document, TRUE);
	keycode_etype = XmlGetElementType(ELEM_TYPE_KEYCODE, document, TRUE);
	key_name_etype = XmlGetElementType(ELEM_TYPE_KEYNAME, document, TRUE);
	default_action_etype = XmlGetElementType(ELEM_TYPE_DEFAULTACTION, document, TRUE);
	plugin_name_etype = XmlGetElementType(ELEM_TYPE_PLUGINNAME, document, TRUE);
	plugin_function_etype = XmlGetElementType(ELEM_TYPE_PLUGINFUNCTION, document, TRUE);
	event_descr_etype = XmlGetElementType(ELEM_TYPE_ACPI_EVENT_DESCR, document, TRUE);
	
	action_type_attrname = XmlGetAttributeName(ATTR_NAME_ACTIONTYPE, document, TRUE);
	key_type_attrname = XmlGetAttributeName(ATTR_NAME_KEYTYPE, document, TRUE);
	
	for (key = KTKeyList_head(key_list); key; key = key->next)
	{
		/* Create and append the key element */
		key_element = XmlCreateElement(key_etype, FALSE);
		XmlContentAppend (key_element, XmlElementContentList(key_list_element));
		
		/* Add the name element: */
		xml_add_string_element (key_element, key_name_etype, KTKey_get_name(key));
		
		acpi_event_descr = KTKey_get_acpi_event_descr (key);
		if (acpi_event_descr)
		{
			/* Set the key-type attibute of key_element to indicate that it contains
			   an event-descr element instead of a keycode element: */
			XmlSetAttributeValue (key_type_attrname, ATTR_VAL_ACPIHOTKEY, key_element);
			/* Add the event-descr element: */
			xml_add_string_element (key_element, event_descr_etype, acpi_event_descr);
		}
		else
		{
			usagecode = KTKey_get_usagecode(key);
			scancode = KTKey_get_scancode(key);
			if (usagecode)
			{
				/* Convert the usage code to a string */
				snprintf (usbcode_string, MAX_USBCODE_LEN+1, "0x%x", usagecode);
				usbcode_string[MAX_USBCODE_LEN] = '\0';
				/* Add the USB code element: */
				xml_add_string_element (key_element, usbcode_etype, usbcode_string);
			}
			if (scancode || (usagecode == 0 && scancode == 0))
			{
				/* Convert the scancode to a string */
				snprintf (scancode_string, MAX_SCANCODE_LEN+1, "%d", scancode);
				scancode_string[MAX_SCANCODE_LEN] = '\0';
				/* Add the scancode element: */
				xml_add_string_element (key_element, scancode_etype, scancode_string);
			}
		}
		
		/* Add the keycode element: */
		xml_add_string_element (key_element, keycode_etype,
		                        KTKey_get_keycode(key) ?
		                         keycode2string( KTKey_get_keycode(key) ) : "");
		
		if (KTKey_get_action_type(key) == KTActionTypePlugin)
		{
			default_action_element = XmlCreateElement(default_action_etype, FALSE);
			XmlSetAttributeValue (action_type_attrname, ATTR_VAL_PLUGIN, default_action_element);
			/* Add the default-action element: */
			XmlContentAppend (default_action_element, XmlElementContentList(key_element));
			/* Add the plugin element to the default-action element: */
			xml_add_string_element (default_action_element, plugin_name_etype, 
			                        KTKey_get_plugin_name(key));
			/* Add the plugin function element to the default-action element: */
			xml_add_string_element (default_action_element, plugin_function_etype,
			                        KTKey_get_plugin_function_name(key));
		}
		else
		{
			/* Add the default-action element: */
			xml_add_string_element (key_element, default_action_etype,
			                        KTKey_get_action_command(key));
		}
	}
}


Boolean
write_keyboard_file (char      *manufacturer,
                     char      *model,
                     KTKeyList *key_list,
                     int       usb_vendor_id,
                     int       usb_product_id,
                     char      *author,
                     char      *filename)
/*
Input:
	manufacturer    - The keyboards manufacturer
	model           - The keyboards model
	key_list        - The list containing information for every extra function
	                  key
	usb_vendor_id   -
	usb_product_id  -
	author          - The author of the keyboard file
	filename        - The name of the keyboard file to write to
Returns:
	TRUE if the keyboard file was successfully written, otherwise FALSE.
Description:
	This function creates a keyboard file, named 'filename', which will contain
	the contents of 'manufacturer', 'model', 'key_list' and 'author'.
*/
{
	XmlDocument *document;
	XmlElementType *element_type;
	XmlContent *keyboard_element;
	Boolean return_val;
	
	document = XmlCreateDocument();
	element_type = XmlGetElementType(ELEM_TYPE_KEYBOARD, document, TRUE);
	keyboard_element = XmlCreateElement(element_type, FALSE);
	XmlContentAppend (keyboard_element, XmlDocumentContentList(document));
	/* Add the file-info element: */
	add_file_info (author, keyboard_element, document);
	/* Add the keyboard-info element: */
	add_keyboard_info (manufacturer, model, keyboard_element, key_list, usb_vendor_id, usb_product_id, document);
	/* Add the key-list element */
	add_key_list (key_list, keyboard_element, document);
	
	return_val = XmlWriteDocument (document, filename);
	XmlFreeDocument (document);
	return return_val;
}


Boolean
keyboard_file_is_valid (char *file_name)
/*
Input:
	file_name	- The file name of the keyboard file to check
Returns:
	TRUE if the keyboard file, named 'file_name', is a valid keyboard file,
	otherwise FALSE. If FALSE is returned, an explaination message can be
	retrieved by KTGetErrorMsg().
*/
{
	XmlDocument *document;
	Boolean valid;
	
	document = XmlParseDocument (file_name);
	if (document)
	{
		valid = check_keyboard_file (document);
		XmlFreeDocument (document);
	}
	else
	{
		KTSetErrorMsg (_("'%s' is not a valid XML document."), file_name);
		valid = FALSE;
	}
	return valid;
}
