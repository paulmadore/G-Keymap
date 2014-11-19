/*-------------------------------------------------------------------------------
Name               : callbacks.c
Author             : Marvin Raaijmakers
Description        : Handles UI callbacks
Date of last change: 23-December-2004
History            :
                    VERSION 1.0:
                     29-June-2004     Created:- on_keyboard_ok_button_clicked()
                                              - on_keyboard_cancel_button_clicked()
                                              - functions for bottom buttons
                                              - GET_LIST_POSITION()
                     30-June-2004     Modified on_key_list_selection_changed()
                                      that the signal handlers of program_entry
                                      and program_radiobutton will be blocked
                                      during the modifications of these widgets
                     1-July-2004      Created functions for the file selection
                                      window
                     2-July-2004      Created functions for the message box/
                                      window
                     7-July-2004      Created callback functions for the entries
                                      of preferences.
                                      Modified on_program_entry_focus_out_event()
                     8-July-2004      Created:
                                      - on_special_action_optionmenu_clicked()
                                      - handle_changed_special_action()
                                      - on_special_action_radiobutton_toggled()
                                      - get_selection_plugin()
                     13-July-2004     Modified:
                                      - on_key_list_selection_changed()
                                      - select_special_action()
                     27-July-2004     Created handle_changed_program_user()
                     25-August-2004   Created on_help_button_clicked()
                     3-September-2004 Fixed bug in change_key_program_user()
                    VERSION 1.1:
                     4-December-2004  Modified:
                                      - on_ok_button_clicked()
                                      - on_apply_button_clicked()
                                      - change_key_program_user()
                     5-December-2004  Modified:
                                      - on_keyboard_ok_button_clicked()
                       December-2004  Modified this file for the new GUI (GTK+2)
                  

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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <keytouch.h>
#include <set_data.h>
#include <plugin_list.h>
#include <keyboard_config.h>

#define ONE_COLUMN	(1)
#define FIRST_COLUMN	(0)


static void handle_changed_special_action (GtkWidget *special_action_radiobutton, GtkEditable *special_action_combo_entry);
static void handle_changed_program_user (GtkWidget *program_entry, GtkWidget *program_radiobutton);
static char select_special_action (PLUGIN_LIST *plugin_list, char *special_action_name, GtkList *special_action_list);
static void linkto_keyboard_import (char *filename, GtkWidget *this_widget);
static void linkto_plugin_import (char *filename, GtkWidget *this_widget);
static void change_key_program_user (GtkWidget *widget, char *label_string, KEY_SETTINGS_LIST *key_settings_list, char *program_user);


gboolean
on_main_window_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	gtk_main_quit();
	exit (0);
}


void
linkto_keyboard_import (	char		*filename,
				GtkWidget	*this_widget    )
/*
input:
	filename	- The file name of the keyboard file to import
	this_widget	- A widget which is in the same tree as the keyboard list widget
output:
	-
returns:
	-
description:
	This function adds the keyboard of filename to the keyboard list and reloads the
	keyboard list widget. this_widget must be in the same tree as the keyboard list
	widget.
*/
{
	keyboard_import (filename, &keyboard_list);
	reload_keyboard_treeview (this_widget, &keyboard_list);
}


void
linkto_plugin_import (	char		*filename,
			GtkWidget	*this_widget  )
/*
input:
	filename	- The file name of the plugin to import
	this_widget	- A widget which is in the same tree as the plugin list widget
output:
	-
returns:
	-
description:
	This function adds the pluginof filename to the plugin list and reloads the plugin
	list widget. this_widget must be in the same tree as the plugin list widget.
*/
{
	plugin_import (filename, &plugin_list);
	reload_plugin_treeview (this_widget, &plugin_list);
	reload_special_action_list (this_widget, &plugin_list);
}


/*******************************************************************************
 *          CALLBACKS FOR THE BUTTONS AT THE BOTTOM OF THE MAIN WINDOW         *
 *******************************************************************************/

void
on_cancel_button_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_main_quit();
	exit (0);
}

void
on_ok_button_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
	save_settings (&current_keyboard_config, &keyboard_list, &xmodmap_list, &plugin_list, &preferences);
	gtk_main_quit();
	exit (0);
}

void
on_apply_button_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
	save_settings (&current_keyboard_config, &keyboard_list, &xmodmap_list, &plugin_list, &preferences);
}


