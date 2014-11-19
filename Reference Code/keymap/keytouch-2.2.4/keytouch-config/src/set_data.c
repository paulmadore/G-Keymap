/*----------------------------------------------------------------------------------
Name               : set_data.c
Author             : Marvin Raaijmakers
Description        : Sets data to the GUI
Date of last change: 27-May-2006
History            : 27-May-2006 reload_key_treeview() selects the first row
                     30-Apr-2006 Modified reload_plugin_function_list(),
                                 reload_plugin_list(), reload_plugin_treeview(),
                                 reload_key_treeview() and reload_current_keyboard()
                                 so that they don't take arguments to widgets.
                                 Modified init_main_window() so that it uses the
                                 'gui' structure.
                     12-Jan-2006 - Added sorted to the argument list of
                                   treeview_create_list()
                                 - init_key_treeview() now creates an unsorted list

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
------------------------------------------------------------------------------------*/
#include <stdio.h>
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <gtk/gtk.h>
#include <string.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include <keytouch.h>
#include <gui.h>
#include <set_data.h>
#include <key_settings.h>
#include <callbacks.h>

#define GTK_ALIGN_LEFT		(0)
#define GTK_ALIGN_CENTER	(0.5)

#define ONE_COLUMN	(1)
#define FIRST_COLUMN	(0) 
#define SORTID_FIRSTCOL	(0)


static gint sort_iter_compare (GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer userdata);


/* This is our data identification string to store
 * data in list items
 */
