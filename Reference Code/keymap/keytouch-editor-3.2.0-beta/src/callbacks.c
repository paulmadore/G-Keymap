/*---------------------------------------------------------------------------------
Name               : callbacks.c
Author             : Marvin Raaijmakers
Description        : Handles callbacks of the user interface
Date of last change: 16-Mar-2008
History            : 16-Mar-2008 Reloads evdev_list() before calling
                                 show_selectkb_window()
                     22-Aug-2007 Added support for USB keys
                     24-Jun-2007 Compiler warnings fixes
                     30-Aug-2006 Added support for ACPI hotkeys
                     01-Jul-2006 Added syntax check call to save_keyboard_file()
                     29-Apr-2006 save_keyboard_file() asks for a confirmation
                                 before overwriting a file
                     08-Apr-2006 on_new_key_button_clicked() suggests a name for
                                 the new key to the user, if the key is known and
                                 fills in the default action for such key.

    Copyright (C) 2006-2008 Marvin Raaijmakers

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
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include <keytouch-editor.h>
#include <set_data.h>
#include <evdev.h>
#include <email.h>
#include <gui.h>

#define PRESS_KEY_MSG		(_("Press the extra function key."))
#define SCANCODE_LABEL_TEXT	(_("Scancode:"))
#define EVENT_DESCR_LABEL_TEXT	(_("Event description:"))


static void handle_changed_special_action (	GtkWidget	*special_action_radiobutton,
						GtkEntry	*plugin_combo_entry,
						GtkEntry	*plugin_function_combo_entry );
static Boolean ask_to_save (	GtkWidget	*this_widget,
				char		*filename );
static Boolean save_keyboard_file (	GtkWidget	*this_widget,
					char		**filename,
					KTKeyList	*key_list );



/********************************
 * Callbacks of selectkb_window *
 ********************************/


void
on_selectkb_window_show                (GtkWidget       *widget,
                                        gpointer         user_data)
{
	/* Block the interaction with the main window, so that
	 * the user can only use this window
	 */
	gtk_grab_add (GTK_WIDGET(widget));
}


void
on_selectkb_ok_button_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkTreeView *keyboard_treeview;
	GtkTreeSelection *tree_selection;
	GtkTreeModel *model;
	GtkTreeIter iter;
	char *event_descr;
	Boolean device_ok,
	        acpi_selected;
	EVENT_DEVICE *selected_evdev;
	KTKeycodeScancode keycode_scancode;

#define CHECK_DEVICE_MSG (_("Please press one of the extra function keys.\n" \
                            "Press \"Cancel\" and select another event device\n" \
                            "when this window does not disappear after\n" \
                            "pressing the key."))
	
	keyboard_treeview = GTK_TREE_VIEW (lookup_widget(GTK_WIDGET(button), "keyboard_treeview"));
	tree_selection = gtk_tree_view_get_selection (keyboard_treeview);
	device_ok = FALSE;
	/* If a row was selected instead of deselected */
	if (gtk_tree_selection_get_selected(tree_selection, &model, &iter))
	{
		/* Get the evdev structure of the selected keyboard */
		gtk_tree_model_get (model, &iter, COL_EVDEV_STRUCT, &selected_evdev, -1);
		acpi_selected = (selected_evdev == NULL);
		if (acpi_selected)
		{
			event_descr = get_acpi_event_descr (CHECK_DEVICE_MSG);
			device_ok = (event_descr != NULL);
			if (device_ok)
			{
				free (event_descr);
			}
		}
		else /* if an evdev has been selected */
		{
			device_ok = get_keycode_scancode (evdev_complete_file_name(selected_evdev), CHECK_DEVICE_MSG, &keycode_scancode);
		}
	}
	else
	{
		msg_box (_("Please select the event device for your keyboard."), GTK_MESSAGE_INFO);
	}
	if (device_ok)
	{
		/* Close the window */
		gtk_widget_destroy (gtk_widget_get_toplevel( GTK_WIDGET(button) ));
		if (GUI_main_window() == NULL) /* If there is no main window yet */
		{
			show_main_window (&main_window_gui);
		}
		use_acpi_dev = acpi_selected;
		if (use_acpi_dev)
		{
			current_evdev = NULL;
			gtk_widget_set_sensitive (GTK_WIDGET(GUI_scancode_change_button()), TRUE);
			gtk_widget_set_sensitive (GTK_WIDGET(GUI_usbcode_change_button()), FALSE);
		}
		else
		{
			current_evdev = selected_evdev;
			if (evdev_bus_id(current_evdev) == BUS_USB)
			{
				usb_vendor_id = current_evdev->vendor_id;
				usb_product_id = current_evdev->product_id;
			}
			gtk_widget_set_sensitive (GUI_scancode_change_button(), evdev_bus_id(current_evdev) != BUS_USB);
			gtk_widget_set_sensitive (GUI_usbcode_change_button(), evdev_bus_id(current_evdev) == BUS_USB);
		}
	}
}


