/*-------------------------------------------------------------------------------
Name               : config.c
Author             : Marvin Raaijmakers
Description        : Reading and writing configuration
Date of last change: 10-December-2004
History            :
                    VERSION 1.0:
                     3-July-2004     - Created read_plugin_list() and
                                       read_plugin()
                                     - Modified write_xbindkeys_file() (special
                                       action compatible)
                                     - Created write_plugin_file()
                     6-July-2004     Created:
                                     - read_preferences()
                                     - read_pref_line()
                                     Modified:
                                     - read_config()
                                     - write_config_file()
                                     - save_setting()
                     9-August-2004   Modified:
                                     - write_xbindkeys_file(): After saving the
                                       file restart xbindkeys
                                     - write_xmodmap_file(): After saving the file
                                       start xmodmap again
                     20-August-2004  Fixed bug in write_config_file()
                     31-August-2004  Fixed:
                                     - read_plugin_list()
                                     - read_config()
                                     - write_config_file()
                                     - write_xmodmap_file()
                                     - write_xbindkeys_file()
                                     - write_plugin_file()
                    VERSION 1.1:
                     20-November-2004 Modified/(partly)rebuilded:
                                      - read_keyboard()
                                      - read_keyboard_list()
                                      - read_key_settings()
                                      - read_config()
                                      - read_current_keyboard()
                     21-November-2004  Modified/(partly)rebuilded:
                                      - read_keyboard()
                     24-November-2004 Modified/(partly)rebuild:
                                      - write_config_file()
                     6-December-2004  Modified:
                                      - write_xmodmap_file()
                                      - save_settings()
                                      - read_keyboard_list()
                                      - read_keyboard()
                                      - write_config_file()
                     10-December-2004 - Builded read_manufacture()
                                      - Rebuilded read_keyboard()
                     1-January-2005   Fixed a litlle bug in read_line() (preform
                                      EOF check before doing anything else)

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
#include <keyboard_config.h>
#include <keyboard_list.h>
#include <xmodmap_list.h>
#include <plugin_list.h>

/************ File scope functions ************/
static int read_non_empty_line (FILE *file, char *line, int  max_line_size);
static char read_keyboard (KEYBOARD_MANUFACTURE *manufacture, FILE *config_file);
static char read_manufacture (KEYBOARD_LIST *keyboard_list, FILE *config_file);
static void read_keyboard_list (KEYBOARD_LIST *keyboard_list, FILE *config_file);
static char read_key_settings (KEY_SETTINGS_LIST *key_settings_list, FILE *config_file);
static void read_current_keyboard (KEYBOARD_CONFIG *current_keyboard_config, FILE *config_file);
static void write_config_file (KEYBOARD_CONFIG *current_keyboard_config, KEYBOARD_LIST *keyboard_list, PREFERENCES *preferences);
static void write_xmodmap_file (XMODMAP_LIST *xmodmap_list);
static void write_xbindkeys_file (KEY_SETTINGS_LIST *key_settings);
static PLUGIN *read_plugin (FILE *plugin_file);
static void read_preferences (FILE *config_file, PREFERENCES *preferences);
static void read_pref_line (FILE *file, char *line, int  max_line_size);
static void write_plugin_file (PLUGIN_LIST *plugin_list);

int
readline (	FILE *file,
		char *line,
		int  max_line_size  )
