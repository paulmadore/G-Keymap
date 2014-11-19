/*---------------------------------------------------------------------------------
Name               : config.c
Author             : Marvin Raaijmakers
Description        : Reads and writes the configuration files
Date of last change: 16-Jun-2006
History            : 16-Jun-2006 Modified the following functions so that they do
                                 not exit the program when a config file is
                                 invalid:
                                 - get_current_keyboard()
                                 - read_action()
                                 - read_configuration()
                                 - read_key_settings()
                                 - read_key_settings_action()
                                 - read_keyboard_file()
                                 - read_preferences()
                     07-May-2006 Fixed in read_action(): plugin_name and
                                 function_name will be point to an empty string
                                 (instead of NULL) if the plugin and/or function
                                 does not exist
                     18-Mar-2006 get_current_keyboard() first checks if
                                 current_keyboard.xml exists
                     29-Jan-2006 Added free(file_name) to read_keyboard_config()
                                 and read_preferences() before returning
                     23-Jan-2006 Use variable _home_dir instead of calling
                                 getenv("HOME")
                     12-Jan-2006 read_key_settings_action() now continues after
                                 reading a key that does appear in the keyboard
                                 configuration file but not in the keyboard file

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
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include <config_element_types.h>
#include <mxml.h>
#include <keytouch.h>
#include <key_settings.h>

static char *create_keyboard_config_file_name (KTKeyboardName *keyboard_name);
static Boolean read_action (	XmlContent		*action_element,
				XmlElementType		*pluginname_type,
				XmlElementType		*pluginfunction_type,
				XmlAttributeName	*action_type_attrname,
				KTAction		*action  );
static Boolean read_key_settings (	XmlContent		*keylist_element,
					int			index,
					XmlElementType		*key_type,
					XmlElementType		*keyname_type,
					XmlElementType		*default_action_type,
					XmlElementType		*pluginname_type,
					XmlElementType		*pluginfunction_type,
					XmlAttributeName	*action_type_attrname,
					KTKeySettings		**key_settings );
static KTConfErr read_keyboard_file (	KTKeySettingsList	*key_list,
					KTKeyboardName		*keyboard_name  );
static Boolean read_key_settings_action (	KTKeySettingsList	*key_list,
						XmlContent		*keylist_element,
						int			index,
						XmlElementType		*key_type,
						XmlElementType		*keyname_type,
						XmlElementType		*action_type,
						XmlAttributeName	*action_type_attrname,
						XmlElementType		*pluginname_type,
						XmlElementType		*pluginfunction_type,
						XmlAttributeName	*isdefault_attrname );
static void read_keyboard_config (	KTKeySettingsList	*key_list,
					KTKeyboardName		*keyboard_name );
static void write_keyboard_name (	KTKeyboardName		*keyboard_name,
					XmlContent		*keyboard_element,
					XmlDocument		*document   );
static void write_key_settings (	KTKeySettings		*key,
					XmlContent		*key_list_element,
					XmlDocument		*document,
					XmlElementType		*key_element_type,
					XmlElementType		*key_name_element_type,
					XmlElementType		*action_element_type,
					XmlElementType		*pluginname_element_type,
					XmlElementType		*pluginfunction_element_type,
					XmlAttributeName	*isdefault_attrname,
					XmlAttributeName	*action_type_attrname         );
static void write_key_list (	KTKeySettingsList	*key_list,
				XmlContent		*keyboard_element,
				XmlDocument		*document          );
static void add_preference (	char		*element_type_string,
				char		*value,
				XmlContent	*preferences_element,
				XmlDocument	*document              );
static char *create_preferences_file_name (void);


Boolean
read_action (	XmlContent		*action_element,
		XmlElementType		*pluginname_type,
		XmlElementType		*pluginfunction_type,
		XmlAttributeName	*action_type_attrname,
		KTAction		*action )
/*
Input:
	action_element		- The element to read from.
	pluginname_type		- The XmlElementType for the plugin-name element
	pluginfunction_type	- The XmlElementType for the plugin-function element
	action_type_attrname	- The XmlElementName for the action-type attribute
Output:
	action			- The will contain the readen action
Returns:
	TRUE if 'action_element' has a correct syntax, otherwise FALSE.
Desciption:
	This function reads the KTAction from action_element and returns it via
	'action'.
*/
{
	XmlContent	*pluginname_element,
			*pluginfunction_element;
	char		*action_type_argument;
	Boolean		error;
	
	error = FALSE;
	/* Get the type of the action */
	action_type_argument = XmlGetAttributeValue(action_type_attrname, action_element);
	/* If it is a program */
	if (action_type_argument == NULL || strcmp(action_type_argument, ATTR_VAL_PROGRAM) == EQUAL)
	{
		action->type = KTActionTypeProgram;
		action->program.command = XmlGetElementString(action_element, "");
	}
	/* If it is a plugin */
	else if (strcmp(action_type_argument, ATTR_VAL_PLUGIN) == EQUAL)
	{
		action->type = KTActionTypePlugin;
		if ((pluginname_element =
		       XmlGetElementOfType(XmlElementContentList(action_element), pluginname_type, 0)) == NULL ||
		    (pluginfunction_element =
		       XmlGetElementOfType(XmlElementContentList(action_element), pluginfunction_type, 0)) == NULL)
		{
			KTSetErrorMsg (_("The keyboard (configuration) file contains an invalid action element."), "");
			error = TRUE;
		}
		else
		{
			action->plugin.plugin_name = XmlGetElementString(pluginname_element, "");
			action->plugin.function_name = XmlGetElementString(pluginfunction_element, "");
		}
	}
	else
	{
		KTSetErrorMsg (_("The keyboard (configuration) file contains "
		                 "an invalid action-type value."), "");
		error = TRUE;
	}
	
	return !error;
}


