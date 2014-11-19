/*---------------------------------------------------------------------------------
Name               : callbacks.c
Author             : Marvin Raaijmakers
Description        : Handles the GUI callbacks
Date of last change: 21-May-2006
                     02-May-2006 Removed get_selected_key(), global variable
                                 selected_key is now used instead
                     01-May-2006 The KGUI_*() macros are now used instead of
                                 lookup_widget()
                     30-Apr-2006 Modified calls to function in set_data.c
                     23-Apr-2006 Strings that are allocated by gtk_tree_model_get()
                                 are now freed with g_free()

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
#include <string.h>
#include <unistd.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include <keytouch.h>
#include <gui.h>
#include <set_data.h>
#include <key_settings.h>
#include <plugin_control.h>
#include <plugin-info.h>
#include <application_dialog.h>

#define ONE_COLUMN	(1)
#define FIRST_COLUMN	(0)

/* Points to the key settings of the currently
 * selected key or to NULL if no key was selected: */
static KTKeySettings *selected_key = NULL;
static char *current_command = NULL;

static void handle_changed_special_action (GtkToggleButton *special_action_radiobutton);
static void handle_changed_program (GtkEntry *program_entry, GtkToggleButton *program_radiobutton);
static KTPluginInfoEntry *get_selected_plugin (void);


gboolean
on_main_window_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	gtk_main_quit();
	exit (0);
}


void
on_change_button_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
	msg_box (_("To change the keyboard keyTouch has to be closed. "
	           "After you changed the keyboard keyTouch will be started again."),
	         GTK_MESSAGE_INFO);
	system ("keytouch-keyboard --restart-keytouch &");
	gtk_main_quit();
	exit (0);
}


/*******************************************************************************
 *                         CALLBACKS FOR PREFERENCES                           *
 *******************************************************************************/

void
on_plugin_download_button_clicked      (GtkButton       *button,
                                        gpointer         user_data)
{
	run_browser (preferences.browser, URL_DOWNLOAD_PLUGINS);
}


KTPluginInfoEntry
*get_selected_plugin (void)
/*
Returns:
	The address of the KTPluginInfoEntry which name equals the selected row of
	the tree view returned by KTGUI_plugin_treeview(). If no such
	KTPluginInfoEntry appears in plugin_list, this function will return NULL.
*/
{
	GtkTreeSelection	*tree_selection;
	GtkTreeModel		*model;
	GtkTreeIter		iter;
	gchar			*plugin_name;
	KTPluginInfoEntry	*plugin;
	
	plugin = NULL;
	tree_selection = gtk_tree_view_get_selection( KTGUI_plugin_treeview() );
	/* If a row was selected instead of deselected */
	if (gtk_tree_selection_get_selected(tree_selection, &model, &iter))
	{
		/* Get the name of the selected plugin */
		gtk_tree_model_get (model, &iter, FIRST_COLUMN, &plugin_name, -1);
		if (plugin_name != NULL) /* If a listitem was selected instead of deselected */
		{
			plugin = find_plugin((char *)plugin_name, &plugin_list);
			g_free (plugin_name);
		}
	}
	return (plugin);
}


void
on_plugin_info_button_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget		*plugin_info_dialog;
	KTPluginInfoEntry	*plugin;
	
	plugin = get_selected_plugin();
	if (plugin != NULL)
	{
		plugin_info_dialog = create_plugin_info_dialog();
		fill_plugin_info_dialog (plugin_info_dialog, plugin);
		gtk_widget_show (plugin_info_dialog);
	}
}


void
on_plugin_info_dialog_show             (GtkWidget       *widget,
                                        gpointer         user_data)
{
	/* Block the interaction with the main window, so that
	 * the user can only use this window
	 */
	gtk_grab_add (GTK_WIDGET(widget));
}


void
on_plugin_info_ok_button_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{
	/* Close the plugin info diaglog */
	gtk_widget_destroy ((gtk_widget_get_toplevel(GTK_WIDGET(button))));
}


