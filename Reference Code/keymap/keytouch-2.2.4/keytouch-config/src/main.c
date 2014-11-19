/*---------------------------------------------------------------------------------
Name               : main.c
Author             : Marvin Raaijmakers
Description        : The main source file of the keytouch configuration program.
                     This program is a provides a GUI to configure keytouch with.
Date of last change: 17-Jan-2007
History            : 17-Jan-2007 If no keyboard was specified after running
                                 "keytouch-keyboard" (instead of
                                 "keytouch-keyboard --restart-keytouch") then exit.
                     17-Jun-2006 Failure of get_current_keyboard() and
                                 read_configuration() will now be handled
                     30-Apr-2006 Added keytouch_gui
                     19-Mar-2006 _home_dir will now point to a copy of the home
                                 directory path
                     23-Jan-2006 Added the global variable _home_dir to avoid bug

    Copyright (C) 2005-2007 Marvin Raaijmakers

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
#include <gtk/gtk.h>
#include <gnome-menus/gmenu-tree.h>

#include "interface.h"
#include "support.h"

#include <keytouch.h>
#include <plugin-info.h>
#include <set_data.h>
#include <gui.h>


KTKeyboardName		current_keyboard_name;
KTKeySettingsList	key_list;
/* Set the values in case reading the prefernces failed */
KTPreferences		preferences = {NULL, NULL, NULL, NULL, NULL, NULL};
KTPluginInfoList	plugin_list;
const char		*_home_dir;
GMenuTree		*menu_tree;
KTGUI_type		keytouch_gui;

int
main (int argc, char *argv[])
{
	_home_dir = (const char *)strdup(getenv("HOME"));
	if (_home_dir == NULL)
	{
		KTError (_("Could not allocate memory."), "");
		exit (EXIT_FAILURE);
	}
	
#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif
	
	gtk_set_locale ();
	gtk_init (&argc, &argv);
	
	add_pixmap_directory (PACKAGE_DATA_DIR "/" PACKAGE "/pixmaps");
	
	menu_tree = gmenu_tree_lookup (MENU_SPEC_FILE, GMENU_TREE_FLAGS_NONE | 0x05);
	
	create_user_config_dir();
	init_plugin_list (&plugin_list);
	read_plugins (&plugin_list);
	if (!get_current_keyboard (&current_keyboard_name))
	{
		msg_box (_("No keyboard was specified, please select your keyboard."), GTK_MESSAGE_INFO);
		system ("keytouch-keyboard");
		if (!get_current_keyboard (&current_keyboard_name))
		{
			/* There is no reasing to continue, so exit */
			clear_plugin_list (&plugin_list);
			exit (EXIT_SUCCESS);
		}
	}
	if (read_configuration (&key_list, &current_keyboard_name) != KTConfErr_NoError)
	{
		KTError (_("%s\nPlease select your keyboard model to solve this problem."), KTGetErrorMsg());
		system ("keytouch-keyboard --restart-keytouch &");
		clear_plugin_list (&plugin_list);
		free (current_keyboard_name.manufacturer);
		free (current_keyboard_name.model);
		exit (EXIT_SUCCESS);
	}
	read_preferences (&preferences);
	
	init_main_window (&current_keyboard_name, &key_list, &plugin_list, &preferences, &keytouch_gui);
	gtk_main ();
	
	return 0;
}