void
on_selectkb_cancel_button_clicked      (GtkButton       *button,
                                        gpointer         user_data)
{
	/* If there is no main window */
	if (GUI_main_window() == NULL)
	{
		gtk_main_quit();
		gtk_exit (0);
	}
	/* Close the window */
	gtk_widget_destroy ((gtk_widget_get_toplevel(GTK_WIDGET(button))));
}


gboolean
on_selectkb_window_delete_event        (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	/* If there is no main window */
	if (GUI_main_window() == NULL)
	{
		gtk_main_quit();
		gtk_exit (0);
	}
	return FALSE;
}



/****************************
 * Callbacks of main_window *
 ****************************/


void
on_manufacturer_model_entry_changed    (GtkEditable     *editable,
                                        gpointer         user_data)
{
	file_modified = TRUE;
}


Boolean
save_keyboard_file (	GtkWidget	*this_widget,
			char		**filename,
			KTKeyList	*key_list )
/*
Input:
	this_widget	- A widget in the same tree as the text entries named
			  "manufacturer_entry" and "model_entry"
	filename	- Points to the filename to save the file to. The user will
			  be asked to fill in a filename if the string (= *filename)
			  points to NULL.
	key_list	- The list to write to the keyboard file.
Output:
	filename	- Will point to the filename where the keyboard file was
			  written to or NULL if writing the file failed.
Returns:
	TRUE if the keyboard file was (successfully) written, otherwise FALSE.
Description:
	This function writes a keyboard file to the file named *filename, if
	*filename does not point to NULL. The user will be asked for a file name if
	'*filename' points to NULL and if the user wants to overwrite the file if
	the filled file name already exists.
	The manufacturer and model will be retrieved from the text entries
	"manufacturer_entry" and "model_entry" respectively. All the keys in
	'key_list' will be used in the "key-list" element of the keyboard file.
*/
{
	char			*manufacturer,
				*model,
				*author,
				*email_address,
				*subject;
	GtkWidget		*overwrite_question_dialog,
				*msg_dialog,
				*send_question_dialog,
				*contact_dialog,
				*save_dialog;
	Boolean			send_keyboard_file,
				ask_to_overwrite,
				save_file;	/* Indicates at some point of this function if
						 * the file should still be saved. At the end
						 * of this function it indicates if the file
						 * was successfully saved.
						 */
	
	manufacturer = (char *) gtk_entry_get_text( GTK_ENTRY(
	                           lookup_widget(this_widget, "manufacturer_entry") ) );
	model = (char *) gtk_entry_get_text( GTK_ENTRY(
	                           lookup_widget(this_widget, "model_entry") ) );
	
	author = "";
	/*** Ask the user to send the keyboard file: ***/
	send_question_dialog =  gtk_message_dialog_new (
					GTK_WINDOW (gtk_widget_get_toplevel (this_widget)),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_MESSAGE_QUESTION,
					GTK_BUTTONS_YES_NO,
					_("Do you want to send this keyboard file to the developer of keyTouch?\n"
					  "This will help other people with the same keyboard.\n"
					  "Only say \"Yes\" when the keyboard file is finished."));
	send_keyboard_file = (gtk_dialog_run (GTK_DIALOG(send_question_dialog)) == GTK_RESPONSE_YES);
	gtk_widget_destroy (send_question_dialog);
	/* If the user wants to send it */
	if (send_keyboard_file)
	{
		/*** Get contact information ***/
		contact_dialog = create_contact_dialog();
		send_keyboard_file = (gtk_dialog_run( GTK_DIALOG (contact_dialog) ) == GTK_RESPONSE_OK);
		if (send_keyboard_file)
		{
			author = (char *) gtk_entry_get_text( GTK_ENTRY(
						lookup_widget(contact_dialog, "author_entry")) );
			email_address = (char *) gtk_entry_get_text( GTK_ENTRY(
						lookup_widget(contact_dialog, "email_entry")) );
			/* Note: The contact_dialog should be destroyed later because we still need
			 *       the contents of author and email_address entries
			 */
		}
		else
		{
			gtk_widget_destroy (contact_dialog);
		}
	}
	
	ask_to_overwrite = (*filename == NULL);
	if (*filename == NULL)
	{
		save_dialog = gtk_file_chooser_dialog_new (_("Save keyboard file"),
					GTK_WINDOW ( gtk_widget_get_toplevel (GTK_WIDGET(this_widget)) ),
					GTK_FILE_CHOOSER_ACTION_SAVE,
					GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
					NULL);
		if (gtk_dialog_run( GTK_DIALOG (save_dialog) ) == GTK_RESPONSE_ACCEPT)
		{
			*filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (save_dialog));
		}
		gtk_widget_destroy (save_dialog);
	}
	save_file = (*filename != NULL); /* = TRUE when the user specified a filename */
	if (save_file)
	{
		/* If a file with the same name already exists */
		if (access (*filename, F_OK) == 0 && ask_to_overwrite)
		{
			overwrite_question_dialog = gtk_message_dialog_new (
							GTK_WINDOW ( gtk_widget_get_toplevel (GTK_WIDGET(this_widget)) ),
							GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
							_("A file named \"%s\" already exists. "
							  "Are you sure you want to overwrite it?"), *filename);
			save_file = (gtk_dialog_run (GTK_DIALOG(overwrite_question_dialog)) == GTK_RESPONSE_YES);
			gtk_widget_destroy (overwrite_question_dialog);
		}
		if (save_file)
		{
			/* If writing the file failed */
			if (write_keyboard_file (manufacturer, model, key_list, usb_vendor_id,
			                         usb_product_id, author, *filename) == FALSE)
			{
				KTError (_("Failed to save \"%s\"."), *filename);
				save_file = FALSE;
			}
			else
			{
				if (send_keyboard_file)
				{
					if (asprintf (&subject, "kbfile: %s %s (%s)", manufacturer, model, author) == -1)
					{
						KTError (_("Could not allocate memory."), "");
						exit (EXIT_FAILURE);
					}
					if (!email_file (KBFILE_EMAIL_ADDRESS, email_address, subject, *filename, smtp_hostnames))
					{
						KTError (_("Failed to send the keyboard file."), "");
					}
					free (subject);
					gtk_widget_destroy (contact_dialog);
				}
				if (!keyboard_file_is_valid (*filename))
				{
					msg_dialog = gtk_message_dialog_new (
							GTK_WINDOW ( gtk_widget_get_toplevel (GTK_WIDGET(this_widget)) ),
							GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
							_("The keyboard file has been saved successfully, but cannot yet be "
							  "imported in keyTouch because it is not a valid keyboard file."));
					gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG(msg_dialog),
					                                          (const gchar *) KTGetErrorMsg() );
					gtk_dialog_run (GTK_DIALOG( msg_dialog ));
					gtk_widget_destroy (GTK_WIDGET( msg_dialog ));
				}
			}
		}
		if (!save_file)
		{
			/* We do not need the filename anymore if the file is not saved */
			free (*filename);
			*filename = NULL;
		}
	}
	return save_file;
}