void
on_plugin_remove_button_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{
	KTPluginInfoEntry	*plugin;
	GtkWidget		*question_dialog;
	gint			response;
	GtkTreeSelection	*selection;
	GtkTreeIter		iter;
	GtkTreeModel		*model;
	
	plugin = get_selected_plugin();
	if (plugin != NULL)
	{
		question_dialog = gtk_message_dialog_new(
					GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))), 
					GTK_DIALOG_MODAL,
					GTK_MESSAGE_QUESTION,
					GTK_BUTTONS_YES_NO,
					_("Do you really want to remove the plugin \"%s\"?"),
					plugin->info.name);
		/*
		 * Block the interaction with the other windows, so that
		 * the user can only use this window
		 */
		gtk_grab_add (question_dialog);
		response = gtk_dialog_run(GTK_DIALOG(question_dialog));
		gtk_widget_destroy (question_dialog);
		/* If the user pressed the "Yes"-button */
		if (response == GTK_RESPONSE_YES)
		{
			remove_plugin (plugin, &plugin_list);
			reload_plugin_treeview (KTGUI_plugin_treeview(), &plugin_list);
			reload_plugin_list (KTGUI_plugin_combo_list(),
			                    GTK_OBJECT( KTGUI_plugin_combo_entry() ),
			                    on_plugin_combo_entry_changed,
			                    &plugin_list);
			
			selection = gtk_tree_view_get_selection( KTGUI_key_treeview() );
			/* If a key is selected */
			if (gtk_tree_selection_get_selected(selection, &model, &iter))
			{
				/* Reselect it */
				gtk_tree_selection_unselect_iter (selection, &iter);
				gtk_tree_selection_select_iter (selection, &iter);
			}
		}
	}
}


void
on_plugin_import_button_clicked        (GtkButton       *button,
                                        gpointer         user_data)
{
	char *file_name;
	
	file_name = get_filename_from_user (_("Import plugin"),
	                                    GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button))));
	if (file_name != NULL)
	{
		import_plugin (file_name, &plugin_list);
		reload_plugin_treeview (KTGUI_plugin_treeview(), &plugin_list);
		reload_plugin_list (KTGUI_plugin_combo_list(),
		                    GTK_OBJECT( KTGUI_plugin_combo_entry() ),
		                    on_plugin_combo_entry_changed,
		                    &plugin_list);
	}
}


void
on_documents_entry_changed             (GtkEditable     *editable,
                                        gpointer         user_data)
{
	if (preferences.documents_dir != NULL)
	{
		free (preferences.documents_dir);
	}
	preferences.documents_dir = keytouch_strdup (gtk_entry_get_text(GTK_ENTRY(editable)));
}


void
on_home_page_entry_changed             (GtkEditable     *editable,
                                        gpointer         user_data)
{
	if (preferences.home_page != NULL)
	{
		free (preferences.home_page);
	}
	preferences.home_page = keytouch_strdup (gtk_entry_get_text(GTK_ENTRY(editable)));
}


void
on_search_page_entry_changed           (GtkEditable     *editable,
                                        gpointer         user_data)
{
	if (preferences.search_page != NULL)
	{
		free (preferences.search_page);
	}
	preferences.search_page = keytouch_strdup (gtk_entry_get_text(GTK_ENTRY(editable)));
}


void
on_browser_entry_changed               (GtkEditable     *editable,
                                        gpointer         user_data)
{
	if (preferences.browser != NULL)
	{
		free (preferences.browser);
	}
	preferences.browser = keytouch_strdup (gtk_entry_get_text(GTK_ENTRY(editable)));
}


void
on_email_prog_entry_changed            (GtkEditable     *editable,
                                        gpointer         user_data)
{
	if (preferences.email_program != NULL)
	{
		free (preferences.email_program);
	}
	preferences.email_program = keytouch_strdup (gtk_entry_get_text(GTK_ENTRY(editable)));
}


void
on_chat_entry_changed                  (GtkEditable     *editable,
                                        gpointer         user_data)
{
	if (preferences.chat_program != NULL)
	{
		free (preferences.chat_program);
	}
	preferences.chat_program = keytouch_strdup (gtk_entry_get_text(GTK_ENTRY(editable)));
}


void
on_email_prog_choose_button_clicked    (GtkButton       *button,
                                        gpointer         user_data)
{
	const char *command;
	
	command = get_application_command_from_user ((char *) gtk_entry_get_text(KTGUI_email_prog_entry()), menu_tree);
	if (command)
	{
		gtk_entry_set_text (KTGUI_email_prog_entry(), command);
	}
}


void
on_chat_choose_button_clicked          (GtkButton       *button,
                                        gpointer         user_data)
{
	const char *command;
	
	command = get_application_command_from_user ((char *) gtk_entry_get_text(KTGUI_chat_entry()), menu_tree);
	if (command)
	{
		gtk_entry_set_text (KTGUI_chat_entry(), command);
	}
}


void
on_browser_choose_button_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{
	const char *command;
	
	command = get_application_command_from_user ((char *) gtk_entry_get_text(KTGUI_browser_entry()), menu_tree);
	if (command)
	{
		gtk_entry_set_text (KTGUI_browser_entry(), command);
	}
}


