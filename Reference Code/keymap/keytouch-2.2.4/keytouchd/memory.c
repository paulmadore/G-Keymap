/*---------------------------------------------------------------------------------
Name               : memory.c
Author             : Marvin Raaijmakers
Description        : Allocates memory
Date of last change: 20-Aug-2005

    Copyright (C) 2005 Marvin Raaijmakers

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

#include <keytouchd.h>

void
*keytouch_malloc (size_t size)
{
	void *ptr;
	
	ptr = malloc(size);
	if (ptr == NULL)
	{
		KTError ("Could not allocate memory.", "");
		exit (EXIT_FAILURE);
	}
	return (ptr);
}
