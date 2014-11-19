/*---------------------------------------------------------------------------------
Name               : set_data.h
Author             : Marvin Raaijmakers
Description        : Header file for set_data.c
Date of last change: 15-Mar-2006
History            :

    Copyright (C) 2006 Marvin Raaijmakers

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
#ifndef _SET_DATA_H
#define _SET_DATA_H

#include <gtk/gtk.h>

#include <evdev.h>
#include <keytouch-editor.h>
#include <gui.h>

#define GTK_ALIGN_LEFT		(0)
#define GTK_ALIGN_CENTER	(0.5)

#define ONE_COLUMN	(1)
#define FIRST_COLUMN	(0) 
#define SORTID_FIRSTCOL	(0)

enum {
	COL_EVDEV_FILE_NAME,
	COL_EVDEV_DEVICE_NAME,
	COL_EVDEV_BUS,
	COL_EVDEV_STRUCT,
	NUM_COLS_EVDEV_TREEVIEW
};

extern void msg_box (gchar *message, GtkMessageType msg_type);
extern void gui_error_handler (const char *msg);
extern GtkWidget *show_selectkb_window (EVDEV_LIST *evdev_list, Boolean acpi_usable);
extern GtkWidget *show_main_window (MainWindowGUI_type *main_window_gui);
extern void clear_key_settings_frame (GtkWidget *this_widget);
extern void reload_plugin_function_list (GtkWidget *this_widget, KTPlugin *plugin);
extern void reload_plugin_list (GtkWidget *this_widget, const KTPlugin plugin_list[], int num_plugins);
extern void reload_key_treeview (KTKeyList *list, GtkWidget *this_widget);

#endif
