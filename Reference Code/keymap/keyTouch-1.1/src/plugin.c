/*-------------------------------------------------------------------------------
Name               : plugin.c
Author             : Marvin Raaijmakers
Description        : Handling plugin stuff
Date of last change: 4-July-2004
History            :
                    VERSION 1.0:
                     4-July-2004      Created: - This file
                                               - create_plugin_filename()
                                               - plugin_import()
                     13-August-2004   Fixed bug in plugin_import(): if plugin_list
                                      was empty the new plugin will not be added.
                                      To fix the bug add_plugin() was made.
                     31-August-2004   Fixed create_plugin_filename()
                    VERSION 1.1:
                     12-December-2004 Modified:
                                      - create_plugin_filename()
                                      - plugin_import()
                                      Removed add_plugin()

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
#include <stdlib.h>
#include <dlfcn.h>

#include <keytouch.h>
#include <plugin_list.h>

static void create_plugin_filename (char *filename, int size, PLUGIN *plugin);

void
create_plugin_filename (	char	*filename,
				int	size,
				PLUGIN	*plugin          )
/*
input:
	size		- The maximum size of filename (excluding the '\0' character)
	plugin		- The plugin to create filename for
output:
	filename	- The file name of plugin
returns:
	-
description:
	This function creates a string which is the plugin file name of plugin. It puts
	the string at filename. size will be the maximum number of characters of filename
	(excluding the '\0' character)
*/
{
	*filename = '\0';
	size -= (MAX_PLUGIN_NAME_SIZE + 1);
	strncpy (filename, getenv("HOME"), size); /* Find out the home directory */
	strncat (filename, "/.keytouch/plugin.d/", size - strlen(filename)); /* Append the keytouchdir to the homedir */
	/* Append the keboard filename */
	strncat (filename, plugl_plugin_get_file(plugin), MAX_PLUGIN_NAME_SIZE);
	/*sprintf (filename + strlen(filename), "%s", plugl_plugin_get_file(plugin));*/
}


void
plugin_import (	char		*filename,
		PLUGIN_LIST	*plugin_list  )
/*
input:
	filename	- The name of the file to read the plugin from
	plugin_list	- List containing all available plugins
output:
	plugin_list	- List containing all available plugins
returns:
	-
description:
	This function opens a plugin, named filename. If the plugin can be opened and
	is a keytouch plugin the function will copy it to the directory in which the
	other plugins are located. The plugin will also be added to plugin_list.
*/
{
	FILE *plugin_input_file, *plugin_output_file;
	char plugin_filename[MAX_FILE_SIZE+1], **plugin_name, **plugin_file;
	void *plugin;
	PLUGIN *new_plugin;

	/***************************************************
	 *         GET INFORMATION ABOUT THE PLUGIN        *
	 ***************************************************/

	/* Open the plugin */
	plugin = dlopen (filename, RTLD_LAZY);
	if (plugin == NULL)
	{
		handle_error (ERR_NO_PLUGIN, filename);
		return;
        }

	/* Read information from the plugin */
	plugin_name = dlsym(plugin, "keytouch_plugin_name");
	plugin_file = dlsym(plugin, "keytouch_plugin_file");
	if (dlerror() != NULL)
	{
		handle_error (ERR_INVALID_PLUGIN, filename);
		dlclose (plugin); /* Close the opened plugin */
		return;
        }
	
	new_plugin = plugl_plugin_new();
	plugl_plugin_set_file (new_plugin, *plugin_file);
	plugl_plugin_set_name (new_plugin, *plugin_name);
	plugl_plugin_add (plugin_list, new_plugin);
	
	/* Close the plugin */
	dlclose (plugin);

	/***************************************************
	 *                 COPY THE PLUGIN                 *
	 ***************************************************/

	/* Open the plugin input file */
	plugin_input_file = fopen(filename, "r");
	if (plugin_input_file == NULL)
	{
		handle_error (ERR_PLUGIN_R, filename);
		fclose (plugin_input_file);
		return;
	}

	create_plugin_filename (plugin_filename, MAX_FILE_SIZE, new_plugin);
	/* Open the plugin output file */
	plugin_output_file = fopen(plugin_filename, "w");
	if (plugin_output_file == NULL)
	{
		handle_error (ERR_PLUGIN_W, plugin_filename);
		fclose (plugin_input_file);
		fclose (plugin_output_file);
		return;
	}

	/* Copy the import file to the keyboard file */
	while (!feof(plugin_input_file))
	{
		putc (getc(plugin_input_file), plugin_output_file);
	}

	/* Close both files */
	fclose (plugin_input_file);
	fclose (plugin_output_file);
}