/*******************************************************************************
 *                          CALLBACKS FOR KEY SETTINGS                         *
 *******************************************************************************/

void
change_key_program_user (	GtkWidget		*widget,
				char			*label_string,
				KEY_SETTINGS_LIST	*key_settings_list,
				char			*program_user        )
/*
input:
	widget			- This widget must be in the same tree a the widget from 
				  which the name is equal to label_string
	label_string		- This is the name of a label (widget) which contains the
				  name of the key to change the program_user element from
	key_settings_list	- A list containing all keys, of the current keyboard, with
				  their settings
	program_user	- The program_user element of the specified key will be changed to
			  this string
output:
	key_settings	- A list containing all keys, of the current keyboard, with their
			  settings. 
returns:
	-
description:
	This function changes the program_user (not the input variable) element of a key,
	in the key_settings list, to program_user (the input_variable). label_string is
	the name of a label (widget) which contains the name of the key to change. widget
	must be in the same tree as this label.
*/
{
	char *key_name;
	KEY_SETTINGS *selected_key;
	GtkWidget *key_name_label;

	key_name_label = lookup_widget(GTK_WIDGET(widget), label_string);
	if (key_name_label)
	{
		/* Get the name of the selected key */
		gtk_label_get (GTK_LABEL(key_name_label), &key_name);
		if (*key_name) /* If a listitem was selected */
		{
			/* Select the key in the linked list */
			selected_key = ksl_find_key( key_settings_list, key_name );
			if (selected_key) /* If the selected key exists */
			{
				/* Update the key settings */
				ks_set_program_user (selected_key, program_user);
			}
		}
	}
}


char
select_special_action (	PLUGIN_LIST	*plugin_list,
			char		*special_action_name,
			GtkList		*special_action_list )
/*
input:
	plugin_list		- List containing all available plugins
	special_action_name	- The name of the special action/plugin
	special_action_list	- A list (widget) containing all special actions/plugins
output:
	-
returns:
	TRUE if the plugin was found else FALSE.
description:
	This function changes the program_user (not the input variable) element of a key,
	in the key_settings list, to program_user (the input_variable). label_string is
	the name of a label (widget) which contains the name of the key to change. widget
	must be in the same tree as this label.
*/
{
	int count;
	PLUGIN *plugin;
	
	special_action_name++; /* Because the first character is a ';' */
	plugin = plugl_first_plugin(plugin_list);
	/* Find the matching plugin */
	for (count = 0; plugin && (strcmp(special_action_name, plugl_plugin_get_name(plugin)) != EQUAL); count++)
	{
		plugin = plugin->next;
	}

	if (plugin == NULL) /* If the plugin was not found */
	{
		return (FALSE);
	}
	/* Select the matching list item of special_action_list */
	gtk_list_select_item (GTK_LIST(special_action_list), count);
	return (TRUE);
}


