/*-------------------------------------------------------------------------------
Name               : set_data.c
Author             : Marvin Raaijmakers
Description        : Sets data to widgets
Date of last change: 15-December-2004
History            :
                     2-July-2004      Created msg_box()
                     7-July-2004      Created:
                                      - entry_set_text()
                                      - entry_set_max_length()
                                      - reload_preferences()
                                      Modified init_main_window()
                     12-July-2004     Created reload_plugin_list_widget()
                                      Modified reload_special_action_list()
                     27-July-2004     Created icon_init()
                     7-August-2004    Wrote some comments for some functions
                     8-August-2004    ,, (finished it for all functions)
                     15-December-2004 Created:
                                      - treeview_create_list()
                                      - reload_key_treeview()
                                      - reload_plugin_treeview()
                     16-December-2004 Created:
                                      - init_key_treeview()
                                      - init_plugin_treeview()

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
#include <stdio.h>
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include <keytouch.h>
#include <set_data.h>
#include <keyboard_config.h>
#include <keyboard_list.h>
#include <plugin_list.h>
#include <callbacks.h>

#define GTK_ALIGN_LEFT		(0)
#define GTK_ALIGN_CENTRE	(0.5)

#define ONE_COLUMN	(1)
#define FIRST_COLUMN	(0)



/* This is our data identification string to store
 * data in list items
 */
const gchar *list_item_data_key = "list_item_data";

static void entry_set_max_length (GtkWidget *this_widget, char *entry_name, int max_size);
static void entry_set_text (GtkWidget *this_widget, char *entry_name, char *text);
static void reload_plugin_list_widget (GtkList *plugin_list_widget, PLUGIN_LIST *plugin_list);
static void list_append_item (GtkList *list, char *item_string);
static void list_store_append_string (GtkListStore *list_store, char *string);
static void treeview_create_list (GtkTreeView *treeview);
static void init_key_treeview (GtkWidget *this_widged);
static void init_plugin_treeview (GtkWidget *this_widged);
static void tree_view_add_new_text_column (GtkTreeView *tree_view, char *column_title, int column_pos);
static void keyboard_treeview_append_keyboard (GtkListStore *list_store, char *manufacture, char *model);


void
entry_set_max_length (	GtkWidget	*this_widget,
			char		*entry_name,
			int		max_size      )
/*
input:
	this_widget		- A widget in same tree as the entry
	entry_name		- The name of the entry widget
	max_size		- The maximum lenght of an entry
output:
	-
returns:
	-
description:
	This function sets a maximum length to an entry. this_widget is a widget which is
	in the same tree as the entry to change the maximum length of, entry_name is the
	name of the entry widget and the maximum length of the entry will be set to
	max_size.
*/
{
	GtkWidget *entry;

	entry = lookup_widget(GTK_WIDGET(this_widget), entry_name);
	gtk_entry_set_max_length (GTK_ENTRY(entry), max_size);
}


void
entry_set_text (	GtkWidget	*this_widget,
			char		*entry_name,
			char		*text             )
/*
input:
	this_widget		- A widget in same tree as the entry
	entry_name		- The name of the entry widget
	text			- The new text of the entry
output:
	-
returns:
	-
description:
	This function sets a new text to an entry. this_widget is a widget which is in
	the same tree as the entry to set te new text to, entry_name is the name of the
	entry widget and the text of the entry will be set to text.
*/
{
	GtkWidget *entry;

	entry = lookup_widget(GTK_WIDGET(this_widget), entry_name);
	gtk_entry_set_text (GTK_ENTRY(entry), text);
}


void
msg_box (	char		*message,
		GtkMessageType	msg_type       )
/*
input:
	message		- The message to display
	msg_type	- The type of the message
output:
	-
returns:
	-
description:
	msg_box() shows a message dialog containing text and an ok-button. The text of
	the dialog will be message. The type of the message is indicated by msg_type.
*/
{
	GtkWidget *msg_dialog;

	msg_dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, msg_type, GTK_BUTTONS_OK, message);
	gtk_dialog_run (GTK_DIALOG (msg_dialog));
	gtk_widget_destroy (msg_dialog);
	/*
	 * Block the interaction with the other windows, so that
	 * the user can only use this window
	 */
	//gtk_grab_add (GTK_WIDGET(msg_dialog));
}


