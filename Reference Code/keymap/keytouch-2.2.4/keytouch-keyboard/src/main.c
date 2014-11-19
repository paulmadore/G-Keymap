/*---------------------------------------------------------------------------------
Name               : main.c
Author             : Marvin Raaijmakers
Description        : The main source file of keytouch-keyboard. This program
                     provides a GUI to choose the current keyboard.
Date of last change: 17-Jun-2006
History            : 17-Jun-2006 Handles failure of get_current_keyboard()

    Copyright (C) 2005 Marvin Raaijmakers

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

#include <keytouch-keyboard.h>
#include <set_data.h>

KTKeyboardList keyboard_list = {NULL, NULL};

int
main (int argc, char *argv[])
{
	GtkWidget	*keyboard_dialog;
	KTKeyboardName	current_keyboard_name;
	
#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif
	
	gtk_set_locale ();
	gtk_init (&argc, &argv);
	
	add_pixmap_directory (PACKAGE_DATA_DIR "/" PACKAGE "/pixmaps");
	
	read_keyboards (&keyboard_list);
	if (!get_current_keyboard (&current_keyboard_name))
	{
		current_keyboard_name.manufacturer = keytouch_strdup ("");
		current_keyboard_name.model = keytouch_strdup ("");
	}
	else if (!find_keyboard_model (&current_keyboard_name, &keyboard_list))
	{
		free (current_keyboard_name.manufacturer);
		free (current_keyboard_name.model);
		current_keyboard_name.manufacturer = keytouch_strdup ("");
		current_keyboard_name.model = keytouch_strdup ("");
	}
	gtk_widget_show (init_keyboard_dialog (&current_keyboard_name, &keyboard_list));
	free (current_keyboard_name.manufacturer);
	free (current_keyboard_name.model);
	gtk_main();
	
	return 0;
}

