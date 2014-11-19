/*---------------------------------------------------------------------------------
Name               : set_data.c
Author             : Marvin Raaijmakers
Description        : Sets data to the GUI
Date of last change: 22-Aug-2006
History            : 30-Aug-2006 show_selectkb_window(): added acpi_usable
                                 parameter and code that uses this parameter.

    Copyright (C) 2006-2007 Marvin Raaijmakers

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

#include <gtk/gtk.h>

#include "interface.h"
#include "support.h"
#include "callbacks.h"

#include <set_data.h>
#include <keytouch-editor.h>
#include <evdev.h>

/* This is our data identification string to store
 * data in list items
 */
const gchar *list_item_data_key = "list_item_data";


static void init_evdev_treeview (GtkTreeView *treeview);
static void tree_view_add_new_text_column (	GtkTreeView	*tree_view,
						char		*column_title,
						int		column_pos  );
static void list_append_item (GtkList *list, char *item_string);
static void list_store_append_string (	GtkListStore	*list_store,
					gchar		*string );
static void treeview_create_list (GtkTreeView *treeview);


void
clear_key_settings_frame (GtkWidget *this_widget)
/*
Input:
	this_widget	- A widget in the same tree as the widgets in the frame
Description:
	This function clears the contents of the following widgets:
	- key_name_entry
	- scancode_entry
	- keycode_combobox
	- program_entry
	- plugin_comboentry
	- plugin_function_comboentry
*/
{
	gtk_entry_set_text (GTK_ENTRY( lookup_widget(this_widget, "key_name_entry") ), "");
	gtk_entry_set_text (GTK_ENTRY( lookup_widget(this_widget, "scancode_entry") ), "");
	gtk_combo_box_set_active (GTK_COMBO_BOX( lookup_widget(this_widget, "keycode_combobox") ), -1);
	gtk_entry_set_text (GTK_ENTRY( lookup_widget(this_widget, "program_entry") ), "");
	gtk_entry_set_text (GTK_ENTRY( lookup_widget(this_widget, "plugin_comboentry") ), "");
	gtk_entry_set_text (GTK_ENTRY( lookup_widget(this_widget, "plugin_function_comboentry") ), "");
}


void
msg_box (	gchar		*message,
		GtkMessageType	msg_type       )
/*
Input:
	message		- The message to display
	msg_type	- The type of the message
Output:
	-
Returns:
	-
Description:
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
gui_error_handler (const char *msg)
/*
Input:
	msg		- The message to display
Output:
	-
Returns:
	-
Description:
	This function calls msg_box(msg, GTK_MESSAGE_ERROR)
*/
{
	msg_box ((gchar *)msg, GTK_MESSAGE_ERROR);
}


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
	gtk_misc_set_alignment (GTK_MISC (label), GTK_ALIGN_LEFT, GTK_ALIGN_CENTER);
	list_item = gtk_list_item_new();
	gtk_container_add (GTK_CONTAINER(list_item), label);
	gtk_widget_show (label);
	gtk_container_add (GTK_CONTAINER(list), list_item);
	gtk_widget_show (list_item);
	gtk_object_set_data (GTK_OBJECT(list_item), list_item_data_key, item_string);
}


void
reload_plugin_function_list (	GtkWidget	*this_widget,
				KTPlugin	*plugin       )
/*
Input:
	this_widget	- A widget in the same tree as the widget named
			  "plugin_combo"
	plugin		- The address of the currently selected plugin
Description:
	This function deletes all list items of a list widget and appends all
	functions names, of plugin, to the list. The list is a widget of a combo
	named "plugin_function_combo", which is in the same tree as this_widget.
	The list will only be cleared if plugin points to NULL.
*/
{
	GtkCombo	*plugin_function_combo;
	GtkWidget	*plugin_function_combo_entry;
	GtkList		*plugin_function_list_widget;
	int		count;

	plugin_function_combo_entry = lookup_widget(this_widget, "plugin_function_comboentry");
	/* Block the signal handler of plugin_entry */
	g_signal_handlers_block_by_func (GTK_OBJECT(plugin_function_combo_entry), 
	                                  on_plugin_function_comboentry_changed, NULL);
	
	plugin_function_combo = GTK_COMBO(lookup_widget(this_widget, "plugin_function_combo"));
	
	plugin_function_list_widget = GTK_LIST(plugin_function_combo->list);
	/* Clear the plugin function list */
	gtk_list_clear_items (plugin_function_list_widget, 0, -1); /* Clear the whole list */
	
	if (plugin)
	{
		/* Add all function names to the list */
		for (count = 0; count < plugin->num_functions; count++)
		{
			list_append_item (plugin_function_list_widget, plugin->function[count]);
		}
	}
	
	/* Unblock the signal handler of plugin_entry */
	g_signal_handlers_unblock_by_func (GTK_OBJECT(plugin_function_combo_entry), 
	                                    on_plugin_function_comboentry_changed, NULL);
}


