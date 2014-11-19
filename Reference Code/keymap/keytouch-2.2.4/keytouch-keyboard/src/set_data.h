/*---------------------------------------------------------------------------------
Name               : set_data.h
Author             : Marvin Raaijmakers
Description        : Header file of set_data.c
Date of last change: 17-Jun-2006

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
#ifndef INCLUDE_SET_DATA
#define INCLUDE_SET_DATA

#include <gtk/gtk.h>

enum {
	COL_KEYBOARD_MANUFACTURER,
	COL_KEYBOARD_MODEL,
	NUM_COLS_KEYBOARD_LIST
};


extern char *get_filename_from_user (	char		*title,
					GtkWindow	*parent_window   );
extern void msg_box (	gchar		*message,
			GtkMessageType	msg_type       );
extern GtkWidget *init_keyboard_dialog (	KTKeyboardName		*current_keyboard_name,
						KTKeyboardList		*keyboard_list          );
extern void reload_keyboard_treeview (	GtkWidget	*this_widget,
					KTKeyboardList	*keyboard_list  );

#endif
