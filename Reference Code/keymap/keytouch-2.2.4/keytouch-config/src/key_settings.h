/*---------------------------------------------------------------------------------
Name               : key_settings.h
Author             : Marvin Raaijmakers
Description        : Headerfile for key_settings.c
Date of last change: 02-Jun-2005

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
#ifndef INCLUDE_KEY_SETTINGS
#define INCLUDE_KEY_SETTINGS

#include <string.h>

#define ksl_first_key(_key_list)		((_key_list)->head)
#define ks_get_key_name(_key)			((_key)->key_name)

#define ks_action_type(_key)			((_key)->action.type)
#define ks_default_action_type(_key)		((_key)->default_action.type)

#define ks_action_command(_key)			((_key)->action.program.command)
#define ks_default_action_command(_key)		((_key)->default_action.program.command)

#define ks_action_plugin_name(_key)		((_key)->action.plugin.plugin_name)
#define ks_default_action_plugin_name(_key)	((_key)->default_action.plugin.plugin_name)

#define ks_action_plugin_function(_key)		((_key)->action.plugin.function_name)
#define ks_default_action_plugin_function(_key)	((_key)->default_action.plugin.function_name)

#define ks_action_is_default(_key)		( !memcmp(&(_key)->action, &(_key)->default_action, sizeof(KTAction)) )

extern KTKeySettings *find_key_settings (char *key_name, KTKeySettingsList *key_list);
extern void clear_key_settings_action (KTKeySettings *key);
extern void clear_key_setting_list (KTKeySettingsList *key_list);
extern void ks_set_program (KTKeySettings *key, char *command);
extern void ks_set_plugin (KTKeySettings *key, char *plugin_name, char *function_name);

#endif
