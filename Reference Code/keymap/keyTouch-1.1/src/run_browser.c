/*-------------------------------------------------------------------------------
Name               : run_browser.c
Author             : Marvin Raaijmakers
Description        : Runs a browser
Date of last change: 22-December-2004
History            :
                     22-December-2004 Created run_browser()

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
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

#include "support.h"

#include <keytouch.h>
#include <set_data.h>
#include <check_desktop.h>

void
run_browser (	char	browser[MAX_PROG_SIZE],
		char	url[MAX_FILE_SIZE]       )
/*
input:
	browser	- The name of the browser to run
	url	- The url to open in the browser
output:
	-
returns:
	-
description:
	This function runs browser (variable), which opens url. If the browser string
	is empty this funtion will attempt to run the browser of the currently running
*/
{
	char browser_command[MAX_PROG_SIZE+MAX_FILE_SIZE+6];
	
	if (browser[0] == '\0') /* If the browser was not set */
	{
		switch (check_desktop())
		{
			case KDE:
				browser = "konqueror";
				break;
			case GNOME:
				browser = "nautilus";
				break;
			default:
				/* A unknown desktop environment is running,
				 * so we just try to run mozilla
				 */
				browser = "mozilla";
				/* Inform the user about this */
				msg_box (	_("Because you did not set your browser at the "
						  "preferences tab and because keyTouch could "
						  "not identify your desktop environment, keyTouch "
						  "will try to run 'mozilla'.\n"
						  "Please set your browser at the preferences tab."),
						GTK_MESSAGE_INFO );
				break;
		}
	}
	
	sprintf (browser_command, "%s \"%s\" &", browser, url);
	system (browser_command);
}