Boolean
read_key_settings (	XmlContent		*keylist_element,
			int			index,
			XmlElementType		*key_type,
			XmlElementType		*keyname_type,
			XmlElementType		*default_action_type,
			XmlElementType		*pluginname_type,
			XmlElementType		*pluginfunction_type,
			XmlAttributeName	*action_type_attrname,
			KTKeySettings		**key_settings )
/*
Input:
	keylist_element		- The element to read from.
	index			- Read the index'th key element
	key_type		- The XmlElementType for the key element
	keyname_type		- The XmlElementType for the key-name element
	default_action_type	- The XmlElementType for the default-action element
	pluginname_type		- The XmlElementType for the plugin-name element
	pluginfunction_type	- The XmlElementType for the plugin-function element
	action_type_attrname	- The XmlElementName for the action-type attribute
Output:
	key_settings		- The pointer pointed to by this pointer will point
				  the readen key settings or to NULL if no settings
				  were read.
Returns:
	TRUE if the key settings were successfully read or there does not exist an
	index'th element of 'key_type' in 'keylist_element'. If the index'th element
	of 'key_type' in 'keylist_element' does not have a valid syntax, this
	function will return FALSE.
Desciption:
	This function reads the key_name and default_action member from the index'th
	key element that is a child of 'keylist_element'.
*/
{
	XmlContent	*key_element,
			*keyname_element,
			*default_action_element;
	char		*action_type_string;
	Boolean		error;
	
	*key_settings = NULL;
	
	error = FALSE;
	key_element = XmlGetElementOfType(XmlElementContentList(keylist_element), key_type, index);
	if (key_element)
	{
		if ((keyname_element =
		     XmlGetElementOfType(XmlElementContentList(key_element), keyname_type, 0)) == NULL ||
		    (default_action_element =
		     XmlGetElementOfType(XmlElementContentList(key_element), default_action_type, 0)) == NULL)
		{
			KTSetErrorMsg (_("The keyboard file contains an incomplete key element."), "");
			error = TRUE;
		}
		else
		{
			*key_settings = (KTKeySettings *) keytouch_malloc( sizeof(KTKeySettings) );
			/* Read the default action */
			if ( read_action (default_action_element, pluginname_type, pluginfunction_type,
			                  action_type_attrname, &((*key_settings)->default_action)) )
			{
				/* Read the key name */
				(*key_settings)->key_name = XmlGetElementString (keyname_element, "");
				/* Initialize the remaining elements: */
				(*key_settings)->next = NULL;
				(*key_settings)->action = (*key_settings)->default_action;
			}
			else /* Reading the default action failed */
			{
				/* NOTE: KTSetErrorMsg() was already called by read_action() */
				error = TRUE;
				free (*key_settings);
			}
		}
	}
	
	return !error;
}


KTConfErr
read_keyboard_file (	KTKeySettingsList	*key_list,
			KTKeyboardName		*keyboard_name  )
