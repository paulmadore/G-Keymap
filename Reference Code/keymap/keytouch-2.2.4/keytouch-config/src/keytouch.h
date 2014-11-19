/*---------------------------------------------------------------------------------
Name               : keytouch.h
Author             : Marvin Raaijmakers
Description        : The main headerfile for the keytouch configuration program
Date of last change: 21-Sep-2006
History            :
                     09-Feb-2006 Modified PLUGIN_DIR
                     23-Jan-2006 Added prototype for global _home_dir
                     17-Jan-2006 Changed VERSION to "2.1.0 beta"
                     02-Jan-2006 Added #include <config.h>

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
#ifndef INCLUDE_KEYTOUCH
#define INCLUDE_KEYTOUCH

#define EQUAL	(0)
#ifndef FALSE
# define FALSE	(0)
#endif
#ifndef TRUE
# define TRUE	(1)
#endif
#define R_ERROR	(2)

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#ifndef _
#  ifdef ENABLE_NLS
#    include <libintl.h>
#    undef _
#    define _(String) dgettext (PACKAGE, String)
#    define Q_(String) g_strip_context ((String), gettext (String))
#    ifdef gettext_noop
#      define N_(String) gettext_noop (String)
#    else
#      define N_(String) (String)
#    endif
#  else
#    define textdomain(String) (String)
#    define gettext(String) (String)
#    define dgettext(Domain,Message) (Message)
#    define dcgettext(Domain,Message,Type) (Message)
#    define bindtextdomain(Domain,Directory) (Domain)
#    define _(String) (String)
#    define Q_(String) g_strip_context ((String), (String))
#    define N_(String) (String)
#  endif
#endif

#ifndef XmlGetContentType /* If <mxml.h> is not included */
	typedef char Boolean;
#endif

#include <gnome-menus/gmenu-tree.h>


#define VERSION ("2.2.4")

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
#define MENU_SPEC_FILE			KEYTOUCH_DATA_DIR "/applications.menu"

#define DOC_DIR				PACKAGE_DATA_DIR "/" PACKAGE "/doc/html" /* For html documentation */

#define URL_DOWNLOAD_PLUGINS		"http://keytouch.sourceforge.net/dl-plugins.php"

#define kbname_model(_keyboard_name)		((_keyboard_name)->model)
#define kbname_manufacturer(_keyboard_name)	((_keyboard_name)->manufacturer)

#define PREF_DEFAULT_DOC_DIR		"~/Documents"
#define PREF_DEFAULT_HOME_PAGE		"http://keytouch.sf.net"
#define PREF_DEFAULT_SEARCH_PAGE	"http://www.google.com"
#define PREF_DEFAULT_BROWSER		""
#define PREF_DEFAULT_EMAIL_PROG		""
#define PREF_DEFAULT_CHAT_PROG		"gaim"


#include <plugin-info.h>

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
	KTActionType	type;
	char		*plugin_name,
			*function_name;
} KTActionPlugin;

typedef union {
	KTActionType	type;
	KTActionProgram	program;
	KTActionPlugin	plugin;
} KTAction;

typedef struct _key_settings {
	char			*key_name;	/* The name of the function key */
	KTAction		default_action;	/* The default action to perform when the key is pressed */
	KTAction		action;		/* The action to perform when the key is pressed */
	struct _key_settings	*next;
} KTKeySettings;

typedef struct {
	KTKeySettings	*head,	/* The first element in the list */
			*tail;	/* The last element in the list */
} KTKeySettingsList;


typedef struct {
	char	*documents_dir,
		*home_page,
		*search_page,
		*browser,
		*email_program,
		*chat_program;
} KTPreferences;


typedef enum {
	KTConfErr_NoError,
	KTConfErr_NoAccess,
	KTConfErr_NotExist,
	KTConfErr_KbFileInvalid
} KTConfErr;

/*************** Global variables ***************/
extern KTKeyboardName		current_keyboard_name;
extern KTKeySettingsList	key_list;
extern KTPreferences		preferences;
extern KTPluginInfoList		plugin_list;
extern const char		*_home_dir;
extern GMenuTree		*menu_tree;

/******* Functions in config.c *******/
extern KTConfErr read_configuration (KTKeySettingsList *key_list, KTKeyboardName *keyboard_name);
extern void read_preferences (KTPreferences *preferences);
extern Boolean get_current_keyboard (KTKeyboardName *keyboard_name);
extern void create_user_config_dir (void);
extern void write_configuration (KTKeySettingsList *key_list, KTKeyboardName *keyboard_name);
extern void write_preferences (KTPreferences *preferences);

/******* Functions in error.c *******/
extern void KTError (char *msg, char *str);
extern void KTSetErrorMsg (char *msg, char *str);
extern char *KTGetErrorMsg (void);

/******* Functions in memory.c *******/
extern void *keytouch_malloc (size_t size);
extern char *keytouch_strdup (const char *s);

/******* Functions in run_browser.c *******/
extern void run_browser (char *browser, char *url);

#endif
