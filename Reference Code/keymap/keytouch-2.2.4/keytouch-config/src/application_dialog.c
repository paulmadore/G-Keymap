/*---------------------------------------------------------------------------------
Name               : application_dialog.c
Author             : Marvin Raaijmakers
Description        : Provides a dialog that enables the user to choose an
                     application
Date of last change: 28-Oct-2006
History            : 28-Oct-2006 + #include "support.h"

    Copyright (C) 2006 Marvin Raaijmakers

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
#include <gtk/gtk.h>
#include <gnome-menus/gmenu-tree.h>
#include <string.h>

#include "interface.h"
#include "support.h"
#include <keytouch.h>

enum {
	COL_ENTRY_NAME,
	COL_COMMENT,
	COL_COMMAND,
	NUM_MENU_COLS
};


static char *create_exec_command (	char	*exec,
					char	*caption,
					char	*desktop_file_location );
static void tree_store_add_menu_directory (	GtkTreeStore		*treestore,
						GtkTreeIter		*parent,
						GMenuTreeDirectory	*directory );
static GtkTreeModel *create_and_fill_menu_model (GMenuTreeDirectory *root);
static GtkDialog *create_and_init_application_dialog (GMenuTree *menu_tree);
static gboolean on_menu_treeview_selection (	GtkTreeSelection	*selection,
						GtkTreeModel		*model,
						GtkTreePath		*path,
						gboolean		path_currently_selected,
						gpointer		userdata );


#define NUM_EXEC_PARAMS	13

char
*create_exec_command (	char	*exec,
			char	*caption,
			char	*desktop_file_location )
/*
Input:
	exec			- The command of the application
	caption			- The name of the application
	desktop_file_location	- The location of the applications desktop file
Returns:
	A pointer to the created command. The allocated space for the command must
	be freed if it is no longer needed.
Description:
	This function makes a copy of 'exec' and replaces every occurence of %c by
	'caption' and of %k by 'desktop_file_location'. Every occurence of %
	followed by i, f, F, u, U, d, D, n, N, v and m will be removed.
*/
{
	char	*icon_ptr = NULL,
		*caption_ptr = NULL,
		*desktop_file_location_ptr = NULL,
		*command, *cmd_ptr;
	int i, j, command_len;
	struct { char name, *val; } param[NUM_EXEC_PARAMS] = {	{'i', ""},
								{'c', caption},
								{'k', desktop_file_location},
								{'f', ""},
								{'F', ""},
								{'u', ""},
								{'U', ""},
								{'d', ""},
								{'D', ""},
								{'n', ""},
								{'N', ""},
								{'v', ""},
								{'m', ""} };
	
	command_len = 0;
	for (i = 0; exec[i]; i++)
	{
		if (exec[i] == '%')
		{
			/* Find the value to fill in for this parameter */
			for (j = 0; j < NUM_EXEC_PARAMS && param[j].name != exec[i+1]; j++)
				; /* NULL Statement */
			/* If the value was found */
			if (j < NUM_EXEC_PARAMS)
			{
				command_len += strlen(param[j].val);
				i++;
			}
			else
			{
				/* There is no parameter, only a '%' character. So: */
				command_len++;
			}
		}
		else
		{
			command_len++;
		}
	}
	command = keytouch_malloc (command_len+1);
	cmd_ptr = command;
	for (i = 0; exec[i]; i++)
	{
		if (exec[i] == '%')
		{
			/* Find the value to fill in for this parameter */
			for (j = 0; j < NUM_EXEC_PARAMS && param[j].name != exec[i+1]; j++)
				; /* NULL Statement */
			/* If the value was found */
			if (j < NUM_EXEC_PARAMS)
			{
				strcpy (cmd_ptr, param[j].val);
				cmd_ptr += strlen(param[j].val);
				i++;
			}
			else
			{
				/* There is no parameter, only a '%' character. So: */
				*cmd_ptr = exec[i];
				cmd_ptr++;
			}
		}
		else
		{
			*cmd_ptr = exec[i];
			cmd_ptr++;
		}
	}
	*cmd_ptr = '\0';
	
	return command;
}


void
tree_store_add_menu_directory (	GtkTreeStore		*treestore,
				GtkTreeIter		*parent,
				GMenuTreeDirectory	*directory )
