/*---------------------------------------------------------------------------------
Name               : run_browser.c
Author             : Marvin Raaijmakers
Description        : Runs the webbrowser
Date of last change: 03-Jun-2006
History            : 03-Jun-2006 "kfmclient openURL" is used as the KDE browser and
                                 "gnome-open" as the GNOME browser

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
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

#include "support.h"

#include <keytouch.h>
#include <set_data.h>
#include <check_desktop.h>

void
run_browser (	char	*browser,
		char	*url        )
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
	desktop environment or firefox.
*/
{
	char *browser_command;
	
	if (browser == NULL || browser[0] == '\0') /* If the browser was not set */
	{
		switch (check_desktop())
		{
			case KDE:
				browser = "kfmclient openURL";
				break;
			case GNOME:
				browser = "gnome-open";
				break;
			default:
				/* An unknown desktop environment is running,
				 * so we just try to run firefox
				 */
				browser = "firefox";
				/* Inform the user about this */
				msg_box (	_("Because you did not set your browser in the "
						  "preferences tab and because keyTouch could "
						  "not identify your desktop environment, keyTouch "
						  "will try to run 'firefox'.\n"
						  "Please set your browser in the preferences tab."),
						GTK_MESSAGE_INFO );
				break;
		}
	}
	
	browser_command = g_strdup_printf ("%s \"%s\" &", browser, url);
	system (browser_command);
	g_free (browser_command);
}