/*******************************************************************************
 *                         CALLBACKS FOR HELP & ABOUT                          *
 *******************************************************************************/

void
on_help_button_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
	static char *help_file = NULL;
	
	if (help_file == NULL)
	{
		help_file = g_strdup_printf("file:%s/%s", DOC_DIR, _("user_manual.html"));
	}
	run_browser (preferences.browser, help_file);
}


/*******************************************************************************
 *          CALLBACKS FOR THE BUTTONS AT THE BOTTOM OF THE MAIN WINDOW         *
 *******************************************************************************/

void
on_ok_button_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
	write_configuration (&key_list, &current_keyboard_name);
	write_preferences (&preferences);
	system ("killall keytouchd");
	usleep (5000);
	system ("keytouchd &");
	gtk_main_quit();
	exit (0);
}


void
on_apply_button_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
	write_configuration (&key_list, &current_keyboard_name);
	write_preferences (&preferences);
	system ("killall keytouchd");
	usleep (5000);
	system ("keytouchd &");
}


void
on_cancel_button_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_main_quit();
	exit (0);
}


/*******************************************************************************
 *                          CALLBACKS FOR KEY SETTINGS                         *
 *******************************************************************************/

gboolean
on_key_treeview_selection (	GtkTreeSelection	*selection,
				GtkTreeModel		*model,
				GtkTreePath		*path,
				gboolean		path_currently_selected,
				gpointer		userdata )