Boolean
ask_to_save (	GtkWidget	*this_widget,
		char		*filename )
/*
Input:
	this_widget	- This widget must appear in in the main window
	filename	- The filename of the current keyboard file, or NULL if it
			  has no name yet
Returns:
	TRUE if the user wants to save the file, otherwise FALSE.
Description:
	This function creates a dialog that asks the user to save the file named
	'filename' because it was modified.
*/
{
	GtkWidget *question_dialog;
	Boolean save_file;
	
	question_dialog =  gtk_message_dialog_new (
				GTK_WINDOW (gtk_widget_get_toplevel( this_widget )),
				GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_MESSAGE_QUESTION,
				GTK_BUTTONS_YES_NO,
				_("The keyboard file \"%s\" has been modified.\n"
				  "Do you want to save your changes?"),
				(filename ? filename : ""));
	save_file = (gtk_dialog_run( GTK_DIALOG(question_dialog) ) == GTK_RESPONSE_YES);
	gtk_widget_destroy (question_dialog);
	
	return save_file;
}


void
on_new1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	/* If the currently opened file was modified and the user wants to save it */
	if (file_modified && ask_to_save(GTK_WIDGET(menuitem), current_filename))
	{
		save_keyboard_file (GTK_WIDGET(menuitem), &current_filename, &key_list);
	}
	KTKeyList_clear (&key_list);
	reload_key_treeview (&key_list, GTK_WIDGET(menuitem));
	file_modified = FALSE;
	selected_key = NULL;
	clear_key_settings_frame (GTK_WIDGET( menuitem ));
	gtk_entry_set_text (GTK_ENTRY( lookup_widget(GTK_WIDGET(menuitem), "manufacturer_entry") ), "");
	gtk_entry_set_text (GTK_ENTRY( lookup_widget(GTK_WIDGET(menuitem), "model_entry") ), "");
	if (current_filename)
	{
		free (current_filename);
		current_filename = NULL;
	}
}