/*
Input:
	keyboard_name	- The name of the keyboard to read the configuration from.
Output:
	key_list	- List containing information for every key.
Returns:
	TRUE if the keyboard was successfully read, otherwise FALSE.
Desciption:
	This function reads the keyboard file of the keyboard named keyboard_name,
	which means that it for every special function key an entry in key_list and
	fills in the following elements:
	- keyname
	- key_code
	- default_action
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
				*pluginname_type,
				*pluginfunction_type;
	XmlAttributeName	*action_type_attrname;
	KTKeySettings		*key_settings;
	KTConfErr		error; /* Contains the type of error that occurred */
	char			*file_name;
	int			count;
	
	error = KTConfErr_NoError;
	file_name = (char *) keytouch_malloc(	strlen(KEYBOARD_FILES_DIR "/.")+
						strlen(keyboard_name->manufacturer)+
						strlen(keyboard_name->model)+1 );
	strcpy (file_name, KEYBOARD_FILES_DIR "/");
	strcat (file_name, keyboard_name->model);
	strcat (file_name, ".");
	strcat (file_name, keyboard_name->manufacturer);
	
	document = XmlParseDocument(file_name);
	if (document == NULL)
	{
		if (access (file_name, R_OK) == -1)
		{
			if (errno == ENOENT)
			{
				KTSetErrorMsg (_("The keyboard file '%s' does not exist."), file_name);
				error = KTConfErr_NotExist;
			}
			else
			{
				KTSetErrorMsg (_("You have no permissions to read from keyboard file '%s'."), file_name);
				error = KTConfErr_NoAccess;
			}
		}
		else
		{
			KTSetErrorMsg (_("'%s' is an invalid keyboard file."), file_name);
			error = KTConfErr_KbFileInvalid;
		}
	}
	else
	{
		if ((keyboard_type = XmlGetElementType (ELEM_TYPE_KEYBOARD, document, FALSE)) == NULL ||
		    (keyboard_element = XmlGetElementOfType(XmlDocumentContentList(document), keyboard_type, 0)) == NULL)
		{
			KTSetErrorMsg (_("The keyboard file '%s' does not contain a keyboard element."), file_name);
			error = KTConfErr_KbFileInvalid;
		}
		else if ((keylist_type = XmlGetElementType(ELEM_TYPE_KEYLIST, document, FALSE)) == NULL ||
			 (keylist_element =
			  XmlGetElementOfType(XmlElementContentList(keyboard_element), keylist_type, 0)) == NULL)
		{
			KTSetErrorMsg (_("The keyboard file '%s' does not contain a key-list element."), file_name);
			error = KTConfErr_KbFileInvalid;
		}
		else if ((default_action_type = XmlGetElementType(ELEM_TYPE_DEFAULTACTION, document, FALSE)) == NULL ||
		         (key_type = XmlGetElementType(ELEM_TYPE_KEY, document, FALSE)) == NULL ||
			 (keyname_type = XmlGetElementType(ELEM_TYPE_KEYNAME, document, FALSE)) == NULL)
		{
			KTSetErrorMsg (_("The keyboard file '%s' is invalid."), file_name);
			error = KTConfErr_KbFileInvalid;
		}
		else
		{
			action_type_attrname = XmlGetAttributeName(ATTR_NAME_ACTIONTYPE, document, TRUE);
			/* Create the element types "plugin-name" and "plugin-function" if they
			 * do not appear in the document, because these element do not necessary
			 * appear in the document */
			pluginname_type = XmlGetElementType(ELEM_TYPE_PLUGINNAME, document, TRUE);
			pluginfunction_type = XmlGetElementType(ELEM_TYPE_PLUGINFUNCTION, document, TRUE);
			
			/* Now read the key settings list */
			if (read_key_settings (keylist_element, 0, key_type,
			                       keyname_type, default_action_type,
			                       pluginname_type, pluginfunction_type,
			                       action_type_attrname, &(key_list->head)))
			{
				key_settings = key_list->tail = key_list->head;
				for (count = 1; key_settings != NULL; count++)
				{
					if (read_key_settings (keylist_element, count, key_type,
					                       keyname_type, default_action_type,
					                       pluginname_type, pluginfunction_type,
					                       action_type_attrname, &key_settings))
					{
						key_list->tail->next = key_settings;
						if (key_settings != NULL)
						{
							key_list->tail = key_settings;
						}
					}
					else
					{
						/* NOTE: KTSetErrorMsg() was already called by read_key_settings() */
						error = KTConfErr_KbFileInvalid;
						break;
					}
				}
				/* If the keyboard file is invalid */
				if (error == KTConfErr_KbFileInvalid)
				{
					/* Clean up the readen key settings */
					clear_key_setting_list (key_list);
				}
			}
			else
			{
				/* NOTE: KTSetErrorMsg() was already called by read_key_settings() */
				error = KTConfErr_KbFileInvalid;
			}
		}
		XmlFreeDocument (document);
	}
	free (file_name);
	
	if (error != KTConfErr_NoError)
	{
		key_list->head = key_list->tail = NULL;
	}
	
	return error;
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
				XmlElementType		*pluginfunction_type,
				XmlAttributeName	*isdefault_attrname     )
