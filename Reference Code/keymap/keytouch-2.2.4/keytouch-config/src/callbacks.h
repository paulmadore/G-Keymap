/*---------------------------------------------------------------------------------
Name               : callback.h
Author             : Marvin Raaijmakers
Description        : Contains the prototypes for the callback functions in
                     callbacks.c
Date of last change: 18-Aug-2005

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
#include <gtk/gtk.h>


gboolean
on_main_window_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_default_radiobutton_toggled         (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_program_radiobutton_toggled         (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_program_entry_changed               (GtkEditable     *editable,
                                        gpointer         user_data);

gboolean
on_program_entry_focus_out_event       (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data);

void
on_plugin_combo_entry_changed          (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_plugin_function_combo_entry_changed (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_special_action_radiobutton_toggled  (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_change_button_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_plugin_download_button_clicked      (GtkButton       *button,
                                        gpointer         user_data);

void
on_plugin_info_button_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_plugin_remove_button_clicked        (GtkButton       *button,
                                        gpointer         user_data);

void
on_plugin_import_button_clicked        (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_documents_entry_focus_out_event     (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data);

gboolean
on_home_page_entry_focus_out_event     (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data);

gboolean
on_search_page_entry_focus_out_event   (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data);

gboolean
on_browser_entry_focus_out_event       (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data);

gboolean
on_email_prog_entry_focus_out_event    (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data);

gboolean
on_chat_entry_focus_out_event          (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data);

void
on_help_button_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_ok_button_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_apply_button_clicked                (GtkButton       *button,
                                        gpointer         user_data);

void
on_cancel_button_clicked               (GtkButton       *button,
                                        gpointer         user_data);

void
on_file_cancel_button_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_file_open_button_clicked            (GtkButton       *button,
                                        gpointer         user_data);

void
on_plugin_info_ok_button_clicked       (GtkButton       *button,
                                        gpointer         user_data);

void
on_key_treeview_row_activated          (GtkTreeView     *treeview,
                                        GtkTreePath     *path,
                                        GtkTreeViewColumn *column,
                                        gpointer         user_data);

gboolean
on_key_treeview_selection (	GtkTreeSelection	*selection,
				GtkTreeModel		*model,
				GtkTreePath		*path,
				gboolean		path_currently_selected,
				gpointer		userdata                    );

void
on_documents_entry_changed             (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_home_page_entry_changed             (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_search_page_entry_changed           (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_browser_entry_changed               (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_email_prog_entry_changed            (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_chat_entry_changed                  (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_plugin_info_dialog_show             (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_choose_program_button_clicked       (GtkButton       *button,
                                        gpointer         user_data);

void
on_email_prog_choose_button_clicked    (GtkButton       *button,
                                        gpointer         user_data);

void
on_chat_choose_button_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_browser_choose_button_clicked       (GtkButton       *button,
                                        gpointer         user_data);
