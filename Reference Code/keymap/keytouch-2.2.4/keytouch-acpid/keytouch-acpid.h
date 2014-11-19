/*---------------------------------------------------------------------------------
Name               : keytouch-apcid.h
Author             : Marvin Raaijmakers
Description        : The main headerfile of keytouchd
Date of last change: 03-Jan-2006
History            : 03-Jan-2006 ?

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

#define ACPI_SOCKETFILE	"/var/run/acpid.socket"
#define ACPI_MAX_ERRS	5

#define KEYTOUCH_CONF_DIR		SYSCONF_DIR "/keytouch"
#define KEYTOUCH_DATA_DIR		PACKAGE_DATA_DIR "/keytouch"
#define KEYBOARD_FILES_DIR		KEYTOUCH_DATA_DIR "/keyboards"
#define CURRENT_KEYBOARD_FILE		KEYTOUCH_CONF_DIR "/current_keyboard.xml"

#define TRUE	1
#define FALSE	0

#define EQUAL	0


#ifndef XmlGetContentType /* If <mxml.h> is not included */
	typedef char Boolean;
#endif


typedef struct {
	char	*manufacturer,
		*model;
} KTKeyboardName;

typedef struct _acpi_key {
	char			*event_descr;
	unsigned int		keycode;	/* The keycode that the key has in X */
	struct _acpi_key	*next;
} KTAcpiKey;

typedef struct {
	KTAcpiKey	*head,	/* The first element in the list */
			*tail;	/* The last element in the list */
} KTAcpiKeyList;


/******* Functions in config.c *******/
extern void read_configuration (KTAcpiKeyList *key_list, KTKeyboardName *keyboard_name);
extern void get_current_keyboard (KTKeyboardName *keyboard_name);

/******* Functions in error.c *******/
extern void KTError (char *msg, char *str);

/******* Functions in memory.c *******/
extern void *keytouch_malloc (size_t size);

/******* Functions in acpi_key.c *******/
extern unsigned int get_keycode (char *event_descr, KTAcpiKeyList *key_list);
extern void clear_acpi_key_list (KTAcpiKeyList *key_list);

