/*---------------------------------------------------------------------------------
Name               : main.c
Author             : Marvin Raaijmakers
Description        : Initializes the program and creates the GUI
Date of last change: 22-Aug-2007
History            : 24-Jun-2007 Compiler warnings fixes

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

#include <stdlib.h>
#include <gtk/gtk.h>

#include "interface.h"
#include "support.h"

#include <keytouch-editor.h>
#include <evdev.h>
#include <acpi.h>
#include <set_data.h>
#include <gui.h>


MainWindowGUI_type main_window_gui = {
	main_window: NULL,
	scancode_event_descr_label: NULL,
	manufacturer_entry: NULL,
	model_entry: NULL,
	key_name_entry: NULL,
	scancode_entry: NULL,
	usbcode_entry: NULL,
	program_entry: NULL,
	plugin_function_comboentry: NULL,
	plugin_comboentry: NULL,
	program_radiobutton: NULL,
	special_action_radiobutton: NULL,
	key_treeview: NULL,
	keycode_combobox: NULL,
	usbcode_change_button: NULL
};

EVDEV_LIST	evdev_list;
EVENT_DEVICE	*current_evdev = NULL;
Boolean		use_acpi_dev, acpi_usable;
KTKeyList	key_list;
KTKey		*selected_key = NULL;	/* The currently selected key */
Boolean		file_modified;		/* Indicates of the currently opened file was modified */
char		*current_filename;	/* The name of the currently opened file
                                         * (= NULL if no name is defined yet) */
int usb_vendor_id = 0,
    usb_product_id = 0;
/* The hostnames of the SMTP servers that can be used for
 * sending the keyboard file: */
const char	*smtp_hostnames[] = {	"gmail-smtp-in.l.google.com",
					"alt2.gmail-smtp-in.l.google.com",
					"alt1.gmail-smtp-in.l.google.com",
					"gsmtp163.google.com",
					"gsmtp183.google.com",
					NULL};


int
main (int argc, char *argv[])
{
	char *modprobe_error;
	
#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif
	
	gtk_set_locale ();
	gtk_init (&argc, &argv);
	
	add_pixmap_directory (PACKAGE_DATA_DIR "/" PACKAGE "/pixmaps");
	
	
	KTSetErrorHandler (gui_error_handler);
	use_acpi_dev = FALSE;
	acpi_usable = check_acpi();
	if (!read_evdev_list(&evdev_list, EVDEV_DIR))
	{
		modprobe_error = load_evdev_module();
		if (modprobe_error)
		{
			KTError (_("Error while loading the evdev module: \n%s"), modprobe_error);
			free (modprobe_error);
			return (EXIT_FAILURE);
		}
		if (!read_evdev_list(&evdev_list, EVDEV_DIR))
		{
			KTError (_("No event devices are available in %s."), EVDEV_DIR);
			return (EXIT_FAILURE);
		}
	}
	KTKeyList_init (&key_list);
	file_modified = FALSE;
	show_selectkb_window (&evdev_list, acpi_usable);
	gtk_main ();
	
	return 0;
}

