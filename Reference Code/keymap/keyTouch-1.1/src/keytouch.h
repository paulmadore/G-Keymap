/*-------------------------------------------------------------------------------
Name               : keytouch.h
Author             : Marvin Raaijmakers
Description        : keyTouch header file
Date of last change: 11-Dec-2004
History            :
                     1-July-2004   Added filename to the arguments of
                                   keyboard_import()
                     2-July-2004   Changed ERROR and FATAL_ERROR
                     3-July-2004   Changed ERROR and FATAL_ERROR
                     5-July-2004   Created typedef PREFERENCES
                     6-July-2004   Added FERR_INVALID_CONFIG_FILE to FATAL_ERROR
                     7-July-2004   Fixed bug of typedef PREFERENCES: increased the
                                   size of the element with 1
                     19-Nov-2004   Created types: KEYBOARD_MODEL,
                                   KEYBOARD_MANUFACTURE, KEYBOARD_CONFIG
                     11-Dec-2004   Created types: PLUGIN, PLUGIN_LIST
                     29-Dec-2004   Changed:
                                   - MAX_KEY_SYM_SIZE from 20 to 40
                                   - MAX_KEY_NAME_SIZE from 20 to 40

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
#define EQUAL	(0)
#define FALSE	(0)
#define TRUE	(1)
#define R_ERROR	(2)

#define VERSION ("1.1.0")

#define MAX_NAME_SIZE		(50)
#define MAX_KEY_SYM_SIZE	(40)
#define MAX_KEY_NAME_SIZE	(40)
#define MAX_PLUGIN_NAME_SIZE	(40)
#define MAX_PROG_SIZE		(299)
#define MAX_FILE_SIZE		(299)
#define MAX_LINE_SIZE		(299)

#define URL_DOWNLOAD_KEYBOARDS	("http://keytouch.sourceforge.net/dl-keyboards.html")
#define URL_DOWNLOAD_PLUGINS	("http://keytouch.sourceforge.net/dl-plugins.html")


typedef struct {
	char	manufacture [MAX_NAME_SIZE+1],
		model [MAX_NAME_SIZE+1];
} KEYBOARD_NAME;

typedef struct _keyboard_model {
	char			model_name [MAX_NAME_SIZE+1];
	struct	_keyboard_model	*next;
} KEYBOARD_MODEL;

typedef struct {
	KEYBOARD_MODEL	*head,
			*tail;
} KEYBOARD_MODEL_LIST;


typedef struct _keyboard_manufacture {
	char				manufacture_name [MAX_NAME_SIZE+1];
	KEYBOARD_MODEL_LIST		model_list;
	struct	_keyboard_manufacture	*next;
} KEYBOARD_MANUFACTURE;

typedef struct {
	KEYBOARD_MANUFACTURE	*manufacture_list_head,
				*manufacture_list_tail;
} KEYBOARD_LIST;

typedef struct _key_settings {
	char	key_sym [MAX_KEY_SYM_SIZE+1],
		key_name [MAX_KEY_NAME_SIZE+1],
		program_default [MAX_PROG_SIZE+1],
		program_user [MAX_PROG_SIZE+1];
	struct	_key_settings *next;
} KEY_SETTINGS;

typedef struct {
	KEY_SETTINGS	*head,
			*tail;
} KEY_SETTINGS_LIST;

typedef struct {
	KEY_SETTINGS_LIST	key_settings_list;
	KEYBOARD_NAME		keyboard_name;
} KEYBOARD_CONFIG;

typedef struct _xmodmap {
	int	hardware_code;
	char	key_sym [MAX_KEY_SYM_SIZE+1];
	struct	_xmodmap *next;
} XMODMAP;

typedef struct _xmodmap_list {
	XMODMAP	*head,
		*tail;
} XMODMAP_LIST;

typedef struct _plugin {
	char	file [MAX_FILE_SIZE+1],
		name [MAX_PLUGIN_NAME_SIZE+1];
	struct	_plugin *next;
} PLUGIN;

typedef struct {
	PLUGIN	*head,
		*tail;
} PLUGIN_LIST;

typedef struct {
	char	documents_dir[MAX_FILE_SIZE+1],
		home_page[MAX_FILE_SIZE+1],
		search_page[MAX_FILE_SIZE+1],
		browser[MAX_PROG_SIZE+1],
		email_program[MAX_PROG_SIZE+1],
		messenger[MAX_PROG_SIZE+1];
} PREFERENCES;

typedef enum {
	ERR_KEY_NEXISTS = 0,
	ERR_KEYBOARD_NEXISTS,
	ERR_IMPORT_FILE,
	ERR_INVALID_IMPORT_FILE,
	ERR_KEYBOARD_CONFIG,
	ERR_INVALID_KEYBOARD_FILE,
	ERR_PLUGIN_R,
	ERR_PLUGIN_W,
	ERR_NO_PLUGIN,
	ERR_INVALID_PLUGIN,
	NUM_ERROR
} ERROR;

typedef enum {
	FERR_MALLOC = 0,
	FERR_NO_KEYBOARDS,
	FERR_CONFIG_R,
	FERR_CONFIG_W,
	FERR_XMODMAP,
	FERR_XBINDKEYS,
	FERR_OUTPUT_KEYBOARD_FILE,
	FERR_INVALID_PLUGIN_FILE,
	FERR_PLUGIN_FILE_W,
	FERR_INVALID_CONFIG_FILE,
	NUM_FERROR
} FATAL_ERROR;

/*************** Global variables ***************/
extern KEYBOARD_CONFIG	current_keyboard_config;
extern KEYBOARD_LIST	keyboard_list;
extern XMODMAP_LIST	xmodmap_list;
extern PLUGIN_LIST	plugin_list;
extern PREFERENCES	preferences;

