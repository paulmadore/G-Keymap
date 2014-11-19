/*---------------------------------------------------------------------------------
Name               : memory.c
Author             : Marvin Raaijmakers
Description        : Allocates memory
Date of last change: 10-May-2006
History            :
                     10-May-2006 Added keytouch_strdup()

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
#include <stdlib.h>
#include <string.h>

#include <keytouch.h>

void
*keytouch_malloc (size_t size)
/*
Description:
	Does the same like malloc() accept that it shows an error message and exits
	the program if the memory allocation failed.
*/
{
	void *ptr;
	
	ptr = malloc(size);
	if (ptr == NULL)
	{
		KTError (_("Could not allocate memory."), "");
		exit (EXIT_FAILURE);
	}
	return ptr;
}

char
*keytouch_strdup (const char *s)
/*
Description:
	Does the same like strdup() accept that it shows an error message and exits
	the program if the memory allocation failed.
*/
{
	char *string;
	
	string = strdup (s);
	if (string == NULL)
	{
		KTError (_("Could not allocate memory."), "");
		exit (EXIT_FAILURE);
	}
	return string;
}
