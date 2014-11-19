/*-------------------------------------------------------------------------------
Name               : filemanager.c
Author             : Marvin Raaijmakers
Description        : KeyTouch plugin for opening the "documents directory" in the
                     filemanager
Date of last change: 25-September-2004

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
#include <stdlib.h>
#include <string.h>
#include <check_desktop.h>
#include <keytouch-plugin.h>

const char	*keytouch_plugin_name = "My Documents",
		*keytouch_plugin_file = "my_documents.so";

void
plugin_function (PREFERENCES *preferences)
{
	char filemanager[MAX_PROG_SIZE+MAX_FILE_SIZE+2];
	
	switch (check_desktop())
	{
		case KDE:
			strncpy (filemanager, "konqueror ", MAX_PROG_SIZE+2);
			break;
		case GNOME:
			strncpy (filemanager, "nautilus ", MAX_PROG_SIZE+2);
			break;
		default:
			return ((void) 0);
	}
	strncat (filemanager, preferences->documents_dir, MAX_FILE_SIZE);
	system (filemanager);
} 