/*
input:
	file		- The file to read from
	line		- The string where the line will be written to
	max_line_size	- Maximum number of characters to read
output:
	-
returns:
	The number of written characters to line or EOF on end of file.
description:
	This function reads characters from file until a either a terminating newline,
	a EOF or max_line_size characters where read. It skips space characters at the
	beginning of a line. All readen characters will be placed in line. It stops
	putting characters in line if more then max_line_size characters had been read,
	but will continue reading from file. A terminating '\0' character will be add.
	The return value is the number of written characters to line or EOF on end of
	file.
*/
{
	char c;
	int count; /* Invariant: count = number of written characters to line */

	if (feof(file))
	{
		return (EOF);
	}
	
	do /* Read characters until a non-space character reached */
	{
		c = (char) getc(file);
	} while ( (c == ' ') || (c == '\t') );

	count = 0; /* Make the invariant true */
	/* Read until a newline character or an EOF */
	while ((c != '\n') && !feof(file))
	{
		if (count < max_line_size)
		{
			line[count] = c;
			count++; /* Correct the invariant */
		}
		c = (char) getc(file);
	}
	line[count] = '\0';

	return (count);
}

int
read_non_empty_line (	FILE *file,
			char *line,
			int  max_line_size  )
/*
input:
	file		- The file to read from
	line		- The string where the line will be written to
	max_line_size	- Maximum number of characters to read
output:
	-
returns:
	The number of written characters to line or EOF on end of file.
description:
	This function reads characters from file until a either a terminating newline,
	a EOF or max_line_size characters where read. It skips space characters at the
	beginning of a line and empty lines. All readen characters will be placed in
	line. It stops putting characters in line if more then max_line_size characters
	had been read, but will continue reading from file. A terminating '\0' character
	will be add.The return value is the number of written characters to line or EOF
	on end of file.
*/
{
	int n;

	while ( ( (n = readline (file, line, max_line_size)) == 0 ) && (n != EOF) )
		; /* NULL Statement */

	return (n);
}


/*char
read_keyboard (	KEYBOARD_LIST	*keyboard_list,
		FILE		*config_file    )
/*
input:
	config_file	- The file to read the keyboard from
output:
	keyboard_list	- The list where the new keyboard was added to
returns:
	TRUE if the keyboard was succesfully read. Otherwise (EOF for example) FALSE.
description:
	This function reads information about a keyboard from config_file, and adds it
	to keyboard_list.
*
{
	char line[MAX_NAME_SIZE+1] = "";
	KEYBOARD_MODEL *new_keyboard;
	KEYBOARD_MANUFACTURE *new_manufacture;

	/* Create a new keyboard *
	new_keyboard = kbl_model_new();

	/* Read the next non-empty line *
	read_non_empty_line (config_file, line, MAX_NAME_SIZE);

	if ( feof(config_file) )
	{
		return (FALSE);
	}
	if (*line == '}' || kbl_is_empty(keyboard_list))
	{
		if (!kbl_is_empty(keyboard_list))
		{
			read_non_empty_line (config_file, line, MAX_NAME_SIZE);
			if ( feof(config_file) )
			{
				return (FALSE);
			}
		}
		
		/* Create a new manufacture *
		new_manufacture = kbl_manufacture_new();
		/* Set the name of the new manufacture *
		kbl_manufacture_set_name (new_manufacture, line);
		/* Add the new manufacture to keyboard_list *
		kbl_manufacture_add (keyboard_list, new_manufacture);
		
		fscanf (config_file, "{\n"); /* We just have to do this (config file syntax) *
		/* Read the first keyboard of the new manufacture *
		return ( read_keyboard(keyboard_list, config_file) );
	}
	else
	{
		kbl_model_set_name (new_keyboard, line); /* Set the name of the new keyboard *
		 /* Add new_keyboard to the last manufacture of keyboard_list *
		kbl_model_add (kbl_last_manufacture(keyboard_list), new_keyboard);
	}

	return (TRUE);
}*/

char
read_keyboard (	KEYBOARD_MANUFACTURE	*manufacture,
		FILE			*config_file  )
