/*---------------------------------------------------------------------------------
Name               : keytouchd.h
Author             : Marvin Raaijmakers
Description        : The main headerfile of keytouchd
Date of last change: 09-Feb-2006
History            :
                     09-Feb-2006 Modified PLUGIN_DIR
                     26-Feb-2006 Added auto_repeat_on member to KTKeySettings

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
#ifndef RAND_MAX
#	include <stdlib.h>
#endif
#ifndef XA_WM_HINTS
#	include <X11/Xlib.h>
#endif

#define KEYTOUCH_CONF_DIR		SYSCONF_DIR "/keytouch"
#define KEYTOUCH_DATA_DIR		PACKAGE_DATA_DIR "/keytouch"
#define KEYTOUCH_LIB_DIR		PACKAGE_LIB_DIR "/keytouch"
#define USER_CFG_DIR			".keytouch2"				/* In the users home-dir */
#define KEYBOARD_FILES_DIR		KEYTOUCH_DATA_DIR "/keyboards"
#define KEYBOARD_CONFIG_FILES_DIR	USER_CFG_DIR
#define PREFERENCES_FILE		USER_CFG_DIR "/preferences.xml"		/* In the users home-dir */
#define CURRENT_KEYBOARD_FILE		KEYTOUCH_CONF_DIR "/current_keyboard.xml"
#define PLUGIN_DIR			KEYTOUCH_LIB_DIR "/plugins"
#define LOCAL_PLUGIN_DIR		USER_CFG_DIR "/plugins"			/* In the users home-dir */

#define TRUE	1
#define FALSE	0

#define EQUAL	0


#ifndef XmlGetContentType /* If <mxml.h> is not included */
	typedef char Boolean;
#endif

#ifndef PLUGIN_H_INCLUDED
#	include <plugin.h>
#endif


typedef struct {
	char	*manufacturer,
		*model;
} KTKeyboardName;


typedef enum {
	KTActionTypeProgram,
	KTActionTypePlugin,
	NUM_KTActionType
} KTActionType;

typedef struct {
	KTActionType	type;
	char		*command;
} KTActionProgram;

typedef struct {
	KTActionType		type;
	KTPluginFunction	*function;
} KTActionPlugin;

typedef union {
	KTActionType	type;
	KTActionProgram	program;
	KTActionPlugin	plugin;
} KTAction;

typedef struct _key_settings {
	char			*key_name;	/* The name of the function key */
	KTAction		action;		/* The action to perform when the key is pressed */
	unsigned int		keycode;	/* The keycode that the key has in X */
	Boolean			auto_repeat_on;
	struct _key_settings	*next;
} KTKeySettings;

typedef struct {
	KTKeySettings	*head,	/* The first element in the list */
			*tail;	/* The last element in the list */
} KTKeySettingsList;


extern KTPluginList plugin_list; /* Global variable in main.c */

/******* Functions in config.c *******/
extern void read_configuration (KTKeySettingsList *key_list, KTKeyboardName *keyboard_name);
extern void read_preferences (KTPreferences *preferences);
extern void get_current_keyboard (KTKeyboardName *keyboard_name);

/******* Functions in error.c *******/
extern void KTError (char *msg, char *str);

/******* Functions in memory.c *******/
extern void *keytouch_malloc (size_t size);

/******* Functions in key_settings.c *******/
extern KTKeySettings *find_key_settings (char *key_name, KTKeySettingsList *key_list);
extern KTKeySettings *key_settings_of_keycode (unsigned int keycode, KTKeySettingsList *key_list);
extern void clear_key_settings_list (KTKeySettingsList *key_list);

/******* Functions in grab_keys.c *******/
extern void grab_keys (KTKeySettingsList *key_list, Display *display);

/******* Functions in handle_key.c *******/
extern void handle_key (KTKeySettings *key, KTPreferences *preferences, Display *display);
