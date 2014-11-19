/*-------------------------------------------------------------------------------
Name               : handle_error.c
Author             : Marvin Raaijmakers
Description        : Error handling
Date of last change: 2-July-2004
History            :
                     2-July-2004   Moddified handle_error(): The error message
                                   will be showed by a message box
                     3-July-2004   Removed error message of handle_fatal_error()
                                   and added it to handle_error()

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
#include <gtk/gtk.h>

#include "support.h"

#include <keytouch.h>
#include <set_data.h>


void
handle_error (ERROR error, char *info)
/*
input:
	error	- Indicates which error occured
	info	- String which could be a part of the error message
output:
	-
returns:
	-
description:
	This function prints a error message to stderr and shows a dialog containing the
	message. error indicates which error occured. info is a string which could be a
	part of the error message if it needs extra information.
*/
{
	char *error_message;
	const char *msg[NUM_ERROR] = {	_("Key '%s' not exists. Type 'list' for a list of keys."),
					_("The entered keyboard does not exist!"),
					_("Can not open '%s'!"),
					_("The import file '%s' has an invalid syntax!"),
					_("Can not open the keyboard file."),
					_("The keyboard file has an invalid syntax!\n"
					  "Your settings will be left unchanged."),
					_("Can not open plugin '%s'."),
					_("Can not write the plugin to '%s'."),
					_("The file '%s' is not a plugin!"),
					_("The file '%s' is not a valid keyTouch plugin!")              };

	/* Print the error message to the stderr */
	fprintf (stderr, "> ");
	fprintf (stderr, msg[error], info);
	fprintf (stderr, "\n");
	/* Show a message box containing the message */
	error_message = g_strdup_printf (msg[error], info); /* Create the error message */
	msg_box (error_message, GTK_MESSAGE_ERROR); /* Show the message box */
	g_free (error_message); /* Free the allocated memory for the error message */
}

void
handle_fatal_error (FATAL_ERROR error)
/*
input:
	error	- Indicates which error occured
output:
	-
returns:
	-
description:
	This function prints a error message to stderr and shows a dialog containing the
	message. error indicates which error occured. After printing the message the
	program will be exited and give a return value which is equal to error.
*/
{
	const char *msg[NUM_FERROR] = {	_("Can not allocate any memory!\n"),
					_("There are no keyboards available.\n"
					  "This might be caused by editing the configuration file, which is not recommended.\n"),
					_("Can not read the configuration file.\n"),
					_("Can not write to the configuration file.\n"),
					_("Can not write to the xmodmap configuration file.\n"),
					_("Can not write to the xbindkeys configuration file.\n"),
					_("Can not write to the keyboard file.\n"),
					_("The plugin file has an invalid syntax!\n"),
					_("Can not write to the plugin file.\n"),
					_("Invallid configuration file!\n")      };

	/* Print the error message to the stderr */
	fprintf (stderr, _("> FATAL ERROR: %s\n"), msg[error-1]);
	/* Show a message box containing the message */
	msg_box ((char *) msg[error], GTK_MESSAGE_ERROR);

	exit (error+1);
}