/* Wait for this until GTK+2.4 is standard:
char
*get_filename_from_user (	char		*title,
				GtkWindow	*parent_window   )
/*
input:
	title		- The title of the file chooser dialog.
	parent_window	- The parent window of the file chooser dialog.
output:
	-
returns:
	The filename of the user choosed. If the user did not choose a file it returns
	NULL.
description:
	This function gets a filename from the user by using a file chooser dialog. The
	title of this dialog is title (the input variable).
*
{
	GtkWidget *dialog;
	char *filename = NULL;

	dialog = gtk_file_chooser_dialog_new (	title,
						parent_window,
						0,
						GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
						GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
						NULL                                   );
	
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		filename = gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(dialog) );
		open_file (filename);
	}
	gtk_widget_destroy (dialog);
	
	return (filename);
}*/


void
list_append_item (	GtkList	*list,
			char	*item_string   )
/*
input:
	list		- The list to append the item to
	item_string	- The string of the new list item
output:
	-
returns:
	-
description:
	This function appends a list item to list. The string of the item is item_string.
*/
{
	GtkWidget *list_item;
	GtkWidget *label;

	label = gtk_label_new( item_string );
	gtk_misc_set_alignment (GTK_MISC (label), GTK_ALIGN_LEFT, GTK_ALIGN_CENTRE);
	list_item = gtk_list_item_new();
	gtk_container_add (GTK_CONTAINER(list_item), label);
	gtk_widget_show (label);
	gtk_container_add (GTK_CONTAINER(list), list_item);
	gtk_widget_show (list_item);
	gtk_object_set_data (GTK_OBJECT(list_item), list_item_data_key, item_string);
}


void
list_store_append_string (	GtkListStore	*list_store,
				char		*string        )
/*
input:
	string		- The string to append to list_store
output:
	list_store	- The GtkListStore where the string was appended to
returns:
	-
description:
	This function appends string as a new row to list_store.
*/
{
	GtkTreeIter iter;
	
	gtk_list_store_append (list_store, &iter);
	gtk_list_store_set (list_store, &iter, FIRST_COLUMN, string, -1);
}


void
tree_view_add_new_text_column (	GtkTreeView	*tree_view,
				char		*column_title,
				int		column_pos         )
/*
input:
	column_title	- The title of the new column
	column_pos	- The position of the new column
output:
	tree_view	- The tree view to add the new text column to
returns:
	-
description:
	This function adds a new text column to tree_view at the position column_pos.
*/
{
	GtkTreeViewColumn	*column;
	GtkCellRenderer		*renderer;
	
	column = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title (column, column_title);
	/* pack tree view column into tree view */
	gtk_tree_view_append_column (tree_view, column);
	renderer = gtk_cell_renderer_text_new();
	/* pack cell renderer into tree view column */
	gtk_tree_view_column_pack_start (column, renderer, TRUE);
	/* connect 'text' property of the cell renderer to
	 * model column that contains the text */
	gtk_tree_view_column_add_attribute (column, renderer, "text", column_pos);
}


void
reload_plugin_list_widget (	GtkList		*plugin_list_widget,
				PLUGIN_LIST	*plugin_list          )
/*
input:
	plugin_list		- List containing all available plugins
output:
	plugin_list_widget	- The list (widget) containing the plugin names
returns:
	-
description:
	This function deletes all list items of plugin_list_widget and appends all plugin
	names, which it will read from plugin_list, to the list(widget).
*/
{
	PLUGIN *plugin;
	
	gtk_list_clear_items (plugin_list_widget, 0, -1); /* Clear the whole list */
	plugin = plugl_first_plugin(plugin_list);
	/* Add all plugin names to the list */
	while (plugin)
	{
		list_append_item (GTK_LIST(plugin_list_widget), plugl_plugin_get_name(plugin));
		plugin = plugin->next;
	}
	//gtk_list_select_item (GTK_LIST(plugin_list_widget), 0); /* Select the first item of the list */
}


void
reload_plugin_treeview (	GtkWidget	*this_widget,
				PLUGIN_LIST	*plugin_list       )
/*
input:
	this_widget	- A widget in the same tree as the widget named "plugin_list"
	plugin_list	- This is a PLUGIN_LIST to read the plugin names from
output:
	-
returns:
	-
description:
	This function deletes all rows of a treeview named "plugin_treeview", which is in
	the same tree as this_widget, and appends all plugin names, which it will read form
	plugin_list, to the list.
*/
{
	PLUGIN		*plugin;
	GtkTreeView	*plugin_treeview;
	GtkListStore	*list_store;
	
	plugin_treeview = GTK_TREE_VIEW( lookup_widget(this_widget, "plugin_treeview") );
	list_store = GTK_LIST_STORE( gtk_tree_view_get_model(plugin_treeview) );
	
	gtk_list_store_clear (list_store);/* Clear the whole treeview */
	
	plugin = plugl_first_plugin(plugin_list);
	/* Add all plugin names to the list */
	while (plugin)
	{
		list_store_append_string (list_store, plugl_plugin_get_name(plugin));
		plugin = plugin->next;
	}
}