void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
/*
Globals:
	file_modified		- Indicates if the currently opend file was modified
	current_filename	- The name if the currently opened file (= NULL if
				  no name is defined yet)
*/
{
	GtkWidget *open_dialog;
	KTKeyList new_key_list;
	char *filename,
	     *manufacturer,
	     *model;
	int vendor_id, product_id;
	
	/* If the currently opened file was modified and the user wants to save it */
	if (file_modified && ask_to_save(GTK_WIDGET(menuitem), current_filename))
	{
		save_keyboard_file (GTK_WIDGET(menuitem), &current_filename, &key_list);
	}
	open_dialog = gtk_file_chooser_dialog_new (_("Open keyboard file"),
					GTK_WINDOW ( gtk_widget_get_toplevel (GTK_WIDGET(menuitem)) ),
					GTK_FILE_CHOOSER_ACTION_OPEN,
					GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
					NULL);
	if (gtk_dialog_run (GTK_DIALOG (open_dialog)) == GTK_RESPONSE_ACCEPT)
	{
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (open_dialog));
		/* If the keyboard file was successfully read */
		if (read_keyboard_file (&new_key_list, &manufacturer, &model,
		                        &vendor_id, &product_id, filename))
		{
			/* Set the USB vendor and product ID to the values read from the
			 * the keyboard file, only when the currently selected keyboard
			 * is not connected via USB 
			 */
			if (evdev_bus_id(current_evdev) != BUS_USB)
			{
				usb_vendor_id = vendor_id;
				usb_product_id = product_id;
			}
			if (current_filename)
			{
				g_free (current_filename);
			}
			current_filename = filename;
			
			gtk_entry_set_text (
				GTK_ENTRY( lookup_widget(GTK_WIDGET(menuitem), "manufacturer_entry") ),
				manufacturer);
			free (manufacturer);
			gtk_entry_set_text (
				GTK_ENTRY( lookup_widget(GTK_WIDGET(menuitem), "model_entry") ),
				model);
			free (model);
			
			KTKeyList_clear (&key_list);
			key_list = new_key_list;
			reload_key_treeview (&key_list, GTK_WIDGET(menuitem));
			file_modified = FALSE;
			selected_key = NULL;
			clear_key_settings_frame (GTK_WIDGET( menuitem ));
		}
		else
		{
			/* Because the filename will not be used: */
			g_free (filename);
		}
	}
	
	gtk_widget_destroy (open_dialog);
}


void
on_save1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	file_modified &= !save_keyboard_file (GTK_WIDGET(menuitem), &current_filename, &key_list);
}


void
on_save_as1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	char *tmp_filename = NULL;
	
	/* The the keyboard file was successfully saved */
	if (save_keyboard_file (GTK_WIDGET(menuitem), &tmp_filename, &key_list))
	{
		if (current_filename)
		{
			free (current_filename);
		}
		current_filename = tmp_filename;
		file_modified = FALSE;
	}
	else if (tmp_filename)
	{
		free (tmp_filename);
	}
}


void
on_quit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	/* If the currently opened file was modified and the user wants to save it */
	if (file_modified && ask_to_save(GTK_WIDGET(menuitem), current_filename))
	{
		save_keyboard_file (GTK_WIDGET(menuitem), &current_filename, &key_list);
	}
	gtk_main_quit();
	gtk_exit (0);
}


void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkWidget *about_dialog;
	
	about_dialog = create_about_dialog();
	gtk_dialog_run (GTK_DIALOG( about_dialog ));
	gtk_widget_destroy (about_dialog);
}


void
on_remove_key_button_clicked           (GtkButton       *button,
                                        gpointer         user_data)
{
	if (selected_key)
	{
		KTKeyList_remove_key (&key_list, selected_key);
		reload_key_treeview (&key_list, GTK_WIDGET(button));
		selected_key = NULL;
		clear_key_settings_frame (GTK_WIDGET( button ));
	}
}


