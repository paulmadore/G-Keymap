/*---------------------------------------------------------------------------------
Name               : config.c
Author             : Marvin Raaijmakers
Description        : Reads and writes the configuration files
Date of last change: 19-Mar-2006
History            : 22-Jun-2006 Modified this file so that read_keyboard() uses
                                 check_keyboard_file()
                     19-Mar-2006 get_current_keyboard() first checks if
                                 current_keyboard.xml exists
                     12-Jan-2006 write_current_keyboard() now returns a Boolean to
                                 indicate if the file was successfully writen
                     10-Jan-2006 - Wrote read_keyboard()
                                 - Wrote get_kbfile_last_change()
                                 - Rewrote get_keyboard_name()
                     09-Jan-2006 Wrote syntax_version_ok() and added a call to this 
                                 function in get_keyboard_name()
                     03-Jan-2006 Now the program exits if KEYBOARD_FILES_DIR does
                                 not exist

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
#include <dirent.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <mxml.h>
#include <config_element_types.h>
#include <keytouch-keyboard.h>
#include <check_syntax.h>


static int always_true (const struct dirent *file);
static void add_name (	char		*element_type_string,
			char		*name,
			XmlContent	*curr_keyboard_element,
			XmlDocument	*document );
static Boolean get_keyboard_name (	XmlContent	*keyboard_element,
					XmlDocument	*document,
					KTKeyboardName	*keyboard_name );
static Boolean get_kbfile_last_change (	XmlContent	*file_info_element,
					XmlDocument	*document,
					struct tm	*kbfile_date );


Boolean
get_current_keyboard (KTKeyboardName *keyboard_name)
/*
Input:
	-
Output:
	keyboard_name	- The readen keyboard name
Returns:
	TRUE if the keyboard name was successfully read, otherwise FALSE.
Desciption:
	This function reads the name of the current keyboard.
*/
{
	XmlDocument	*document;
	XmlElementType	*current_keyboard_type,
			*model_type,
			*manufacturer_type;
	XmlContent	*current_keyboard_element,
			*model_element,
			*manufacturer_element;
	Boolean		successfully_read;
	
	successfully_read = FALSE;
	if ((access( CURRENT_KEYBOARD_FILE, F_OK ) != -1) && (document = XmlParseDocument(CURRENT_KEYBOARD_FILE)))
	{
		if ((current_keyboard_type = XmlGetElementType(ELEM_TYPE_CURRENTKEYBOARD, document, FALSE)) != NULL &&
		    (model_type = XmlGetElementType(ELEM_TYPE_MODEL, document, FALSE)) != NULL &&
		    (manufacturer_type = XmlGetElementType(ELEM_TYPE_MANUFACTURER, document, FALSE)) != NULL &&
		    (current_keyboard_element =
		     XmlGetElementOfType(XmlDocumentContentList(document), current_keyboard_type, 0)) != NULL &&
		    (model_element =
		     XmlGetElementOfType(XmlElementContentList(current_keyboard_element), model_type, 0)) != NULL &&
		    (manufacturer_element =
		     XmlGetElementOfType(XmlElementContentList(current_keyboard_element), manufacturer_type, 0)) != NULL)
		{
			keyboard_name->model = XmlGetElementString(model_element, "");
			keyboard_name->manufacturer = XmlGetElementString(manufacturer_element, "");
			successfully_read = TRUE;
		}
		XmlFreeDocument (document);
	}
	return successfully_read;
}


void
add_name (	char		*element_type_string,
		char		*name,
		XmlContent	*curr_keyboard_element,
		XmlDocument	*document              )
/*
Input:
	element_type_string	- The string of the element type to create
	name			- The string that the new element will contain
Output:
	curr_keyboard_element	- The new element will be add to this element
	document		- The document where curr_keyboard_element is in
Returns:
	-
Desciption:
	This function adds a element, who's type is element_type_string, to
	current_keyboard_element. The element will contain the string name.
*/
{
	XmlElementType	*element_type;
	XmlContent	*element,
			*string;
	
	element_type = XmlGetElementType(element_type_string, document, TRUE);
	element = XmlCreateElement(element_type, FALSE);
	string = XmlCreateContent(XmlCType_String);
	XmlStringSetString (string, name);
	XmlContentAppend (string, XmlElementContentList(element));
	XmlContentAppend (element, XmlElementContentList(curr_keyboard_element));
}


