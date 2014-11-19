/*-------------------------------------------------------------------------------
Name               : keys.c
Author             : Marvin Raaijmakers
Description        : Changing key settings
Date of last change: 5-December-2004
History            :
                    VERSION 1.0:
                     2-July-2004       Modified change_key_settings(): If a
                                       keyboard file could not be opened the
                                       program will not exit.
                     3-September-2004  Changed the return value of find_key()
                     2-October-2004    Fixed bug in change_key_settings():
                                       Skipping the first lines_to_skip lines,
                                       with a max size of MAX_LINE_SIZE (now
                                       MAX_NAME_SIZE)
                    VERSION 1.1:
                     5-December-2004   Modified:
                                       - read_key_settings_xmodmap()
                                       - change_key_settings()
    
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
#include <string.h>

#include <keytouch.h>
#include <keyboard_config.h>
#include <xmodmap_list.h>

/************ File scope functions ************/
static char read_key_settings_xmodmap  (KEY_SETTINGS_LIST *key_settings_list, XMODMAP_LIST *xmodmap_list, FILE *keyboard_file);

char
read_key_settings_xmodmap  (	KEY_SETTINGS_LIST	*key_settings_list,
				XMODMAP_LIST		*xmodmap_list,
				FILE			*keyboard_file  )
/*
input:
	keyboard_file		- The file to read from
output:
	key_settings_list	- List containing settings of every key of the current
				  keyboard
	xmodmap_list		- List containing information needed for writing the
				  xmodmap file
returns:
	FALSE on end of file, R_ERROR if an error occured, else TRUE.
description:
	This function reads an entry for key_settings_list and an entry for xmodmap_list
	from keyboard_file. It returns FALSE on end of file, otherwise it returns TRUE.
*/
{
	char key_sym[MAX_KEY_SYM_SIZE+1], key_name[MAX_KEY_NAME_SIZE+1], program_default[MAX_PROG_SIZE+1];
	int hardware_code, scanf_return_val;
	KEY_SETTINGS	*key_settings;
	XMODMAP		*xmodmap;

	scanf_return_val = fscanf(keyboard_file, "\n%d\n", &hardware_code);
	if (scanf_return_val == EOF)
	{
		return (FALSE);
	}
	
	if ( scanf_return_val != 1 ||
	     readline (keyboard_file, key_sym, MAX_KEY_SYM_SIZE) == EOF   ||
	     readline (keyboard_file, key_name, MAX_KEY_NAME_SIZE) == EOF ||
	     readline (keyboard_file, program_default, MAX_PROG_SIZE) == EOF  )
	{
		handle_error (ERR_INVALID_KEYBOARD_FILE, "");
		return (R_ERROR);
	}

	/* Create a new entry for key_settings_list */
	key_settings = ks_new();
	/* Create a new entry for xmodmap_list */
	xmodmap = xmm_new();
	
	/* Assign the values */
	xmm_set_hardware_code (xmodmap, hardware_code);
	xmm_set_key_sym (xmodmap, key_sym);
	ks_set_key_sym (key_settings, key_sym);
	ks_set_key_name (key_settings, key_name);
	ks_set_program_default (key_settings, program_default);
	ks_set_program_user (key_settings, "default");
	
	/* Add the entries to the lists */
	ksl_add (key_settings_list, key_settings);
	xmml_add (xmodmap_list, xmodmap);

	return (TRUE);
}

char
change_key_settings (	KEY_SETTINGS_LIST	*key_settings_list,
			XMODMAP_LIST		*xmodmap_list,
			KEYBOARD_NAME		*keyboard_name       )
/*
input:
	keyboard_name		- The name of the keyboard to change the key to
output:
	key_settings_list	- List containing all key settings of the keyboard, named
				  keyboard_name
	xmodmap_list		- List containing information needed for writing the
				  xmodmap file
returns:
	FALSE it went wrong, otherwise TRUE.
description:
	This function clears key_settings_list and xmodmap_list and then adds all the keys to
	them. These keys are from the keyboard name keyboard_name. It reads the values from a
	keyboard file.
*/
{
	FILE *keyboard_file;
	char 	keyboard_filename[MAX_FILE_SIZE+1],
		line[MAX_LINE_SIZE+1],
		rksx = TRUE; /* Contains the return value of read_key_settings_xmodmap () in the while loop */
	int lines_to_skip = 2;

	/* Create the filename of the file to open */
	create_keyboard_filename (keyboard_filename, MAX_FILE_SIZE, keyboard_name->manufacture, keyboard_name->model);
	/* Open the keyboard file */
	keyboard_file = fopen(keyboard_filename, "r");
	if (keyboard_file == NULL)
	{
		handle_error (ERR_KEYBOARD_CONFIG, "");
		return (FALSE);
	}
	/* Skip the first lines_to_skip lines */
 	while (lines_to_skip--)
	{
		fgets (line, MAX_NAME_SIZE+2, keyboard_file);
	}
	
	/* Clear key_settings_list and xmodmap_list */
	ksl_clear (key_settings_list);
	xmml_clear (xmodmap_list);
	
	/* Read every key setting */
	while ( (rksx = read_key_settings_xmodmap(key_settings_list, xmodmap_list, keyboard_file)) == TRUE )
		; /* NULL Statement */
	
	/* If an error occured while reading the new key settings */
	if (rksx == R_ERROR)
	{
		return (FALSE); /* Leave the settings unchanged and exit this function */
	}

	fclose (keyboard_file);
	return (TRUE);
}
