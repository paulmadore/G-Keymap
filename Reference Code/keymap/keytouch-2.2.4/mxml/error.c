/*-------------------------------------------------------------------------------
Name               : error.c
Author             : Marvin Raaijmakers
Description        : Error message handler
Date of last change: 24-Jun-2006
History            : 24-Jun-2006 Added XmlSetErrorMsg() and XmlGetErrorMsg()

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
#include <string.h>

#include <mxml.h>

static char *error_msg = NULL;

void
XmlError (char *message)
/*
Input:
	message	- The error message to print.
Output:
	-
Returns:
	-
Description:
	This function prints the error message 'message' to the stderr.
*/
{
	fprintf (stderr, "libMXml: %s\n", message);
}

void
XmlSetErrorMsg (char *msg)
/*
Input:
	msg		- The error message to store.
Global output:
	error_msg	- A copy of the string 'msg'.
Description:
	This function lets 'error_msg' point to a copy of the string 'msg'. If
	'error_msg' does not point to NULL, this function will free the memory
	pointed to by 'error_msg'. The value of 'error_msg' can be retrieved by
	calling XmlGetErrorMsg().
*/
{
	if (error_msg)
	{
		free (error_msg);
	}
	error_msg = strdup (msg);
	if (error_msg == NULL)
	{
		fputs ("libMXml: Did not have enough memory for"
		       "copying the following error message:", stderr);
		fputs (msg, stderr);
		error_msg = NULL;
	}
}


char
*XmlGetErrorMsg (void)
/*
Global input:
	error_msg	- This pointer will be returned
Returns:
	The error message that was contructed by the latest XmlSetErrorMsg() call.
*/
{
	return error_msg;
}