Boolean
write_current_keyboard (KTKeyboardName *keyboard_name)
/*
Input:
	keyboard_name	- The name of the current keyboard
Output:
	-
Returns:
	TRUE if the keyboard file was successfully written, otherwise FALSE.
Description:
	This function writes the name of the current keyboard (keyboard_name) to the
	the file that contains the current keyboards name.
*/
{
	XmlDocument	*document;
	XmlElementType	*element_type;
	XmlContent	*curr_keyboard_element,
			*element,
			*string;
	Boolean		return_val;
	
	document = XmlCreateDocument();
	element_type = XmlGetElementType(ELEM_TYPE_CURRENTKEYBOARD, document, TRUE);
	curr_keyboard_element = XmlCreateElement(element_type, FALSE);
	
	add_name (ELEM_TYPE_MANUFACTURER, keyboard_name->manufacturer, curr_keyboard_element, document);
	add_name (ELEM_TYPE_MODEL, keyboard_name->model, curr_keyboard_element, document);
	XmlContentAppend (curr_keyboard_element, XmlDocumentContentList(document));
	mkdir (KEYTOUCH_CONF_DIR, S_IWUSR | S_IRUSR | S_IXUSR | S_IXOTH | S_IROTH);
	/* Write the file */
	if (XmlWriteDocument(document, CURRENT_KEYBOARD_FILE))
	{
		return_val = TRUE;
	}
	else /* If writing the file failed */
	{
		KTError (_("An error occurred while saving the configuration. "
		           "Check your permissions for the file \"%s\"."), CURRENT_KEYBOARD_FILE);
		return_val = FALSE;
	}
	XmlFreeDocument (document);
	
	return (return_val);
}


Boolean
get_keyboard_name (	XmlContent	*keyboard_element,
			XmlDocument	*document,
			KTKeyboardName	*keyboard_name   )
/*
Input:
	keyboard_element	- The element that to read the keyboard-info element
	                          from
	document		- This must be the document keyboard_element is in
Output:
	keyboard_name	- The name of the keyboard that was read from the
	                  keyboard-info element in keyboard_element
Returns:
	TRUE if the keyboard name was succesfully read, otherwise FALSE.
Description:
	This function reads the name of the keyboard from keyboard_element and puts
	it in keyboard_name.
*/
{
	XmlElementType	*keyboard_name_type,
			*keyboard_info_type,
			*model_type,
			*manufacturer_type;
	XmlContent	*keyboard_name_element,
			*keyboard_info_element,
			*model_element,
			*manufacturer_element;
	
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
	
	keyboard_name->model = XmlGetElementString(model_element, "");
	keyboard_name->manufacturer = XmlGetElementString(manufacturer_element, "");
	
	return (TRUE);
}


Boolean
get_kbfile_last_change (	XmlContent	*file_info_element,
				XmlDocument	*document,
				struct tm	*kbfile_date )
/*
Input:
	file_info_element	- The last-change element will be read from this
				  element
	document		- file_info_element must located in this XmlDocument
Output:
	kbfile_date		- The date read from the last-change element
Returns:
	TRUE if the date was succesfully read, otherwise FALSE.
Description:
	This function reads the last-change element that is located in
	file_info_element and converts to a time tm structure that will be copied to
	kbfile_date.
*/
{
	XmlElementType		*last_change_type;
	XmlContent		*last_change_element;
	XmlAttributeName	*format_attrname;
	char			*date_string,
				*format;
	Boolean			return_val;
	
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
	return_val = (strptime(date_string, format, kbfile_date) != NULL);
	
	XmlFree (date_string);
	return (return_val);
}


Boolean
read_keyboard (	char		*file_name,
		KTKeyboardName	*keyboard_name,
		struct tm	*kbfile_date )
