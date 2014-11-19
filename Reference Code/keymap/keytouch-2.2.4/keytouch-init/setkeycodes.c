/*---------------------------------------------------------------------------------
Name               : setkeycodes.c
Author             : Marvin Raaijmakers
Description        : Assigns a keycode to every function key of the current
                     keyboard.
Date of last change: 25-Jun-2006
History            : 25-Jun-2006 setkeycodes() now skips keys with scancode = 0

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
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/kd.h>

#include <getfd.h>
#include <string_to_keycode.h> /* For NUM_KEYCODES */
#include <keytouch-init.h> 

void
setkeycodes (KTKeySettingsList *key_list)
/*
Input:
	key_list	- The list of keys to set the keycodes for
Output:
	-
Returns:
	-
Description:
	For every key in key_list this function sets the keycode to the scancode of
	the key in kernel scancode-to-keycode mapping table.
*/
{
	KTKeySettings		*key;
	int			fd;
	struct kbkeycode	a;
	
	fd = getfd(NULL);
	for (key = key_list->head; key != NULL; key = key->next)
	{
		a.keycode = key->keycode;
		a.scancode = key->scancode;
		if (a.scancode >= 0xe000)
		{
			a.scancode -= 0xe000;
			a.scancode += 128;	/* some kernels needed +256 */
		}
		if (a.keycode >= NUM_KEYCODES)
		{
			KTError ("Failed to set keycode:", "");
			fprintf (stderr, "keycode %d is larger than %d\n",
			         a.keycode, NUM_KEYCODES-1);
		}
		else if (a.scancode && ioctl(fd, KDSETKEYCODE, &a)) /* Set the keycode */
		{
			KTError ("Failed to set keycode:", "");
			fprintf (stderr, "keycode %d to scancode %d (0x%x)\n",
			         a.keycode, a.scancode, a.scancode);
		}
	}
}
