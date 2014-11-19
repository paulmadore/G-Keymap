/*-------------------------------------------------------------------------------
Name               : memory.c
Author             : Marvin Raaijmakers
Description        : Provides functions for allocating, resizing and freeing
                     memory.
Date of last change: ?
History            : ?

    Copyright (C) 2005-2006 Marvin Raaijmakers

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

    You can contact me at: marvinr(at)users(dot)sf(dot)net
    (replace (at) by @ and (dot) by .)
---------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

#include <mxml.h>


void
*XmlMalloc (size_t size)
/*
Input:
	size	- The number of bytes to allocate.
Output:
	-
Returns:
	A pointer to the allocated space.
Description:
	This function allocates memory of size bytes. If allocating failed it will
	print an error message and exit the program.
*/
{
	void *memory;
	
	memory = malloc(size);
	if (memory == NULL)
	{
		XmlError ("Failed to allocate memory.");
		exit (1);
	}
	
	return (memory);
}

void
*XmlRealloc (	void	*ptr,
		size_t	size  )
/*
Input:
	ptr	- Pointer to the memory block to resize
	size	- The number of bytes to allocate.
Output:
	-
Returns:
	A pointer  to the newly allocated memory, which is suitably aligned for any
	kind of variable and  may  be  different  from ptr,  or NULL if the request
	fails.
Description:
	This function changes  the  size  of the memory block pointed to by ptr to
	size bytes.  The contents will be unchanged to the minimum of  the  old and
	new sizes; newly allocated memory will be uninitialized.
*/
{
	return (realloc(ptr, size));
}


void
XmlFree (void *pointer)
/*
Input:
	pointer	- Pointer to the memory to free.
Output:
	-
Returns:
	-
Description:
	This function frees the memory, pointed to by pointer, that was previously
	allocated by XmlMalloc.
*/
{
	free (pointer);
}