/*
Input:
	key_list		- List of keys available on the current keyboard.
	keylist_element		- The element to read from.
	index			- Read the index'th key element
	key_type		- The XmlElementType for the key element
	keyname_type		- The XmlElementType for the key-name element
	action_type		- The XmlElementType for the action element
	action_type_attrname	- The XmlAttributeName for the action-type attribute
	pluginname_type		- The XmlElementType for the plugin-name element
	pluginfunction_type	- The XmlElementType for the plugin-function element
	isdefault_attrname	- The XmlAttributeName for the isdefault attribute
Output:
	key_list		- List containing the action member of the readen
				  key.
Returns:
	TRUE if the index'th key element exists, otherwise FALSE.
Desciption:
	This function reads action element of the index'th key element that is a
	child of keylist_element. The string value of the action element will be
	filled in in the matching key of key_list.
*/
{
	XmlContent	*key_element,
			*keyname_element,
			*action_element;
	char		*key_name,
			*isdefault_argument;
	KTKeySettings	*key_settings;
	
	key_element = XmlGetElementOfType(XmlElementContentList(keylist_element), key_type, index);
	if (key_element == NULL)
	{
		return (FALSE);
	}
	if ((keyname_element =
	       XmlGetElementOfType(XmlElementContentList(key_element), keyname_type, 0)) &&
	    (action_element =
	       XmlGetElementOfType(XmlElementContentList(key_element), action_type, 0)))
	{
		key_name = XmlGetElementString (keyname_element, "");
		key_settings = find_key_settings (key_name, key_list);
		if (key_settings) /* If the key appears in the keyboard file */
		{
			/* Get the type of the action */
			isdefault_argument = XmlGetAttributeValue(isdefault_attrname, action_element);
			/* If 'isdefault_argument' is "true" or reading the keysettings failed */
			if ((isdefault_argument != NULL && strcmp(isdefault_argument, "true") == EQUAL) ||
			    !read_action (action_element, pluginname_type, pluginfunction_type,
			                  action_type_attrname, &(key_settings->action)))
			{
				key_settings->action = key_settings->default_action;
			}
		}
		else
		{
			KTError (_("The keyboard configuration file contains a key, named "
				"\'%s\', which does not appear in the keyboard file."), key_name);
		}
		XmlFree (key_name);
	}
	
	return (TRUE);
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
	XmlAttributeName	*action_type_attrname,
				*isdefault_attrname;
	char			*file_name;
	int			count;
	
	file_name = create_keyboard_config_file_name(keyboard_name);
	document = XmlParseDocument(file_name);
	if (document != NULL &&
	    (keyboard_type = XmlGetElementType(ELEM_TYPE_KEYBOARD, document, FALSE)) != NULL &&
	    (keyboard_element = XmlGetElementOfType(XmlDocumentContentList(document), keyboard_type, 0)) != NULL &&
	    (keylist_type = XmlGetElementType(ELEM_TYPE_KEYLIST, document, FALSE)) != NULL &&
	    (keylist_element = XmlGetElementOfType(XmlElementContentList(keyboard_element), keylist_type, 0)) != NULL &&
	    (action_type = XmlGetElementType(ELEM_TYPE_ACTION, document, FALSE)) != NULL &&
	    (key_type = XmlGetElementType(ELEM_TYPE_KEY, document, FALSE)) != NULL &&
	    (keyname_type = XmlGetElementType(ELEM_TYPE_KEYNAME, document, FALSE)) != NULL)
	{
		action_type_attrname = XmlGetAttributeName(ATTR_NAME_ACTIONTYPE, document, TRUE);
		isdefault_attrname = XmlGetAttributeName(ATTR_NAME_ISDEFAULT, document, TRUE);
		/* Create the element types "plugin-name" and "plugin-function" if they
		* do not appear in the document, because these element do not necessary
		* appear in the document */
		pluginname_type = XmlGetElementType(ELEM_TYPE_PLUGINNAME, document, TRUE);
		pluginfunction_type = XmlGetElementType(ELEM_TYPE_PLUGINFUNCTION, document, TRUE);
		
		/* Read the action members of the keys */
		for (count = 0;
		     read_key_settings_action (key_list, keylist_element, count, key_type,
		                               keyname_type, action_type, action_type_attrname,
		                               pluginname_type, pluginfunction_type, isdefault_attrname);
		     count++)
			; /* NULL Statement */
		
		XmlFreeDocument (document);
	}
	free (file_name);
}


KTConfErr
read_configuration (	KTKeySettingsList	*key_list,
			KTKeyboardName		*keyboard_name  )
