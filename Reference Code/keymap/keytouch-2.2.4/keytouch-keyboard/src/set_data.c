/*---------------------------------------------------------------------------------
Name               : set_data.c
Author             : Marvin Raaijmakers
Description        : Sets data to the GUI
Date of last change: 17-Jun-2006
History            : 17-Jun-2006 Modified init_keyboard_dialog()

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
#include <stdio.h>
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include <keytouch-keyboard.h>
#include <set_data.h>
#include <callbacks.h>

#define GTK_ALIGN_LEFT		(0)
#define GTK_ALIGN_CENTER	(0.5)

#define ONE_COLUMN	(1)
#define FIRST_COLUMN	(0) 
#define SORTID_FIRSTCOL	(0)


static void keyboard_treeview_append_keyboard (	GtkListStore	*list_store,
						char		*manufacturer,
						char		*model         );
static void tree_view_add_new_text_column (	GtkTreeView	*tree_view,
						char		*column_title,
						int		column_pos       );
static void entry_set_text (	GtkWidget	*this_widget,
				char		*entry_name,
				char		*text             );
static void reload_current_keyboard (	GtkWidget	*this_widget,
					KTKeyboardName	*current_keyboard_name );

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
	
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
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


void
reload_current_keyboard (	GtkWidget	*this_widget,
				KTKeyboardName	*current_keyboard_name )
/*
input:
	this_widget		- A widget which in the same tree as the widgets named 
				  "keyboard_model_statusbar" and
				  "keyboard_manufacture_statusbar"
	current_keyboard_name	- The name of the current keyboard
output:
	-
returns:
	-
description:
	This function pushes the keyboard model and manufacturer in "model_entry"
	and "manufacturer_entry" respectively. These two entries must be int the
	same tree as this_widget.
*/
{
	entry_set_text (this_widget, "model_entry", kbname_model(current_keyboard_name));
	entry_set_text (this_widget, "manufacturer_entry", kbname_manufacturer(current_keyboard_name));
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
keyboard_treeview_append_keyboard (	GtkListStore	*list_store,
					char		*manufacturer,
					char		*model         )
/*
input:
	manufacturer	- The name of the keyboards manufacturer
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
				COL_KEYBOARD_MANUFACTURER, manufacturer,
				COL_KEYBOARD_MODEL, model,
				-1 );
}


void
reload_keyboard_treeview (	GtkWidget	*this_widget,
				KTKeyboardList	*keyboard_list      )
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
	KTKeyboardManufacturer	*manufacturer;
	KTKeyboardModel		*model;
	GtkTreeView		*keyboard_treeview;
	GtkListStore		*list_store;
	GtkTreeIter		iter;
	
	keyboard_treeview = GTK_TREE_VIEW( lookup_widget(this_widget, "keyboard_treeview") );
	list_store = GTK_LIST_STORE( gtk_tree_view_get_model(keyboard_treeview) );
	
	gtk_list_store_clear (list_store); /* Clear the whole treeview */

	/* Fill the treeview */
	for (manufacturer = keyboard_list->head;
	     manufacturer != NULL;
	     manufacturer = manufacturer->next)
	{
		for (model = manufacturer->model_list.head;
		     model != NULL;
		     model = model->next)
		{
			/* Add a new row to the model */
			keyboard_treeview_append_keyboard (	list_store,
								manufacturer->name,
								model->name );
		}
	}
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
	tree_view_add_new_text_column (keyboard_treeview, _("Manufacturer"), COL_KEYBOARD_MANUFACTURER);
	tree_view_add_new_text_column (keyboard_treeview, _("Model"), COL_KEYBOARD_MODEL);
}



GtkWidget
*init_keyboard_dialog (	KTKeyboardName		*current_keyboard_name,
			KTKeyboardList		*keyboard_list )
/*
Input:
	current_keyboard_name	- The name of the current keyboard
	keyboard_list		-
Output:
	-
Returns:
	The address of the created and initialized dialog.
Description:
	This function initialises the keyboard dialog, by creating and setting the
	data (readen from the input variables) to its widgets.
*/
{
	GtkWidget *keyboard_dialog;

	/*
	 * Add the installation directory of the pixmaps to the location where the program
	 * should look for its pixmaps
	 */
	add_pixmap_directory (PACKAGE_DATA_DIR "/" PACKAGE "/pixmaps");
	add_pixmap_directory ("../pixmaps");
	
	keyboard_dialog = create_keyboard_dialog();
	init_keyboard_treeview (keyboard_dialog);
	reload_current_keyboard (keyboard_dialog, current_keyboard_name);
	reload_keyboard_treeview (keyboard_dialog, keyboard_list);
	
	return keyboard_dialog;
}