/*
input:
	config_file	- The file to read the keyboard from
output:
	manufacture	- A keyboard manufacture
returns:
	TRUE if the keyboard was succesfully read. Otherwise (EOF for example) FALSE.
description:
	This function reads a keyboard from config_file, and adds it to manufacture.
*/
{
	char line[MAX_NAME_SIZE+1] = "";
	KEYBOARD_MODEL *new_keyboard;

	/* Create a new keyboard */
	new_keyboard = kbl_model_new();

	/* Read the next non-empty line */
	read_non_empty_line (config_file, line, MAX_NAME_SIZE);

	if ( feof(config_file) || *line == '}' )
	{
		return (FALSE);
	}
	/* Set the name of the new keyboard */
	kbl_model_set_name (new_keyboard, line);
	/* Add new_keyboard to the last manufacture of keyboard_list */
	kbl_model_add (manufacture, new_keyboard);

	return (TRUE);
}


char
read_manufacture (	KEYBOARD_LIST	*keyboard_list,
			FILE		*config_file    )
/*
input:
	config_file	- The file to read the keyboard from
output:
	keyboard_list	- The list where the new manufacture was added to
returns:
	TRUE if the manufacture was succesfully read. Otherwise (EOF for example) FALSE.
description:
	This function reads a manufacture from config_file, and adds it to keyboard_list.
*/
{
	char line[MAX_NAME_SIZE+1] = "";
	KEYBOARD_MANUFACTURE *new_manufacture;
	
	/* Read the name of the manufacture */
	read_non_empty_line (config_file, line, MAX_NAME_SIZE);
	if ( feof(config_file) )
	{
		return (FALSE);
	}
	fscanf (config_file, "{\n"); /* We just have to do this (config file syntax) */
	
	/* Create a new manufacture */
	new_manufacture = kbl_manufacture_new();
	/* Set the name of the new manufacture */
	kbl_manufacture_set_name (new_manufacture, line);
	
	/* Read all the keyboards of the manufacture */
	while ( read_keyboard(new_manufacture, config_file) )
		; /* NULL Statement */
	
	/* Add the new manufacture to keyboard_list */
	kbl_manufacture_add (keyboard_list, new_manufacture);
	
	return (TRUE);
}

void
read_keyboard_list (	KEYBOARD_LIST	*keyboard_list,
			FILE		*config_file    )
/*
input:
	config_file	- The file to read the keyboard list from
output:
	keyboard_list	- A list containing all keyboards read from config_file
returns:
	-
description:
	This function reads information about keyboards from config_file and adds them to
	keyboard_list.
*/
{
	/* Read all keyboards */
	while ( read_manufacture(keyboard_list, config_file) )
		; /* NULL Statement */
}


char
read_key_settings (	KEY_SETTINGS_LIST	*key_settings_list,
			FILE			*config_file        )
/*
input:
	config_file		- The file to read the key settings of the keyboard from
output:
	key_settings_list	- A list containing settings of keys
returns:
	TRUE if the key settings where read succesfully. Otherwise (EOF for example) FALSE.
description:
	This function reads key settings from config_file, and adds them to key_settings_list.
*/
{
	char line[MAX_KEY_SYM_SIZE+1];
	KEY_SETTINGS *new_key_settings;

	fgets (line, MAX_KEY_SYM_SIZE+1, config_file);
	/* If the next line does not contain key settings */
	if ( !((line[0] == '{') || (line[0] == '\n')) )
	{
		return (FALSE);
	}
	/*
	 * Read the next line. Which is probably a key symbol, but if its last character
	 * is '}', it is not a key symbol. So thats why we put it into line.
	 */
	readline (config_file, line, MAX_KEY_SYM_SIZE);
	/* Now check if it is not a key symbol */
	if ( *line == '}')
	{
		return (FALSE);
	}

	/* Create a new KEY_SETTINGS */
	new_key_settings = ks_new();

	/*** Read/assign settings ***/
	ks_set_key_sym (new_key_settings, line);
	readline (config_file, line, MAX_KEY_NAME_SIZE);
	ks_set_key_name (new_key_settings, line);
	readline (config_file, line, MAX_PROG_SIZE);
	ks_set_program_default (new_key_settings, line);
	readline (config_file, line, MAX_PROG_SIZE);
	ks_set_program_user (new_key_settings, line);
	/****************************/
	
	/* Add new_key_settings to the key_settings_list */
	ksl_add (key_settings_list, new_key_settings);

	return (TRUE);
}


