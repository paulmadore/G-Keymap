/*-------------------------------------------------------------------------------
Name               : start_prog.c
Author             : Marvin Raaijmakers
Description        : KeyTouch plugin for starting a program
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


#define EMAIL		(1)
#define MESSENGER	(2)

/* This source file can produce 2 plugins:
 * - A plugin that starts an e-mail program
 * - A plugin that starts a messenger
 * KIND_OF_PROG indicates which one should been created.
 * Two value's are possible: EMAIL and MESSENGER
 */
#if KIND_OF_PROG == EMAIL
#	define PROG(_pref) ((_pref)->email_program)
#	define PLUGIN_NAME "E-mail"
#elif KIND_OF_PROG == MESSENGER
#	define PROG(_pref) ((_pref)->messenger)
#	define PLUGIN_NAME "Messenger"
#endif

const char	*keytouch_plugin_name = PLUGIN_NAME,
		*keytouch_plugin_file = PLUGIN_NAME ".so"; 

void
plugin_function (PREFERENCES *preferences)
{
	/* Run the program */
	system (PROG(preferences));
}
