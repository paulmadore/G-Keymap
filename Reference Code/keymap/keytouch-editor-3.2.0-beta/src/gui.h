/*---------------------------------------------------------------------------------
Name               : gui.h
Author             : Marvin Raaijmakers
Description        : Contains macros and structures for accessing GUI widgets
Date of last change: 22-Aug-2007
                     19-Aug-2007 Created this file

    Copyright (C) 2007 Marvin Raaijmakers

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
#ifndef INCLUDE_GUI
#define INCLUDE_GUI

#include <gtk/gtk.h>

/* Macro's for accessing objects of the GUI: */
#define GUI_main_window() (main_window_gui.main_window)
#define GUI_scancode_event_descr_label() (main_window_gui.scancode_event_descr_label)
#define GUI_manufacturer_entry() (main_window_gui.manufacturer_entry)
#define GUI_model_entry() (main_window_gui.model_entry)
#define GUI_key_name_entry() (main_window_gui.key_name_entry)
#define GUI_scancode_entry() (main_window_gui.scancode_entry)
#define GUI_usbcode_entry() (main_window_gui.usbcode_entry)
#define GUI_program_entry() (main_window_gui.program_entry)
#define GUI_plugin_function_comboentry() (main_window_gui.plugin_function_comboentry)
#define GUI_plugin_comboentry() (main_window_gui.plugin_comboentry)
#define GUI_program_radiobutton() (main_window_gui.program_radiobutton)
#define GUI_special_action_radiobutton() (main_window_gui.special_action_radiobutton)
#define GUI_key_treeview() (main_window_gui.key_treeview)
#define GUI_keycode_combobox() (main_window_gui.keycode_combobox)
#define GUI_usbcode_change_button() (main_window_gui.usbcode_change_button)
#define GUI_scancode_change_button() (main_window_gui.scancode_change_button)

/*
 * This structure contains pointers to the objects in the main
 * windows GUI used by the code in callbacks.c and setdata.c:
 */
typedef struct {
	GtkWidget *main_window;
	GtkLabel *scancode_event_descr_label;
	GtkEntry *manufacturer_entry,
	         *model_entry,
	         *key_name_entry,
	         *scancode_entry,
	         *usbcode_entry,
	         *program_entry,
	         *plugin_function_comboentry,
	         *plugin_comboentry;
	GtkRadioButton *program_radiobutton,
	               *special_action_radiobutton;
	GtkTreeView *key_treeview;
	GtkComboBox *keycode_combobox;
	GtkButton *usbcode_change_button,
	          *scancode_change_button;
} MainWindowGUI_type;

extern MainWindowGUI_type main_window_gui;

#endif
