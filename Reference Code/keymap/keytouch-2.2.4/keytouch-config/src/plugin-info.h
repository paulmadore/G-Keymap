/*---------------------------------------------------------------------------------
Name               : plugin-info.h
Author             : Marvin Raaijmakers
Description        : This header file contains the types used for storing
                     information about plugins
Date of last change: 13-May-2006

    Copyright (C) 2006 Marvin Raaijmakers

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
#ifndef PLUGIN_INFO_H_INCLUDED
#define PLUGIN_INFO_H_INCLUDED

#define plugil_first_plugin(_plugl)		((_plugl)->head)
#define plugil_is_empty(_plugl)			((_plugl)->head == NULL)
#define plugil_plugin_get_name(_plugin)		((_plugin)->info.name)

#ifndef TRUE
#	define TRUE 1
#	define FALSE 0
	typedef char Boolean;
#endif

typedef struct _plugin_info_entry {
	struct {
		char	*name;
		char	*author;
		char	*license;
		char	*version;
		char	*description;
	} info;
	char	*file_name,		/* The filename that the plugin will have in the plugin directory */
		*complete_file_name;	/* The complete (= including its path) file name of the plugin */
	int num_functions;
	char **function_names; /* Points to an array of 'num_functions' function names */
	struct _plugin_info_entry *next;
} KTPluginInfoEntry;

typedef struct {
	KTPluginInfoEntry	*head,
				*tail;
} KTPluginInfoList;


/******* Functions in plugin.c *******/
extern Boolean plugin_function_exists (char *plugin_name, char *function_name, KTPluginInfoList *plugin_list);
extern void read_plugins (KTPluginInfoList *plugin_list);
extern void init_plugin_list (KTPluginInfoList *plugin_list);
extern void clear_plugin_list (KTPluginInfoList *plugin_list);
extern KTPluginInfoEntry *find_plugin (char *plugin_name, KTPluginInfoList *plugin_list);
extern void plugin_remove (KTPluginInfoEntry *plugin, KTPluginInfoList *plugin_list);
extern void add_plugin (KTPluginInfoEntry *plugin, KTPluginInfoList *plugin_list);
extern KTPluginInfoEntry *load_plugin_info (char *dir, char *file_name);
extern void free_plugin (KTPluginInfoEntry *plugin);

#endif
