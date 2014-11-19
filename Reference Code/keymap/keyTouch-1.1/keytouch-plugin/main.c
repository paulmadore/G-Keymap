/*-------------------------------------------------------------------------------
Name               : main.c
Author             : Marvin Raaijmakers
Description        : PLugin handler
Date of last change: 7-August-2004
History            :
                     7-August-2004  Created this file

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
#include <dlfcn.h>
#include <stdlib.h>
#include <keytouch-plugin.h>
#include <string.h>

#define EQUAL	(0)

typedef enum {
	FERR_MALLOC = 1,
	FERR_CONFIG_R,
	FERR_INVALID_PLUGIN_FILE,
	FERR_INVALID_CONFIG_FILE,
	FERR_PLUGIN_NOT_FOUND,
	FERR_INVALID_PLUGIN
} FATAL_ERROR;


void
handle_fatal_error (FATAL_ERROR error)
/*
input:
	error	- Indicates which error occured
output:
	-
returns:
	-
description:
	This function prints a error message to stderr and shows a dialog containing the
	message. error indicates which error occured. After printing the message the
	program will be exited and give a return value which is equal to error.
*/
{
	const char *msg[] = {	"Can not allocate any memory!\n",
				"Can not read the configuration file.\n",
				"The plugin file has an invalid syntax!\n",
				"Invallid configuration file!\n",
				"Plugin was not found!\n",
				"Invallid plugin!\n"             };

	/* Print the error message to the stderr */
	fprintf (stderr, "> FATAL ERROR: %s\n", msg[error-1]);

	exit (error);
}


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

	if (feof(file))
	{
		return (EOF);
	}

	return (count);
}


int
read_non_empty_line (	FILE *file,
			char *line,
			int  max_line_size  )
{
	int n;

	while ( ( (n = readline (file, line, max_line_size)) == 0 ) && (n != EOF) )
		; /* NULL Statement */

	return (n);
}


void
read_pref_line (	FILE *file,
			char *line,
			int  max_line_size  )
{
	if ( readline(file, line, max_line_size) == EOF )
	{
		handle_fatal_error (FERR_INVALID_CONFIG_FILE);
	}
}


PREFERENCES
*get_preferences (void)
{
	char	config_file_name[MAX_FILE_SIZE+1] = "",
		line[MAX_LINE_SIZE+1],
		c;
	PREFERENCES *preferences;
	FILE *config_file;

	strncat (config_file_name, getenv("HOME"), MAX_FILE_SIZE);
	strncat (config_file_name, "/.keytouch/config", MAX_FILE_SIZE-strlen(config_file_name));
	
	config_file = fopen(config_file_name, "r");
	if (config_file == NULL)
	{
		handle_fatal_error (FERR_INVALID_CONFIG_FILE);
	}
	
	do /* Read all lines before the preferences */
	{
		readline(config_file, line, MAX_LINE_SIZE);
	} while ( !feof(config_file) && (line[0] != '}') );
	
	/* If the following text is not "preferences\n{\n" */
	if ( feof(config_file) || !fscanf(config_file, "preferences\n{%c", &c) || (c != '\n') )
	{
		handle_fatal_error (FERR_INVALID_CONFIG_FILE);
	}
	
	/* Allocate memory for the preferences */
	preferences = malloc( sizeof(PREFERENCES) );
	if (preferences == NULL)
	{
		handle_fatal_error (FERR_MALLOC);
	}

	/* Read all preferences */
	read_pref_line (config_file, preferences->documents_dir, MAX_FILE_SIZE);
	read_pref_line (config_file, preferences->home_page, MAX_FILE_SIZE);
	read_pref_line (config_file, preferences->search_page, MAX_FILE_SIZE);
	read_pref_line (config_file, preferences->browser, MAX_PROG_SIZE);
	read_pref_line (config_file, preferences->email_program, MAX_PROG_SIZE);
	read_pref_line (config_file, preferences->messenger, MAX_PROG_SIZE);
	
	fclose (config_file);
	
	return (preferences);
}


char
*find_plugin (char *req_plugin_name)
{
	char	plugin_list_file_name[MAX_FILE_SIZE+1] = "",
		plugin_file_name[MAX_FILE_SIZE+1],
		plugin_name[MAX_PLUGIN_NAME_SIZE+1];
	static char return_plugin_file_name[MAX_FILE_SIZE+1];
	FILE *plugin_list_file;

	strncat (plugin_list_file_name, getenv("HOME"), MAX_FILE_SIZE);
	strncat (plugin_list_file_name, "/.keytouch/plugins", MAX_FILE_SIZE-strlen(plugin_list_file_name));
	
	plugin_list_file = fopen(plugin_list_file_name, "r");
	if (plugin_list_file == NULL)
	{
		handle_fatal_error (FERR_INVALID_PLUGIN_FILE);
	}
	/* Search for the matching plugin name */
	while ( !feof(plugin_list_file) )
	{
		read_non_empty_line (plugin_list_file, plugin_file_name, MAX_FILE_SIZE);
		read_non_empty_line (plugin_list_file, plugin_name, MAX_FILE_SIZE);
		if ( strcmp(req_plugin_name, plugin_name) == EQUAL ) /* If the plugin name matches */
		{
			fclose (plugin_list_file);
			strncat (return_plugin_file_name, getenv("HOME"), MAX_FILE_SIZE);
			strncat (return_plugin_file_name, "/.keytouch/plugin.d/", MAX_FILE_SIZE-strlen(return_plugin_file_name));
			strncat (return_plugin_file_name, plugin_file_name, MAX_FILE_SIZE-strlen(return_plugin_file_name));
			return (return_plugin_file_name);
		}
	}
	fclose (plugin_list_file);
	/* The plugin name was not found. So: */
	return (NULL);
}


int
main (	int argc,
	char *argv[] )
{
	char *plugin_file_name;
	void *plugin;
	PREFERENCES *preferences;
	void (*plugin_function)(PREFERENCES *);

	if (argc != 2)
	{
		exit (1);
	}
	/* Find the file name of the plugin */
	plugin_file_name = find_plugin (argv[1]);
	if (plugin_file_name == NULL) /* If no plugin was found */
	{
		handle_fatal_error (FERR_PLUGIN_NOT_FOUND);
	}

	/* Open the plugin */
	plugin = dlopen (plugin_file_name, RTLD_LAZY);
	if (plugin == NULL)
	{
		handle_fatal_error (FERR_INVALID_PLUGIN);
        }
	/* Read information from the plugin */
	plugin_function = dlsym(plugin, "plugin_function");
	if (dlerror() != NULL)
	{
		dlclose (plugin); /* Close the opened plugin */
		handle_fatal_error (FERR_INVALID_PLUGIN);
        }
	
	preferences = get_preferences(); /* Get the user preferences */
	(*plugin_function)(preferences); /* Run the plugin (function) */
	dlclose (plugin); /* Close the plugin */
	
	exit (0);
}
