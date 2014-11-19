/*---------------------------------------------------------------------------------
Name               : callbacks.c
Author             : Marvin Raaijmakers
Description        : Handles the callbacks of the GUI
Date of last change: 24-Jun-2006
History            : 24-Jun-2006 on_keyboard_import_button_clicked() now shows why
                                 importing the keyboard file failed.
                     10-Jan-2006 on_keyboard_import_button_clicked() now checks if
                                 there is a newer version of the keyboard file
                                 available on the system

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
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdlib.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include <keytouch-keyboard.h>
#include <set_data.h>


void
on_keyboard_import_button_clicked      (GtkButton       *button,
                                        gpointer         user_data)
{
	char		*file_name;
	KTKeyboardName	keyboard_name;
	struct tm	kbfile_date;
	
	file_name = (char *) get_filename_from_user(_("Import keyboard"), 
	                                   GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))));
	if (file_name != NULL)
	{
		if (read_keyboard (file_name, &keyboard_name, &kbfile_date))
		{
			/* If there is a newer version of the keyboard file
			 * available on the system
			 */
			if (old_keyboard_file(&keyboard_name, &keyboard_list, kbfile_date))
			{
				KTError (_("Cannot import '%s', because a newer version of "
				           "the keyboard file has already been installed."),
				         file_name);
			}
			else
			{
				insert_keyboard (&keyboard_name, kbfile_date, &keyboard_list);
				copy_keyboard (file_name, &keyboard_name);
				reload_keyboard_treeview (GTK_WIDGET(button), &keyboard_list);
			}
			XmlFree (keyboard_name.model);
			XmlFree (keyboard_name.manufacturer);
		}
		else
		{
			KTError (_("The keyboard file could not be imported because it is invalid:\n%s"), KTGetErrorMsg());
		}
	}
}


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
	GtkTreeView		*keyboard_treeview;
	GtkTreeSelection	*selection;
	GtkTreeModel		*tree_model;
	GtkTreeIter		iter;
	KTKeyboardName		keyboard_name;
	
	keyboard_treeview = GTK_TREE_VIEW( lookup_widget(GTK_WIDGET(button), "keyboard_treeview") );
	
	selection = gtk_tree_view_get_selection( GTK_TREE_VIEW(keyboard_treeview) );
	if ( gtk_tree_selection_get_selected(selection, &tree_model, &iter) )/* If a keyboard was selected */
	{
		gtk_tree_model_get (	tree_model, &iter,
					COL_KEYBOARD_MANUFACTURER, &(keyboard_name.manufacturer),
					COL_KEYBOARD_MODEL, &(keyboard_name.model),
					-1  );
		if (write_current_keyboard(&keyboard_name))
		{
			/* The current keyboard file was successfully written */
			system ("keytouch-init");
		}
	}
	
	gtk_main_quit();
	exit (0);
}

gboolean
on_keyboard_dialog_delete_event        (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	gtk_main_quit();
	exit (0);
}