void
reload_special_action_list (	GtkWidget	*this_widget,
				PLUGIN_LIST	*plugin_list  )
/*
input:
	this_widget	- A widget in the same tree as the widget named
			  "special_action_combo"
	plugin_list	- This is a PLUGIN_LIST to read the plugin names from
output:
	-
returns:
	-
description:
	This function deletes all list items of a list widget and appends all plugin names,
	which it will read from plugin_list, to the list. The list is a widget of a combo
	named "special_action_combo", which is in the same tree as this_widget.
*/
{
	GtkCombo *special_action_combo;
	GtkWidget *special_action_combo_entry;
	GtkList *plugin_list_widget;
	PLUGIN *plugin;

	special_action_combo_entry = lookup_widget(this_widget, "special_action_combo_entry");
	/* Block the signal handler of special_action_entry */
	gtk_signal_handler_block_by_func (GTK_OBJECT(special_action_combo_entry), on_special_action_combo_entry_changed, NULL);
	
	special_action_combo = GTK_COMBO(lookup_widget(this_widget, "special_action_combo"));
	
	plugin_list_widget = GTK_LIST(special_action_combo->list);
	/* Reload the special action list */
	gtk_list_clear_items (plugin_list_widget, 0, -1); /* Clear the whole list */
	plugin = plugl_first_plugin(plugin_list);
	/* Add all plugin names to the list */
	while (plugin)
	{
		list_append_item (plugin_list_widget, plugl_plugin_get_name(plugin));
		plugin = plugin->next;
	}
	
	/* Unblock the signal handler of special_action_entry */
	gtk_signal_handler_unblock_by_func (GTK_OBJECT(special_action_combo_entry), on_special_action_combo_entry_changed, NULL);
}


void
keyboard_treeview_append_keyboard (	GtkListStore	*list_store,
					char		*manufacture,
					char		*model         )
/*
input:
	manufacture	- The name of the keyboards manufacture
	model		- The model of the keyboard
output:
	list_store	- The GtkListStore to add the new keyboard to
returns:
	-
description:
	This function appends a keyboard to list_store.
*/
{
	GtkTreeIter	iter;
	
	gtk_list_store_append (list_store, &iter);
	gtk_list_store_set (	list_store, &iter,
				COL_KEYBOARD_MANUFACTURE, manufacture,
				COL_KEYBOARD_MODEL, model,
				-1 );
}


void
reload_keyboard_treeview (	GtkWidget	*this_widget,
				KEYBOARD_LIST	*keyboard_list      )
/*
input:
	this_widget		- A widget in the same tree as the widget named "key_treeview"
	keyboard_list		- List containing keyboards
output:
	-
returns:
	-
description:
	This function deletes all rows of a treeview named "keyboard_treeview", which is
	in the same tree as this_widget, and appends all keyboard names, which it will
	read form keyboard_list, to the list.
*/
{
	KEYBOARD_MANUFACTURE	*manufacture;
	KEYBOARD_MODEL		*model;
	GtkTreeView		*keyboard_treeview;
	GtkListStore		*list_store;
	GtkTreeIter		iter;
	
	keyboard_treeview = GTK_TREE_VIEW( lookup_widget(this_widget, "keyboard_treeview") );
	list_store = GTK_LIST_STORE( gtk_tree_view_get_model(keyboard_treeview) );
	
	gtk_list_store_clear (list_store); /* Clear the whole treeview */

	manufacture = kbl_first_manufacture(keyboard_list);
	/* Fill the treeview */
	while (manufacture)
	{
		model = kbl_first_model(manufacture);
		while (model)
		{
			/* Add a new row to the model */
			keyboard_treeview_append_keyboard (	list_store,
								kbl_manufacture_get_name(manufacture),
								kbl_model_get_name(model) );
			model = model->next;
		}
		manufacture = manufacture->next;
	}
}


void
reload_key_treeview (	GtkWidget	*this_widget,
			KEYBOARD_CONFIG	*keyboard_config     )