gboolean
on_key_treeview_selection (	GtkTreeSelection	*selection,
				GtkTreeModel		*model,
				GtkTreePath		*path,
				gboolean		path_currently_selected,
				gpointer		userdata                    )
{
	GtkLabel		*key_name_label = NULL;
	GtkEntry		*program_entry = NULL,
				*default_entry,
				*special_action_combo_entry;
	GtkRadioButton		*radiobutton,
				*default_radiobutton,
				*program_radiobutton,
				*special_action_radiobutton;
	GtkList			*special_action_list;
	GtkTreeView		*key_treeview;
	GtkTreeIter		iter;
	char			*key_name,
				*program_user = "",
				*program_default;
	KEY_SETTINGS		*selected_key;

	key_treeview = gtk_tree_selection_get_tree_view(selection);
	key_name_label = GTK_LABEL( lookup_widget(GTK_WIDGET(key_treeview), "key_name_label") );
	program_entry = GTK_ENTRY( lookup_widget(GTK_WIDGET(key_treeview), "program_entry") );
	default_entry = GTK_ENTRY( lookup_widget(GTK_WIDGET(key_treeview), "default_entry") );
	
	if ( gtk_tree_model_get_iter(model, &iter, path) ) /* If a row was selected instead of deselected */
	{
		/* Get the name of the selected key */
		gtk_tree_model_get (model, &iter, FIRST_COLUMN, &key_name, -1);
		if (key_name) /* If a listitem was selected instead of deselected */
		{
			default_radiobutton = GTK_RADIO_BUTTON( lookup_widget(GTK_WIDGET(key_treeview), "default_radiobutton") );
			program_radiobutton = GTK_RADIO_BUTTON( lookup_widget(GTK_WIDGET(key_treeview), "program_radiobutton") );
			special_action_radiobutton = GTK_RADIO_BUTTON(
				lookup_widget(GTK_WIDGET(key_treeview), "special_action_radiobutton") );
			special_action_combo_entry = GTK_ENTRY( lookup_widget(GTK_WIDGET(key_treeview), "special_action_combo_entry") );
			special_action_list = GTK_COMBO(lookup_widget(GTK_WIDGET(key_treeview), "special_action_combo"))->list;
			
			/* Block the signal handlers of the entries and the radiobuttons */
			gtk_signal_handler_block_by_func (	GTK_OBJECT(program_entry),
								(void *)on_program_entry_changed, NULL  );
			gtk_signal_handler_block_by_func (	GTK_OBJECT(default_radiobutton),
								(void *)on_default_radiobutton_toggled, NULL  );
			gtk_signal_handler_block_by_func (	GTK_OBJECT(program_radiobutton),
								(void *)on_program_radiobutton_toggled, NULL  );
			gtk_signal_handler_block_by_func (	GTK_OBJECT(special_action_radiobutton), 
								(void *)on_special_action_radiobutton_toggled, NULL  );
			gtk_signal_handler_block_by_func (	GTK_OBJECT(special_action_combo_entry),
								(void *)on_special_action_combo_entry_changed, NULL  );
			
			/* Select the first item of special_action_list */
			/* Select the matching list item of special_action_list */
			gtk_list_select_item (GTK_LIST(special_action_list), 0);
			/* Select the key in the linked list */
			selected_key = ksl_find_key( kbcfg_key_settings_list(&current_keyboard_config), key_name );
			/* Read the settings of the key */
			if ( strcmp( ks_get_program_user(selected_key), "default" ) == EQUAL )
			{
				radiobutton = default_radiobutton;
			}
			else if ( (*ks_get_program_user(selected_key) ==  ':') &&
				  (select_special_action(&plugin_list, ks_get_program_user(selected_key), special_action_list)) )
			{
				radiobutton = special_action_radiobutton;
			}
			else
			{
				program_user = ks_get_program_user(selected_key);
				radiobutton = GTK_RADIO_BUTTON( lookup_widget(GTK_WIDGET(key_treeview), "program_radiobutton") );
			}
			
			/* Show the settings of the key */
			gtk_label_set_text (GTK_LABEL(key_name_label), key_name);
			gtk_entry_set_text (GTK_ENTRY(program_entry), program_user);
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(radiobutton), TRUE);
			program_default = ks_get_program_default(selected_key);
			if (program_default[0] == ':') /* If program_default is a special action */
			{
				program_default++; /* Now the user only sees the name of the special action */
			}
			gtk_entry_set_text (GTK_ENTRY(default_entry), program_default);
			/* Unblock the signal handlers of the entries and the radiobuttons */
			gtk_signal_handler_unblock_by_func (	GTK_OBJECT(program_entry),
								(void *)on_program_entry_changed, NULL  );
			gtk_signal_handler_unblock_by_func (	GTK_OBJECT(default_radiobutton),
								(void *)on_default_radiobutton_toggled, NULL  );
			gtk_signal_handler_unblock_by_func (	GTK_OBJECT(program_radiobutton),
								(void *)on_program_radiobutton_toggled, NULL  );
			gtk_signal_handler_unblock_by_func (	GTK_OBJECT(special_action_radiobutton),
								(void *)on_special_action_radiobutton_toggled, NULL  );
			gtk_signal_handler_unblock_by_func (	GTK_OBJECT(special_action_combo_entry), 
								(void *)on_special_action_combo_entry_changed, NULL  );
		}
	}
	
	return (TRUE);
}


void
handle_changed_program_user (	GtkWidget	*program_entry,
				GtkWidget	*program_radiobutton )