/*
Global input:
	key_list	- The list containing the settings for every key
Global output:
	selected_key	- Will point to the KTKeySettings in 'key_list' that
			  corresponds to the selected key (if a key is selected)
Description:
	This function sets all the settings of the selected key (if one selected) to
	the GUI and sets 'selected_key' to the address of the selected key settings.
	The key settings will be found in 'key_list'.
*/
{
	GtkEntry		*program_entry = NULL,
				*default_entry,
				*plugin_combo_entry,
				*plugin_function_combo_entry;
	GtkRadioButton		*radiobutton,
				*default_radiobutton,
				*program_radiobutton,
				*special_action_radiobutton;
	GtkList			*plugin_combo_list,
				*plugin_function_combo_list;
	GtkTreeView		*key_treeview;
	GtkTreeIter		iter;
	char			*key_name,
				*program_user = "",
				*default_action;
	KTPluginInfoEntry	*plugin;
	
	/* If a row was selected instead of deselected */
	if (gtk_tree_model_get_iter(model, &iter, path)) 
	{
		/* Get the name of the selected key */
		gtk_tree_model_get (model, &iter, FIRST_COLUMN, &key_name, -1);
		if (key_name != NULL) /* If a listitem was selected instead of deselected */
		{
			key_treeview = gtk_tree_selection_get_tree_view(selection);
			program_entry = KTGUI_program_entry();
			default_entry = KTGUI_default_entry();
			plugin_combo_entry = KTGUI_plugin_combo_entry();
			plugin_function_combo_entry = KTGUI_plugin_function_combo_entry();
			default_radiobutton = KTGUI_default_radiobutton();
			program_radiobutton = KTGUI_program_radiobutton();
			special_action_radiobutton = KTGUI_special_action_radiobutton();
			plugin_combo_list = KTGUI_plugin_combo_list();
			plugin_function_combo_list = KTGUI_plugin_function_combo_list();
			
			/* Block the signal handlers of the entries and the radiobuttons */
			g_signal_handlers_block_by_func (GTK_OBJECT(program_entry),
			                                  (void *)on_program_entry_changed, NULL);
			g_signal_handlers_block_by_func (GTK_OBJECT(default_radiobutton),
			                                  (void *)on_default_radiobutton_toggled, NULL);
			g_signal_handlers_block_by_func (GTK_OBJECT(program_radiobutton),
			                                  (void *)on_program_radiobutton_toggled, NULL);
			g_signal_handlers_block_by_func (GTK_OBJECT(special_action_radiobutton), 
			                                  (void *)on_special_action_radiobutton_toggled, NULL);
			g_signal_handlers_block_by_func (GTK_OBJECT(plugin_combo_entry),
			                                  (void *)on_plugin_combo_entry_changed, NULL);
			g_signal_handlers_block_by_func (GTK_OBJECT(plugin_function_combo_entry),
			                                  (void *)on_plugin_function_combo_entry_changed, NULL);
			
			gtk_entry_set_text (plugin_combo_entry, "");
			gtk_entry_set_text (plugin_function_combo_entry, "");
			/* Clear the plugin function list */
			gtk_list_clear_items (plugin_function_combo_list, 0, -1); /* Clear the whole list */
			/* Select the key in the linked list */
			selected_key = find_key_settings(key_name, &key_list);
			/* Read the settings of the key */
			if (!memcmp(&selected_key->action, &selected_key->default_action, sizeof(KTAction)))
			{
				radiobutton = default_radiobutton;
			}
			else if (selected_key->action.type == KTActionTypeProgram)
			{
				program_user = selected_key->action.program.command;
				radiobutton = GTK_RADIO_BUTTON( lookup_widget(GTK_WIDGET(key_treeview), "program_radiobutton") );
			}
			else if (selected_key->action.type == KTActionTypePlugin)
			{
				plugin = find_plugin (selected_key->action.plugin.plugin_name, &plugin_list);
				if (plugin_function_exists (selected_key->action.plugin.plugin_name,
				                            selected_key->action.plugin.function_name,
				                            &plugin_list))
				{
					reload_plugin_function_list (KTGUI_plugin_function_combo_list(),
					                             GTK_OBJECT( KTGUI_plugin_function_combo_entry() ),
					                             on_plugin_function_combo_entry_changed,
					                             plugin);
					gtk_entry_set_text (plugin_combo_entry,
					                    selected_key->action.plugin.plugin_name);
					gtk_entry_set_text (plugin_function_combo_entry,
					                    selected_key->action.plugin.function_name);
					radiobutton = special_action_radiobutton;
				}
				else
				{
					radiobutton = default_radiobutton;
				}
			}
			
			/* Show the settings of the key */
			gtk_label_set_text (KTGUI_key_name_label(), key_name);
			gtk_entry_set_text (GTK_ENTRY(program_entry), program_user);
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(radiobutton), TRUE);
			
			if (selected_key->default_action.type == KTActionTypePlugin)
			{
				/* If the plugin is not available on this system */
				if (selected_key->default_action.plugin.plugin_name == NULL)
				{
					default_action = "";
				}
				else
				{
					default_action = (char *)g_strdup_printf ("%s - %s",
							selected_key->default_action.plugin.plugin_name,
							selected_key->default_action.plugin.function_name);
				}
			}
			else if (selected_key->default_action.type == KTActionTypeProgram)
			{
				default_action = (char *)g_strdup_printf(
						selected_key->default_action.program.command);
			}
			/* Show the default action */
			gtk_entry_set_text (GTK_ENTRY(default_entry), default_action);
			if (default_action[0] != '\0')
			{
				g_free (default_action);
			}
			
			/* Unblock the signal handlers of the entries and the radiobuttons */
			g_signal_handlers_unblock_by_func (GTK_OBJECT(program_entry),
			                                    (void *)on_program_entry_changed, NULL);
			g_signal_handlers_unblock_by_func (GTK_OBJECT(default_radiobutton),
			                                    (void *)on_default_radiobutton_toggled, NULL);
			g_signal_handlers_unblock_by_func (GTK_OBJECT(program_radiobutton),
			                                    (void *)on_program_radiobutton_toggled, NULL);
			g_signal_handlers_unblock_by_func (GTK_OBJECT(special_action_radiobutton),
			                                    (void *)on_special_action_radiobutton_toggled, NULL);
			g_signal_handlers_unblock_by_func (GTK_OBJECT(plugin_combo_entry), 
			                                    (void *)on_plugin_combo_entry_changed, NULL);
			g_signal_handlers_unblock_by_func (GTK_OBJECT(plugin_function_combo_entry), 
			                                    (void *)on_plugin_function_combo_entry_changed, NULL);
			
			g_free (key_name);
		}
	}
	
	return (TRUE);
}