void
read_current_keyboard (	KEYBOARD_CONFIG	*current_keyboard_config,
			FILE 		*config_file                )
/*
input:
	config_file		- The file to read the configuration of the current keyboard
				  from
output:
	current_keyboard_config	- The current keyboard configuration
returns:
	-
description:
	This function reads data from config_file. The name of the current keyboard will be
	placed in current_keyboard_config. A linked list containing settings for every key of
	the current keyboard will be produced and also be placed in current_keyboard_config.
*/
{
	char	keyboard_model [MAX_NAME_SIZE+1],
		keyboard_manufacture [MAX_NAME_SIZE+1];

	readline (config_file, keyboard_model, MAX_NAME_SIZE);
	readline (config_file, keyboard_manufacture, MAX_NAME_SIZE);
	kbcfg_set_name (current_keyboard_config, keyboard_model, keyboard_manufacture);
	
	/* Read all key settings */
	while ( read_key_settings(kbcfg_key_settings_list(current_keyboard_config), config_file) )
		; /* NULL Statement */
}


PLUGIN
*read_plugin (FILE *plugin_file)
/*
input:
	plugin_file		- The file to read from
output:
	-
returns:
	The address of readen plugin, or NULL on eof.
description:
	This function reads information about a plugin from plugin_file and returns the
	address of the PLUGIN containing this information. On EOF it will return NULL.
*/
{
	PLUGIN *new_plugin;
	char line[MAX_LINE_SIZE+1];

	/* Read the file name of the plugin */
	read_non_empty_line (plugin_file, line, MAX_FILE_SIZE);
	if ( feof(plugin_file) )
	{
		if (line[0]) /* If the line contained text */
		{
			handle_fatal_error (FERR_INVALID_PLUGIN_FILE);
		}
		return (NULL);
	}
	/* Create the new plugin */
	new_plugin = plugl_plugin_new();
	/* Set the file name of the plugin */
	plugl_plugin_set_file (new_plugin, line);
	
	/* Read the name of the plugin */
	read_non_empty_line (plugin_file, line, MAX_PLUGIN_NAME_SIZE);
	if ( line[0] == '\0') /* If the plugin has no name */
	{
		handle_fatal_error (FERR_INVALID_PLUGIN_FILE);
	}
	/* Set the name of the plugin */
	plugl_plugin_set_name (new_plugin, line);

	return (new_plugin);
}

void
read_plugin_list (PLUGIN_LIST *plugin_list)
/*
input:
	-
output:
	plugin_list	- List containing all readen plugins
returns:
	-
description:
	This function reads plugins from the plugin file ('~/.keytouch/plugins')
	and adds these to plugin_list.
*/
{
	FILE *plugin_file;
	char plugin_file_name[MAX_FILE_SIZE+1];
	PLUGIN *plugin;

	strncpy (plugin_file_name, getenv("HOME"), MAX_FILE_SIZE);
	strncat (plugin_file_name, "/.keytouch/plugins", MAX_FILE_SIZE-strlen(plugin_file_name));

	plugin_file = fopen(plugin_file_name, "r");
	if (plugin_file == NULL)
	{
		handle_fatal_error (FERR_CONFIG_R);
	}
	plugl_init (plugin_list);
	/* Read all plugins */
	while (plugin = read_plugin(plugin_file))
	{
		plugl_plugin_add (plugin_list, plugin);
	}
}


void
read_pref_line (	FILE *file,
			char *line,
			int  max_line_size  )
