/*---------------------------------------------------------------------------------
Name               : keytouch-init.h
Author             : Marvin Raaijmakers
Description        : The main header file of keytouch-init
Date of last change: 27-Aug-2005

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
#ifndef RAND_MAX
#	include <stdlib.h>
#endif

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


typedef struct _key_settings {
	unsigned int		scancode,	/* The scancode of the key that is sent by the keyboard */
				keycode;	/* The Linux kernel keycode of the key */
	struct _key_settings	*next;
} KTKeySettings;

typedef struct {
	KTKeySettings	*head,	/* The first element in the list */
			*tail;	/* The last element in the list */
} KTKeySettingsList;


/******* Functions in config.c *******/
extern void read_keyboard_file (KTKeySettingsList *key_list, KTKeyboardName *keyboard_name);
extern void get_current_keyboard (KTKeyboardName *keyboard_name);

/******* Functions in error.c *******/
extern void KTError (char *msg, char *str);

/******* Functions in memory.c *******/
extern void *keytouch_malloc (size_t size);

/******* Functions in setkeycodes.c *******/
extern void setkeycodes (KTKeySettingsList *key_list);