const gchar *list_item_data_key = "list_item_data";



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
*/
{
	GtkWidget *dialog;
	char *filename = NULL;

	dialog = gtk_file_chooser_dialog_new (	(gchar *)title,
						parent_window,
						0,
						GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
						GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
						NULL                                   );
	
	if (gtk_dialog_run(GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		filename = (char *)gtk_file_chooser_get_filename( GTK_FILE_CHOOSER(dialog) );
	}
	gtk_widget_destroy (dialog);
	
	return (filename);
}


void
msg_box (	gchar		*message,
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
	/*
	 * Block the interaction with the other windows, so that
	 * the user can only use this window
	 */
	gtk_grab_add (GTK_WIDGET(msg_dialog));
	gtk_dialog_run (GTK_DIALOG (msg_dialog));
	gtk_widget_destroy (msg_dialog);
	/* Wait until the window has disappeared */
	while (g_main_context_iteration(NULL, FALSE))
		; /* NULL Statement */
}


void
label_set_text (	GtkWidget	*this_widget,
			char		*label_name,
			char		*text         )
/*
Input:
	this_widget	- A widget in the same tree as the label
	label_name	- The name of the label widget
	text		- The text to set to the label
Output:
	-
Returns:
	-
Description:
	This function sets the text 'text' to the label, named label_name.
*/
{
	GtkLabel *label;
	
	label = GTK_LABEL(lookup_widget(this_widget, label_name));
	gtk_label_set_text (label, (gchar *)text);
}


void
fill_plugin_info_dialog (	GtkWidget		*this_widget,
				KTPluginInfoEntry	*plugin )
/*
Input:
	this_widget	- A widget in the plugin info dialog, or the dialog itselfs
	plugin		- The plugin to show the info of
Output:
	-
Returns:
	-
Description:
	This function sets the values of the members of plugin->info to the labels
	of the plugin info dialog.
*/
{
	char	*function_names;
	int	function_names_len,
		count;
	
	if (plugin == NULL)
	{
		return;
	}
	function_names_len = 0;
	/* Get the size of the function_names string to create */
	for (count = 0; count < plugin->num_functions; count++)
	{
		function_names_len += strlen(plugin->function_names[count]);
		if (count < plugin->num_functions-1)
		{
			function_names_len += strlen(", ");
		}
	}
	function_names = keytouch_malloc(function_names_len+1);
	function_names[0] = '\0';
	/* Create the function_names string */
	for (count = 0; count < plugin->num_functions; count++)
	{
		strcat (function_names, plugin->function_names[count]);
		if (count < plugin->num_functions-1)
		{
			strcat (function_names, ", ");
		}
	}
	label_set_text (this_widget, "plugin_functions_value_label", function_names);
	free (function_names);
	label_set_text (this_widget, "plugin_name_value_label", plugin->info.name);
	label_set_text (this_widget, "plugin_author_value_label", plugin->info.author);
	label_set_text (this_widget, "plugin_version_value_label", plugin->info.version);
	label_set_text (this_widget, "plugin_license_value_label", plugin->info.license);
	label_set_text (this_widget, "plugin_description_value_label", plugin->info.description);
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
	
	if (text != NULL)
	{
		entry = lookup_widget(this_widget, entry_name);
		gtk_entry_set_text (GTK_ENTRY(entry), text);
	}
}


char
*entry_get_text (	GtkWidget	*this_widget,
			char		*entry_name    )
/*
input:
	this_widget		- A widget in same tree as the entry
	entry_name		- The name of the entry widget
output:
	-
returns:
	A pointer to the string containing the text of the entry.
description:
	This function gets text from an entry. this_widget is a widget which is in
	the same tree as the entry to get the text from, entry_name is the name of
	the entry widget. In addition to gtk_entry_get_text() the returned pointer
	points to a string that should be freed if it is no longer needed.
	Ok the function may not SET anything to a widget but it is a brother of
	entry_set_text().
*/
{
	GtkEntry *entry;

	entry = GTK_ENTRY(lookup_widget(GTK_WIDGET(this_widget), entry_name));
	return ((char *) g_strdup_printf (gtk_entry_get_text(entry)));
}


void
list_append_item (	GtkList	*list,
			char	*item_string   )
/*
Input:
	list		- The list to append the item to
	item_string	- The string of the new list item
Output:
	-
Returns:
	-
Description:
	This function appends a list item to list. The string of the item is item_string.
*/
{
	GtkWidget *list_item;
	GtkWidget *label;

	label = gtk_label_new( item_string );
	gtk_misc_set_alignment (GTK_MISC (label), GTK_ALIGN_LEFT, GTK_ALIGN_CENTER);
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
reload_plugin_function_list (	GtkList			*plugin_function_list_widget,
				GtkObject		*plugin_function_combo_entry,
				gpointer		entry_changed_handler,
				KTPluginInfoEntry	*plugin )
/*
Input:
	plugin_function_combo_entry	- The entry of the combo
	entry_changed_handler		- The address of the function that handles
					  the "changed" signal for
					  'plugin_funtion_combo_entry'
	plugin				- The currently selected plugin
Output:
	plugin_function_list_widget	- Will contain all function names of 'plugin'
Description:
	This function deletes all list items of 'plugin_function_list_widget' and
	appends all function names of 'plugin' to the list.
	During this function entry_changed_handler will be blocked as a signal
	handler for the plugin_function_combo_entry.
*/
{
	int count;

	/* Block the signal handler of plugin_entry */
	g_signal_handlers_block_by_func (plugin_function_combo_entry, entry_changed_handler, NULL);
	
	/* Clear the plugin function list */
	gtk_list_clear_items (plugin_function_list_widget, 0, -1); /* Clear the whole list */
	
	/* Add all function names to the list */
	for (count = 0; count < plugin->num_functions; count++)
	{
		list_append_item (plugin_function_list_widget, plugin->function_names[count]);
	}
	
	/* Unblock the signal handler of plugin_entry */
	g_signal_handlers_unblock_by_func (plugin_function_combo_entry, entry_changed_handler, NULL);
}


void
reload_plugin_list (	GtkList			*plugin_list_widget,
			GtkObject		*plugin_combo_entry,
			gpointer		entry_changed_handler,
			KTPluginInfoList	*plugin_list )
/*
Input:
	plugin_combo_entry	- The entry of the combo
	entry_changed_handler	- The address of the function that handles the
				  'changed' signal for plugin_combo_entry
	plugin_list		- This is a KTPluginList to read the plugin names from
Output:
	plugin_list_widget	- The list of the combo that will contain all the
				  plugin names that appear in plugin_list
Description:
	This function deletes all list items of 'plugin_list_widget' and appends all
	plugin names that appear in 'plugin_list' to the list.
	During this function entry_changed_handler will be blocked as a signal
	handler for the plugin_combo_entry.
*/
{
	KTPluginInfoEntry *plugin;

	/* Block the signal handler of plugin_entry */
	g_signal_handlers_block_by_func (plugin_combo_entry, entry_changed_handler, NULL);
	
	/* Clear the plugin action list */
	gtk_list_clear_items (plugin_list_widget, 0, -1); /* Clear the whole list */
	plugin = plugil_first_plugin (plugin_list);
	/* Add all plugin names to the list */
	while (plugin != NULL)
	{
		list_append_item (plugin_list_widget, plugil_plugin_get_name(plugin));
		plugin = plugin->next;
	}
	
	/* Unblock the signal handler of plugin_entry */
	g_signal_handlers_unblock_by_func (plugin_combo_entry, entry_changed_handler, NULL);
}


void
reload_plugin_treeview (	GtkTreeView		*treeview,
				KTPluginInfoList	*plugin_list )
/*
Input:
	plugin_list	- This is a KTPluginList to read the plugin names from
Output:
	treeview	- Will contain all the plugin names that appear in
Description:
	This function deletes all rows in 'treeview' and appends all plugin names
	that appear in 'plugin_list'
*/
{
	KTPluginInfoEntry	*plugin;
	GtkListStore		*list_store;
	
	list_store = GTK_LIST_STORE( gtk_tree_view_get_model(treeview) );
	
	gtk_list_store_clear (list_store);/* Clear the whole treeview */
	
	plugin = plugil_first_plugin(plugin_list);
	/* Add all plugin names to the list */
	while (plugin != NULL)
	{
		list_store_append_string (list_store, plugil_plugin_get_name(plugin));
		plugin = plugin->next;
	}
}


void
reload_key_treeview (	GtkTreeView		*treeview,
			KTKeySettingsList	*key_list )
/*
Input:
	key_list	- The list of all keys and their settings
Output:
	treeview	- Will contain all key names that appear in 'key_list'
Description:
	This function deletes all rows in 'treeview' and appends all key names, that
	which appear in 'key_list'. After doing this the first row of 'treeview'
	will be selected.
*/
{
	KTKeySettings	*key;
	GtkListStore	*list_store;
	GtkTreeIter	first_iter;
	
	list_store = GTK_LIST_STORE( gtk_tree_view_get_model(treeview) );
	gtk_list_store_clear (list_store); /* Clear the whole treeview */
	key = ksl_first_key(key_list);
	/* Fill the treeview */
	while (key != NULL)
	{
		/* Add a new row to the model */
		list_store_append_string (list_store, ks_get_key_name(key));
		key = key->next;
	}
	if (gtk_tree_model_get_iter_first (GTK_TREE_MODEL( list_store ), &first_iter))
	{
		/* Select the first row */
		gtk_tree_selection_select_iter (gtk_tree_view_get_selection(treeview), &first_iter);
	}
}


gint
sort_iter_compare (GtkTreeModel *model,
                   GtkTreeIter  *a,
                   GtkTreeIter  *b,
                   gpointer      userdata)
/*
Description:
	Function for comparing two rows of a tree view.
*/
{
	gchar *string1, *string2;
	gint ret;
	
	gtk_tree_model_get(model, a, FIRST_COLUMN, &string1, -1);
	gtk_tree_model_get(model, b, FIRST_COLUMN, &string2, -1);
	
	if (string1 == NULL || string2 == NULL)
	{
		if (string1 == NULL && string2 == NULL)
		{
			ret = 0;
		}
		else
		{
			ret = (string1 == NULL) ? -1 : 1;
		}
	}
	else
	{
		ret = g_utf8_collate(string1,string2);
	}
	g_free(string1);
	g_free(string2);
	
	return (ret);
}


void
treeview_create_list (	GtkTreeView	*treeview,
			Boolean		sorted )
/*
Input:
	-
Output:
	treeview	- The GtkTreeView to make a list of
Returns:
	-
Description:
	This function makes a list of treeview. That means treeview will have one 
	treeview column which contains text. If sorted is TRUE the list will be
	sorted in alphabetical order.
*/
{
	GtkListStore		*list_store;
	GtkTreeViewColumn	*column;
	GtkCellRenderer		*renderer;
	GtkTreeSortable		*sortable;
	
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
	if (sorted)
	{
		sortable = GTK_TREE_SORTABLE(list_store);
		/* Set the compare function for sorting the list */
		gtk_tree_sortable_set_sort_func (sortable, SORTID_FIRSTCOL, sort_iter_compare, NULL, NULL);
		/* Set the sort order */
		gtk_tree_sortable_set_sort_column_id (sortable, SORTID_FIRSTCOL, GTK_SORT_ASCENDING);
	}
	gtk_tree_view_set_model (treeview, GTK_TREE_MODEL(list_store));
	g_object_unref (list_store);
}


void
init_plugin_treeview (GtkTreeView *treeview)
/*
Output:
	treeview	- Will be initialized
Description:
	This function initializes 'treeview' to be a sorted list with selection mode
	set to GTK_SELECTION_SINGLE.
*/
{
	GtkTreeSelection	*selection;
	
	selection = gtk_tree_view_get_selection (treeview);
	gtk_tree_selection_set_mode (selection, GTK_SELECTION_SINGLE);
	treeview_create_list (treeview, TRUE);
}


void
init_key_treeview (GtkTreeView *treeview)
/*
Output:
	treeview	- Will be initialized
Description:
	This function initializes 'treeview' to be an unsorted list with selection
	mode set to GTK_SELECTION_SINGLE. The selection handler of the treeview will
	be set to on_key_treeview_selection.
*/
{
	GtkTreeSelection	*selection;
	
	selection = gtk_tree_view_get_selection (treeview);
	gtk_tree_selection_set_mode (selection, GTK_SELECTION_SINGLE);
	/* Set the signal handler */
	gtk_tree_selection_set_select_function (selection, on_key_treeview_selection, NULL, NULL);
	treeview_create_list (treeview, FALSE);
}


void
reload_current_keyboard (	GtkEntry	*model_entry,
				GtkEntry	*manu_entry,
				KTKeyboardName	*current_keyboard_name )
/*
Input:
	current_keyboard_name	- The name of the current keyboard
Output:
	model_entry		- Will contain kbname_model (current_keyboard_name)
	manu_entry		- Will contain
				  kbname_manufacturer (current_keyboard_name)
*/
{
	gtk_entry_set_text (model_entry, kbname_model(current_keyboard_name));
	gtk_entry_set_text (manu_entry, kbname_manufacturer(current_keyboard_name));
}


void
reload_preferences (	KTGUI_type	*gui,
			KTPreferences	*preferences )
/*
Input:
	preferences	- These preferences must be set to the preferences entries
Output:
	gui		- The documents_entry, home_page_entry, search_page_entry,
			  browser_entry, email_prog_entry and chat_entry members
			  will contain the corresponding member in 'preferences'.
Description:
	This function sets the information of preferences to all prefecences entries
	in 'gui'.
*/
{
	gtk_entry_set_text (gui->documents_entry, preferences->documents_dir);
	gtk_entry_set_text (gui->home_page_entry, preferences->home_page);
	gtk_entry_set_text (gui->search_page_entry, preferences->search_page);
	gtk_entry_set_text (gui->browser_entry, preferences->browser);
	gtk_entry_set_text (gui->email_prog_entry, preferences->email_program);
	gtk_entry_set_text (gui->chat_entry, preferences->chat_program);
}


void
init_main_window (	KTKeyboardName		*current_keyboard_name,
			KTKeySettingsList	*key_list,
			KTPluginInfoList	*plugin_list,
			KTPreferences		*preferences,
			KTGUI_type		*gui )
/*
Input:
	current_keyboard_name	- The name of the current keyboard
	key_list		- The list of all keys and their settings
	plugin_list		- A list of all plugins
	preferences		- The user preferences
Output:
	gui			- Its members will be initialized with the address
				  of their corresponding objects in the GUI
Returns:
	-
Description:
	This function initialises the main window, by creating and showing it and setting
	the data (readen from the input variables) to all widgets of it.
*/
{
	/*
	 * Add the installation directory of the pixmaps to the location where the program
	 * should look for its pixmaps
	 */
	add_pixmap_directory (PACKAGE_DATA_DIR "/" PACKAGE "/pixmaps");
	add_pixmap_directory ("../pixmaps");
	
	gui->main_window = create_main_window();
	gtk_widget_show (gui->main_window);
	/* Initialize the rest of gui: */
	gui->plugin_treeview = GTK_TREE_VIEW( lookup_widget(gui->main_window, "plugin_treeview") );
	gui->key_treeview = GTK_TREE_VIEW( lookup_widget(gui->main_window, "key_treeview") );
	gui->key_name_label = GTK_LABEL( lookup_widget(gui->main_window, "key_name_label") );
	gui->program_entry = GTK_ENTRY( lookup_widget(gui->main_window, "program_entry") );
	gui->default_entry = GTK_ENTRY( lookup_widget(gui->main_window, "default_entry") );
	gui->plugin_combo_entry = GTK_ENTRY( lookup_widget(gui->main_window, "plugin_combo_entry") );
	gui->plugin_function_combo_entry = GTK_ENTRY( lookup_widget(gui->main_window, "plugin_function_combo_entry") );
	gui->documents_entry = GTK_ENTRY( lookup_widget(gui->main_window, "documents_entry") );
	gui->home_page_entry = GTK_ENTRY( lookup_widget(gui->main_window, "home_page_entry") );
	gui->search_page_entry = GTK_ENTRY( lookup_widget(gui->main_window, "search_page_entry") );
	gui->browser_entry = GTK_ENTRY( lookup_widget(gui->main_window, "browser_entry") );
	gui->email_prog_entry = GTK_ENTRY( lookup_widget(gui->main_window, "email_prog_entry") );
	gui->chat_entry = GTK_ENTRY( lookup_widget(gui->main_window, "chat_entry") );
	gui->default_radiobutton = GTK_RADIO_BUTTON( lookup_widget(gui->main_window, "default_radiobutton") );
	gui->program_radiobutton = GTK_RADIO_BUTTON( lookup_widget(gui->main_window, "program_radiobutton") );
	gui->special_action_radiobutton = GTK_RADIO_BUTTON( lookup_widget(gui->main_window, "special_action_radiobutton") );
	gui->plugin_combo_list = GTK_LIST( GTK_COMBO( lookup_widget(gui->main_window, "plugin_combo") )->list );
	gui->plugin_function_combo_list = GTK_LIST( GTK_COMBO( lookup_widget(gui->main_window, "plugin_function_combo") )->list );
	
	/****** Initialize the treeviews ******/
	init_plugin_treeview (gui->plugin_treeview);
	init_key_treeview (gui->key_treeview);
	/****** Load contents of the UI ******/
	reload_plugin_treeview (gui->plugin_treeview, plugin_list);
	reload_plugin_list (gui->plugin_combo_list,
	                    GTK_OBJECT( gui->plugin_combo_entry ),
	                    on_plugin_combo_entry_changed,
	                    plugin_list);
	reload_key_treeview (gui->key_treeview, key_list);
	reload_current_keyboard (GTK_ENTRY( lookup_widget(gui->main_window, "keyboard_model_entry") ),
	                         GTK_ENTRY( lookup_widget(gui->main_window, "keyboard_manufacturer_entry") ),
	                         current_keyboard_name);
	reload_preferences (gui, preferences);
	gtk_label_set_text (GTK_LABEL( lookup_widget(GTK_WIDGET(gui->main_window), "version_val_label") ), VERSION);
}