void
on_new_key_button_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget		*key_name_dialog;
	GtkEntry		*key_name_entry;
	char			*key_name,
				*acpi_event_descr;
	KTKeycodeScancode	keycode_scancode;
	KTKey			*new_key;
	Boolean			key_identified;
	int keycode,
	    scancode,
	    usagecode;

	keycode_scancode.keycode = 0;
	keycode_scancode.scancode = 0;
	acpi_event_descr = NULL;
	if (use_acpi_dev)
	{
		acpi_event_descr = get_acpi_event_descr (PRESS_KEY_MSG);
		key_identified = (acpi_event_descr != NULL);
	}
	else
	{
		key_identified = get_keycode_scancode (evdev_complete_file_name(current_evdev), PRESS_KEY_MSG, &keycode_scancode);
	}
	if (key_identified)
	{
		key_name_dialog = create_new_key_name_dialog();
		key_name_entry = GTK_ENTRY (lookup_widget(key_name_dialog, "new_key_name_entry"));
		/* If the key has a keycode and if there is a default name for such a key */
		if (keycode_scancode.keycode && key_defaults[keycode_scancode.keycode].name)
		{
			/* Fill in the default name as a suggestion */
			gtk_entry_set_text (key_name_entry, key_defaults[keycode_scancode.keycode].name);
		}
		/* Ask the user for the key name */
		if (gtk_dialog_run(GTK_DIALOG(key_name_dialog)) == GTK_RESPONSE_OK)
		{
			/* Get the key name */
			key_name = (char *) gtk_entry_get_text(key_name_entry);
			if (*key_name != '\0' && !KTKeyList_find_key(&key_list, key_name))
			{
				keycode = keycode_scancode.keycode;
				if (evdev_bus_id(current_evdev) == BUS_USB)
				{
					scancode = 0;
					usagecode = keycode_scancode.scancode;
				}
				else
				{
					scancode = keycode_scancode.scancode;
					usagecode = 0;
				}
				
				new_key = KTKey_new(key_name, scancode, usagecode, keycode, acpi_event_descr);
				/* If the key has a keycode and if there is an action for such a key */
				if (keycode && key_defaults[keycode].name)
				{
					if (key_defaults[keycode].action.type == KTActionTypeProgram)
					{
						KTKey_set_program (new_key, 
							key_defaults[keycode].action.program.command);
					}
					else if (key_defaults[keycode].action.type == KTActionTypePlugin)
					{
						KTKey_set_plugin (new_key,
							key_defaults[keycode].action.plugin.plugin_name,
							key_defaults[keycode].action.plugin.function_name);
					}
				}
				/* Add the new key to the key list */
				KTKeyList_append (&key_list, new_key);
				reload_key_treeview (&key_list, GTK_WIDGET(button));
				file_modified = TRUE;
			}
			else
			{
				msg_box (_("Cannot add the key because the name is already in the list."), GTK_MESSAGE_WARNING);
				if (use_acpi_dev)
				{
					free (acpi_event_descr);
				}
			}
		}
		else if (use_acpi_dev)
		{
			free (acpi_event_descr);
		}
		gtk_widget_destroy (key_name_dialog);
	}
}


void
on_scancode_change_button_clicked      (GtkButton       *button,
                                        gpointer         user_data)
{
	KTKeycodeScancode keycode_scancode;
	char str_scancode[64],
	     *acpi_event_descr;
	
	if (selected_key)
	{
		if (use_acpi_dev)
		{
			acpi_event_descr = get_acpi_event_descr (PRESS_KEY_MSG);
			if (acpi_event_descr)
			{
				KTKey_set_acpi_event_descr (selected_key, acpi_event_descr);
				gtk_entry_set_text (GUI_scancode_entry(), acpi_event_descr);
				gtk_label_set_text (GUI_scancode_event_descr_label(), EVENT_DESCR_LABEL_TEXT);
			}
		}
		else
		{
			/* If we received a scancode */
			if (get_keycode_scancode (evdev_complete_file_name(current_evdev), PRESS_KEY_MSG, &keycode_scancode))
			{
				KTKey_set_scancode (selected_key, keycode_scancode.scancode);
				if (keycode_scancode.scancode)
				{
					/* Convert the scancode to a string */
					snprintf (str_scancode, sizeof(str_scancode), "%d", keycode_scancode.scancode);
					str_scancode[sizeof(str_scancode)-1] = '\0';
					/* Show the scancode */
					gtk_entry_set_text (GUI_scancode_entry(), str_scancode);
				}
				else
				{
					gtk_entry_set_text (GUI_scancode_entry(), "");
				}
				gtk_label_set_text (GUI_scancode_event_descr_label(), SCANCODE_LABEL_TEXT);
				
				if (keycode_scancode.keycode)
				{
					KTKey_set_keycode (selected_key, keycode_scancode.keycode);
					
					g_signal_handlers_block_by_func (GTK_OBJECT(GUI_keycode_combobox()),
					                                 (void *)on_keycode_combobox_changed, NULL);
					gtk_combo_box_set_active (GUI_keycode_combobox(), keycode2index(keycode_scancode.keycode));
					g_signal_handlers_unblock_by_func (GTK_OBJECT(GUI_keycode_combobox()),
					                                   (void *)on_keycode_combobox_changed, NULL);
				}
			}
		}
	}
}


