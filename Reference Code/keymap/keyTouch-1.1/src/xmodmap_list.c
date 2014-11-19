/*-------------------------------------------------------------------------------
Name               : xmodmap_list.c
Author             : Marvin Raaijmakers
Description        : Providing operations for XMODMAP_LIST
Date of last change: 5-December-2004
History            :
                     5-December-2004 Created:
                                     - xmm_new()
                                     - xmml_clear()
                                     - xmml_add()

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

#include <keytouch.h>

XMODMAP
*xmm_new (void)
/*
input:
	-
output:
	-
returns:
	The address of the new XMODMAP.
description:
	This function creates a new (initialized) XMODMAP and returns its address.
*/
{
	XMODMAP *xmodmap;
	
	xmodmap = (XMODMAP *) malloc( sizeof(XMODMAP) );
	if (xmodmap == NULL)
	{
		handle_fatal_error (FERR_MALLOC);
	}
	/* Initialize the elements values */
	xmodmap->hardware_code = 0;
	*(xmodmap->key_sym) = '\0';
	xmodmap->next = NULL;
	
	return (xmodmap);
}

void
xmml_clear (XMODMAP_LIST *xmodmap_list)
/*
input:
	-
output:
	xmodmap_list	- Empty XMODMAP_LIST
returns:
	-
description:
	The function clears xmodmap_list
*/
{
	XMODMAP *xmodmap, *temp;
	
	xmodmap = xmodmap_list->head;
	while (xmodmap)
	{
		temp = xmodmap;
		xmodmap = xmodmap->next;
		free (temp);
	}
	xmodmap_list->head = xmodmap_list->tail = NULL;
}


void
xmml_add (	XMODMAP_LIST	*xmodmap_list,
		XMODMAP		*new_xmodmap       )
/*
input:
	new_xmodmap	- The new XMODMAP to add to xmodmap_list
output:
	xmodmap_list	- The list from which new_xmodmap was added to
returns:
	-
description:
	This function adds new_xmodmap to xmodmap_list.
*/
{
	if (xmodmap_list->head == NULL)
	{
		xmodmap_list->head = xmodmap_list->tail = new_xmodmap;
	}
	else
	{
		xmodmap_list->tail->next = new_xmodmap;
		xmodmap_list->tail = new_xmodmap;
	}
}
