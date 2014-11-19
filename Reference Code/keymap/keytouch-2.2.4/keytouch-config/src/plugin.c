/*----------------------------------------------------------------------------------
Name               : plugin.c
Author             : Marvin Raaijmakers
Description        : Does all the plugin managing things
Date of last change: 13-May-2006
History            : 13-May-2006 - load_plugin() replaced by load_plugin_info() that
                                   copies info about the plugin to a list and closes
                                   the plugin
                                 - find_plugin() now takes the plugin list as an arg
                                 - Replaced usage of the KeytouchPlugin type by the
                                   KTPluginInfoEntry type

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
------------------------------------------------------------------------------------*/
#include <string.h>
#include <dirent.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <keytouch.h>
#include <plugin.h>
#include <plugin-info.h>


static void read_plugin_dir (char *dir, KTPluginInfoList *plugin_list);
static int always_true (const struct dirent *file);


Boolean
plugin_function_exists (	char			*plugin_name,
				char			*function_name,
				KTPluginInfoList	*plugin_list )
/*
Input:
	plugin_name	- The name of the plugin to find
	function_name	- The function in the plugin to find
	plugin_list	- List containing al available plugins
Returns:
	TRUE if there is plugin named 'plugin_name' in 'plugin_list' that has a
	function named 'function_name', otherwise FALSE.
*/
{
	KTPluginInfoEntry	*plugin;
	int			count;
	
	plugin = find_plugin (plugin_name, plugin_list);
	
	/* If the plugin exists */
	if (plugin)
	{
		/* Find the function */
		for (count = 0; count < plugin->num_functions; count++)
		{
			if (strcmp(plugin->function_names[count], function_name) == EQUAL)
			{
				return TRUE;
			}
		}
	}
	
	return FALSE;
}


KTPluginInfoEntry
*find_plugin (	char			*plugin_name,
		KTPluginInfoList	*plugin_list )
/*
Input:
	plugin_name	- The name of the plugin to find
	plugin_list	- List containing al available plugins
Returns:
	A pointer to the KTPluginInfoEntry named plugin_name. If no such plugin was
	found it returns NULL.
Description:
	The function searches for a KTPluginInfoEntry named 'plugin_name' in
	'plugin_list'.
*/
{
	KTPluginInfoEntry *plugin;
	
	/* Find the plugin */
	for (plugin = plugin_list->head;
	     plugin && strcmp(plugin->info.name, plugin_name) != EQUAL;
	     plugin = plugin->next)
		; /* NULL Statement */
	
	return (plugin);
}


void
add_plugin (	KTPluginInfoEntry	*plugin,
		KTPluginInfoList	*plugin_list  )
/*
Input:
	plugin		- The plugin to add to plugin_list
Output:
	plugin_list	- plugin is added to this list
Returns:
	-
Description:
	This function adds 'plugin' to the end of 'plugin_list'.
*/
{
	plugin->next = NULL;
	if (plugin_list->head == NULL)
	{
		plugin_list->head = plugin_list->tail = plugin;
	}
	else
	{
		plugin_list->tail->next = plugin;
		plugin_list->tail = plugin;
	}
}


void
free_plugin (KTPluginInfoEntry *plugin)
/*
Input:
	plugin	- Points to the KTPluginInfoEntry to be freed
Output:
	plugin	- The KTPluginInfoEntry pointed to by this pointer will be freed
		  including the strings that are used by the KTPluginInfoEntry
Description:
	This function frees the memory used by 'plugin' and 'plugin' itselfs.
*/
{
	int function;
	
	free (plugin->info.name);
	free (plugin->info.author);
	free (plugin->info.license);
	free (plugin->info.version);
	free (plugin->info.description);
	free (plugin->file_name);
	free (plugin->complete_file_name);
	for (function = 0; function < plugin->num_functions; function++)
	{
		free (plugin->function_names[function]);
	}
	free (plugin->function_names);
	free (plugin);
}


void
plugin_remove (	KTPluginInfoEntry	*plugin,
		KTPluginInfoList	*plugin_list  )
/*
Input:
	plugin		- The plugin to remove the entry of from plugin_list
Output:
	plugin		- The memory pointed to by this pointer will be freed
	plugin_list	- The list where the plugin's entry was removed from
Returns:
	-
Description:
	This function removes 'plugin' in from 'plugin_list'. If the plugin pointed
	to by 'plugin' does not appear in 'plugin_list' then the nothing will be
	done by this function.
*/
{
	KTPluginInfoEntry *entry, *tmp_entry;
	
	if (plugin_list->head == NULL)
	{
		return;
	}
	if (plugin_list->head == plugin)
	{
		entry = plugin_list->head;
		plugin_list->head = entry->next;
		if (plugin_list->head == NULL)
		{
			plugin_list->tail = NULL;
		}
	}
	else
	{
		/* Find the entry before the entry that contains the plugin */
		for (entry = plugin_list->head;
		     entry->next != NULL && entry->next != plugin;
		     entry = entry->next)
			; /* NULL Statement */
		
		if (entry->next == NULL)
		{
			return;
		}
		/* If the entry is the last one in the list */
		if (entry->next->next == NULL)
		{
			plugin_list->tail = entry;
			entry = entry->next;
			plugin_list->tail->next = NULL;
		}
		else
		{
			tmp_entry = entry;
			entry = entry->next;
			tmp_entry->next = entry->next;
		}
	}
	free_plugin (entry);
}