void
on_usbcode_change_button_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{
	KTKeycodeScancode keycode_scancode;
	char str_scancode[64];
	
	if (selected_key)
	{
		/* If we received a usage code */
		if (get_keycode_scancode (evdev_complete_file_name(current_evdev), PRESS_KEY_MSG, &keycode_scancode))
		{
			KTKey_set_usagecode (selected_key, keycode_scancode.scancode);
			if (keycode_scancode.scancode)
			{
				/* Convert the usage code to a string */
				snprintf (str_scancode, sizeof(str_scancode), "0x%x", keycode_scancode.scancode);
				str_scancode[sizeof(str_scancode)-1] = '\0';
				/* Show the usage code */
				gtk_entry_set_text (GUI_usbcode_entry(), str_scancode);
			}
			else
			{
				gtk_entry_set_text (GUI_usbcode_entry(), "");
			}
			
			if (keycode_scancode.keycode)
			{
				KTKey_set_keycode (selected_key, keycode_scancode.keycode);
				
				g_signal_handlers_block_by_func (GTK_OBJECT(GUI_keycode_combobox()),
				                                 (void *)on_keycode_combobox_changed, NULL);
				gtk_combo_box_set_active (GUI_keycode_combobox(), keycode2index(keycode_scancode.keycode));
				g_signal_handlers_unblock_by_func (GTK_OBJECT(GUI_keycode_combobox()),
				                                   (void *)on_keycode_combobox_changed, NULL);
			}
		}
	}
}


void
on_keycode_combobox_changed            (GtkComboBox     *combobox,
                                        gpointer         user_data)
{
	int selection_index;
	
	if (selected_key)
	{
		selection_index = gtk_combo_box_get_active(combobox);
		KTKey_set_keycode (selected_key, visible_keycodes[selection_index]);
		file_modified = TRUE;
	}
}


void
on_key_name_entry_changed              (GtkEditable     *editable,
                                        gpointer         user_data)
{
	KTKey *other_key;
	char *entry_text;
	
	if (selected_key)
	{
		entry_text = (char*) gtk_entry_get_text (GTK_ENTRY(editable));
		/* Search in key_list for a key with the same name */
		other_key = KTKeyList_find_key (&key_list, entry_text);
		/* If no other key with the same name exists */
		if (other_key == NULL || other_key == selected_key)
		{
			/* Change the key name */
			KTKey_set_name (selected_key, entry_text);
			reload_key_treeview (&key_list, GTK_WIDGET(editable));
			file_modified = TRUE;
		}
	}
}


void
on_program_radiobutton_toggled         (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	GtkEntry *program_entry;
	
	/* If special_action_radiobutton is pressed */
	if (gtk_toggle_button_get_active(togglebutton) == TRUE && selected_key)
	{
		program_entry = GTK_ENTRY (lookup_widget (GTK_WIDGET(togglebutton), "program_entry"));
		KTKey_set_program (selected_key, (char *) gtk_entry_get_text(program_entry));
		file_modified = TRUE;
	}
}


void
on_program_entry_changed               (GtkEditable     *editable,
                                        gpointer         user_data)
{
	GtkRadioButton	*program_radiobutton;
	
	if (selected_key)
	{
		program_radiobutton = GTK_RADIO_BUTTON (
		     lookup_widget (GTK_WIDGET(editable), "program_radiobutton") );
		g_signal_handlers_block_by_func (GTK_OBJECT(program_radiobutton),
		                                 (void *)on_program_radiobutton_toggled, NULL);
		
		KTKey_set_program (selected_key, (char*)gtk_entry_get_text(GTK_ENTRY(editable)));
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(program_radiobutton), TRUE);
		
		g_signal_handlers_unblock_by_func (GTK_OBJECT(program_radiobutton),
		                                   (void *)on_program_radiobutton_toggled, NULL);
		file_modified = TRUE;
	}
}


void
handle_changed_special_action (	GtkWidget	*special_action_radiobutton,
				GtkEntry	*plugin_combo_entry,
				GtkEntry	*plugin_function_combo_entry )
/*
input:
	special_action_radiobutton	- The radiobutton which indicates if the
					  selected plugin and its function
	plugin_combo_entry		- The entry that contains the name of the
					  plugin
	plugin_function_combo_entry	- The entry that contains the name of the
					  plugin function
output:
	-
returns:
	-
description:
	This function changes the default action of the selected key to the selected
	plugin and its function. It also toggles special_action_radiobutton to TRUE.
*/
{
	char		*plugin_name,
			*function_name;

	if (selected_key)
	{
		plugin_name = (char *) gtk_entry_get_text(plugin_combo_entry);
		function_name = (char *) gtk_entry_get_text(plugin_function_combo_entry);
		if (plugin_name[0] != '\0' && function_name[0] != '\0')
		{
			KTKey_set_plugin (selected_key, plugin_name, function_name);
		}
		/* Block the signal handler of and special_action_radiobutton */
		g_signal_handlers_block_by_func (GTK_OBJECT(special_action_radiobutton),
		                                 (void *)on_special_action_radiobutton_toggled, NULL);
		/* Toggle special_action_radiobutton */
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(special_action_radiobutton), TRUE);
		/* Unblock the signal handler of and special_action_radiobutton */
		g_signal_handlers_unblock_by_func (GTK_OBJECT(special_action_radiobutton),
		                                   (void *)on_special_action_radiobutton_toggled, NULL);
		file_modified = TRUE;
	}
}