/*
input:
	program_entry		- The program entry widget which could contain the
				  program_user data
	program_radiobutton	- The radio button which in indicates if program_entry
				  contains the program_user data
output:
	-
returns:
	-
description:
	This function changes the program_user element the selected key to the value of
	program_entry if program_radiobutton is toggled. If program_entry is empty and
	program_radiobutton is toggled, the program_user element will be changed to
	"default".
*/
{
	char *entry_text;

	/* If program_radiobutton is pressed */
	if ( gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(program_radiobutton)) )
	{
		/* Get the text op the program entry */
		entry_text = (char *) gtk_entry_get_text( GTK_ENTRY(program_entry) );
		if (entry_text[0] == '\0') /* If the entry does not contain text */
		{
			/* Set entry_text to default */
			entry_text = "default";
		}
		/* Correct the key settings */
		change_key_program_user (	program_entry,
						"key_name_label",
						kbcfg_key_settings_list(&current_keyboard_config),
						entry_text                                          );
	}
}


gboolean
on_program_entry_focus_out_event       (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data)
{
	GtkWidget *program_radiobutton;

	program_radiobutton = lookup_widget(GTK_WIDGET(widget), "program_radiobutton");
	handle_changed_program_user (widget, program_radiobutton);
	
	return (FALSE);
}


void
on_program_radiobutton_toggled         (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	GtkWidget *program_entry;

	program_entry = lookup_widget(GTK_WIDGET(togglebutton), "program_entry");
	handle_changed_program_user (program_entry, GTK_WIDGET(togglebutton));
}


void
on_default_radiobutton_toggled     (GtkToggleButton *togglebutton,
                                    gpointer         user_data)
{
	if ( gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(togglebutton)) ) /* If togglebutton is pressed */
	{
		/* Correct the key information */
		change_key_program_user (	GTK_WIDGET(togglebutton),
						"key_name_label",
						kbcfg_key_settings_list(&current_keyboard_config),
						"default"                                          );
	}
}


void
on_program_entry_changed               (GtkEditable     *editable,
                                        gpointer         user_data)
{
	GtkWidget *program_radiobutton;

	program_radiobutton = lookup_widget(GTK_WIDGET(editable), "program_radiobutton");
	/* Press program_radiobutton */
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(program_radiobutton), TRUE);
}


void
handle_changed_special_action (	GtkWidget	*special_action_radiobutton,
				GtkEditable	*special_action_combo_entry   )
/*
input:
	special_action_radiobutton	- The radiobutton which indicates if the value
					  of special_action_combo_entry will be the
					  program_user element of the selected key
	special_action_combo_entry	- This entry could contain the data of the
					  program_user element of the selected key
output:
	-
returns:
	-
description:
	This function changes the program_user element of the selected key to ":" plus
	the value of special_action_combo_entry. It also toggles special_action_radiobutton
	to TRUE.
*/
{
	char *program_user;

	/* Create a string for the key settings */
	program_user = g_strdup_printf (":%s", gtk_entry_get_text(GTK_ENTRY(special_action_combo_entry)) );
	/* Change the key settings */
	change_key_program_user (	GTK_WIDGET(special_action_radiobutton), "key_name_label",
					kbcfg_key_settings_list(&current_keyboard_config), program_user   );
	g_free (program_user);
	/* Block the signal handler of and special_action_radiobutton */
	gtk_signal_handler_block_by_func (GTK_OBJECT(special_action_radiobutton), (void *)on_special_action_radiobutton_toggled, NULL);
	/* Press special_action_radiobutton */
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(special_action_radiobutton), TRUE);
	/* Unblock the signal handler of and special_action_radiobutton */
	gtk_signal_handler_unblock_by_func (GTK_OBJECT(special_action_radiobutton), (void *)on_special_action_radiobutton_toggled, NULL);
}


void
on_special_action_radiobutton_toggled  (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	GtkEditable *special_action_combo_entry;

	/* If special_action_radiobutton is pressed */
	if ( gtk_toggle_button_get_active(togglebutton) == TRUE )
	{
		special_action_combo_entry = GTK_EDITABLE(lookup_widget(GTK_WIDGET(togglebutton), "special_action_combo_entry"));
		handle_changed_special_action (GTK_WIDGET(togglebutton), special_action_combo_entry);
	}
}


void
on_special_action_combo_entry_changed  (GtkEditable     *editable,
                                        gpointer         user_data)
{
	GtkWidget *special_action_radiobutton;

	special_action_radiobutton = lookup_widget(GTK_WIDGET(editable), "special_action_radiobutton");
	handle_changed_special_action (special_action_radiobutton, editable);
}