/*
Input:
	keyboard_name	- The name of the keyboard to read the configuration from.
Output:
	key_list	- List containing the settings for every key.
Returns:
	TRUE if the keyboard file was successfully read, otherwise false.
Desciption:
	This function reads the configuration from the configuration files of the
	keyboard named keyboard_name.
*/
{
	KTConfErr keyboard_file_error;
	
	/* Create the list and fill in for every KTKeySettings the following members:
	 * - key_name
	 * - keycode
	 * - default_action
	 */
	keyboard_file_error = read_keyboard_file (key_list, keyboard_name);
	if (keyboard_file_error == KTConfErr_NoError)
	{
		/* Fill in for every KTKeySettings the following action member */
		read_keyboard_config (key_list, keyboard_name);
	}
	return keyboard_file_error;
}


char
*create_preferences_file_name (void)
{
	char		*file_name;
	const char	*home_dir;
	
	/*home_dir = (char *)getenv("HOME");*/
	home_dir = _home_dir;
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
	This function reads the users preferences from the preferences file. If some
	or all preferences could not be read, then those preferences will contain
	their default value as defined in the keytouch.h header file.
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
	free (file_name);
	if (document == NULL ||
	    (preferences_type = XmlGetElementType(ELEM_TYPE_PREFERENCES, document, FALSE)) == NULL ||
	    (preferences_element = XmlGetElementOfType(XmlDocumentContentList(document), preferences_type, 0)) == NULL)
	{
		preferences->documents_dir = keytouch_strdup (PREF_DEFAULT_DOC_DIR);
		preferences->home_page = keytouch_strdup (PREF_DEFAULT_HOME_PAGE);
		preferences->search_page = keytouch_strdup (PREF_DEFAULT_SEARCH_PAGE);
		preferences->browser = keytouch_strdup (PREF_DEFAULT_BROWSER);
		preferences->email_program = keytouch_strdup (PREF_DEFAULT_EMAIL_PROG);
		preferences->email_program = keytouch_strdup (PREF_DEFAULT_EMAIL_PROG);
		preferences->chat_program = keytouch_strdup (PREF_DEFAULT_CHAT_PROG);
	}
	else
	{
		if ((documents_dir_type = XmlGetElementType(ELEM_TYPE_DOCUMENTSDIR, document, FALSE)) == NULL ||
		    (documents_dir_element =
		     XmlGetElementOfType(XmlElementContentList(preferences_element), documents_dir_type, 0)) == NULL)
		{
			preferences->documents_dir = keytouch_strdup (PREF_DEFAULT_DOC_DIR);
		}
		else
		{
			preferences->documents_dir = XmlGetElementString(documents_dir_element, "");
		}
		if ((home_page_type = XmlGetElementType(ELEM_TYPE_HOMEPAGE, document, FALSE)) == NULL ||
		    (home_page_element =
		     XmlGetElementOfType(XmlElementContentList(preferences_element), home_page_type, 0)) == NULL)
		{
			preferences->home_page = keytouch_strdup (PREF_DEFAULT_HOME_PAGE);
		}
		else
		{
			preferences->home_page = XmlGetElementString(home_page_element, "");
		}
		if ((search_page_type = XmlGetElementType(ELEM_TYPE_SEARCHPAGE, document, FALSE)) == NULL ||
		    (search_page_element =
		     XmlGetElementOfType(XmlElementContentList(preferences_element), search_page_type, 0)) == NULL)
		{
			preferences->search_page = keytouch_strdup (PREF_DEFAULT_SEARCH_PAGE);
		}
		else
		{
			preferences->search_page = XmlGetElementString(search_page_element, "");
		}
		if ((browser_type = XmlGetElementType(ELEM_TYPE_BROWSER, document, FALSE)) == NULL ||
		    (browser_element =
		     XmlGetElementOfType(XmlElementContentList(preferences_element), browser_type, 0)) == NULL)
		{
			preferences->browser = keytouch_strdup (PREF_DEFAULT_BROWSER);
		}
		else
		{
			preferences->browser = XmlGetElementString(browser_element, "");
		}
		if ((email_program_type = XmlGetElementType(ELEM_TYPE_EMAILPROG, document, FALSE)) == NULL ||
		    (email_program_element =
		     XmlGetElementOfType(XmlElementContentList(preferences_element), email_program_type, 0)) == NULL)
		{
			preferences->email_program = keytouch_strdup (PREF_DEFAULT_EMAIL_PROG);
		}
		else
		{
			preferences->email_program = XmlGetElementString(email_program_element, "");
		}
		if ((chat_program_type = XmlGetElementType(ELEM_TYPE_CHATPROG, document, FALSE)) == NULL ||
		    (chat_program_element =
		     XmlGetElementOfType(XmlElementContentList(preferences_element), chat_program_type, 0)) == NULL)
		{
			preferences->chat_program = keytouch_strdup (PREF_DEFAULT_CHAT_PROG);
		}
		else
		{
			preferences->chat_program = XmlGetElementString(chat_program_element, "");
		}
	}
	if (document)
	{
		XmlFreeDocument (document);
	}
}


