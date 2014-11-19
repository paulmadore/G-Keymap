/*---------------------------------------------------------------------------------
Name               : plugin.h
Author             : Marvin Raaijmakers
Description        : Headerfile for plugin.c
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
#define PLUGIN_H_INCLUDED	1


typedef struct {
	char	*documents_dir,
		*home_page,
		*search_page,
		*browser,
		*email_program,
		*chat_program;
} KTPreferences;


typedef struct {
	struct {
		char	*name;	/* This string must appear in the name of the window */
		Boolean	is_end; /* If TRUE name appears at the end of the
				 * window's name, otherwise at the beginning */
	} window_name;
	unsigned int state;	/* The state of the key-press event to send */
	char         *keysym;	/* The keysym of the key to send */
} KTPluginKey;

typedef struct {
	int		num_programs;
	KTPluginKey	*key;
} KTPluginKeyFunction;	/* This structure contains information about shortcuts.
			 * Each element of this array contains shortcut information
			 * for one specific application. All shortcuts perform the
			 * same action.
			 * The window name of the last element may be
			 * NULL. In this case the shortcut will be used for every
			 * application that does not appear in the array. */


typedef enum {
	KTPluginFunctionType_Key,
	KTPluginFunctionType_Function
} KTPluginFunctionType;

typedef struct {
	char *name;
	KTPluginFunctionType type;
	union {
		KTPluginKeyFunction key_function;
		void (*function) (KTPreferences *preferences);
	} function;
} KTPluginFunction;

typedef struct {
	struct {
		char	*name;
		char	*author;
		char	*license;
		char	*version;
		char	*description;
	} info;
	char *file_name;
	int num_functions;
	KTPluginFunction function[];
} KeytouchPlugin;


typedef struct _plugin_list_entry {
	KeytouchPlugin			*plugin;
	struct _plugin_list_entry	*next;
} KTPluginListEntry;

typedef struct {
	KTPluginListEntry	*head,
				*tail;
} KTPluginList;


/******* Functions in plugin.c *******/
extern KTPluginFunction *get_plugin_function (char *plugin_name, char *function_name);
extern void read_plugins (KTPluginList *plugin_list);
extern void init_plugins (KTPluginList *plugin_list);
extern KeytouchPlugin *find_plugin (char *plugin_name);
