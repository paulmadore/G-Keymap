/*-------------------------------------------------------------------------------
Name               : aumix-mute.c
Author             : Marvin Raaijmakers
Description        : KeyTouch plugin for muting/unmuting the volume
Date of last change: 31-August-2004

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
#include <stdio.h>
#include <string.h>
#include <keytouch-plugin.h>

#define OVERWRITE		(1)
#define EQUAL			(0)
#define MAX_VOLUME_SIZE		(10)
#define MAX_SETTINGS_SIZE	(10)
#define UNMUTED			("UNMUTED")

const char	*keytouch_plugin_name = "Aumix - Mute/Unmute",
		*keytouch_plugin_file = "aumix-mute.so";

void
plugin_function (PREFERENCES *preferences)
{ 
	char aumix[MAX_PROG_SIZE+1], volume[MAX_VOLUME_SIZE+1], settings_file_name[MAX_FILE_SIZE+1];
	FILE *settings_file, *aumix_pipe;
	
	strncpy (settings_file_name, getenv("HOME"), MAX_FILE_SIZE);
	strncat (settings_file_name, "/.aumix_mute", MAX_FILE_SIZE-strlen(settings_file_name));
	
	settings_file = fopen(settings_file_name, "r");
	if (settings_file)
	{
		fgets (volume, MAX_VOLUME_SIZE+1, settings_file);
		fclose (settings_file);
	}
	else /* If the file could not be opened, it probably not exists so the volume is unmuted */
	{
		strcpy (volume, UNMUTED);
	}
	
	if ( strcmp(volume, UNMUTED) == EQUAL ) /* If the volume was unmuted */
	{
		aumix_pipe = popen ("aumix -q | grep vol | tr -d \'vol \'", "r");
		if (aumix_pipe == NULL)
		{
			exit (1);
		}
		fgets(volume, MAX_VOLUME_SIZE+1, aumix_pipe); /* Get the current volume */
		pclose (aumix_pipe);
		strcpy (aumix, "aumix -v 0,0");
	}
	else
	{
		strcpy (aumix, "aumix -v ");
		strncat (aumix, volume, MAX_PROG_SIZE-strlen(aumix));
		strcpy (volume, "UNMUTED");
	}
	
	/* Open the settings file for writing */
	settings_file = fopen(settings_file_name, "w");
	if (settings_file == NULL)
	{
		printf ("ERROR: Could not write %s. Check your permissions!\n", settings_file_name);
		exit (1);
	}
	fputs (volume, settings_file); /* Write the settings */
	fclose (settings_file);
	system (aumix);
}