int
always_true (const struct dirent *file)
{
	return (TRUE);
}


KTPluginInfoEntry
*load_plugin_info (char *dir, char *file_name)
/*
Input:
	dir		- The directory where the plugin is located in
	file_name	- The file name of the plugin
Output:
	-
Returns:
	A pointer to the KTPluginInfoEntry structure if information was succesfully
	read from the plugin, otherwise NULL.
Description:
	This function loads the plugin which file name is 'file_name' and fills in
	the values of a new KTPluginInfoEntry that will be read from the plugin.
	The next element of the new KTPluginInfoEntry will point to NULL.
*/
{
	void			*plugin_file;
	KTPluginInfoEntry	*plugin_info = NULL;
	KeytouchPlugin		*plugin;
	char			*complete_file_name;
	int			function;
	
	complete_file_name = keytouch_malloc(strlen(file_name) + strlen(dir) + 2);
	strcpy (complete_file_name, dir);
	strcat (complete_file_name, "/");
	strcat (complete_file_name, file_name);
	/* Open the plugin */
	plugin_file = dlopen (complete_file_name, RTLD_LAZY);
	if (plugin_file)
	{
		/* Get the plugin structure */
		plugin = dlsym(plugin_file, "plugin_struct");
		/* If the plugin_struct symbol was successfully read */
		if (dlerror() == NULL)
		{
			plugin_info = (KTPluginInfoEntry *) keytouch_malloc( sizeof(KTPluginInfoEntry) );
			plugin_info->info.name =  keytouch_strdup (plugin->info.name);
			plugin_info->info.author =  keytouch_strdup (plugin->info.author);
			plugin_info->info.license =  keytouch_strdup (plugin->info.license);
			plugin_info->info.version =  keytouch_strdup (plugin->info.version);
			plugin_info->info.description =  keytouch_strdup (plugin->info.description);
			plugin_info->file_name = file_name;
			plugin_info->num_functions = plugin->num_functions;
			plugin_info->function_names = (char **) keytouch_malloc( sizeof(char *) * plugin->num_functions);
			for (function = 0; function < plugin->num_functions; function++)
			{
				plugin_info->function_names[function] = keytouch_strdup (plugin->function[function].name);
			}
			plugin_info->file_name = keytouch_strdup (plugin->file_name);
			plugin_info->complete_file_name = complete_file_name;
			plugin_info->next = NULL;
		}
		dlclose (plugin_file); /* Close the opened plugin */
	}
	if (plugin_info == NULL)
	{
		free (complete_file_name);
	}
	return (plugin_info);
}


void
read_plugin_dir (	char			*dir,
			KTPluginInfoList	*plugin_list  )
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
	struct dirent		**file_list;
	int			num_files,
				count;
	KTPluginInfoEntry	*plugin;
	
	num_files = scandir (dir, &file_list, always_true, alphasort);
	if (num_files >= 0)
	{
		for (count = 0; count < num_files; count++)
		{
			
			plugin = load_plugin_info (dir, file_list[count]->d_name);
			if (plugin != NULL && !find_plugin (plugin->info.name, plugin_list))
			{
				add_plugin (plugin, plugin_list);
			}
		}
	}
	else if (mkdir(dir, S_IWUSR | S_IRUSR | S_IXUSR) == -1)
	{
		KTError (_("Plugin directory '%s' does not exist and "
			   "creating the directory failed. This program "
			   "will continue without loading the plugins "
			   "in this directory."), dir);
	}
}


void
read_plugins (KTPluginInfoList *plugin_list)
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
	const char	*home_dir;
	char		*local_plugin_dir;
	int		local_plugin_dir_len;
	
	read_plugin_dir (PLUGIN_DIR, plugin_list);
	/*home_dir = (char *)getenv("HOME");*/
	home_dir = _home_dir;
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
init_plugin_list (KTPluginInfoList *plugin_list)
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


void
clear_plugin_list (KTPluginInfoList *plugin_list)
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
	KTPluginInfoEntry *plugin;
	
	for (plugin = plugin_list->head; plugin; plugin = plugin->next)
	{
		free_plugin (plugin);
	}
	plugin_list->head = plugin_list->tail = NULL;
}
