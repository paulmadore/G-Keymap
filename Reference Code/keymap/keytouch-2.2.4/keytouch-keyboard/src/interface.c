/*
 * DO NOT EDIT THIS FILE - it is generated by Glade.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#define GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
  g_object_set_data (G_OBJECT (component), name, widget)

GtkWidget*
create_keyboard_dialog (void)
{
  GtkWidget *keyboard_dialog;
  GdkPixbuf *keyboard_dialog_icon_pixbuf;
  GtkWidget *dialog_vbox;
  GtkWidget *keyboard_vbox;
  GtkWidget *current_keyboard_frame;
  GtkWidget *current_keyboard_table;
  GtkWidget *manufacturer_label;
  GtkWidget *model_label;
  GtkWidget *manufacturer_entry;
  GtkWidget *model_entry;
  GtkWidget *current_keyboard_label;
  GtkWidget *available_keyboards_frame;
  GtkWidget *change_keyboard_vbox;
  GtkWidget *keyboard_treeview_scrolledwindow;
  GtkWidget *keyboard_treeview;
  GtkWidget *keyboard_import_hbuttonbox;
  GtkWidget *keyboard_import_button;
  GtkWidget *alignment3;
  GtkWidget *hbox4;
  GtkWidget *image5;
  GtkWidget *label34;
  GtkWidget *available_keyboards_label;
  GtkWidget *dialog_action_area;
  GtkWidget *cancel_button;
  GtkWidget *ok_button;

  keyboard_dialog = gtk_dialog_new ();
  gtk_window_set_title (GTK_WINDOW (keyboard_dialog), _("keyTouch - change keyboard"));
  gtk_window_set_position (GTK_WINDOW (keyboard_dialog), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size (GTK_WINDOW (keyboard_dialog), 450, 350);
  keyboard_dialog_icon_pixbuf = create_pixbuf ("icon.png");
  if (keyboard_dialog_icon_pixbuf)
    {
      gtk_window_set_icon (GTK_WINDOW (keyboard_dialog), keyboard_dialog_icon_pixbuf);
      gdk_pixbuf_unref (keyboard_dialog_icon_pixbuf);
    }
  gtk_window_set_type_hint (GTK_WINDOW (keyboard_dialog), GDK_WINDOW_TYPE_HINT_DIALOG);

  dialog_vbox = GTK_DIALOG (keyboard_dialog)->vbox;
  gtk_widget_show (dialog_vbox);

  keyboard_vbox = gtk_vbox_new (FALSE, 2);
  gtk_widget_show (keyboard_vbox);
  gtk_box_pack_start (GTK_BOX (dialog_vbox), keyboard_vbox, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (keyboard_vbox), 6);

  current_keyboard_frame = gtk_frame_new (NULL);
  gtk_widget_show (current_keyboard_frame);
  gtk_box_pack_start (GTK_BOX (keyboard_vbox), current_keyboard_frame, FALSE, TRUE, 0);

  current_keyboard_table = gtk_table_new (2, 2, FALSE);
  gtk_widget_show (current_keyboard_table);
  gtk_container_add (GTK_CONTAINER (current_keyboard_frame), current_keyboard_table);
  gtk_container_set_border_width (GTK_CONTAINER (current_keyboard_table), 7);
  gtk_table_set_row_spacings (GTK_TABLE (current_keyboard_table), 3);
  gtk_table_set_col_spacings (GTK_TABLE (current_keyboard_table), 6);

  manufacturer_label = gtk_label_new (_("Manufacturer:"));
  gtk_widget_show (manufacturer_label);
  gtk_table_attach (GTK_TABLE (current_keyboard_table), manufacturer_label, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (manufacturer_label), 0, 0.5);

  model_label = gtk_label_new (_("Model:"));
  gtk_widget_show (model_label);
  gtk_table_attach (GTK_TABLE (current_keyboard_table), model_label, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (model_label), 0, 0.5);

  manufacturer_entry = gtk_entry_new ();
  gtk_widget_show (manufacturer_entry);
  gtk_table_attach (GTK_TABLE (current_keyboard_table), manufacturer_entry, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_editable_set_editable (GTK_EDITABLE (manufacturer_entry), FALSE);

  model_entry = gtk_entry_new ();
  gtk_widget_show (model_entry);
  gtk_table_attach (GTK_TABLE (current_keyboard_table), model_entry, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_editable_set_editable (GTK_EDITABLE (model_entry), FALSE);

  current_keyboard_label = gtk_label_new (_("Current keyboard"));
  gtk_widget_show (current_keyboard_label);
  gtk_frame_set_label_widget (GTK_FRAME (current_keyboard_frame), current_keyboard_label);
  gtk_label_set_use_markup (GTK_LABEL (current_keyboard_label), TRUE);

  available_keyboards_frame = gtk_frame_new (NULL);
  gtk_widget_show (available_keyboards_frame);
  gtk_box_pack_start (GTK_BOX (keyboard_vbox), available_keyboards_frame, TRUE, TRUE, 0);

  change_keyboard_vbox = gtk_vbox_new (FALSE, 5);
  gtk_widget_show (change_keyboard_vbox);
  gtk_container_add (GTK_CONTAINER (available_keyboards_frame), change_keyboard_vbox);
  gtk_container_set_border_width (GTK_CONTAINER (change_keyboard_vbox), 7);

  keyboard_treeview_scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (keyboard_treeview_scrolledwindow);
  gtk_box_pack_start (GTK_BOX (change_keyboard_vbox), keyboard_treeview_scrolledwindow, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (keyboard_treeview_scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (keyboard_treeview_scrolledwindow), GTK_SHADOW_ETCHED_IN);

  keyboard_treeview = gtk_tree_view_new ();
  gtk_widget_show (keyboard_treeview);
  gtk_container_add (GTK_CONTAINER (keyboard_treeview_scrolledwindow), keyboard_treeview);
  gtk_tree_view_set_reorderable (GTK_TREE_VIEW (keyboard_treeview), TRUE);

  keyboard_import_hbuttonbox = gtk_hbutton_box_new ();
  gtk_widget_show (keyboard_import_hbuttonbox);
  gtk_box_pack_start (GTK_BOX (change_keyboard_vbox), keyboard_import_hbuttonbox, FALSE, TRUE, 0);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (keyboard_import_hbuttonbox), GTK_BUTTONBOX_END);

  keyboard_import_button = gtk_button_new ();
  gtk_widget_show (keyboard_import_button);
  gtk_container_add (GTK_CONTAINER (keyboard_import_hbuttonbox), keyboard_import_button);
  GTK_WIDGET_SET_FLAGS (keyboard_import_button, GTK_CAN_DEFAULT);

  alignment3 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment3);
  gtk_container_add (GTK_CONTAINER (keyboard_import_button), alignment3);

  hbox4 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox4);
  gtk_container_add (GTK_CONTAINER (alignment3), hbox4);

  image5 = gtk_image_new_from_stock ("gtk-open", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image5);
  gtk_box_pack_start (GTK_BOX (hbox4), image5, FALSE, FALSE, 0);

  label34 = gtk_label_new_with_mnemonic (_("_Import..."));
  gtk_widget_show (label34);
  gtk_box_pack_start (GTK_BOX (hbox4), label34, FALSE, FALSE, 0);

  available_keyboards_label = gtk_label_new (_("Available keyboards"));
  gtk_widget_show (available_keyboards_label);
  gtk_frame_set_label_widget (GTK_FRAME (available_keyboards_frame), available_keyboards_label);
  gtk_label_set_use_markup (GTK_LABEL (available_keyboards_label), TRUE);

  dialog_action_area = GTK_DIALOG (keyboard_dialog)->action_area;
  gtk_widget_show (dialog_action_area);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area), GTK_BUTTONBOX_END);

  cancel_button = gtk_button_new_from_stock ("gtk-cancel");
  gtk_widget_show (cancel_button);
  gtk_dialog_add_action_widget (GTK_DIALOG (keyboard_dialog), cancel_button, GTK_RESPONSE_CANCEL);
  GTK_WIDGET_SET_FLAGS (cancel_button, GTK_CAN_DEFAULT);

  ok_button = gtk_button_new_from_stock ("gtk-ok");
  gtk_widget_show (ok_button);
  gtk_dialog_add_action_widget (GTK_DIALOG (keyboard_dialog), ok_button, GTK_RESPONSE_OK);
  GTK_WIDGET_SET_FLAGS (ok_button, GTK_CAN_DEFAULT);

  g_signal_connect ((gpointer) keyboard_dialog, "delete_event",
                    G_CALLBACK (on_keyboard_dialog_delete_event),
                    NULL);
  g_signal_connect ((gpointer) keyboard_import_button, "clicked",
                    G_CALLBACK (on_keyboard_import_button_clicked),
                    NULL);
  g_signal_connect ((gpointer) cancel_button, "clicked",
                    G_CALLBACK (on_cancel_button_clicked),
                    NULL);
  g_signal_connect ((gpointer) ok_button, "clicked",
                    G_CALLBACK (on_ok_button_clicked),
                    NULL);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (keyboard_dialog, keyboard_dialog, "keyboard_dialog");
  GLADE_HOOKUP_OBJECT_NO_REF (keyboard_dialog, dialog_vbox, "dialog_vbox");
  GLADE_HOOKUP_OBJECT (keyboard_dialog, keyboard_vbox, "keyboard_vbox");
  GLADE_HOOKUP_OBJECT (keyboard_dialog, current_keyboard_frame, "current_keyboard_frame");
  GLADE_HOOKUP_OBJECT (keyboard_dialog, current_keyboard_table, "current_keyboard_table");
  GLADE_HOOKUP_OBJECT (keyboard_dialog, manufacturer_label, "manufacturer_label");
  GLADE_HOOKUP_OBJECT (keyboard_dialog, model_label, "model_label");
  GLADE_HOOKUP_OBJECT (keyboard_dialog, manufacturer_entry, "manufacturer_entry");
  GLADE_HOOKUP_OBJECT (keyboard_dialog, model_entry, "model_entry");
  GLADE_HOOKUP_OBJECT (keyboard_dialog, current_keyboard_label, "current_keyboard_label");
  GLADE_HOOKUP_OBJECT (keyboard_dialog, available_keyboards_frame, "available_keyboards_frame");
  GLADE_HOOKUP_OBJECT (keyboard_dialog, change_keyboard_vbox, "change_keyboard_vbox");
  GLADE_HOOKUP_OBJECT (keyboard_dialog, keyboard_treeview_scrolledwindow, "keyboard_treeview_scrolledwindow");
  GLADE_HOOKUP_OBJECT (keyboard_dialog, keyboard_treeview, "keyboard_treeview");
  GLADE_HOOKUP_OBJECT (keyboard_dialog, keyboard_import_hbuttonbox, "keyboard_import_hbuttonbox");
  GLADE_HOOKUP_OBJECT (keyboard_dialog, keyboard_import_button, "keyboard_import_button");
  GLADE_HOOKUP_OBJECT (keyboard_dialog, alignment3, "alignment3");
  GLADE_HOOKUP_OBJECT (keyboard_dialog, hbox4, "hbox4");
  GLADE_HOOKUP_OBJECT (keyboard_dialog, image5, "image5");
  GLADE_HOOKUP_OBJECT (keyboard_dialog, label34, "label34");
  GLADE_HOOKUP_OBJECT (keyboard_dialog, available_keyboards_label, "available_keyboards_label");
  GLADE_HOOKUP_OBJECT_NO_REF (keyboard_dialog, dialog_action_area, "dialog_action_area");
  GLADE_HOOKUP_OBJECT (keyboard_dialog, cancel_button, "cancel_button");
  GLADE_HOOKUP_OBJECT (keyboard_dialog, ok_button, "ok_button");

  return keyboard_dialog;
}