/*
input:
	file		- The file to read from
	line		- The string where the line will be written to
	max_line_size	- Maximum number of characters to read
output:
	-
returns:
	The number of written characters to line or EOF on end of file.
description:
	This function reads a preference from file until a either a terminating newline,
	a EOF or max_line_size characters where read. It skips space characters at the
	beginning of a line. All readen characters will be placed in line. It stops
	putting characters in line if more then max_line_size characters had been read,
	but will continue reading from file. A terminating '\0' character will be add.
	The return value is the number of written characters to line or EOF on end of
	file.
*/
{
	if (readline (file, line, max_line_size) == EOF)
	{
		handle_fatal_error (FERR_INVALID_CONFIG_FILE);
	}
}


void
read_preferences (FILE *config_file, PREFERENCES *preferences)
/*
input:
	config_file	- The file to read from
output:
	preferences	- Structure containing the user preferences
returns:
	-
description:
	This function reads the users perferences from config_file. These preferences
	will be assigned to the output variable preferences.
*/
{
	char c;

	/* If the following text is not "preferences\n{\n" */
	if ( !fscanf(config_file, "preferences\n{%c", &c) || (c != '\n') )
	{
		handle_fatal_error (FERR_INVALID_CONFIG_FILE);
	}

	/* Read all preferences */
	read_pref_line (config_file, preferences->documents_dir, MAX_FILE_SIZE);
	read_pref_line (config_file, preferences->home_page, MAX_FILE_SIZE);
	read_pref_line (config_file, preferences->search_page, MAX_FILE_SIZE);
	read_pref_line (config_file, preferences->browser, MAX_PROG_SIZE);
	read_pref_line (config_file, preferences->email_program, MAX_PROG_SIZE);
	read_pref_line (config_file, preferences->messenger, MAX_PROG_SIZE);

	/* If the following text is not "}\n" */
	if ( !fscanf(config_file, "}%c", &c) || (c != '\n') )
	{
		handle_fatal_error (FERR_INVALID_CONFIG_FILE);
	}
}


void
read_config (	KEYBOARD_CONFIG	*current_keyboard_config,
		KEYBOARD_LIST	*keyboard_list,
		PREFERENCES	*preferences       )
/*
input:
	-
output:
	current_keyboard_config	- The configuration of the current keyboard
	keyboard_list		- List of available keyboards
	preferences		- Structure containing the users preferences
returns:
	-
description:
	This function assignes values to current_keyboard, key_settings and keyboard_list
	which where read from the configuration file ('~/.keytouch/config')
*/
{
	FILE *config_file;
	char config_file_name[MAX_FILE_SIZE+1];

	strncpy (config_file_name, getenv("HOME"), MAX_FILE_SIZE);
	strncat (config_file_name, "/.keytouch/config", MAX_FILE_SIZE-strlen(config_file_name));
	config_file = fopen(config_file_name, "r");
	if (config_file == NULL)
	{
		handle_fatal_error (FERR_CONFIG_R);
	}

	kbcfg_init (current_keyboard_config);
	read_current_keyboard(current_keyboard_config, config_file);
	read_preferences (config_file, preferences);
	kbl_init (keyboard_list);
	read_keyboard_list(keyboard_list, config_file);
	fclose (config_file);
}


void
write_config_file (	KEYBOARD_CONFIG	*current_keyboard_config,
			KEYBOARD_LIST	*keyboard_list,
			PREFERENCES	*preferences       )