void
add_preference (	char		*element_type_string,
			char		*value,
			XmlContent	*preferences_element,
			XmlDocument	*document              )
/*
Input:
	element_type_string	- The string of the element type to create
	value			- The string that the new element will contain
Output:
	preferences_element	- The new element will be add to this element
	document		- The document where preferences_element is in
Returns:
	-
Desciption:
	This function adds a element, who's type is element_type_string, to
	preferences_element. The element will contain the string value.
*/
{
	XmlElementType	*element_type;
	XmlContent	*element,
			*string;
	
	if (value == NULL)
	{
		value = "";
	}
	element_type = XmlGetElementType(element_type_string, document, TRUE);
	element = XmlCreateElement(element_type, FALSE);
	string = XmlCreateContent(XmlCType_String);
	XmlStringSetString (string, value);
	XmlContentAppend (string, XmlElementContentList(element));
	XmlContentAppend (element, XmlElementContentList(preferences_element));
}


void
write_preferences (KTPreferences *preferences)
/*
Input:
	preferences	- The preferences to write to the preferences file
Output:
	-
Returns:
	-
Desciption:
	This function reads the data in preferences to the preferences file.
*/
{
	XmlDocument	*document;
	char		*file_name;
	XmlElementType	*element_type;
	XmlContent	*preferences_element;
	
	document = XmlCreateDocument();
	
	element_type = XmlGetElementType(ELEM_TYPE_PREFERENCES, document, TRUE);
	preferences_element = XmlCreateElement(element_type, FALSE);
	
	add_preference (ELEM_TYPE_DOCUMENTSDIR, preferences->documents_dir, preferences_element, document);
	add_preference (ELEM_TYPE_HOMEPAGE, preferences->home_page, preferences_element, document);
	add_preference (ELEM_TYPE_SEARCHPAGE, preferences->search_page, preferences_element, document);
	add_preference (ELEM_TYPE_BROWSER, preferences->browser, preferences_element, document);
	add_preference (ELEM_TYPE_EMAILPROG, preferences->email_program, preferences_element, document);
	add_preference (ELEM_TYPE_CHATPROG, preferences->chat_program, preferences_element, document);
	
	XmlContentAppend (preferences_element, XmlDocumentContentList(document));
	
	file_name = create_preferences_file_name();
	/* Write the file */
	if (!XmlWriteDocument(document, file_name))
	{
		KTError (_("An error occurred while writing the "
		           "preferences file \"%s\"."), file_name);
	}
	free (file_name);
	XmlFreeDocument (document);
}


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
create_user_config_dir (void)
/*
Input:
	-
Output:
	-
Returns:
	-
Description:
	This function creates the directory USER_CFG_DIR in the home directory of
	the user.
*/
{
	const char	*home_dir;
	char		*user_config_dir;
	
	/*home_dir = (char *)getenv("HOME");*/
	home_dir = _home_dir;
	if (home_dir != NULL)
	{
		user_config_dir = keytouch_malloc(strlen(home_dir) + strlen("/" USER_CFG_DIR) + 1);
		strcpy (user_config_dir, home_dir);
		strcat (user_config_dir, "/" USER_CFG_DIR);
		mkdir (user_config_dir, S_IWUSR | S_IRUSR | S_IXUSR);
		free (user_config_dir);
	}
}


