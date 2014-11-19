/*---------------------------------------------------------------------------------
Name               : gui.h
Author             : Marvin Raaijmakers
Description        : Contains macros and structures for accessing GUI widgets
Date of last change: 30-Apr-2006
                     30-Apr-2006 Created this file

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
#ifndef INCLUDE_GUI
#define INCLUDE_GUI

#include <gtk/gtk.h>

/* Macro's for accessing objects of the GUI: */
#define KTGUI_main_window()			(keytouch_gui.main_window)
#define KTGUI_plugin_treeview()			(keytouch_gui.plugin_treeview)
#define KTGUI_key_treeview()			(keytouch_gui.key_treeview)
#define KTGUI_key_name_label()			(keytouch_gui.key_name_label)
#define KTGUI_program_entry()			(keytouch_gui.program_entry)
#define KTGUI_default_entry()			(keytouch_gui.default_entry)
#define KTGUI_plugin_combo_entry()		(keytouch_gui.plugin_combo_entry)
#define KTGUI_plugin_function_combo_entry()	(keytouch_gui.plugin_function_combo_entry)
#define KTGUI_default_radiobutton()		(keytouch_gui.default_radiobutton)
#define KTGUI_program_radiobutton()		(keytouch_gui.program_radiobutton)
#define KTGUI_special_action_radiobutton()	(keytouch_gui.special_action_radiobutton)
#define KTGUI_plugin_combo_list()		(keytouch_gui.plugin_combo_list)
#define KTGUI_plugin_function_combo_list()	(keytouch_gui.plugin_function_combo_list)
#define KTGUI_email_prog_entry()		(keytouch_gui.email_prog_entry)
#define KTGUI_browser_entry()			(keytouch_gui.browser_entry)
#define KTGUI_chat_entry()			(keytouch_gui.chat_entry)

/*
 * This structure contains pointers to the objects in the GUI used by
 * the code in callbacks.c and setdata.c:
 */
typedef struct {
	GtkWidget	*main_window;
	GtkTreeView	*plugin_treeview,
			*key_treeview;
	GtkLabel	*key_name_label;
	GtkEntry	*program_entry,
			*default_entry,
			*plugin_combo_entry,
			*plugin_function_combo_entry,
			*documents_entry,
			*home_page_entry,
			*search_page_entry,
			*browser_entry,
			*email_prog_entry,
			*chat_entry;
	GtkRadioButton	*default_radiobutton,
			*program_radiobutton,
			*special_action_radiobutton;
	GtkList		*plugin_combo_list,
			*plugin_function_combo_list;
} KTGUI_type;

extern KTGUI_type keytouch_gui;

#endif