/*
Input:
	file_name	- The name of the keyboard file to read
Output:
	keyboard_name	- The name of the keyboard that was read from the keyboard
			  file
	kbfile_date	- The "last-changed" date that was read from the keyboard
			  file
Returns:
	TRUE if the keyboard file was succesfully read, otherwise FALSE.
Description:
	This function reads the name of the keyboard from the keyboard file, named
	file_name, and puts it in keyboard_name. It also reads "last-changed" date
	that will be put in kbfile_date. If it fails to read the keyboard file, an
	error message describing why it failed can be retrieved by calling
	KTGetErrorMsg().
*/
{
	XmlDocument	*document;
	XmlElementType	*keyboard_type,
			*file_info_type;
	XmlContent	*keyboard_element,
			*file_info_element;
	struct stat	file_attr;
	Boolean		return_val;
	
	stat (file_name, &file_attr);
	/* If the file is a directory */
	if (S_ISDIR(file_attr.st_mode))
	{
		KTSetErrorMsg (_("'%s' is a directory."), file_name);
		return FALSE;
	}
	document = XmlParseDocument(file_name);
	if (document == NULL)
	{
		KTSetErrorMsg (_("'%s' is not a valid XML document."), file_name);
		return_val = FALSE;
	}
	else
	{
		if (!check_keyboard_file (document) ||
		    /* The following functions will not return NULL if check_keyboard_file() returned TRUE: */
		    (keyboard_type = XmlGetElementType(ELEM_TYPE_KEYBOARD, document, FALSE)) == NULL ||
		    (file_info_type = XmlGetElementType(ELEM_TYPE_FILEINFO, document, FALSE)) == NULL ||
		    (keyboard_element =
		     XmlGetElementOfType(XmlDocumentContentList(document), keyboard_type, 0)) == NULL ||
		    (file_info_element =
		     XmlGetElementOfType(XmlElementContentList(keyboard_element), file_info_type, 0)) == NULL)
		{
			return_val = FALSE;
		}
		else
		{
			/* Will be TRUE because check_keyboard_file() returned TRUE: */
			return_val = get_kbfile_last_change (file_info_element, document, kbfile_date) &&
			             get_keyboard_name (keyboard_element, document, keyboard_name);
		}
		XmlFreeDocument (document);
	}
	return (return_val);
}



int
always_true (const struct dirent *file)
{
	return (TRUE);
}


void
read_keyboards (KTKeyboardList *keyboard_list)
/*
Input:
	-
Output:
	keyboard_list	- The keyboards will be add to this list.
Returns:
	-
Description:
	This function reads the plugins from the keyboard files directory and adds
	them to keyboard_list.
*/
{
	struct dirent	**file_list;
	int		num_files,
			count;
	KTKeyboardName	keyboard_name;
	char		*complete_file_name;
	struct tm	kbfile_date;
	
	num_files = scandir (KEYBOARD_FILES_DIR, &file_list, always_true, alphasort);
	if (num_files >= 0)
	{
		for (count = 0; count < num_files; count++)
		{
			complete_file_name = (char *) keytouch_malloc(strlen(KEYBOARD_FILES_DIR "/") +
			                                              strlen(file_list[count]->d_name) + 1);
			strcpy (complete_file_name, KEYBOARD_FILES_DIR "/");
			strcat (complete_file_name, file_list[count]->d_name);
			if (read_keyboard(complete_file_name, &keyboard_name, &kbfile_date))
			{
				insert_keyboard (&keyboard_name, kbfile_date, keyboard_list);
				XmlFree (keyboard_name.model);
				XmlFree (keyboard_name.manufacturer);
			}
			free (complete_file_name);
		}
	}
	else if (mkdir (KEYBOARD_FILES_DIR, S_IRWXU | S_IRGRP  | S_IXGRP | S_IROTH  | S_IXOTH) == -1)
	{
		KTError (_("The keyboard file directory '%s' does not exist and "
		           "could not be created, so this program cannot continue."), KEYBOARD_FILES_DIR);
		exit (EXIT_FAILURE);
	}
}


Boolean
old_keyboard_file (	KTKeyboardName	*keyboard_name,
			KTKeyboardList	*keyboard_list,
			struct tm	kbfile_date )
/*
Input:
	keyboard_name	- The name of the keyboard to check
	keyboard_list	- The list to search in for the keyboard named keyboard_name
	kbfile_date	- The "last-change" date of the keyboard file for the
	                  keyboard named keyboard_name
Output:
	-
Returns:
	TRUE if a keyboard named keyboard_name appears in keyboard_list AND the
	"last-change" date of the keyboard file of that keyboard is after
	kbfile_date. Otherwise FALSE will be returned.
*/
{
	KTKeyboardModel *model;
	
	model = find_keyboard_model (keyboard_name, keyboard_list);
	return (model &&
		model->kbfile_last_change.tm_year*10000 +
		model->kbfile_last_change.tm_mon*100 +
		model->kbfile_last_change.tm_mday >=
		kbfile_date.tm_year*10000 +
		kbfile_date.tm_mon*100 +
		kbfile_date.tm_mday);
}
