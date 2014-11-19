/*---------------------------------------------------------------------------------
Name               : config.c
Author             : Marvin Raaijmakers
Description        : Reads and writes the configuration files
Date of last change: 30-Aug-2006
History            : 30-Aug-2006 In read_key_settings strcmp, for comparing the
                                 keycode with "VOLUMEUP" or "VOLUMEDOWN", by
                                 strcasecmp
                     19-Mar-2006 get_current_keyboard() exists quietly if
                                 CURRENT_KEYBOARD_FILE does not exist
                     26-Feb-2006 auto_repeat_on will be set by read_key_settings()
                     12-Jan-2006 read_key_settings_action() now continues after
                                 reading a key that does appear in the keyboard
                                 configuration file but not in the keyboard file
                     04-Jan-2006 read_key_settings() now allocates a character that
                                 will contain '\0' for the action.program.command
                                 member of the key settings

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
#include <keytouchd.h>

static KTKeySettings *read_key_settings (	XmlContent		*keylist_element,
						int			index,
						XmlElementType		*key_type,
						XmlElementType		*keyname_type,
						XmlElementType		*keycode_type      );
static void read_keyboard_file (	KTKeySettingsList	*key_list,
					KTKeyboardName		*keyboard_name    );
static Boolean read_key_settings_action (	KTKeySettingsList	*key_list,
						XmlContent		*keylist_element,
						int			index,
						XmlElementType		*key_type,
						XmlElementType		*keyname_type,
						XmlElementType		*action_type,
						XmlAttributeName	*action_type_attrname,
						XmlElementType		*pluginname_type,
						XmlElementType		*pluginfunction_type   );
static void read_keyboard_config (	KTKeySettingsList	*key_list,
					KTKeyboardName		*keyboard_name  );
static char *create_keyboard_config_file_name (KTKeyboardName *keyboard_name);


KTKeySettings
*read_key_settings (	XmlContent		*keylist_element,
			int			index,
			XmlElementType		*key_type,
			XmlElementType		*keyname_type,
			XmlElementType		*keycode_type       )
/*
Input:
	keylist_element		- The element to read from.
	index			- Read the index+1'th key element
	key_type		- The XmlElementType for the key element
	keyname_type		- The XmlElementType for the key-name element
	keycode_type		- The XmlElementType for the keycode element
Output:
	-
Returns:
	A pointer to the new KTKeySettings if it was read, otherwise NULL.
Desciption:
	This function reads the key_name and keycode member from the index+1'th key
	element that is a child of keylist_element.
	The action.type member of the KTKeySettings will be set to
	KTActionTypeProgram and the action.program.command member will point to an
	empty string that is allocated.
	The auto_repeat_on member of the KTKeySettings will be TRUE if the keycode
	(string) equals "VOLUMEUP" or "VOLUMEDOWN".
*/
{
	XmlContent	*key_element,
			*keyname_element,
			*keycode_element;
	char		*keycode_string;
	KTKeySettings	*key_settings;
	
	key_element = XmlGetElementOfType(XmlElementContentList(keylist_element), key_type, index);
	if (key_element == NULL)
	{
		return (NULL);
	}
	if ((keyname_element =
	       XmlGetElementOfType(XmlElementContentList(key_element), keyname_type, 0)) == NULL ||
	    (keycode_element =
	       XmlGetElementOfType(XmlElementContentList(key_element), keycode_type, 0)) == NULL)
	{
		KTError ("The keyboard file contains an incomplete key element.", "");
		exit (EXIT_FAILURE);
	}
	key_settings = keytouch_malloc(sizeof(KTKeySettings));
	/* Read the key name */
	key_settings->key_name = XmlGetElementString(keyname_element, "");
	/* Read the string of the keycode */
	keycode_string = XmlGetElementString(keycode_element, "");
	/* Convert the keycode */
	key_settings->keycode = string_to_xkeycode(keycode_string);
	/* If the string is not a string for a keycode */
	if (key_settings->keycode == 0)
	{
		KTError ("The keyboard file contains an invalid keycode: %s.", keycode_string);
		exit (EXIT_FAILURE);
	}
	key_settings->auto_repeat_on = !strcasecmp("VOLUMEUP", keycode_string) ||
	                               !strcasecmp("VOLUMEDOWN", keycode_string);
	XmlFree (keycode_string);
	/* Initialize the remaining elements: */
	key_settings->next = NULL;
	key_settings->action.type = KTActionTypeProgram;
	/* Allocate 1 character to be sure that a key always has an action */
	key_settings->action.program.command = keytouch_malloc(sizeof(char));
	*(key_settings->action.program.command) = '\0';
	
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
				*keyname_type,
				*keycode_type;
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
	    (key_type = XmlGetElementType("key", document, FALSE)) == NULL ||
	    (keyname_type = XmlGetElementType("name", document, FALSE)) == NULL)
	{
		KTError ("The keyboard file '%s' is invalid.", file_name);
		exit (EXIT_FAILURE);
	}
	
	/* Now read the key settings list */
	key_list->head = key_list->tail = read_key_settings(keylist_element, 0, key_type,
	                                                    keyname_type, keycode_type);
	key_settings = key_list->head;
	for (count = 1; key_settings != NULL; count++)
	{
		key_settings = read_key_settings(keylist_element, count, key_type,
		                                 keyname_type, keycode_type);
		key_list->tail->next = key_settings;
		if (key_settings != NULL)
		{
			key_list->tail = key_settings;
		}
	}
	free (file_name);
	XmlFreeDocument (document);
}