/*
input:
	this_widget		- A widget in the same tree as the widget named "key_treeview"
	keyboard_config		- The current keyboard configuration
output:
	-
returns:
	-
description:
	This function deletes all rows of a treeview named "key_treeview", which is in
	the same tree as this_widget, and appends all key names, which it will read form
	keyboard_config, to the list.
*/
{
	KEY_SETTINGS	*key_settings;
	GtkTreeView	*key_treeview;
	GtkListStore	*list_store;
	
	key_treeview = GTK_TREE_VIEW( lookup_widget(this_widget, "key_treeview") );
	list_store = GTK_LIST_STORE( gtk_tree_view_get_model(key_treeview) );
	
	gtk_list_store_clear (list_store);/* Clear the whole treeview */

	key_settings = ksl_first_key( kbcfg_key_settings_list(keyboard_config) );
	/* Fill the treeview */
	while (key_settings)
	{
		/* Add a new row to the model */
		list_store_append_string (list_store, ks_get_key_name(key_settings));
		key_settings = key_settings->next;
	}
}


void
treeview_create_list (GtkTreeView *treeview)
/*
input:
	-
output:
	treeview	- The GtkTreeView to make a list of
returns:
	-
description:
	This function makes a list of treeview. That means treeview will have one 
	treeview column which contains text.
*/
{
	GtkListStore		*list_store;
	GtkTreeViewColumn	*column;
	GtkCellRenderer		*renderer;
	
	column = gtk_tree_view_column_new();
	/* pack tree view column into tree view */
	gtk_tree_view_append_column (treeview, column);
	renderer = gtk_cell_renderer_text_new();
	/* pack cell renderer into tree view column */
	gtk_tree_view_column_pack_start (column, renderer, TRUE);
	/* connect 'text' property of the cell renderer to
	 * model column that contains the text
	 */
	gtk_tree_view_column_add_attribute (column, renderer, "text", FIRST_COLUMN);
	
	list_store = gtk_list_store_new(ONE_COLUMN, G_TYPE_STRING);
	gtk_tree_view_set_model (treeview, GTK_TREE_MODEL(list_store));
	g_object_unref (list_store);
}