void
on_default_radiobutton_toggled         (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
/*
Global output:
	selected_key	- If 'selected_key' does not point to NULL, the action will
			  be cleared and will be set to the default action
*/
{
	/* If a key was selected and the default radiobutton was toggled*/
	if (selected_key != NULL && gtk_toggle_button_get_active(togglebutton))
	{
		clear_key_settings_action (selected_key);
		selected_key->action = selected_key->default_action;
	}
}


void
handle_changed_program (	GtkEntry	*program_entry,
				GtkToggleButton	*program_radiobutton )
/*
Input:
	program_entry		- The program entry widget which could contain the
				  program name
	program_radiobutton	- The radio button which in indicates if program_entry
				  contains the program name
Global output:
	selected_key		- If 'selected_key' does not point to NULL, the
				  current action will be cleared and set to the
				  command filled in by the user in 'program_entry'
Description:
	This function changes the action element the selected key to the value of
	program_entry if program_radiobutton is toggled. If program_entry is empty and
	program_radiobutton is toggled, the action element will be changed to the
	default action.
*/
{
	char *command;

	/* If program_radiobutton is pressed and a key is selected */
	if ( gtk_toggle_button_get_active (program_radiobutton) && selected_key )
	{
		clear_key_settings_action (selected_key);
		/* Get the text of the program entry */
		command = (char *) gtk_entry_get_text (program_entry);
		if (command[0] == '\0') /* If the entry does not contain text */
		{
			selected_key->action = selected_key->default_action;
		}
		else
		{
			selected_key->action.type = KTActionTypeProgram;
			selected_key->action.program.command = keytouch_strdup (command);
		}
	}
}


void
on_program_radiobutton_toggled         (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	handle_changed_program (KTGUI_program_entry(), togglebutton);
}


void
on_program_entry_changed               (GtkEditable     *editable,
                                        gpointer         user_data)
{
	GtkToggleButton *program_radiobutton;

	program_radiobutton = GTK_TOGGLE_BUTTON( KTGUI_program_radiobutton() );
	/* If program_radiobutton is pressed */
	if ( gtk_toggle_button_get_active (program_radiobutton) )
	{
		handle_changed_program (GTK_ENTRY(editable), program_radiobutton);
	}
	else
	{
		/* Press (= toggle) program_radiobutton */
		gtk_toggle_button_set_active (program_radiobutton, TRUE);
	}
}


void
on_choose_program_button_clicked       (GtkButton       *button,
                                        gpointer         user_data)
{
	const char *command;
	
	command = get_application_command_from_user ((char *)gtk_entry_get_text(KTGUI_program_entry()), menu_tree);
	if (command)
	{
		gtk_entry_set_text (KTGUI_program_entry(), command);
	}
}


void
handle_changed_special_action (GtkToggleButton *special_action_radiobutton)
/*
Input:
	special_action_radiobutton	- The radiobutton which indicates if the
					  selected plugin and its function
Global output:
	selected_key	- If 'selected_key' does not point to NULL, the current
			  action will be cleared and set to the selected plugin
			  and function.
Description:
	This function changes the action element of the selected key to the selected
	plugin and its function. It also toggles special_action_radiobutton to TRUE.
*/
{
	char		*plugin_name,
			*function_name;
	
	plugin_name = (char *) gtk_entry_get_text( KTGUI_plugin_combo_entry() );
	function_name = (char *) gtk_entry_get_text( KTGUI_plugin_function_combo_entry() );
	if (plugin_name[0] != '\0' && function_name[0] != '\0' && selected_key)
	{
		clear_key_settings_action (selected_key);
		selected_key->action.type = KTActionTypePlugin;
		selected_key->action.plugin.plugin_name = keytouch_strdup (plugin_name);
		selected_key->action.plugin.function_name = keytouch_strdup (function_name);
	}
	/* Block the signal handler of and special_action_radiobutton */
	g_signal_handlers_block_by_func (GTK_OBJECT(special_action_radiobutton), (void *)on_special_action_radiobutton_toggled, NULL);
	/* Press special_action_radiobutton */
	gtk_toggle_button_set_active (special_action_radiobutton, TRUE);
	/* Unblock the signal handler of and special_action_radiobutton */
	g_signal_handlers_unblock_by_func (GTK_OBJECT(special_action_radiobutton), (void *)on_special_action_radiobutton_toggled, NULL);
}


void
on_plugin_combo_entry_changed          (GtkEditable     *editable,
                                        gpointer         user_data)
{
	char			*plugin_name;
	KTPluginInfoEntry	*plugin;
	GtkWidget		*special_action_radiobutton;
	
	plugin_name = (char *) gtk_entry_get_text(GTK_ENTRY(editable));
	plugin = find_plugin (plugin_name, &plugin_list);
	if (plugin != NULL)
	{
		reload_plugin_function_list (KTGUI_plugin_function_combo_list(),
		                             GTK_OBJECT( KTGUI_plugin_function_combo_entry() ),
		                             on_plugin_function_combo_entry_changed,
		                             plugin);
		handle_changed_special_action (GTK_TOGGLE_BUTTON( KTGUI_special_action_radiobutton() ));
	}
}


void
on_plugin_function_combo_entry_changed (GtkEditable     *editable,
                                        gpointer         user_data)
{
	handle_changed_special_action (GTK_TOGGLE_BUTTON( KTGUI_special_action_radiobutton() ));
}


void
on_special_action_radiobutton_toggled  (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	/* If special_action_radiobutton is pressed */
	if ( gtk_toggle_button_get_active(togglebutton) == TRUE )
	{
		handle_changed_special_action (togglebutton);
	}
}