/*
Input:
	directory	- The menu directory to add to 'parent' in 'treestore'
Output:
	parent		- The GtkTreeIter that will contain the contents of
			  directory
	treestore	- The store where parent is located in
Description:
	This function adds GtkTreeIters to 'treestore' with 'parent' as their parent
	iter. The new iters will be the entries in 'directory'. The first column
	will be filled with the names of the menu entries, the second column with a
	description of the application and the third column with the applications
	command.
*/
{
	GSList		*items;
	GSList		*tmp;
	const char	*path;
	char		*freeme, *command;
	GdkPixbuf	*pixbuf;
	GtkTreeIter	directory_iter,
			entry_iter;
	
	items = gmenu_tree_directory_get_contents (directory);
	
	tmp = items;
	while (tmp != NULL)
	{
		GMenuTreeItem *item = tmp->data;
		switch (gmenu_tree_item_get_type (item))
		{
			case GMENU_TREE_ITEM_ENTRY:
				command = create_exec_command ((char *)gmenu_tree_entry_get_exec( GMENU_TREE_ENTRY(item) ),
				                               (char *)gmenu_tree_entry_get_name( GMENU_TREE_ENTRY(item) ),
				                               (char *)gmenu_tree_entry_get_desktop_file_path( GMENU_TREE_ENTRY(item) ));
				gtk_tree_store_append (treestore, &entry_iter, parent);
				gtk_tree_store_set (	treestore, &entry_iter,
							COL_ENTRY_NAME, gmenu_tree_entry_get_name(GMENU_TREE_ENTRY(item)),
							COL_COMMENT, gmenu_tree_entry_get_comment(GMENU_TREE_ENTRY(item)),
							COL_COMMAND, command, -1);
				free (command);
				break;
			case GMENU_TREE_ITEM_DIRECTORY:
				gtk_tree_store_append (treestore, &directory_iter, parent);
				gtk_tree_store_set (	treestore, &directory_iter,
							COL_ENTRY_NAME, gmenu_tree_directory_get_name( GMENU_TREE_DIRECTORY(item) ),
							-1);
				tree_store_add_menu_directory (treestore, &directory_iter, GMENU_TREE_DIRECTORY(item));
				break;
			case GMENU_TREE_ITEM_HEADER:
			case GMENU_TREE_ITEM_SEPARATOR:
			case GMENU_TREE_ITEM_ALIAS:
				break;
			default:
				g_assert_not_reached();
				break;
		}
		gmenu_tree_item_unref (tmp->data);
		tmp = tmp->next;
	}
	g_slist_free (items);
}


GtkTreeModel
*create_and_fill_menu_model (GMenuTreeDirectory *root)
/*
Input:
	root	- The root directory of the menu tree to fill the model with.
Returns:
	A pointer to the created model.
Description:
	This function creates a tree model with 3 columns. The model will be filled
	with the contents of the 'root' menu tree directory. The first column will
	be filled with the names of the menu entries, the second column with a
	description of the application and the third column with the applications
	command.
*/
{
	GtkTreeStore	*treestore;
	
	treestore = gtk_tree_store_new(	NUM_MENU_COLS,
					G_TYPE_STRING,
					G_TYPE_STRING,
					G_TYPE_STRING);
	if (root)
	{
		tree_store_add_menu_directory (treestore, NULL, root);
		gmenu_tree_item_unref (root);
	}
	
	return GTK_TREE_MODEL( treestore );
}