void
init_keyboard_treeview (GtkWidget *this_widget)
/*
input:
	this_widget		- A widget in the same tree as the widget named
				  "keyboard_treeview"
output:
	-
returns:
	-
description:
	This function initializes a treeview named "keyboard_treeview", which is in
	the same tree as this_widget.
*/
{
	GtkTreeView		*keyboard_treeview;
	GtkTreeSelection	*selection;
	GtkListStore		*list_store;
		
	keyboard_treeview = GTK_TREE_VIEW( lookup_widget(this_widget, "keyboard_treeview") );
	selection = gtk_tree_view_get_selection( GTK_TREE_VIEW(keyboard_treeview) );
	gtk_tree_selection_set_mode (selection, GTK_SELECTION_SINGLE);
	
	/************ Create the model ************/
	list_store = gtk_list_store_new(NUM_COLS_KEYBOARD_LIST, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model (keyboard_treeview, GTK_TREE_MODEL(list_store));
	g_object_unref (list_store);
	
	/************ Create the view ************/
	tree_view_add_new_text_column (keyboard_treeview, _("Manufacturer"), COL_KEYBOARD_MANUFACTURE);
	tree_view_add_new_text_column (keyboard_treeview, _("Model"), COL_KEYBOARD_MODEL);
}


void
init_plugin_treeview (GtkWidget *this_widget)
/*
input:
	this_widget		- A widget in the same tree as the widget named "plugin_treeview"
output:
	-
returns:
	-
description:
	This function initializes atreeview named "kplugin_treeview", which is in
	the same tree as this_widget.
*/
{
	GtkTreeView	*plugin_treeview;
	GtkTreeSelection  *selection;
	
	plugin_treeview = GTK_TREE_VIEW( lookup_widget(this_widget, "plugin_treeview") );
	
	selection = gtk_tree_view_get_selection( GTK_TREE_VIEW(plugin_treeview) );
	gtk_tree_selection_set_mode (selection, GTK_SELECTION_NONE);
	treeview_create_list (plugin_treeview);
}


void
init_key_treeview (GtkWidget *this_widget)
/*
input:
	this_widget		- A widget in the same tree as the widget named "key_treeview"
output:
	-
returns:
	-
description:
	This function initializes a treeview named "key_treeview", which is in
	the same tree as this_widget.
*/
{
	GtkTreeView	*key_treeview;
	GtkTreeSelection  *selection;
	
	key_treeview = GTK_TREE_VIEW( lookup_widget(this_widget, "key_treeview") );
	selection = gtk_tree_view_get_selection( GTK_TREE_VIEW(key_treeview) );
	gtk_tree_selection_set_mode (selection, GTK_SELECTION_SINGLE);
	/* Set the signal handler */
	gtk_tree_selection_set_select_function (selection, on_key_treeview_selection, NULL, NULL);
	treeview_create_list (key_treeview);
}


void
reload_current_keyboard (	GtkWidget	*this_widget,
				KEYBOARD_CONFIG	*current_keyboard_config )
/*
input:
	this_widget		- A widget which in the same tree as the widgets named 
				  "keyboard_model_statusbar" and
				  "keyboard_manufacture_statusbar"
	current_keyboard_config	- The current keyboard configuration
output:
	-
returns:
	-
description:
	This function pushes the keyboard model and manufacture in "keyboard_model_statusbar"
	and "keyboard_manufacture_statusbar" respectively. These two statusbars must be
	in the same tree as this_widget.
*/
{
	entry_set_text (this_widget, "keyboard_model_entry", kbcfg_model(current_keyboard_config));
	entry_set_text (this_widget, "keyboard_manufacture_entry", kbcfg_manufacture(current_keyboard_config));
}


void
reload_preferences (	GtkWidget	*this_widget,
			PREFERENCES	preferences   )
/*
input:
	this_widget		- A widget which in the same tree as the preferences
				  entries
	preferences		- These preferences will be set to the preferences entries
output:
	-
returns:
	-
description:
	This function sets the information of preferences to all prefecences entries. These
	entries must be in the same tree as this widget.
*/
{
	entry_set_text (this_widget, "documents_entry", preferences.documents_dir);
	entry_set_text (this_widget, "home_page_entry", preferences.home_page);
	entry_set_text (this_widget, "search_page_entry", preferences.search_page);
	entry_set_text (this_widget, "browser_entry", preferences.browser);
	entry_set_text (this_widget, "email_prog_entry", preferences.email_program);
	entry_set_text (this_widget, "messenger_entry", preferences.messenger);
}


void
init_main_window (	KEYBOARD_CONFIG	*current_keyboard_config,
			PLUGIN_LIST	*plugin_list,
			PREFERENCES	preferences        )
/*
input:
	current_keyboard_config	- The current keyboard configuration
	plugin_list		- A list of all plugins
	preferences		- The user preferences
output:
	-
returns:
	-
description:
	This function initialises the main window, by creating and showing it and setting
	the data (readen from the input variables) to all widgets of it.
*/
{
	GtkWidget *main_window;

	/*
	 * Add the installation directory of the pixmaps to the location where the program
	 * should look for its pixmaps
	 */
	add_pixmap_directory (PACKAGE_DATA_DIR "/" PACKAGE "/pixmaps");
	add_pixmap_directory ("../pixmaps");
	
	main_window = create_main_window ();
	gtk_widget_show (main_window);
	
	/****** Set maximum lengths to entries ******/
	entry_set_max_length (main_window, "program_entry", MAX_PROG_SIZE);
	entry_set_max_length (main_window, "documents_entry", MAX_FILE_SIZE);
	entry_set_max_length (main_window, "home_page_entry", MAX_FILE_SIZE);
	entry_set_max_length (main_window, "search_page_entry", MAX_FILE_SIZE);
	entry_set_max_length (main_window, "browser_entry", MAX_PROG_SIZE);
	entry_set_max_length (main_window, "email_prog_entry", MAX_PROG_SIZE);
	entry_set_max_length (main_window, "messenger_entry", MAX_PROG_SIZE);
	/****** Initialize some treeviews ******/
	init_plugin_treeview (main_window);
	init_key_treeview (main_window);
	/****** Load contents of the UI ******/
	reload_plugin_treeview (main_window, plugin_list);
	reload_special_action_list (main_window, plugin_list);
	reload_key_treeview (main_window, current_keyboard_config); /* Note: Execute this function AFTER reload_special_action_list() */
	reload_current_keyboard (main_window, current_keyboard_config);
	reload_preferences (main_window, preferences);
	gtk_label_set_text (GTK_LABEL( lookup_widget(GTK_WIDGET(main_window), "version_val_label") ), VERSION);

	gtk_main ();
}
