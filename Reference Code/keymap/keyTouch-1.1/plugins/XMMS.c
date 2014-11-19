/*-------------------------------------------------------------------------------
Name               : XMMS.c
Author             : Marvin Raaijmakers
Description        : KeyTouch plugin for starting xmms or sending a command to it
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
#include <keytouch-plugin.h>

#define PLAYPAUSE	(1)
#define STOP		(2)
#define PREVIOUS	(3)
#define NEXT		(4)

/* This source file can produce 5 plugins, that run:
 * - Xmms
 * - A play/pause signal to xmmms
 * - A stop signal to xmmms
 * - A previous signal to xmmms
 * - A next signal to xmmms
 * ACTION indicates which one should been created.
 * Four value's are possible: PLAYPAUSE, STOP, PREVIOUS and NEXT
 * If ACTION is not defined the plugin that runs xmms will be created.
 */
#ifdef ACTION
#	if ACTION == PLAYPAUSE
#		define PARAMETER " -t &"
#		define PLUGIN_NAME " - Play/Pause"
#		define PLUGIN_FILE_NAME "-PlayPause"
#	elif ACTION == STOP
#		define PARAMETER " --stop &"
#		define PLUGIN_NAME " - Stop"
#		define PLUGIN_FILE_NAME "-Stop"
#	elif ACTION == PREVIOUS
#		define PARAMETER " --rew &"
#		define PLUGIN_NAME " - Previous"
#		define PLUGIN_FILE_NAME "-Previous"
#	elif ACTION == NEXT
#		define PARAMETER " --fwd &"
#		define PLUGIN_NAME " - Next"
#		define PLUGIN_FILE_NAME "-Next"
#	endif
#else
#	define PARAMETER	""
#	define PLUGIN_NAME	""
#	define PLUGIN_FILE_NAME	""
#endif

const char	*keytouch_plugin_name = "XMMS" PLUGIN_NAME,
		*keytouch_plugin_file = "XMMS" PLUGIN_FILE_NAME ".so"; 

void
plugin_function (PREFERENCES *preferences)
{
	/* Run the xmms */
	system ("xmms" PARAMETER);
}
