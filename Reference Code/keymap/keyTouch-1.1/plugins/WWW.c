/*-------------------------------------------------------------------------------
Name               : WWW.c
Author             : Marvin Raaijmakers
Description        : KeyTouch plugin for starting the browser with a particulair
                     URL
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
#include <string.h>
#include <keytouch-plugin.h>

#define HOME	1
#define SEARCH	2

/* This source file can produce 2 plugins:
 * - A plugin that starts the browser with the home page
 * - A plugin that starts the browser with the search page
 * KIND_OF_SITE indicates which one should been created.
 * Two value's are possible: HOME and SEARCH
 */
#if KIND_OF_SITE == HOME
#	define SITE(_pref) ((_pref)->home_page)
#	define PLUGIN_NAME "Home"
#elif KIND_OF_SITE == SEARCH
#	define SITE(_pref) ((_pref)->search_page)
#	define PLUGIN_NAME "Search"
#endif

const char	*keytouch_plugin_name = "WWW - " PLUGIN_NAME,
		*keytouch_plugin_file = "WWW_" PLUGIN_NAME ".so";

void
plugin_function (PREFERENCES *preferences)
{
	char browser[MAX_PROG_SIZE+MAX_FILE_SIZE+6];
	
	/* Add the name of the browser to the browser string */
	strncpy (browser, preferences->browser, MAX_PROG_SIZE);
	strcat (browser, " '");
	/* Add the URL of the site to the browser string */
	strncat (browser, SITE(preferences), MAX_FILE_SIZE);
	/* Add a '&' to the end of the command so that the browser will run at the background */
	strcat (browser, "' &");
	/* Run the browser */
	system (browser);
}