/*******************************************************************************
 *               CALLBACKS FOR KEYBOARD & THE KEYBOARD WINDOW                  *
 *******************************************************************************/

void
on_change_button_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *keyboard_dialog;

	keyboard_dialog = create_keyboard_dialog(); /* Create the keyboard dialog */
	/* Set data, containing the pointer to the main window, to keyboard_window */
	gtk_object_set_data (GTK_OBJECT(keyboard_dialog), "main_window", gtk_widget_get_toplevel( GTK_WIDGET(button) ));
	gtk_widget_show (keyboard_dialog); /* Show the keyboard window */
}


void
on_keyboard_dialog_show                (GtkWidget       *widget,
                                        gpointer         user_data)
{
	init_keyboard_treeview (widget);
	reload_keyboard_treeview (widget, &keyboard_list);
	/*
	 * Block the interaction with the main window, so that
	 * the user can only use this window
	 */
	gtk_grab_add (GTK_WIDGET(widget));
}


void
on_keyboard_import_button_clicked      (GtkButton       *button,
                                        gpointer         user_data)
{
	/* Wait for this until GTK+2.4 is standard:
	char *filename;
	
	filename = get_filename_from_user( "Open keyboard file", gtk_widget_get_toplevel(GTK_WIDGET(button)) );
	if (filename)
	{
		keyboard_import (filename, &keyboard_list);
		reload_keyboard_list (GTK_WIDGET(button), &keyboard_list);
		free (filename);
	}*/
	GtkWidget *fileselection_window;

	fileselection_window = create_fileselection_window(); /* Create the file selection window */
	/*
	 * Set data, containing the pointer to the function which process the filename,
	 * to fileselection_window
	 */
	gtk_object_set_data (GTK_OBJECT(fileselection_window), "process filename", linkto_keyboard_import);
	/* Set data, containing the pointer to the keyboard window, to the file selection window */
	gtk_object_set_data (GTK_OBJECT(fileselection_window), "process widget", gtk_widget_get_toplevel( GTK_WIDGET(button) ));
	/* NOT: Let the file selection window only show file which end with '.kti' */
	/* gtk_file_selection_complete (GTK_FILE_SELECTION(fileselection_window), "*.kti"); */
	gtk_widget_show (fileselection_window); /* Show the file selection window */
}


void
on_keyboard_download_button_clicked    (GtkButton       *button,
                                        gpointer         user_data)
{
	msg_box (	_("After you downloaded the right file for your keyboard, "
			  "click on the 'Import...' button and select the file you "
			  "downloaded."), GTK_MESSAGE_INFO );
	run_browser (preferences.browser, URL_DOWNLOAD_KEYBOARDS);
}



void
on_keyboard_ok_button_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkTreeView		*keyboard_treeview;
	GtkTreeSelection	*selection;
	GtkTreeModel		*tree_model;
	GtkTreeIter		iter;
	GtkWidget		*main_window,
				*keyboard_window;
	char			*manufacture,
				*model;
	
	keyboard_window = gtk_widget_get_toplevel( GTK_WIDGET(button) );
	keyboard_treeview = GTK_TREE_VIEW( lookup_widget(GTK_WIDGET(button), "keyboard_treeview") );
	
	selection = gtk_tree_view_get_selection( GTK_TREE_VIEW(keyboard_treeview) );
	if ( gtk_tree_selection_get_selected(selection, &tree_model, &iter) )/* If a keyboard was selected */
	{
		gtk_tree_model_get (	tree_model, &iter,
					COL_KEYBOARD_MANUFACTURE, &manufacture,
					COL_KEYBOARD_MODEL, &model,
					-1  );
		keyboard_change (manufacture, model, &current_keyboard_config, &xmodmap_list);
		/* Get the address of the main window */
		main_window = gtk_object_get_data (GTK_OBJECT(keyboard_window), "main_window");
		reload_current_keyboard (main_window, &current_keyboard_config);
		reload_key_treeview (GTK_WIDGET(main_window), &current_keyboard_config);
	}
	gtk_widget_destroy (keyboard_window); /* Close the keyboard window */
}


void
on_keyboard_cancel_button_clicked      (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_destroy (gtk_widget_get_toplevel( GTK_WIDGET(button) )); /* Close the keyboard window */
}