Boolean
read_key_settings_action (	KTKeySettingsList	*key_list,
				XmlContent		*keylist_element,
				int			index,
				XmlElementType		*key_type,
				XmlElementType		*keyname_type,
				XmlElementType		*action_type,
				XmlAttributeName	*action_type_attrname,
				XmlElementType		*pluginname_type,
				XmlElementType		*pluginfunction_type   )
/*
Input:
	key_list		- List of keys available on the current keyboard.
	keylist_element		- The element to read from.
	index			- Read the index+1'th key element
	key_type		- The XmlElementType for the key element
	keyname_type		- The XmlElementType for the key-name element
	action_type		- The XmlElementType for the action element
	action_type_attrname	- The XmlElementName for the action-type attribute
	pluginname_type		- The XmlElementType for the plugin-name element
	pluginfunction_type	- The XmlElementType for the plugin-function element
Output:
	key_list		- List containing the action member of the readen
				  key.
Returns:
	TRUE if the index+1'th key element exists, otherwise FALSE.
Desciption:
	This function reads action element of the index+1'th key element that is a
	child of keylist_element. The string value of the action element will be
	filled in in the matching key of key_list.
*/
{
	XmlContent	*key_element,
			*keyname_element,
			*action_element,
			*pluginname_element,
			*pluginfunction_element;
	char		*action_type_argument,
			*plugin_name,
			*plugin_function,
			*key_name;
	KTKeySettings	*key_settings;
	
	key_element = XmlGetElementOfType(XmlElementContentList(keylist_element), key_type, index);
	if (key_element == NULL)
	{
		return (FALSE);
	}
	if ((keyname_element =
	       XmlGetElementOfType(XmlElementContentList(key_element), keyname_type, 0)) == NULL ||
	    (action_element =
	       XmlGetElementOfType(XmlElementContentList(key_element), action_type, 0)) == NULL)
	{
		KTError ("The keyboard configuration file contains an incomplete key element.", "");
		exit (EXIT_FAILURE);
	}
	
	key_name = XmlGetElementString(keyname_element, "");
	key_settings = find_key_settings(key_name, key_list);
	if (key_settings) /* If the key appears in the keyboard file */
	{
		/* Because we previously allocated 1 character to be sure that
		 * a key always has an action
		 */
		free (key_settings->action.program.command);
		/* Get the type of the action */
		action_type_argument = XmlGetAttributeValue(action_type_attrname, action_element);
		/* If it is a program */
		if (action_type_argument == NULL || strcmp(action_type_argument, "program") == EQUAL)
		{
			key_settings->action.type = KTActionTypeProgram;
			key_settings->action.program.command = XmlGetElementString(action_element, "");
		}
		/* If it is a plugin */
		else if (strcmp(action_type_argument, "plugin") == EQUAL)
		{
			key_settings->action.type = KTActionTypePlugin;
			if (!(pluginname_element =
			      XmlGetElementOfType(XmlElementContentList(action_element), pluginname_type, 0)) ||
			    !(pluginfunction_element =
			      XmlGetElementOfType(XmlElementContentList(action_element), pluginfunction_type, 0)))
			{
				KTError ("The keyboard configuration file contains an invalid action element.", "");
				exit (EXIT_FAILURE);
			}
			plugin_name = XmlGetElementString(pluginname_element, "");
			plugin_function = XmlGetElementString(pluginfunction_element, "");
			
			key_settings->action.plugin.function = get_plugin_function(plugin_name, plugin_function);
			
			XmlFree (plugin_name);
			XmlFree (plugin_function);
		}
		else
		{
			KTError ("The keyboard configuration file contains "
				 "an invalid action-type value.", "");
			exit (EXIT_FAILURE);
		}
	}
	else
	{
		KTError ("The keyboard configuration file contains a key, named "
		         "\'%s\', which does not appear in the keyboard file.", key_name);
	}
	XmlFree (key_name);
	
	return (TRUE);
}


