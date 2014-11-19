#include <gtk/gtk.h>


gboolean
on_selektkb_window_delete_event        (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_selectkb_ok_button_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_selectkb_cancel_button_clicked      (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_main_window_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_new1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_save1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_save_as1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_quit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_remove_key_button_clicked           (GtkButton       *button,
                                        gpointer         user_data);

void
on_new_key_button_clicked              (GtkButton       *button,
                                        gpointer         user_data);

void
on_key_name_entry_editing_done         (GtkCellEditable *celleditable,
                                        gpointer         user_data);

void
on_scancode_change_button_clicked      (GtkButton       *button,
                                        gpointer         user_data);

void
on_keycode_comboboxentry_editing_done  (GtkCellEditable *celleditable,
                                        gpointer         user_data);

void
on_program_radiobutton_group_changed   (GtkRadioButton  *radiobutton,
                                        gpointer         user_data);

void
on_program_entry_editing_done          (GtkCellEditable *celleditable,
                                        gpointer         user_data);

void
on_plugin_comboboxentry_editing_done   (GtkCellEditable *celleditable,
                                        gpointer         user_data);

void
on_plugin_function_comboboxentry_editing_done
                                        (GtkCellEditable *celleditable,
                                        gpointer         user_data);

void
on_change_keyboard_menuitem_activate   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
on_selectkb_window_delete_event        (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_selectkb_window_show                (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_get_scancode_cancel_button_clicked  (GtkButton       *button,
                                        gpointer         user_data);

void
on_get_scancode_dialog_show            (GtkWidget       *widget,
                                        gpointer         user_data);

void
on_get_scancode_dialog_close           (GtkDialog       *dialog,
                                        gpointer         user_data);

gboolean
on_get_scancode_dialog_delete_event    (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_plugin_combobox_changed             (GtkComboBox     *combobox,
                                        gpointer         user_data);

void
on_plugin_function_combobox_changed    (GtkComboBox     *combobox,
                                        gpointer         user_data);

void
on_keycode_combobox_changed            (GtkComboBox     *combobox,
                                        gpointer         user_data);

gboolean
on_new_key_name_dialog_delete_event    (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_new_key_cancel_button_clicked       (GtkButton       *button,
                                        gpointer         user_data);

void
on_new_key_okbutton_clicked            (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_key_treeview_selection (	GtkTreeSelection	*selection,
				GtkTreeModel		*model,
				GtkTreePath		*path,
				gboolean		path_currently_selected,
				gpointer		userdata );

gboolean
on_key_name_entry_leave_notify_event   (GtkWidget       *widget,
                                        GdkEventCrossing *event,
                                        gpointer         user_data);

void
on_key_name_entry_grab_notify          (GtkWidget       *widget,
                                        gboolean         was_grabbed,
                                        gpointer         user_data);

gboolean
on_key_name_entry_focus_out_event      (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data);

void
on_key_name_entry_changed              (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_program_entry_changed               (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_program_radiobutton_toggled         (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_plugin_comboentry_changed           (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_plugin_function_comboentry_changed  (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_special_action_radiobutton_toggled  (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

gboolean
on_contact_dialog_delete_event         (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_contact_dialog_cancelbutton_clicked (GtkButton       *button,
                                        gpointer         user_data);

void
on_contact_dialog_okbutton_clicked     (GtkButton       *button,
                                        gpointer         user_data);

void
on_manufacturer_model_entry_changed    (GtkEditable     *editable,
                                        gpointer         user_data);

void
on_usbcode_change_button_clicked       (GtkButton       *button,
                                        gpointer         user_data);