void
on_plugin_comboentry_changed           (GtkEditable     *editable,
                                        gpointer         user_data)
{
	char *plugin_name;
	
	plugin_name = (char *) gtk_entry_get_text (GTK_ENTRY(editable));
	reload_plugin_function_list (GTK_WIDGET(editable),
	                             find_plugin (plugin_name, plugin_list, NUM_PLUGINS));
	handle_changed_special_action (
	                    lookup_widget (GTK_WIDGET(editable), "special_action_radiobutton"),
	                    GTK_ENTRY (editable),
	                    GTK_ENTRY (lookup_widget(GTK_WIDGET(editable), "plugin_function_comboentry")) );
}


void
on_plugin_function_comboentry_changed  (GtkEditable     *editable,
                                        gpointer         user_data)
{
	handle_changed_special_action (
	                   lookup_widget (GTK_WIDGET(editable), "special_action_radiobutton"),
	                   GTK_ENTRY ( lookup_widget (GTK_WIDGET(editable), "plugin_comboentry") ),
	                   GTK_ENTRY (editable) );
}


void
on_special_action_radiobutton_toggled  (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	/* If special_action_radiobutton is pressed */
	if ( gtk_toggle_button_get_active(togglebutton) == TRUE )
	{
		handle_changed_special_action (
		           GTK_WIDGET (togglebutton),
		           GTK_ENTRY (lookup_widget( GTK_WIDGET(togglebutton), "plugin_comboentry" )),
		           GTK_ENTRY (lookup_widget( GTK_WIDGET(togglebutton), "plugin_function_comboentry" )) );
	}
}


void
on_change_keyboard_menuitem_activate   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	clear_evdev_list (&evdev_list);
	read_evdev_list (&evdev_list, EVDEV_DIR);
	show_selectkb_window (&evdev_list, acpi_usable);
}