char
*create_keyboard_config_file_name (KTKeyboardName *keyboard_name)
{
	char	*home_dir,
		*file_name;
	
	home_dir = (char *)getenv("HOME");
	if (home_dir != NULL)
	{
		file_name = keytouch_malloc(	strlen(home_dir) +
						strlen("/" KEYBOARD_CONFIG_FILES_DIR "/.")+
						strlen(keyboard_name->model) +
						strlen(keyboard_name->manufacturer)+1  );
		strcpy (file_name, home_dir);
		strcat (file_name, "/" KEYBOARD_CONFIG_FILES_DIR "/");
		strcat (file_name, keyboard_name->model);
		strcat (file_name, ".");
		strcat (file_name, keyboard_name->manufacturer);
		return (file_name);
	}
	return (NULL);
}


void
read_keyboard_config (	KTKeySettingsList	*key_list,
			KTKeyboardName		*keyboard_name  )
/*
Input:
	keyboard_name	- The name of the keyboard to read the configuration from.
	key_list	- List of keys available on the current keyboard.
Output:
	key_list	- List containing for every key the action member.
Returns:
	-
Desciption:
	This function reads the keyboard configuration file of the keyboard named
	keyboard_name, which means that it for every special function key in
	key_list the action member is filled in.
*/
{
	XmlDocument		*document;
	XmlContent		*keyboard_element,
				*keylist_element;
	XmlElementType		*keyboard_type,
				*keylist_type,
				*key_type,
				*keyname_type,
				*action_type,
				*pluginname_type,
				*pluginfunction_type;
	XmlAttributeName	*action_type_attrname;
	char			*file_name;
	int			count;
	
	file_name = create_keyboard_config_file_name(keyboard_name);
	
	document = XmlParseDocument(file_name);
	if (document == NULL)
	{
		KTError ("'%s' is an invalid keyboard configuration file.", file_name);
		exit (EXIT_FAILURE);
	}
	
	keyboard_type = XmlGetElementType("keyboard", document, FALSE);
	if (keyboard_type == NULL ||
	    (keyboard_element = XmlGetElementOfType(XmlDocumentContentList(document), keyboard_type, 0)) == NULL)
	{
		KTError ("The keyboard configuration file does not contain a keyboard element.", "");
		exit (EXIT_FAILURE);
	}
	keylist_type = XmlGetElementType("key-list", document, FALSE);
	if (keylist_type == NULL ||
	    (keylist_element =
	       XmlGetElementOfType(XmlElementContentList(keyboard_element), keylist_type, 0)) == NULL)
	{
		KTError ("The keyboard configuration file does not contain a key-list element.", "");
		exit (EXIT_FAILURE);
	}
	
	if ((action_type = XmlGetElementType("action", document, FALSE)) == NULL ||
	    (key_type = XmlGetElementType("key", document, FALSE)) == NULL ||
	    (keyname_type = XmlGetElementType("name", document, FALSE)) == NULL)
	{
		KTError ("The keyboard configuration file is invalid.", "");
		exit (EXIT_FAILURE);
	}
	action_type_attrname = XmlGetAttributeName("action-type", document, TRUE);
	/* Create the element types "plugin-name" and "plugin-function" if they
	 * do not appear in the document, because these element do not necessary
	 * appear in the document */
	pluginname_type = XmlGetElementType("plugin-name", document, TRUE);
	pluginfunction_type = XmlGetElementType("plugin-function", document, TRUE);
	
	/* Read the action members of the keys */
	for (count = 0;
	     read_key_settings_action (key_list, keylist_element, count, key_type,                               
	                               keyname_type, action_type, action_type_attrname,
				       pluginname_type, pluginfunction_type);
	     count++)
		; /* NULL Statement */
	
	free (file_name);
	XmlFreeDocument (document);
}