/*
input:
	current_keyboard_config	- Needed for writing the configuration file
	keyboard_list		- ,,
	preferences		- ,,
output:
	-
returns:
	-
description:
	This function writes the config file ('~/.keytouch/config').
*/
{
	FILE			*config_file;
	KEY_SETTINGS		*key_settings;
	KEYBOARD_MANUFACTURE	*manufacture;
	KEYBOARD_MODEL		*model;
	char			config_file_name[MAX_FILE_SIZE+1];

	strncpy (config_file_name, getenv("HOME"), MAX_FILE_SIZE);
	strncat (config_file_name, "/.keytouch/config", MAX_FILE_SIZE-strlen(config_file_name));
	config_file = fopen(config_file_name, "w");
	if (config_file == NULL)
	{
		handle_fatal_error (FERR_CONFIG_W);
	}

	/* Write the model and manufacture of the current keyboard */
	fprintf (config_file, "%s\n%s\n{", kbcfg_model(current_keyboard_config), kbcfg_manufacture(current_keyboard_config));
	
	key_settings = ksl_first_key( kbcfg_key_settings_list(current_keyboard_config) );
	if (key_settings == NULL) /* is there are no key settings */
	{
		fprintf (config_file, "\n");
	}
	/* Write the key settings of the current keyboard */
	while (key_settings)
	{
		fprintf (config_file, "\n\t%s\n\t%s\n\t%s\n\t%s\n",
			 ks_get_key_sym(key_settings), ks_get_key_name(key_settings),
			 ks_get_program_default(key_settings), ks_get_program_user(key_settings));
		key_settings = key_settings->next;
	}
	fprintf (config_file, "}\n");

	/* Print the users preferences */
	fprintf (config_file,	"preferences\n"
				"{\n"
				"\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n\t%s\n"
				"}\n",
		 preferences->documents_dir,
		 preferences->home_page,
		 preferences->search_page,
		 preferences->browser,
		 preferences->email_program,
		 preferences->messenger);

		 
	manufacture = kbl_first_manufacture(keyboard_list);
	/* Write every manufacture and its models */
	while (manufacture)
	{
		/* Write the keyboard manufacture */
		fprintf (config_file, "%s\n{\n", kbl_manufacture_get_name(manufacture));
		model = kbl_first_model(manufacture);
		/* Write every model of manufacture */
		while (model)
		{
			/* Write the keyboard model */
			fprintf (config_file, "\t%s\n", kbl_model_get_name(model));
			model = model->next;
		}
		fprintf (config_file, "}\n"); /* (end of this manufacture) */
		manufacture = manufacture->next;
	}
	
	fclose (config_file);
}


void
write_xmodmap_file (XMODMAP_LIST *xmodmap_list)
/*
input:
	xmodmap_list	- Needed for writing the xmodmap file
output:
	-
returns:
	-
description:
	This function writes the xmodmap file ('~/.Xmodmap').
*/
{
	FILE *xmodmap_file;
	char xmodmap_file_name[MAX_FILE_SIZE+1];
	XMODMAP *xmodmap;

	strncpy (xmodmap_file_name, getenv("HOME"), MAX_FILE_SIZE);
	strncat (xmodmap_file_name, "/.Xmodmap", MAX_FILE_SIZE - strlen(xmodmap_file_name));
	xmodmap_file = fopen(xmodmap_file_name, "w");
	if (xmodmap_file == NULL)
	{
		handle_fatal_error (FERR_XMODMAP);
	}
	fprintf (xmodmap_file,
		 "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
		 "!! This file is generated by keyTouch !!\n"
		 "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");
	xmodmap = xmml_first_xmodmap(xmodmap_list);
	/* Write every key */
	while (xmodmap)
	{
		fprintf (xmodmap_file, "keycode %d = %s\n", xmm_get_hardware_code(xmodmap), xmm_get_key_sym(xmodmap));
		xmodmap = xmodmap->next;
	}
	/* Of course xmodmap must be executed again */
	system ("xmodmap ~/.Xmodmap");

	fclose (xmodmap_file);
}


