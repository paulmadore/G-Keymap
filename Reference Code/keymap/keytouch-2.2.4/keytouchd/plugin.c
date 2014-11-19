/*---------------------------------------------------------------------------------
Name               : plugin.c
Author             : Marvin Raaijmakers
Description        : Manages the plugins
Date of last change: 20-Aug-2005

    Copyright (C) 2005 Marvin Raaijmakers

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
#include <string.h>
#include <dirent.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <keytouchd.h>


static void add_plugin (KeytouchPlugin *plugin, KTPluginList *plugin_list);
static KeytouchPlugin *load_plugin (char *file_name);
static void read_plugin_dir (char *dir, KTPluginList *plugin_list);
static int always_true (const struct dirent *file);


KTPluginFunction
*get_plugin_function (	char	*plugin_name,
			char	*function_name )
/*
Input:
	plugin_name	- The name of the plugin to find
	function_name	- The function in the plugin to find
Output:
	-
Global:
	plugin_list	- List containing al available plugins
Returns:
	A pointer to the KTPluginFunction named function_name of the plugin named
	plugin_name. If no such function was found it returns NULL.
Description:
	The function searches for a KTPluginFunction in the list of available
	plugins.
*/
{
	KeytouchPlugin	*plugin;
	int		count;
	
	plugin = find_plugin(plugin_name);
	
	/* If the plugin does not exist */
	if (plugin == NULL)
	{
		return (NULL);
	}
	/* Find the function */
	for (count = 0;
	      count < plugin->num_functions &&
	      strcmp(plugin->function[count].name, function_name) != EQUAL;
	     count++)
		; /* NULL Statement */
		
	/* If the function does not exist */
	if (count == plugin->num_functions)
	{
		return (NULL);
	}
	return (&plugin->function[count]);
}


KeytouchPlugin
*find_plugin (char *plugin_name)
/*
Input:
	plugin_name	- The name of the plugin to find
Output:
	-
Global:
	plugin_list	- List containing al available plugins
Returns:
	A pointer to the KeytouchPlugin named plugin_name. If no such plugin was
	found it returns NULL.
Description:
	The function searches for a KeytouchPlugin in the list of available plugins.
*/
{
	KTPluginListEntry *plugin;
	
	/* Find the plugin */
	for (plugin = plugin_list.head;
	     plugin != NULL && strcmp(plugin->plugin->info.name, plugin_name) != EQUAL;
	     plugin = plugin->next)
		; /* NULL Statement */
	
	/* If the plugin does not exist */
	if (plugin == NULL)
	{
		return (NULL);
	}
	
	return (plugin->plugin);
}


void
add_plugin (	KeytouchPlugin	*plugin,
		KTPluginList	*plugin_list  )
/*
Input:
	plugin		- The plugin to add to plugin_list
Output:
	plugin_list	- plugin is added to this list
Returns:
	-
Description:
	This function adds plugin to plugin_list.
*/
{
	KTPluginListEntry *entry;
	
	entry = keytouch_malloc(sizeof(KTPluginListEntry));
	entry->plugin = plugin;
	entry->next = NULL;
	if (plugin_list->head == NULL)
	{
		plugin_list->head = plugin_list->tail = entry;
	}
	else
	{
		plugin_list->tail->next = entry;
		plugin_list->tail = entry;
	}
}


int
always_true (const struct dirent *file)
{
	return (TRUE);
}


KeytouchPlugin
*load_plugin (char *file_name)
/*
Input:
	file_name	- The file name of the plugin
Output:
	-
Returns:
	A pointer to the KeytouchPlugin structure if the plugin was succesfully
	loaded, otherwise NULL.
Description:
	This function loads the plugin which file name is file_name.
*/
{
	void		*plugin_file;
	KeytouchPlugin	*plugin;
	
	/* Open the plugin */
	plugin_file = dlopen(file_name, RTLD_LAZY);
	if (plugin_file == NULL)
	{
		return (NULL);
        }
	/* Get the plugin structure */
	plugin = dlsym(plugin_file, "plugin_struct");
	/* If an error occured or the plugin is already in the list */
	if (dlerror() != NULL || find_plugin(plugin->info.name) != NULL)
	{
		dlclose (plugin); /* Close the opened plugin */
		return (NULL);
	}
	return (plugin);
}


void
read_plugin_dir (	char		*dir,
			KTPluginList	*plugin_list  )
/*
Input:
	dir		- The directory to read the plugins from.
Output:
	plugin_list	- The plugins in dir are added to this list.
Returns:
	-
Description:
	This function reads the plugins from the directory which path is dir and
	adds them to plugin_list.
*/
{
	struct dirent	**file_list;
	int		num_files,
			count;
	KeytouchPlugin	*plugin;
	char		*complete_file_name;
	
	num_files = scandir (dir, &file_list, always_true, alphasort);
	if (num_files >= 0)
	{
		for (count = 0; count < num_files; count++)
		{
			complete_file_name = keytouch_malloc(strlen(file_list[count]->d_name) +
			                                     strlen(dir) + 2);
			strcpy (complete_file_name, dir);
			strcat (complete_file_name, "/");
			strcat (complete_file_name, file_list[count]->d_name);
			plugin = load_plugin(complete_file_name);
			if (plugin != NULL)
			{
				add_plugin (plugin, plugin_list);
			}
			free (complete_file_name);
		}
	}
	else
	{
		KTError ("Plugin directory '%s' does not exist. "
		         "This program will continue without loading "
			 "the plugins in this directory.", dir);
	}
}


void
read_plugins (KTPluginList *plugin_list)
/*
Input:
	-
Output:
	plugin_list	- The plugins are added to this list.
Returns:
	-
Description:
	This function reads the plugins from the plugin directories and adds them to
	plugin_list.
*/
{
	char	*home_dir,
		*local_plugin_dir;
	int	local_plugin_dir_len;
	
	read_plugin_dir (PLUGIN_DIR, plugin_list);
	home_dir = getenv("HOME");
	if (home_dir != NULL)
	{
		local_plugin_dir_len = strlen(home_dir)+strlen("/"LOCAL_PLUGIN_DIR);
		local_plugin_dir = keytouch_malloc(local_plugin_dir_len+1);
		strcpy (local_plugin_dir, home_dir);
		strcat (local_plugin_dir, "/"LOCAL_PLUGIN_DIR);
		read_plugin_dir (local_plugin_dir, plugin_list);
		free (local_plugin_dir);
	}
}

void
init_plugins (KTPluginList *plugin_list)
/*
Input:
	-
Output:
	plugin_list	- Initialized empty plugin list.
Returns:
	-
Description:
	This function initializes plugin_list to be empty.
*/
{
	plugin_list->head = plugin_list->tail = NULL;
}
