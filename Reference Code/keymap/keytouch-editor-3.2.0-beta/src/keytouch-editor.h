/*---------------------------------------------------------------------------------
Name               : keytouch-editor.h
Author             : Marvin Raaijmakers
Description        : Main header file of keytouch-editor
Date of last change: 22-Aug-2007
History            : 24-Jun-2007 Compiler warnings fixes

    Copyright (C) 2006-2007 Marvin Raaijmakers

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
#ifndef _KEYTOUH_EDITOR_H
#define _KEYTOUH_EDITOR_H

#ifdef XmlGetContentType
 #define _BOOL_DEF
#endif

#include <evdev.h>
#include <string_to_keycode.h>

#ifndef _BOOL_DEF
 #define _BOOL_DEF
 typedef int Boolean;
 #define TRUE	1
 #define FALSE	0
#endif

#define EQUAL	(0)

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


#define KBFILE_EMAIL_ADDRESS	"marvin.nospam@gmail.com"

typedef struct {
	char		*name;
	unsigned int	num_functions;
	char		**function;
} KTPlugin;

typedef enum {
	KTActionTypeProgram,
	KTActionTypePlugin,
	NUM_KTActionType
} KTActionType;

typedef struct {
	KTActionType type;
	char *command;
} KTActionProgram;

typedef struct {
	KTActionType type;
	char *plugin_name,
	     *function_name;
} KTActionPlugin;

typedef union {
	KTActionType	type;
	KTActionProgram	program;
	KTActionPlugin	plugin;
} KTAction;


typedef struct {
	int keycode, scancode;
} KTKeycodeScancode;

typedef struct _kt_key {
	char *name,
	     *acpi_event_descr;
	int scancode,  /* The scancode of the key that is sent by the keyboard (0 if not used) */
	    usagecode, /* The 'scancode' of the key, when connected via USB (0 if not used) */
	    keycode;   /* The Linux kernel keycode of the key */
	KTAction default_action;
	struct _kt_key *next;
} KTKey;

typedef struct {
	KTKey *head, *tail;
} KTKeyList;

typedef struct {
	char *name;
	KTAction action;
} KTKeyDefaults;


#define KTKeyList_head(_list) ((_list)->head)
#define KTKeyList_tail(_list) ((_list)->tail)
#define KTKey_get_name(_key) ((_key)->name)
/*#define KTKey_get_keycode_index(_key) ((_key)->keycode_index)
#define KTKey_set_keycode_index(_key, _index) ((_key)->keycode_index = (_index))*/
#define KTKey_get_keycode(_key) ((_key)->keycode)
#define KTKey_set_keycode(_key, _keycode) ((_key)->keycode = (_keycode))
#define KTKey_get_scancode(_key) ((_key)->scancode)
#define KTKey_get_usagecode(_key) ((_key)->usagecode)
#define KTKey_get_acpi_event_descr(_key) ((_key)->acpi_event_descr)
#define KTKey_get_action_type(_key) ((_key)->default_action.type)
#define KTKey_get_action_command(_key) ((_key)->default_action.program.command)
#define KTKey_get_plugin_name(_key) ((_key)->default_action.plugin.plugin_name)
#define KTKey_get_plugin_function_name(_key) ((_key)->default_action.plugin.function_name)

#define KTPlugin_get_name(_plugin) ((_plugin).name)

#define index2keycode_string(_i) (kernel_keycode_to_string( visible_keycodes[(_i)]] ))
#define keycode2string(_keycode) (kernel_keycode_to_string((_keycode)) ? : "")

#define NUM_PLUGINS 11

#define SYNTAX_VERSION	"1.2"		/* The syntax version of the file this program produces */
#define DATE_FORMAT	"%d-%m-%Y"
#define MAX_SCANCODE_LEN 64
#define MAX_USBCODE_LEN  64
#define MAX_DATE_LEN		99

#define ACPI_EVDEV_FILE_NAME	""
#define ACPI_DEVICE_NAME	"ACPI device"
#define ACPI_BUS_NAME		"ACPI"

/**** Global variables ****/
extern EVENT_DEVICE *current_evdev;
extern Boolean use_acpi_dev, acpi_usable;
extern const unsigned visible_keycodes[]; /* List of all available keycodes (in keycodes.c) in the GUI list */
extern const KTKeyDefaults key_defaults[KEY_MAX + 1]; /* The defaults for a keycode */
extern const KTPlugin plugin_list[NUM_PLUGINS];
extern EVDEV_LIST evdev_list;
extern KTKeyList key_list;
extern KTKey *selected_key;
extern Boolean file_modified; /* Indicates if the currently opend file was modified */
extern char *current_filename; /* The name if the currently opened file
                                * (= NULL if no name is defined yet) */
extern int usb_vendor_id, usb_product_id;
/* The hostnames of the SMTP servers that can be used for
 * sending the keyboard file: */
extern const char *smtp_hostnames[];

/******* Functions in error.c *******/
extern void KTError (char *msg, char *str);
extern void KTSetErrorHandler (void (*handler)(const char*));
extern void KTSetErrorMsg (char *msg, char *str);
extern char *KTGetErrorMsg (void);

/******* Functions in memory.c *******/
extern void *keytouch_malloc (size_t size);
extern char *keytouch_strdup (const char *s);

/******* Functions in get_scancode.c *******/
extern Boolean get_keycode_scancode (char *evdev_filename, char *msg, KTKeycodeScancode *keycode_scancode);
extern char *get_acpi_event_descr (char *msg);

/******* Functions in key_list.c *******/
extern void KTKeyList_init (KTKeyList *list);
extern void KTKeyList_clear (KTKeyList *list);
extern KTKey *KTKeyList_find_key (const KTKeyList *list, const char *name);
extern void KTKeyList_append (KTKeyList *list, KTKey *key);
extern void KTKeyList_remove_key (KTKeyList *list, KTKey *key);
extern Boolean KTKeyList_contains_usb_key (KTKeyList *list);
extern void KTKey_set_program (KTKey *key, const char *command);
extern void KTKey_set_plugin (KTKey *key, const char *plugin, const char *function);
extern void KTKey_set_name (KTKey *key, const char *name);
extern void KTKey_set_scancode (KTKey *key, int scancode);
extern void KTKey_set_usagecode (KTKey *key, int usagecode);
extern void KTKey_set_acpi_event_descr (KTKey *key, char *acpi_event_descr);
extern void KTKey_delete (KTKey *key);
extern void KTKey_free_default_action (KTKey *key);
extern KTKey *KTKey_new (const char *name, int scancode, int usagecode, int keycode, char *acpi_event_descr);

/******* Functions in keycodes.c *******/
extern unsigned int string2keycode (const char *keycode_str);
extern int string2keycode_index (const char *keycode_str);
extern int keycode2index (unsigned int keycode);

/******* Functions in config.c *******/
extern Boolean read_keyboard_file (KTKeyList *key_list,
                                   char      **manufacturer,
                                   char      **model,
                                   int       *usb_vendor_id,
                                   int       *usb_product_id,
                                   char      *file_name);
extern Boolean keyboard_file_is_valid (char *file_name);
extern Boolean write_keyboard_file (char *manufacturer, char *model, KTKeyList *key_list,
                                    int usb_vendor_id, int usb_product_id, char *author,
                                    char *filename);

/******* Functions in plugins.c *******/
extern KTPlugin *find_plugin (char *name, const KTPlugin list[], unsigned int list_size);

#endif