char
*create_keyboard_config_file_name (KTKeyboardName *keyboard_name)
{
	const char	*home_dir;
	char		*file_name;
	
	/*home_dir = (char *)getenv("HOME");*/
	home_dir = _home_dir;
	if (home_dir != NULL)
	{
		file_name = keytouch_malloc(	strlen(home_dir) +
						strlen("/" KEYBOARD_CONFIG_FILES_DIR "/.")+
						strlen(keyboard_name->manufacturer)+
						strlen(keyboard_name->model)+1  );
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
write_keyboard_name (	KTKeyboardName		*keyboard_name,
			XmlContent		*keyboard_element,
			XmlDocument		*document          )
/*
Input:
	keyboard_name		- The name of the keyboard
Output:
	keyboard_element	- The element where the keyboard name element will
				  be added to
	document		- The document that contains keyboard_element
Returns:
	-
Description:
	This functions creates a keyboard name element and adds the keyboard name
	it. The keyboard name element will be added to keyboard_element.
*/
{
	XmlElementType	*element_type;
	XmlContent	*keyboard_name_element,
			*model_element,
			*manufacturer_element;
	XmlContent	*string;
	
	element_type = XmlGetElementType(ELEM_TYPE_KEYBOARDNAME, document, TRUE);
	/* Create the keyboard name element */
	keyboard_name_element = XmlCreateElement(element_type, FALSE);
	element_type = XmlGetElementType(ELEM_TYPE_MODEL, document, TRUE);
	/* Create the model element */
	model_element = XmlCreateElement(element_type, FALSE);
	element_type = XmlGetElementType(ELEM_TYPE_MANUFACTURER, document, TRUE);
	/* Create the manufacturer element */
	manufacturer_element = XmlCreateElement(element_type, FALSE);
	
	/* Create the string containing the model */
	string = XmlCreateContent(XmlCType_String);
	XmlStringSetString (string, keyboard_name->model);
	/* Add the string to the model element */
	XmlContentAppend (string, XmlElementContentList(model_element));
	
	/* Create the string containing the manufacturer */
	string = XmlCreateContent(XmlCType_String);
	XmlStringSetString (string, keyboard_name->manufacturer);
	/* Add the string to the manufacturer element */
	XmlContentAppend (string, XmlElementContentList(manufacturer_element));
	
	/* Add the model element to the keyboard name element */
	XmlContentAppend (model_element, XmlElementContentList(keyboard_name_element));
	/* Add the manufacturer element to the keyboard name element */
	XmlContentAppend (manufacturer_element, XmlElementContentList(keyboard_name_element));
	/* Add the keyboard name element to the keyboard element */
	XmlContentAppend (keyboard_name_element, XmlElementContentList(keyboard_element));
}


void
write_key_settings (	KTKeySettings		*key,
			XmlContent		*key_list_element,
			XmlDocument		*document,
			XmlElementType		*key_element_type,
			XmlElementType		*key_name_element_type,
			XmlElementType		*action_element_type,
			XmlElementType		*pluginname_element_type,
			XmlElementType		*pluginfunction_element_type,
			XmlAttributeName	*isdefault_attrname,
			XmlAttributeName	*action_type_attrname         )
/*
Input:
	key			- The key settings to add to the key element
	*_element_type
	*_attrname
Output:
	key_list_element	- The element where the key element will be added to
	document		- The document that contains key_list_element
Returns:
	-
Description:
	This functions creates a key element and adds the settings of key to it. The
	key element will be added to key_list_element.
*/
{
	XmlContent	*key_element,
			*key_name_element,
			*action_element,
			*pluginname_element,
			*pluginfunction_element,
			*string;
	
	key_element = XmlCreateElement(key_element_type, FALSE);
	
	key_name_element = XmlCreateElement(key_name_element_type, FALSE);
	/* Create the string containing the name of the key */
	string = XmlCreateContent(XmlCType_String);
	XmlStringSetString (string, key->key_name);
	/* Add the string to the key name element */
	XmlContentAppend (string, XmlElementContentList(key_name_element));
	/* Add the key name element to the key element */
	XmlContentAppend (key_name_element, XmlElementContentList(key_element));
	
	action_element = XmlCreateElement(action_element_type, FALSE);
	/* If the keys action is its default action */
	if (!memcmp(&key->action, &key->default_action, sizeof(KTAction)))
	{
		XmlSetAttributeValue (isdefault_attrname, "true", action_element);
	}
	if (key->action.type == KTActionTypeProgram)
	{
		XmlSetAttributeValue (action_type_attrname, ATTR_VAL_PROGRAM, action_element);
		/* Create the string containing the command */
		string = XmlCreateContent(XmlCType_String);
		XmlStringSetString (string, key->action.program.command);
		/* Add the string to the action element */
		XmlContentAppend (string, XmlElementContentList(action_element));
	}
	else if (key->action.type == KTActionTypePlugin)
	{
		XmlSetAttributeValue (action_type_attrname, ATTR_VAL_PLUGIN, action_element);
		
		pluginname_element = XmlCreateElement(pluginname_element_type, FALSE);
		/* Create the string containing the plugin name */
		string = XmlCreateContent(XmlCType_String);
		if (key->action.plugin.plugin_name == NULL)
		{
			XmlStringSetString (string, "");
		}
		else
		{
			XmlStringSetString (string, key->action.plugin.plugin_name);
		}
		/* Add the string to the plugin name element */
		XmlContentAppend (string, XmlElementContentList(pluginname_element));
		/* Add the plugin name element to the action element */
		XmlContentAppend (pluginname_element, XmlElementContentList(action_element));
		
		pluginfunction_element = XmlCreateElement(pluginfunction_element_type, FALSE);
		/* Create the string containing the function name */
		string = XmlCreateContent(XmlCType_String);
		if (key->action.plugin.function_name == NULL)
		{
			XmlStringSetString (string, "");
		}
		else
		{
			XmlStringSetString (string, key->action.plugin.function_name);
		}
		/* Add the string to the plugin function element */
		XmlContentAppend (string, XmlElementContentList(pluginfunction_element));
		/* Add the plugin function element to the action element */
		XmlContentAppend (pluginfunction_element, XmlElementContentList(action_element));
	}
	/* Add the action element to the key element */
	XmlContentAppend (action_element, XmlElementContentList(key_element));
	
	/* Add the key element to the key list element */
	XmlContentAppend (key_element, XmlElementContentList(key_list_element));
}


void
write_key_list (	KTKeySettingsList	*key_list,
			XmlContent		*keyboard_element,
			XmlDocument		*document          )
/*
Input:
	key_list		- List containing the settings for every key that
				  will be added to the key_list element
Output:
	keyboard_element	- The element where the key list will be added to.
	document		- The document that contains keyboard_element
Returns:
	-
Description:
	This functions creates a key list element and adds for every key in key_list
	a key element to it. The key list element will be added to keyboard_element.
*/
{
	XmlContent		*key_list_element;
	KTKeySettings		*key;
	XmlElementType		*key_list_element_type,
				*key_element_type,
				*key_name_element_type,
				*action_element_type,
				*pluginname_element_type,
				*pluginfunction_element_type;
	XmlAttributeName	*isdefault_attrname,
				*action_type_attrname;
	
	key_list_element_type = XmlGetElementType(ELEM_TYPE_KEYLIST, document, TRUE);
	key_list_element = XmlCreateElement(key_list_element_type, FALSE);
	
	key_element_type = XmlGetElementType(ELEM_TYPE_KEY, document, TRUE);
	key_name_element_type = XmlGetElementType(ELEM_TYPE_KEYNAME, document, TRUE);
	action_element_type = XmlGetElementType(ELEM_TYPE_ACTION, document, TRUE);
	pluginname_element_type = XmlGetElementType(ELEM_TYPE_PLUGINNAME, document, TRUE);
	pluginfunction_element_type = XmlGetElementType(ELEM_TYPE_PLUGINFUNCTION, document, TRUE);
	isdefault_attrname = XmlGetAttributeName(ATTR_NAME_ISDEFAULT, document, TRUE);
	action_type_attrname = XmlGetAttributeName(ATTR_NAME_ACTIONTYPE, document, TRUE);
	
	/* Write the settings for every key : */
	for (key = key_list->head; key != NULL; key = key->next)
	{
		write_key_settings (	key, key_list_element, document,
					key_element_type,
					key_name_element_type,
					action_element_type,
					pluginname_element_type,
					pluginfunction_element_type,
					isdefault_attrname,
					action_type_attrname  );
	}
	/* Add the key list element to the keyboard element */
	XmlContentAppend (key_list_element, XmlElementContentList(keyboard_element));
}


void
write_configuration (	KTKeySettingsList	*key_list,
			KTKeyboardName		*keyboard_name  )
/*
Input:
	key_list	- List containing the settings for every key that will be
			  written to the configuration file
	keyboard_name	- The name of the current keyboard
Output:
	-
Returns:
	-
Description:
	This functions writes the keyboard configuration file for the keyboard named
	keyboard_name.
*/
{
	XmlDocument	*document;
	XmlElementType	*element_type;
	XmlContent	*keyboard_element;
	char		*file_name;
	
	document = XmlCreateDocument();
	
	element_type = XmlGetElementType(ELEM_TYPE_KEYBOARD, document, TRUE);
	/* Create the root element */
	keyboard_element = XmlCreateElement(element_type, FALSE);
	/* Add the keyboard name to the keyboard element */
	write_keyboard_name (keyboard_name, keyboard_element, document);
	/* Add the key settings list to the keyboard element */
	write_key_list (key_list, keyboard_element, document);
	/* Add the element to the document */
	XmlContentAppend (keyboard_element, XmlDocumentContentList(document));
	
	file_name = create_keyboard_config_file_name(keyboard_name);
	/* Write the file */
	if (!XmlWriteDocument(document, file_name))
	{
		KTError (_("An error occurred while writing the "
		           "keyboard configuration file \"%s\"."), file_name);
	}
	free (file_name);
	XmlFreeDocument (document);
}
