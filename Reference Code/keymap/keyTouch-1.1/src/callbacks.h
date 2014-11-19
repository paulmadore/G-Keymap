#include <gtk/gtk.h>


void
on_key_list_selection_changed          (GtkList         *list,
                                        gpointer         user_data);

void
on_default_radiobutton_toggled         (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_program_radiobutton_toggled         (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_special_action_radiobutton_toggled  (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_program_entry_changed               (GtkEditable     *editable,
                                        gpointer         user_data);

gboolean
on_program_entry_focus_out_event       (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data);

void
on_special_action_combo_entry_changed  (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_change_button_clicked               (GtkButton       *button,
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
on_messenger_entry_focus_out_event     (GtkWidget       *widget,
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
on_keyboard_import_button_clicked      (GtkButton       *button,
                                        gpointer         user_data);


void
on_special_action_combo_entry_changed  (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_key_treeview_row_activated          (GtkTreeView     *treeview,
                                        GtkTreePath     *path,
                                        GtkTreeViewColumn *column,
                                        gpointer         user_data);

gboolean
on_key_treeview_select_cursor_row      (GtkTreeView     *treeview,
                                        gboolean         start_editing,
                                        gpointer         user_data);
gboolean
on_key_treeview_selection (	GtkTreeSelection	*selection,
				GtkTreeModel		*model,
				GtkTreePath		*path,
				gboolean		path_currently_selected,
				gpointer		userdata                    );

gboolean
on_main_window_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_keyboard_dialog_show                (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_fileselection_window_show           (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_keyboard_cancel_button_clicked      (GtkButton       *button,
                                        gpointer         user_data);

void
on_keyboard_ok_button_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_file_ok_button_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
on_file_cancel_button_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_keyboard_download_button_clicked    (GtkButton       *button,
                                        gpointer         user_data);

void
on_plugin_download_button_clicked      (GtkButton       *button,
                                        gpointer         user_data);
