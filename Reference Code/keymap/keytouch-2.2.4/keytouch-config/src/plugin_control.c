/*---------------------------------------------------------------------------------
Name               : plugin_control.c
Author             : Marvin Raaijmakers
Description        : Provides functions for importing and removing plugins from and
                     to the plugin list and from and to the user's plugin directory
Date of last change: 20-May-2006
History            : 20-May-2006 Modified remove_plugin() and import_plugin() so
                                 that they use the types KTPluginInfoEntry and
                                 KTPluginInfoList
                     23-Jan-2006 Use variable _home_dir instead of calling
                                 getenv("HOME")

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

#include <stdio.h>
#include <string.h>

#include <keytouch.h>
#include <plugin_control.h>
#include <plugin-info.h>


static void copy_plugin (FILE *from, FILE *to);

void
remove_plugin (	KTPluginInfoEntry	*plugin,
		KTPluginInfoList	*plugin_list )
/*
Input:
	plugin		- The plugin to remove
	plugin_list	- The list that contains plugin
Output:
	plugin_list	- The list where plugin was removed from
Returns:
	-
Description:
	This function removes the file that belongs to plugin and removes the plugin
	from plugin_list.
*/
{
	if (remove(plugin->complete_file_name) == -1)
	{
		KTError (_("Removing plugin \"%s\" failed. The reason for this "
			   "is probably because this plugin is not located in "
			   "your home directory and you don't have permission to "
			   "remove a plugin in that is available for the whole"
			   " system."), plugin->info.name);
	}
	else
	{
		plugin_remove (plugin, plugin_list);
	}
}


void
copy_plugin (	FILE	*from,
		FILE	*to    )
{
	while (!feof(from))
	{
		putc (getc(from), to);
	}
}


void
import_plugin (	char			*file_name,
		KTPluginInfoList	*plugin_list )
/*
Input:
	file_name	- The file name of the pluhin
Output:
	plugin_list	- The list where plugin will be add to
Returns:
	-
Description:
	This function loads the plugin information, from the plugin named
	'file_name', and adds it to 'plugin_list'. If it is not a valid plugin none
	of this will be done.
*/
{
	KTPluginInfoEntry	*plugin_info;
	const char		*home_dir;
	char			*plugin_file_name; /* The file name of the plugin in the plugin dir */
	FILE			*from, *to;
	Boolean			successfully_added;
	
	plugin_info = load_plugin_info ("", file_name);
	if (plugin_info == NULL)
	{
		KTError (_("Failed to open \"%s\", because it is an invalid plugin."), file_name);
	}
	else if (find_plugin (plugin_info->info.name, plugin_list))
	{
		KTError (_("The plugin \"%s\" is already in the list."), plugin_info->info.name);
		free_plugin (plugin_info);
	}
	else
	{
		successfully_added = FALSE;
		/*home_dir = (char *)getenv("HOME");*/
		home_dir = _home_dir;
		if (home_dir != NULL)
		{
			plugin_file_name = keytouch_malloc(strlen(home_dir) +
			                                   strlen("/"LOCAL_PLUGIN_DIR"/") +
			                                   strlen(plugin_info->file_name) + 1);
			strcpy (plugin_file_name, home_dir);
			strcat (plugin_file_name, "/"LOCAL_PLUGIN_DIR"/");
			strcat (plugin_file_name, plugin_info->file_name);
			
			
			if ((to = fopen (plugin_file_name, "w")) == NULL)
			{
				KTError (_("Failed to copy the plugin to \"%s\"."),
				         plugin_file_name);
				free (plugin_file_name);
			}
			else if ((from = fopen (file_name, "r")) == NULL)
			{
				KTError (_("Failed to open \"%s\". Check if you are"
					   " permitted to read this file."), file_name);
				fclose (to);
				free (plugin_file_name);
			}
			else
			{
				/* Instead of reloading the plugin we just
				 * replace the original complete file name by
				 * the new complete file name */
				free (plugin_info->complete_file_name);
				plugin_info->complete_file_name = plugin_file_name;
				
				add_plugin (plugin_info, plugin_list);
				successfully_added = TRUE;
				copy_plugin (from, to);
				
				fclose (from);
				fclose (to);
			}
		}
		if (!successfully_added)
		{
			free_plugin (plugin_info);
		}
	}
}