gboolean
on_key_treeview_selection (	GtkTreeSelection	*selection,
				GtkTreeModel		*model,
				GtkTreePath		*path,
				gboolean		path_currently_selected,
				gpointer		userdata )
{
	GtkRadioButton *selected_radiobutton;
	GtkTreeIter iter;
	char *key_name,
	     str_scancode[64],
	     *prog,
	     *plugin_name,
	     *plugin_function,
	     *acpi_event_descr;
	
	selected_key = NULL;
	/* If a row was selected instead of deselected */
	if (gtk_tree_model_get_iter(model, &iter, path)) 
	{
		/* Get the name of the selected key */
		gtk_tree_model_get (model, &iter, FIRST_COLUMN, &key_name, -1);
		if (key_name != NULL) /* If a listitem was selected instead of deselected */
		{
			/* Block the signal handlers */
			g_signal_handlers_block_by_func (GTK_OBJECT(GUI_key_name_entry()),
			                                 (void *)on_key_name_entry_changed, NULL);
			g_signal_handlers_block_by_func (GTK_OBJECT(GUI_program_entry()),
			                                 (void *)on_program_entry_changed, NULL);
			g_signal_handlers_block_by_func (GTK_OBJECT(GUI_keycode_combobox()),
			                                 (void *)on_keycode_combobox_changed, NULL);
			g_signal_handlers_block_by_func (GTK_OBJECT(GUI_program_radiobutton()),
			                                 (void *)on_program_radiobutton_toggled, NULL);
			g_signal_handlers_block_by_func (GTK_OBJECT(GUI_plugin_comboentry()),
			                                 (void *)on_plugin_comboentry_changed, NULL);
			g_signal_handlers_block_by_func (GTK_OBJECT(GUI_plugin_function_comboentry()),
			                                 (void *)on_plugin_function_comboentry_changed, NULL);
			
			/* Get the data for the selected key */
			selected_key = KTKeyList_find_key(&key_list, key_name);
			
			/* Show the name of the key */
			gtk_entry_set_text (GUI_key_name_entry(), key_name);
			
			acpi_event_descr = KTKey_get_acpi_event_descr (selected_key);
			if (acpi_event_descr)
			{
				gtk_entry_set_text (GUI_scancode_entry(), acpi_event_descr);
				gtk_label_set_text (GUI_scancode_event_descr_label(), EVENT_DESCR_LABEL_TEXT);
			}
			else
			{
				gtk_label_set_text (GUI_scancode_event_descr_label(), SCANCODE_LABEL_TEXT);
				if (KTKey_get_scancode (selected_key))
				{
					/* Convert the scancode to a string */
					snprintf (str_scancode, sizeof(str_scancode), "%d", KTKey_get_scancode(selected_key));
					str_scancode[sizeof(str_scancode)-1] = '\0';
					/* Show the scancode */
					gtk_entry_set_text (GUI_scancode_entry(), str_scancode);
				}
				else
				{
					gtk_entry_set_text (GUI_scancode_entry(), "");
				}
			}
			
			if (KTKey_get_usagecode(selected_key))
			{
				/* Convert the usagecode to a string */
				snprintf (str_scancode, sizeof(str_scancode), "0x%x", KTKey_get_usagecode(selected_key));
				str_scancode[sizeof(str_scancode)-1] = '\0';
				/* Show the usagecode */
				gtk_entry_set_text (GUI_usbcode_entry(), str_scancode);
			}
			else
			{
				gtk_entry_set_text (GUI_usbcode_entry(), "");
			}
			
			/* Show the keycode */
			gtk_combo_box_set_active (GUI_keycode_combobox(), keycode2index(KTKey_get_keycode(selected_key)));
			
			if (KTKey_get_action_type(selected_key) == KTActionTypeProgram)
			{
				prog = KTKey_get_action_command(selected_key);
				selected_radiobutton = GUI_program_radiobutton();
				plugin_name = "";
				plugin_function = "";
			}
			else
			{
				prog = "";
				selected_radiobutton = GUI_special_action_radiobutton();
				plugin_name = KTKey_get_plugin_name(selected_key);
				plugin_function = KTKey_get_plugin_function_name(selected_key);
				
			}
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(selected_radiobutton), TRUE);
			
			/* Show the plugin */
			gtk_entry_set_text (GUI_plugin_comboentry(), plugin_name);
			reload_plugin_function_list (GTK_WIDGET(GUI_special_action_radiobutton()),
			                             find_plugin (plugin_name, plugin_list, NUM_PLUGINS));
			/* Show the plugin function */
			gtk_entry_set_text (GUI_plugin_function_comboentry(), plugin_function);
			
			/* Show the program */
			gtk_entry_set_text (GUI_program_entry(), prog);
			
			/* Unblock the signal handlers */
			g_signal_handlers_unblock_by_func (GTK_OBJECT(GUI_key_name_entry()),
			                                   (void *)on_key_name_entry_changed, NULL);
			g_signal_handlers_unblock_by_func (GTK_OBJECT(GUI_program_entry()),
			                                   (void *)on_program_entry_changed, NULL);
			g_signal_handlers_unblock_by_func (GTK_OBJECT(GUI_keycode_combobox()),
			                                   (void *)on_keycode_combobox_changed, NULL);
			g_signal_handlers_unblock_by_func (GTK_OBJECT(GUI_program_radiobutton()),
			                                   (void *)on_program_radiobutton_toggled, NULL);
			g_signal_handlers_unblock_by_func (GTK_OBJECT(GUI_plugin_comboentry()),
			                                   (void *)on_plugin_comboentry_changed, NULL);
			g_signal_handlers_unblock_by_func (GTK_OBJECT(GUI_plugin_function_comboentry()),
			                                   (void *)on_plugin_function_comboentry_changed, NULL);
		}
	}
	
	return (TRUE);
}


gboolean
on_main_window_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	/* If the currently opened file was modified and the user wants to save it */
	if (file_modified && ask_to_save(widget, current_filename))
	{
		save_keyboard_file (widget, &current_filename, &key_list);
	}
	KTKeyList_clear (&key_list);
	gtk_main_quit();
	gtk_exit (0);
	return FALSE;
}



/************************************
 * Callbacks of get_scancode_dialog *
 ************************************/


void
on_get_scancode_dialog_show            (GtkWidget       *widget,
                                        gpointer         user_data)
{
	/* Block the interaction with the main window, so that
	 * the user can only use this window
	 */
	gtk_grab_add (GTK_WIDGET(widget));
}


gboolean
on_get_scancode_dialog_delete_event    (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	gtk_main_quit();
	return TRUE; /* Do not destroy the window */
}


void
on_get_scancode_cancel_button_clicked  (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_main_quit();
}



/*******************************
 * Callbacks of contact_dialog *
 *******************************/


gboolean
on_contact_dialog_delete_event         (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	gtk_dialog_response (GTK_DIALOG(widget), GTK_RESPONSE_CANCEL);
	return TRUE; /* Do not destroy the window */
}


void
on_contact_dialog_cancelbutton_clicked (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_dialog_response (GTK_DIALOG( gtk_widget_get_toplevel( GTK_WIDGET(button) ) ), GTK_RESPONSE_CANCEL);
}


void
on_contact_dialog_okbutton_clicked     (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_dialog_response (GTK_DIALOG( gtk_widget_get_toplevel( GTK_WIDGET(button) ) ), GTK_RESPONSE_OK);
}