/*******************************************************************************
 *                         CALLBACKS FOR PREFERENCES                           *
 *******************************************************************************/

void
on_plugin_import_button_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *fileselection_window;

	fileselection_window = create_fileselection_window(); /* Create the file selection window */
	/*
	 * Set data, containing the pointer to the function which process the filename,
	 * to fileselection_window
	 */
	gtk_object_set_data (GTK_OBJECT(fileselection_window), "process filename", linkto_plugin_import);
	/* Set data, containing the pointer to the keyboard window, to the file selection window */
	gtk_object_set_data (GTK_OBJECT(fileselection_window), "process widget", gtk_widget_get_toplevel( GTK_WIDGET(button) ));
	/* Let the file selection window only show file which end with '.so' */
	gtk_file_selection_complete (GTK_FILE_SELECTION(fileselection_window), "*.so");
	gtk_widget_show (fileselection_window); /* Show the file selection window */
}


void
on_plugin_download_button_clicked      (GtkButton       *button,
                                        gpointer         user_data)
{
	msg_box (	_("After you downloaded a plugin for the keyTouch site, "
			  "click on the 'Import...' button and select the plugin "
			  "you downloaded."), GTK_MESSAGE_INFO );
	run_browser (preferences.browser, URL_DOWNLOAD_PLUGINS);
}


gboolean
on_documents_entry_focus_out_event     (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data)
{
	strcpy ( preferences.documents_dir, gtk_entry_get_text(GTK_ENTRY(widget)) );
	return (FALSE);
}


gboolean
on_home_page_entry_focus_out_event     (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data)
{
	strcpy ( preferences.home_page, gtk_entry_get_text(GTK_ENTRY(widget)) );
	return (FALSE);
}


gboolean
on_search_page_entry_focus_out_event   (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data)
{
	strcpy ( preferences.search_page, gtk_entry_get_text(GTK_ENTRY(widget)) );
	return (FALSE);
}


gboolean
on_browser_entry_focus_out_event       (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data)
{
	strcpy ( preferences.browser, gtk_entry_get_text(GTK_ENTRY(widget)) );
	return (FALSE);
}


gboolean
on_email_prog_entry_focus_out_event    (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data)
{
	strcpy ( preferences.email_program, gtk_entry_get_text(GTK_ENTRY(widget)) );
	return FALSE;
}


gboolean
on_messenger_entry_focus_out_event     (GtkWidget       *widget,
                                        GdkEventFocus   *event,
                                        gpointer         user_data)
{
	strcpy ( preferences.messenger, gtk_entry_get_text(GTK_ENTRY(widget)) );
	return (FALSE);
}


/*******************************************************************************
 *                         CALLBACKS FOR HELP & ABOUT                          *
 *******************************************************************************/

void
on_help_button_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
	run_browser (preferences.browser, PACKAGE_DATA_DIR "/" PACKAGE "/doc/Manual.html");
}


/*******************************************************************************
 *                  CALLBACKS FOR THE FILE SELECTION WINDOW                    *
 *******************************************************************************/

void
on_file_ok_button_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *fileselection_window, *process_widget;
	char *filename;
	void (*process_filename)();

	fileselection_window = gtk_widget_get_toplevel( GTK_WIDGET(button) );
	/* Get the filename */
	filename = (char *) gtk_file_selection_get_filename( GTK_FILE_SELECTION(fileselection_window) );
	/* Get the function which process the filename */
	process_filename = gtk_object_get_data (GTK_OBJECT(fileselection_window), "process filename");
	/* Get the widget which could be used for processing the filename */
	process_widget = gtk_object_get_data (GTK_OBJECT(fileselection_window), "process widget");
	/* Process the filename */
	(*process_filename) (filename, process_widget);

	gtk_widget_destroy (fileselection_window); /* Close the file selection window */
}


void
on_file_cancel_button_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
	/* Close the file selection window */
	gtk_widget_destroy (gtk_widget_get_toplevel( GTK_WIDGET(button) ));
}


void
on_fileselection_window_show           (GtkWidget       *widget,
                                        gpointer         user_data)
{
	/*
	 * Block the interaction with the other windows, so that
	 * the user can only use this window
	 */
	gtk_grab_add (GTK_WIDGET(widget));
}