void
reload_plugin_list (	GtkWidget	*this_widget,
			const KTPlugin	plugin_list[],
			int		num_plugins  )
/*
input:
	this_widget	- A widget in the same tree as the widget named
			  "plugin_combo"
	plugin_list	- This is array of all available plugins
	num_plugins	- Size of 'plugin_list' array
output:
	-
returns:
	-
description:
	This function deletes all list items of a list widget and appends all plugin names,
	which it will read from plugin_list, to the list. The list is a widget of a combo
	named "plugin_combo", which is in the same tree as this_widget.
*/
{
	GtkCombo		*plugin_combo;
	GtkWidget		*plugin_combo_entry;
	GtkList			*plugin_list_widget;
	int i;
	
	plugin_combo_entry = lookup_widget(this_widget, "plugin_comboentry");
	/* Block the signal handler of plugin_entry */
	g_signal_handlers_block_by_func (GTK_OBJECT(plugin_combo_entry), on_plugin_comboentry_changed, NULL);
	
	plugin_combo = GTK_COMBO(lookup_widget(this_widget, "plugin_combo"));
	
	plugin_list_widget = GTK_LIST(plugin_combo->list);
	/* Clear the plugin action list */
	gtk_list_clear_items (plugin_list_widget, 0, -1); /* Clear the whole list */
	/* Add all plugin names to the list */
	for (i = 0; i < num_plugins; i++)
	{
		list_append_item (plugin_list_widget, KTPlugin_get_name(plugin_list[i]));
	}
	
	/* Unblock the signal handler of plugin_entry */
	g_signal_handlers_unblock_by_func (GTK_OBJECT(plugin_combo_entry), on_plugin_comboentry_changed, NULL);
}


void
list_store_append_string (	GtkListStore	*list_store,
				gchar		*string )
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
treeview_create_list (GtkTreeView *treeview)
/*
Output:
	treeview	- The GtkTreeView to make a list of
Description:
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
reload_key_treeview (	KTKeyList	*list,
			GtkWidget	*this_widget )
{
	KTKey		*key;
	GtkTreeView	*tree_view;
	GtkListStore	*list_store;
	
	tree_view = GTK_TREE_VIEW( lookup_widget(this_widget, "key_treeview") );
	list_store = GTK_LIST_STORE( gtk_tree_view_get_model(tree_view) );
	
	gtk_list_store_clear (list_store);/* Clear the whole treeview */
	/* Fill the treeview */
	for (key = KTKeyList_head(list); key; key = key->next)
	{
		/* Add a new row to the model */
		list_store_append_string (list_store, KTKey_get_name(key));
	}
}


void
tree_view_add_new_text_column (	GtkTreeView	*tree_view,
				char		*column_title,
				int		column_pos  )
/*
Input:
	column_title	- The title of the new column
	column_pos	- The position of the new column
Output:
	tree_view	- The tree view to add the new text column to
Returns:
	-
Description:
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
init_evdev_treeview (GtkTreeView *treeview)
/*
Output:
	treeview	- Will have a "Device", "Device name" and a "Bus" column
Description:
	This function makes a list of treeview with three columns: "Device", "Device
	name" and a "Bus". Only one row in this list can be selected.
*/
{
	GtkTreeSelection	*selection;
	GtkListStore		*list_store;
	
	selection = gtk_tree_view_get_selection( GTK_TREE_VIEW(treeview) );
	gtk_tree_selection_set_mode (selection, GTK_SELECTION_SINGLE);
	
	/************ Create the model ************/
	/* The last column with be 'invisible' and is a pointer to the
	   corresponding EVDEV_DEVICE structure. */
	list_store = gtk_list_store_new(NUM_COLS_EVDEV_TREEVIEW, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER);
	gtk_tree_view_set_model (treeview, GTK_TREE_MODEL(list_store));
	g_object_unref (list_store);
	
	/************ Create the view ************/
	tree_view_add_new_text_column (treeview, _("Device"), COL_EVDEV_FILE_NAME);
	tree_view_add_new_text_column (treeview, _("Device name"), COL_EVDEV_DEVICE_NAME);
	tree_view_add_new_text_column (treeview, _("Bus"), COL_EVDEV_BUS);
}


GtkWidget
*show_selectkb_window (EVDEV_LIST *evdev_list,
                       Boolean    acpi_usable )
