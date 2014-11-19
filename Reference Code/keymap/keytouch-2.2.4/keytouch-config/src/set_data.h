/*---------------------------------------------------------------------------------
Name               : set_data.h
Author             : Marvin Raaijmakers
Description        : Headerfile for set_data.c
Date of last change: 30-Apr-2006
History            :
                     30-Apr-2006 Updated prototypes

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

#include <gtk/gtk.h>

#include <keytouch.h>
#include <gui.h>

extern void reload_plugin_function_list (	GtkList			*plugin_function_list_widget,
						GtkObject		*plugin_function_combo_entry,
						gpointer		entry_changed_handler,
						KTPluginInfoEntry	*plugin );
extern void entry_set_text (GtkWidget *this_widget, char *entry_name, char *text);
extern char *entry_get_text (GtkWidget *this_widget, char *entry_name);
extern void label_set_text (GtkWidget *this_widget, char *label_name, gchar *text);
extern void fill_plugin_info_dialog (GtkWidget *this_widget, KTPluginInfoEntry *plugin);
extern void reload_plugin_treeview (GtkTreeView *treeview, KTPluginInfoList *plugin_list);
extern void msg_box (gchar *message, GtkMessageType msg_type);
extern char *get_filename_from_user (char *title, GtkWindow *parent_window);
extern void reload_plugin_list (GtkList			*plugin_combo_list,
				GtkObject		*plugin_combo_entry,
				gpointer		entry_changed_handler,
				KTPluginInfoList	*plugin_list);
extern void reload_key_treeview (GtkTreeView *treeview, KTKeySettingsList *key_list);
extern void init_main_window (	KTKeyboardName		*current_keyboard_name,
				KTKeySettingsList	*key_list,
				KTPluginInfoList	*plugin_list,
				KTPreferences		*preferences,
				KTGUI_type		*gui );