void
write_xbindkeys_file (KEY_SETTINGS_LIST *key_settings_list)
/*
input:
	key_settings_list	- Needed for writing the xbindkeys file
output:
	-
returns:
	-
description:
	This function writes the xbindkeys file ('~/.xbindkeysrc').
*/
{
	FILE *xbindkeys_file;
	char xbindkeys_file_name[MAX_FILE_SIZE+1], *program;
	KEY_SETTINGS *key_settings;

	strncpy (xbindkeys_file_name, getenv("HOME"), MAX_FILE_SIZE);
	strncat (xbindkeys_file_name, "/.xbindkeysrc", MAX_FILE_SIZE-strlen(xbindkeys_file_name));
	xbindkeys_file = fopen(xbindkeys_file_name, "w");
	if (xbindkeys_file == NULL)
	{
		handle_fatal_error (FERR_XBINDKEYS);
	}
	fprintf (xbindkeys_file,
		 "############################################\n"
		 "# ######################################## #\n"
		 "# #                                      # #\n"
		 "# #  This file is generated by keyTouch  # #\n"
		 "# #                                      # #\n"
		 "# ######################################## #\n"
		 "########## xbindkeys configuration #########\n\n");
	
	key_settings = ksl_first_key(key_settings_list);
	/* Write the key settings_list of the current keyboard */
	while (key_settings)
	{
		if ( strcmp( ks_get_program_user(key_settings), "default" ) == EQUAL ) /* If it is the default program */
		{
			program = ks_get_program_default(key_settings);
		}
		else /* If it is just a program */
		{
			program = ks_get_program_user(key_settings);
		}

		if (program[0] == ':') /* If it is a special action */
		{
			fprintf (xbindkeys_file, "\"keytouch-plugin '%s'\"\n", &(program[1]));
		}
		else
		{
			fprintf (xbindkeys_file, "\"%s\"\n", program);
		}
		fprintf (xbindkeys_file, "  %s\n", ks_get_key_sym(key_settings));

		key_settings = key_settings->next;
	}
	fclose (xbindkeys_file);
	
	/* Kill xbindkeys */
	system ("killall xbindkeys");
	/* Start xbindkeys again */
	system ("xbindkeys");
}


void
write_plugin_file (PLUGIN_LIST *plugin_list)
/*
input:
	plugin_list	- Needed for writing the plugin file
output:
	-
returns:
	-
description:
	This function writes the plugin file ('~/.keytouch/plugins').
*/
{
	FILE *plugin_file;
	PLUGIN *plugin;
	char plugin_file_name[MAX_FILE_SIZE+1];

	strncpy (plugin_file_name, getenv("HOME"), MAX_FILE_SIZE);
	strncat (plugin_file_name, "/.keytouch/plugins", MAX_FILE_SIZE-strlen(plugin_file_name));

	plugin_file = fopen(plugin_file_name, "w");
	if (plugin_file == NULL)
	{
		handle_fatal_error (FERR_PLUGIN_FILE_W);
	}

	plugin = plugl_first_plugin(plugin_list);
	/* Write all plugins */
	while (plugin) 
	{
		fprintf (plugin_file, "%s\n%s\n\n", plugl_plugin_get_file(plugin), plugl_plugin_get_name(plugin));
		plugin = plugin->next;
	}

	fclose (plugin_file);
}


void
save_settings (	KEYBOARD_CONFIG	*current_keyboard_config,
		KEYBOARD_LIST	*keyboard_list,
		XMODMAP_LIST	*xmodmap_list,
		PLUGIN_LIST	*plugin_list,
		PREFERENCES	*preferences      )
/*
input:
	current_keyboard	- Needed for writing the configuration file and xbindkeys file
	keyboard_list		- Needed for writing the configuration file
	xmodmap_list		- Needed for writing the xmodmap file
	plugin_file		- Needed for writing the plugin file
output:
	-
returns:
	-
description:
	This function writes the keyTouch configuration file, the xmodmap file (if xmodmap
	is not NULL), the xbindkeys file and the plugin file.
*/
{
	write_config_file (current_keyboard_config, keyboard_list, preferences);
	write_plugin_file (plugin_list);
	if (!xmml_is_empty(xmodmap_list)) /* If xmodmap has been modified */
	{
		write_xmodmap_file (xmodmap_list);
	}
	write_xbindkeys_file (kbcfg_key_settings_list(current_keyboard_config));
}