/*
Input:
	evdev_list	- List containing information about all event devices
	acpi_usable	- Indicates if an entry for the ACPI device should be add to
			  the keyboard_treeview
Returns:
	The address of the created window
Description:
	This function creates and initializes the keyboard_treeview in the
	selectkb_window and fills it with the following information about the event
	devices in evdev_list: device (file name), device name and bus. If
	'acpi_usable' is TRUE then an entry for the ACPI device will be added to the
	end of the keyboard_treeview.
*/
{
	EVDEV_LIST_ENTRY	*entry;
	GtkTreeView		*treeview;
	GtkListStore		*list_store;
	GtkTreeIter		iter;
	GtkWidget		*window;
	
	window = create_selectkb_window();
	
	treeview = GTK_TREE_VIEW(lookup_widget(window, "keyboard_treeview"));
	init_evdev_treeview (treeview);
	
	list_store = GTK_LIST_STORE( gtk_tree_view_get_model(treeview) );
	
	gtk_list_store_clear (list_store); /* Clear the whole treeview */
	/* Fill the treeview */
	for (entry = evdev_list->head; entry; entry = entry->next)
	{
		/* Add a new row to the model */
		gtk_list_store_append (list_store, &iter);
		gtk_list_store_set (	list_store, &iter,
					COL_EVDEV_FILE_NAME, evdev_file_name(entry->evdev),
					COL_EVDEV_DEVICE_NAME, evdev_device_name(entry->evdev),
					COL_EVDEV_BUS, evdev_bus_string(entry->evdev),
					COL_EVDEV_STRUCT, entry->evdev,
					-1 );
	}
	if (acpi_usable)
	{
		gtk_list_store_append (list_store, &iter);
		gtk_list_store_set (	list_store, &iter,
					COL_EVDEV_FILE_NAME, ACPI_EVDEV_FILE_NAME,
					COL_EVDEV_DEVICE_NAME, ACPI_DEVICE_NAME,
					COL_EVDEV_BUS, ACPI_BUS_NAME,
					COL_EVDEV_STRUCT, NULL,
					-1 );
	}
	gtk_widget_show (window);
	
	return window;
}


GtkWidget
*show_main_window (MainWindowGUI_type *main_window_gui)
/*
Output:
	main_window_gui - Will be intialised with pointers to the
	                  created widgets
Returns:
	The address of the main window.
Description:
	This function creates the main window and shows it.
*/
{
	GtkWidget *window;
	int index;
	
	main_window_gui->main_window = create_main_window();
	window = main_window_gui->main_window;
	
	/* main_window_gui->keycode_combobox = GTK_COMBO_BOX(lookup_widget(window, "keycode_combobox")); */
#define _gui_init_widget(widget_name, type) (main_window_gui->widget_name = type(lookup_widget(window, #widget_name)))
	_gui_init_widget (keycode_combobox, GTK_COMBO_BOX);
	_gui_init_widget (scancode_event_descr_label, GTK_LABEL);
	_gui_init_widget (manufacturer_entry, GTK_ENTRY);
	_gui_init_widget (model_entry, GTK_ENTRY);
	_gui_init_widget (key_name_entry, GTK_ENTRY);
	_gui_init_widget (scancode_entry, GTK_ENTRY);
	_gui_init_widget (usbcode_entry, GTK_ENTRY);
	_gui_init_widget (program_entry, GTK_ENTRY);
	_gui_init_widget (plugin_function_comboentry, GTK_ENTRY);
	_gui_init_widget (plugin_comboentry, GTK_ENTRY);
	_gui_init_widget (program_radiobutton, GTK_RADIO_BUTTON);
	_gui_init_widget (special_action_radiobutton, GTK_RADIO_BUTTON);
	_gui_init_widget (key_treeview, GTK_TREE_VIEW);
	_gui_init_widget (keycode_combobox, GTK_COMBO_BOX);
	_gui_init_widget (usbcode_change_button, GTK_BUTTON);
	_gui_init_widget (scancode_change_button, GTK_BUTTON);
	
	for (index = 0; visible_keycodes[index]; index++)
	{
		gtk_combo_box_append_text (main_window_gui->keycode_combobox,
		                           keycode2string(visible_keycodes[index]));
	}

	reload_plugin_list (window, plugin_list, NUM_PLUGINS);
	
	/* main_window_gui->key_treeview = GTK_TREE_VIEW(lookup_widget(window, "key_treeview")); */
	treeview_create_list (main_window_gui->key_treeview);
	/* Set the signal handler */
	gtk_tree_selection_set_select_function (gtk_tree_view_get_selection(main_window_gui->key_treeview),
	                                        on_key_treeview_selection, NULL, NULL);
	
	gtk_widget_show (window);
	
	return window;
}

