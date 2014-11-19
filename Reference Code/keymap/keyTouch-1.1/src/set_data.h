/*-------------------------------------------------------------------------------
Name               : set_data.h
Author             : Marvin Raaijmakers
Description        : set_data header file
Date of last change: 7-July-2004
History            :
                     1-July-2004   Added allusions init_main_window() and
                                   reload_keyboard_list()
                     2-July-2004   Added allusion msg_box()
                     7-July-2004   Added allusion reload_preferences()

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

enum
{
	COL_KEYBOARD_MANUFACTURE,
	COL_KEYBOARD_MODEL,
	NUM_COLS_KEYBOARD_LIST
};

extern void init_main_window (KEYBOARD_CONFIG *current_keyboard_config, PLUGIN_LIST *plugin_list, PREFERENCES preferences);
extern void reload_key_treeview (GtkWidget *this_widget, KEYBOARD_CONFIG *current_keyboard_config);
extern void reload_preferences (GtkWidget *this_widget, PREFERENCES preferences);
extern void msg_box (char *message, GtkMessageType msg_type);
extern void reload_plugin_treeview (GtkWidget *this_widget, PLUGIN_LIST *plugin_list);
extern void reload_special_action_list (GtkWidget *this_widget, PLUGIN_LIST *plugin_list);
extern void reload_current_keyboard (GtkWidget *this_widget, KEYBOARD_CONFIG *current_keyboard_config);
extern void init_keyboard_treeview (GtkWidget *this_widget);
extern void reload_keyboard_treeview (GtkWidget *this_widget, KEYBOARD_LIST *keyboard_list);
/* Wait for this until GTK+2.4 is standard:
extern char *get_filename_from_user (char *title, GtkWindow *parent_window); */