void
read_configuration (	KTKeySettingsList	*key_list,
			KTKeyboardName		*keyboard_name  )
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
	/* Create the list and fill in for every KTKeySettings the following members:
	 * - key_name
	 * - keycode
	 */
	read_keyboard_file (key_list, keyboard_name);
	/* Fill in for every KTKeySettings the following action member */
	read_keyboard_config (key_list, keyboard_name);
}


char
*create_preferences_file_name (void)
{
	char	*file_name,
		*home_dir;
	
	home_dir = (char *)getenv("HOME");
	if (home_dir != NULL)
	{
		file_name = keytouch_malloc(	strlen(home_dir) +
						strlen("/" PREFERENCES_FILE) + 1  );
		strcpy (file_name, home_dir);
		strcat (file_name, "/" PREFERENCES_FILE);
		return (file_name);
	}
	return (NULL);
}


void
read_preferences (KTPreferences *preferences)
/*
Input:
	-
Output:
	preferences	- The readen preferences
Returns:
	-
Desciption:
	This function reads the users preferences from the preferences file.
*/
{
	XmlDocument	*document;
	char		*file_name;
	XmlElementType	*preferences_type,
			*documents_dir_type,
			*home_page_type,
			*search_page_type,
			*browser_type,
			*email_program_type,
			*chat_program_type;
	XmlContent	*preferences_element,
			*documents_dir_element,
			*home_page_element,
			*search_page_element,
			*browser_element,
			*email_program_element,
			*chat_program_element;
	
	file_name = create_preferences_file_name();
	document = XmlParseDocument(file_name);
	if (document == NULL)
	{
		KTError ("'%s' is an invalid preferences configuration file.", file_name);
		exit (EXIT_FAILURE);
	}
	if ((preferences_type = XmlGetElementType("preferences", document, FALSE)) == NULL ||
	    (documents_dir_type = XmlGetElementType("documents-dir", document, FALSE)) == NULL ||
	    (home_page_type = XmlGetElementType("home-page", document, FALSE)) == NULL ||
	    (search_page_type = XmlGetElementType("search-page", document, FALSE)) == NULL ||
	    (browser_type = XmlGetElementType("browser", document, FALSE)) == NULL ||
	    (email_program_type = XmlGetElementType("email-program", document, FALSE)) == NULL ||
	    (chat_program_type = XmlGetElementType("chat-program", document, FALSE)) == NULL)
	{
		KTError ("'%s' is an invalid preferences configuration file.", file_name);
		exit (EXIT_FAILURE);
	}
	if ((preferences_element =
	       XmlGetElementOfType(XmlDocumentContentList(document), preferences_type, 0)) == NULL)
	{
		KTError ("'%s' does has not a preferences element as the root element.", file_name);
		exit (EXIT_FAILURE);
	}
	if ((documents_dir_element =
	      XmlGetElementOfType(XmlElementContentList(preferences_element), documents_dir_type, 0)) == NULL ||
	    (home_page_element =
	      XmlGetElementOfType(XmlElementContentList(preferences_element), home_page_type, 0)) == NULL ||
	    (search_page_element =
	      XmlGetElementOfType(XmlElementContentList(preferences_element), search_page_type, 0)) == NULL ||
	    (browser_element =
	      XmlGetElementOfType(XmlElementContentList(preferences_element), browser_type, 0)) == NULL ||
	    (email_program_element
	      = XmlGetElementOfType(XmlElementContentList(preferences_element), email_program_type, 0)) == NULL ||
	    (chat_program_element =
	      XmlGetElementOfType(XmlElementContentList(preferences_element), chat_program_type, 0)) == NULL)
	{
		KTError ("'%s' is an invalid preferences configuration file.", file_name);
		exit (EXIT_FAILURE);
	}
	preferences->documents_dir = XmlGetElementString(documents_dir_element, "");
	preferences->home_page = XmlGetElementString(home_page_element, "");
	preferences->search_page = XmlGetElementString(search_page_element, "");
	preferences->browser = XmlGetElementString(browser_element, "");
	preferences->email_program = XmlGetElementString(email_program_element, "");
	preferences->chat_program = XmlGetElementString(chat_program_element, "");
	
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