GtkDialog
*create_and_init_application_dialog (GMenuTree *menu_tree)
/*
Input:
	menu_tree	- Will be used for filling the tree view
Returns:
	A pointer to the created dialog.
Description:
	This function creates the application dialog that contains a tree view and a
	text entry. The treeview will be filled with the entries in 'menu_tree' and
	will have 3 columns. The first column will be visible to the user and
	contain the name of the application. The second column will also be visible
	and will contain a description for the application. The third column will
	not be visible to the user and will contain the command of the application.
*/
{
	GtkWidget		*dialog;
	GtkEntry		*command_entry;
	GtkTreeView		*tree_view;
	GtkTreeViewColumn	*col;
	GtkCellRenderer		*renderer;
	GtkTreeModel		*model;
	GtkTreeSelection	*selection;
	char			*command;
	
	dialog = create_application_dialog();
	tree_view = GTK_TREE_VIEW( lookup_widget (dialog, "app_treeview") );
	command_entry = GTK_ENTRY( lookup_widget (dialog, "app_command_entry") );
	
	col = gtk_tree_view_column_new();
	/* pack tree view column into tree view */
	gtk_tree_view_append_column (tree_view, col);
	renderer = gtk_cell_renderer_text_new();
	/* pack cell renderer into tree view column */
	gtk_tree_view_column_pack_start (col, renderer, TRUE);
	/* connect 'text' property of the cell renderer to
	*  model column that contains the applications name */
	gtk_tree_view_column_add_attribute (col, renderer, "text", COL_ENTRY_NAME);
	
	col = gtk_tree_view_column_new();
	/* pack tree view column into tree view */
	gtk_tree_view_append_column (tree_view, col);
	renderer = gtk_cell_renderer_text_new();
	/* pack cell renderer into tree view column */
	gtk_tree_view_column_pack_start (col, renderer, TRUE);
	g_object_set (renderer,
	              "style", PANGO_STYLE_ITALIC,
	              "style-set", TRUE,
	              NULL);
	/* connect 'text' property of the cell renderer to
	*  model column that contains the applications name */
	gtk_tree_view_column_add_attribute (col, renderer, "text", COL_COMMENT);
	
	model = create_and_fill_menu_model (gmenu_tree_get_root_directory(menu_tree));
	gtk_tree_view_set_model (GTK_TREE_VIEW(tree_view), model);
	
	g_object_unref (model); /* destroy model automatically with view */
	
	selection = gtk_tree_view_get_selection (tree_view);
	/* Use command_entry as an argument to on_menu_treeview_selection() because
	 * the command of the selected application will be set to this text entry.
	 */
	gtk_tree_selection_set_select_function (selection, on_menu_treeview_selection, (gpointer) command_entry, NULL);
	
	return GTK_DIALOG( dialog );
}


gboolean
on_menu_treeview_selection (	GtkTreeSelection	*selection,
				GtkTreeModel		*model,
				GtkTreePath		*path,
				gboolean		path_currently_selected,
				gpointer		userdata )
/*
Input:
	selection
	model
	path
Output:
	userdata	- This variable MUST point to the text entry in the
			  application dialog. If an application was selected in the
			  tree view the command of that application will be put in
			  the text entry.
Returns:
	TRUE
Description:
	This function is the selection handler of the tree view in the application
	dialog. If row was selected in the tree view then the contents of the
	COL_COMMAND'th column will be copied to the text entry, pointed to by
	userdata.
*/
{
	GtkTreeIter	iter;
	char		*command;
	
	/* If a row was selected instead of deselected */
	if (gtk_tree_model_get_iter(model, &iter, path)) 
	{
		/* Get the name of the selected key */
		gtk_tree_model_get (model, &iter, COL_COMMAND, &command, -1);
		if (command != NULL)
		{
			gtk_entry_set_text ((GtkEntry *)userdata, command);
			g_free (command);
		}
	}
	return (TRUE);
}


const char
*get_application_command_from_user (	char		*current_command,
					GMenuTree	*menu_tree )
/*
Input:
	current_command	- The command that is currently used
	menu_tree	- The menu tree containing the applications
Returns:
	The command that was chosen by the user or NULL if the user pressed the
	Cancel button of the application dialog.
Description:
	The first time this function is called, it will create a dialog containing a
	treeview and text entry. The tree view will be filled with the contents of
	'menu_tree'. When the user closes the dialog, it will be hidden and will
	appear the next time this function is called.
	Every time this function is called, the text entry will be filled with
	'current_command'. When the user selects a application in the tree view then
	the command of the selected application will be put in the text entry. When
	the user presses the OK button, this function will return the value of the
	text entry.
*/
{
	static GtkDialog	*dialog = NULL;
	static GtkEntry		*command_entry;
	static GtkTreeView	*tree_view;
	char			*command;
	
	if (dialog == NULL)
	{
		dialog = create_and_init_application_dialog (menu_tree);
		command_entry = GTK_ENTRY( lookup_widget (GTK_WIDGET(dialog), "app_command_entry") );
		tree_view = GTK_TREE_VIEW( lookup_widget (GTK_WIDGET(dialog), "app_treeview") );
	}
	gtk_entry_set_text (command_entry, current_command);
	gtk_tree_view_collapse_all (tree_view);
	if (gtk_dialog_run( GTK_DIALOG (dialog) ) == GTK_RESPONSE_OK)
	{
		command = (char *) gtk_entry_get_text (command_entry);
	}
	else
	{
		command = NULL;
	}
	gtk_widget_hide (GTK_WIDGET(dialog));
	
	return command;
}