/************ Functions of config.c ************/
extern int  readline (FILE *file, char *line, int max_line_size);
extern void read_config (KEYBOARD_CONFIG *current_keyboard_config, KEYBOARD_LIST *keyboard_list, PREFERENCES *preferences);
extern void save_settings (KEYBOARD_CONFIG *current_keyboard_config, KEYBOARD_LIST *keyboard_list,
	XMODMAP_LIST *xmodmap_list, PLUGIN_LIST *plugin_list, PREFERENCES *preferences);
extern void read_plugin_list (PLUGIN_LIST *plugin_list);

/********* Functions of handle_error.c *********/
extern void handle_error (ERROR error, char *info);
extern void handle_fatal_error (FATAL_ERROR error);

/*********** Functions of keyboard.c ***********/
extern void keyboard_change (char *manufacture_name, char *model_name, KEYBOARD_CONFIG *current_keyboard_config,
	XMODMAP_LIST *xmodmap_list);
extern void keyboard_import (char *filename, KEYBOARD_LIST *keyboard_list);
extern void create_keyboard_filename (char *filename, int size, char manufacture[MAX_NAME_SIZE], char model[MAX_NAME_SIZE]);

/************* Functions of keys.c *************/
extern KEY_SETTINGS *find_key (KEY_SETTINGS *key_settings, char *key_name);
extern char change_key_settings (KEY_SETTINGS_LIST *key_settings_list, XMODMAP_LIST *xmodmap_list, KEYBOARD_NAME *keyboard_name);

/************* Functions of menu.c *************/
extern void main_menu (KEYBOARD_NAME *current_keyboard, KEY_SETTINGS **key_settings, KEYBOARD_LIST *keyboard_list);

/************* Functions of print.c ************/
extern void print_main_help ();
extern void print_keys_help ();
extern void print_keyboard_help ();
extern void print_about ();
extern void print_current_keyboard (KEYBOARD_NAME *current_keyboard);
extern void print_key_list (KEY_SETTINGS *key_settings, KEYBOARD_NAME *current_keyboard);
extern void print_keyboard_list (KEYBOARD_LIST *keyboard_list);

/************ Functions of getstr.c ************/
extern char getstr (int size, char string[], char until_space);

/************ Functions of plugin.c ************/
extern void plugin_import (char *filename, PLUGIN_LIST *plugin_list);

/********* Functions of run_browser.c **********/
extern void run_browser (char browser[MAX_PROG_SIZE], char url[MAX_FILE_SIZE]);
