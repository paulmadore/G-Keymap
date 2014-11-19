/*-------------------------------------------------------------------------------
Name               : plugin_list.c
Author             : Marvin Raaijmakers
Description        : Providing operations for PLUGIN_LIST
Date of last change: 12-December-2004
History            :
                     12-December-2004 Created:
                                      - plugl_init()
                                      - plugl_plugin_new()
                                      - plugl_plugin_add()

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
#include <plugin_list.h>

void
plugl_init (PLUGIN_LIST *plugin_list)
/*
input:
	-
output:
	plugin_list	- List containing all available plugins
returns:
	-
description:
	This function initializes plugin_list.
*/
{
	plugin_list->head = plugin_list->tail = NULL;
}


PLUGIN
*plugl_plugin_new (void)
/*
input:
	-
output:
	-
returns:
	The address of the new PLUGIN.
description:
	This function creates a new (initialized) PLUGIN and returns its address.
*/
{
	PLUGIN *plugin;
	
	plugin = (PLUGIN *) malloc(sizeof(PLUGIN));
	if (plugin == NULL)
	{
		handle_fatal_error (FERR_MALLOC);
	}
	plugin->name[0] = plugin->file[0] = '\0';
	plugin->name[MAX_PLUGIN_NAME_SIZE] = plugin->file[MAX_FILE_SIZE] = '\0';
	plugin->next = NULL;
	
	return (plugin);
}


void
plugl_plugin_add (	PLUGIN_LIST	*plugin_list,
			PLUGIN		*new_plugin      )
/*
input:
	new_plugin	- The new PLUGIN to add to plugin_list
output:
	plugin_list	- The PLUGIN_LIST where new_plugin was added to
returns:
	-
description:
	This function adds new_plugin to plugin_list so that all plugins of plugin_list
	are in sequence.
*/
{
	PLUGIN *plugin;
	
	if (plugin_list->head == NULL) /* If plugin_list is empty */
	{
		plugin_list->head = plugin_list->tail = new_plugin;
	}
	  /* If the first plugin of plugin_list comes before new_plugin */
	else if (strcmp(plugin_list->head->name, new_plugin->name) < 0)
	{
		/* Find the plugin that will be the predecessor of new_plugin */
		for (	plugin = plugin_list->head;
			plugin->next && strcmp(plugin->next->name, new_plugin->name) < 0;
			plugin = plugin->next  )
			 ; /* NULL Statement */
		/* If new_plugin has no predecessor */
		if (plugin->next == NULL)
		{
			plugin_list->tail->next = new_plugin;
			plugin_list->tail = new_plugin;
		}
		else
		{
			new_plugin->next = plugin->next;
			plugin->next = new_plugin;
		}
	}
	else /* If new_plugin will be the first plugin of plugin_list */
	{
		new_plugin->next = plugin_list->head;
		plugin_list->head = new_plugin;
	}
}
