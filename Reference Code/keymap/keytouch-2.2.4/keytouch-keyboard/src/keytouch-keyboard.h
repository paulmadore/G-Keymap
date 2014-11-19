/*---------------------------------------------------------------------------------
Name               : keytouch-keyboard.h
Author             : Marvin Raaijmakers
Description        : The main header file of keytouch-keyboard
Date of last change: 22-Jun-2006
History            : 10-Jan-2006 Replaced get_keyboard_name() by read_keyboard()
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
#ifndef INCLUDE_KEYTOUCH_KEYBOARD
#define INCLUDE_KEYTOUCH_KEYBOARD

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

#include <time.h>

#define KEYTOUCH_CONF_DIR		SYSCONF_DIR "/keytouch"
#define KEYTOUCH_DATA_DIR		PACKAGE_DATA_DIR "/keytouch"
#define KEYBOARD_FILES_DIR		KEYTOUCH_DATA_DIR "/keyboards"
#define CURRENT_KEYBOARD_FILE		KEYTOUCH_CONF_DIR "/current_keyboard.xml"

#define kbname_model(_keyboard_name)		((_keyboard_name)->model)
#define kbname_manufacturer(_keyboard_name)	((_keyboard_name)->manufacturer)

#ifndef XmlGetContentType /* If <mxml.h> is not included */
typedef char Boolean;
#endif

typedef struct _KTKeyboardModel {
	char		*name;
	/* Contains the "last-change" date of the keyboard file: */
	struct tm	kbfile_last_change;
	struct _KTKeyboardModel *next;
} KTKeyboardModel;

typedef struct {
	KTKeyboardModel	*head,
			*tail;
} KTKeyboardModelList;

typedef struct _KTKeyboardManufacturer {
	char			*name;
	KTKeyboardModelList	model_list;
	struct _KTKeyboardManufacturer	*next;
} KTKeyboardManufacturer;

typedef struct {
	KTKeyboardManufacturer	*head,
				*tail;
} KTKeyboardList;

typedef struct {
	char	*manufacturer,
		*model;
} KTKeyboardName;


extern KTKeyboardList keyboard_list;


/******* Functions in keyboard.c *******/
extern void insert_keyboard (	KTKeyboardName	*keyboard_name,
				struct tm	kbfile_date,
				KTKeyboardList	*keyboard_list );
extern void copy_keyboard (char *file_name, KTKeyboardName *keyboard_name);
extern KTKeyboardModel *find_keyboard_model (	KTKeyboardName	*name,
						KTKeyboardList	*keyboard_list );

/******* Functions in config.c *******/
extern Boolean get_current_keyboard (KTKeyboardName *keyboard_name);
extern void read_keyboards (KTKeyboardList *keyboard_list);
extern Boolean read_keyboard (	char		*file_name,
				KTKeyboardName	*keyboard_name,
				struct tm	*kbfile_date );
extern Boolean write_current_keyboard (KTKeyboardName *keyboard_name);
extern Boolean old_keyboard_file (	KTKeyboardName	*keyboard_name,
					KTKeyboardList	*keyboard_list,
					struct tm	kbfile_date );

/******* Functions in memmory.c *******/
extern void *keytouch_malloc (size_t size);
extern char *keytouch_strdup (const char *s);

/******* Functions in error.c *******/
extern void KTError (char *msg, char *str);
extern void KTSetErrorMsg (char *msg, char *str);
extern char *KTGetErrorMsg (void);

#endif
