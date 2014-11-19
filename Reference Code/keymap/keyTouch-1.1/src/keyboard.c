/*-------------------------------------------------------------------------------
Name               : keyboard.c
Author             : Marvin Raaijmakers
Description        : Changing keyboard settings
Date of last change: 2-July-2004
History            :
                    VERSION 1.0:
                     29-June-2004     Added index to keyboard_change() as an input
                                      variable
                     1-July-2004      Added filename to keyboard_import() as an
                                      input variable
                     2-July-2004      Modified keyboard_change(): If a keyboard
                                      file could not be opened the program will not
                                      exit
                     31-August-2004   Fixed create_keyboard_filename ()
                     10-October-2004  Fixed bug in keyboard_import():
                                      If only the first keyboard in keyboard_list
                                      is from the same manufacturer as the new
                                      keyboard it would be inserted at the end of
                                      the list (After I fixed it will be inserted
                                      after the first keyboard)
                    VERSION 1.1:
                     5-December-2004  Modified:
                                      - keyboard_change()
                                      - create_keyboard_filename()
                     6-December-2004  Modified:
                                      - keyboard_change()
                                      - keyboard_import()
                     11-June-2004     keyboard_import() now first checks if the
                                      import file is not a directory

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
#include <sys/stat.h>

#include <keytouch.h>
#include <keyboard_config.h>
#include <keyboard_list.h>

void
create_keyboard_filename (	char		*filename,
				int		size,
				char		manufacture[MAX_NAME_SIZE],
				char		model[MAX_NAME_SIZE]       )
/*
input:
	size		- The maximum size of filename (excluding the '\0' character)
	keyboard_config	- The configuration of keyboard
output:
	filename	- The keyboard file name of keyboard
returns:
	-
description:
	This function creates a string which is the keyboard file name of keyboard. It
	puts the string at filename. size will be the maximum number of characters of
	filename (excluding the '\0' character)
*/
{
	size -= (MAX_NAME_SIZE*2 + 2);
	strncpy (filename, getenv("HOME"), size); /* Find out the home directory */
	strncat (filename, "/.keytouch/", size - strlen(filename)); /* Append the keytouchdir to the homedir */
	/* Append the keboard filename */
	sprintf (filename + strlen(filename), "%s.%s", model, manufacture);
}

void
keyboard_change (	char		*manufacture_name,
			char		*model_name,
			KEYBOARD_CONFIG	*current_keyboard_config,
			XMODMAP_LIST	*xmodmap_list              )
/*
input:
	manufacture_name	- The name of the manfacture of the choosen keyboard
	model_name		- The name of the model of the choosen keyboard
output:
	current_keyboard_config	- Contains the configuration of the current keyboard
	xmodmap_list		- List containing information needed for writing the xmodmap
				  file
returns:
	-
description:
	This function will change the current keyboard to the keyboard, named model_name from
	manufacture_name, and will correct current_keyboard_config and xmodmap.
*/
{
	KEYBOARD_NAME new_keyboard_name;
	
	strcpy (new_keyboard_name.manufacture, manufacture_name);
	strcpy (new_keyboard_name.model, model_name);
	/*
	 * Correct the key settings and xmodmap and check
	 * if the settings where succesfully changed
	 */
	if ( change_key_settings (kbcfg_key_settings_list(current_keyboard_config), xmodmap_list, &new_keyboard_name) )
	{
		/* Correct the name of the current keyboard */
		kbcfg_set_name (current_keyboard_config, model_name, manufacture_name);
	}
}


void
keyboard_import (	char		*filename,
			KEYBOARD_LIST	*keyboard_list   )
/*
input:
	filename	- The name of the file to read the keyboard information from
	keyboard_list	- List containing all available keyboards
output:
	keyboard_list	- List containing all available keyboards
returns:
	-
description:
	This function opens an import file, named filename. If the file can be opened
	the function will copy it to the directory in which the other keyboard files
	are located and give it the right name ('<model>.<manufacture>'). The keyboard
	of the import file will also be added to keyboard_list.
*/
{
	FILE *import_file, *keyboard_file;
	char keyboard_filename[MAX_FILE_SIZE+1], c, line[MAX_LINE_SIZE+1];
	KEYBOARD_MODEL *keyboard_model;
	KEYBOARD_MANUFACTURE *manufacture;
	struct stat import_file_attr;

	stat (filename, &import_file_attr);
	/* Check if the import file is a directory */
	if (S_ISDIR(import_file_attr.st_mode))
	{
		handle_error (ERR_IMPORT_FILE, filename);
		return;
	}
	
	/* Open the import filename */
	import_file = fopen (filename, "r");
	if (import_file == NULL)
	{
		handle_error (ERR_IMPORT_FILE, filename);
		return;
	}

	/* Create a new KEYBOARD_MODEL */
	keyboard_model = kbl_model_new();
	/* Read assing the new keyboard model and manufacture and check if a
	 * whole line has been read
	 */
	readline (import_file, line, MAX_NAME_SIZE);
	kbl_model_set_name(keyboard_model, line);
	if ( feof(import_file) )
	{
		handle_error (ERR_INVALID_IMPORT_FILE, filename);
		fclose (import_file);
		return;
	}
	readline (import_file, line, MAX_NAME_SIZE);
	if ( feof(import_file) )
	{
		handle_error (ERR_INVALID_IMPORT_FILE, filename);
		fclose (import_file);
		return;
	}
	manufacture = kbl_manufacture_find(keyboard_list, line);
	if (manufacture == NULL) /* If the manufacture does not already exist */
	{
		manufacture = kbl_manufacture_new();
		kbl_manufacture_set_name (manufacture, line);
		kbl_manufacture_add (keyboard_list, manufacture);
	}
	kbl_model_add (manufacture, keyboard_model);

	/* Create the filename of the file to open */
	create_keyboard_filename ( keyboard_filename, MAX_FILE_SIZE,
		kbl_manufacture_get_name(manufacture), kbl_model_get_name(keyboard_model));
	/* Open the keyboard file */
	keyboard_file = fopen (keyboard_filename, "w");
	if (keyboard_filename == NULL)
	{
		handle_fatal_error (FERR_OUTPUT_KEYBOARD_FILE);
	}

	/* Set the file position indicator of import_file to the beginning of the file */
	fseek (import_file, 0, SEEK_SET);
	/* Copy the import file to the keyboard file */
	while ( (c = getc(import_file)) != EOF )
	{
		putc( c, keyboard_file );
	}

	/* Close both files */
	fclose (keyboard_file);
	fclose (import_file);
}
