/*-------------------------------------------------------------------------------
Name               : aumix-volume.c
Author             : Marvin Raaijmakers
Description        : KeyTouch plugin for changing the volume
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

#define INCREASE	(1)
#define DECREASE	(2)

/* This source file can produce 2 plugins, that:
 * - Increase the volume
 * - Decrease the volume
 * VOLUME indicates which one should been created.
 * Two value's are possible: INCREASE, DECREASE
 */
#if VOLUME == INCREASE
#	define NEW_VOL "+2"
#	define PLUGIN_NAME "increase"
#elif VOLUME == DECREASE
#	define NEW_VOL "-2"
#	define PLUGIN_NAME "decrease"
#endif

const char	*keytouch_plugin_name = "Aumix - Volume " PLUGIN_NAME,
		*keytouch_plugin_file = "aumix-volume_" PLUGIN_NAME ".so"; 

void
plugin_function (PREFERENCES *preferences)
{
	/* Run the xmms */
	system ("aumix -v" NEW_VOL);
}
