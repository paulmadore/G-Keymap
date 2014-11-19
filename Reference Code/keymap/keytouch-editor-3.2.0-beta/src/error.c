/*---------------------------------------------------------------------------------
Name               : error.c
Author             : Marvin Raaijmakers
Description        : Shows an error message to the user
Date of last change: 24-Jun-2007
History            : 24-Jun-2007 Compiler warnings fixes
                     01-Jul-2006 Added KTSetErrorMsg() and KTGetErrorMsg()
                     25-Jan-2006 Added KTSetErrorHandler() and modified KTError()
                                 so that it calls (*error_handler)()

    Copyright (C) 2005-2007 Marvin Raaijmakers

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
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

static char *error_msg = NULL;
static void (*error_handler)(const char*) = NULL;

void
KTError (char *msg, char *str)
{
	gchar *error_message;
	
	fprintf (stderr, "keytouch-editor: ");
	fprintf (stderr, msg, str);
	putc ('\n', stderr);
	
	/* Create the error message */
	if (error_handler && asprintf (&error_message, msg, str) != -1)
	{
		(*error_handler) (error_message);
		free (error_message); /* Free the allocated memory for the error message */
	}
} 


void
KTSetErrorHandler (void (*handler)(const char*))
{
	error_handler = handler;
}


void
KTSetErrorMsg (char *msg, char *str)
/*
Input:
	msg		- The error message to store.
	str		- If 'msg' contains "%s", "%s" will be replaced by 'str'.
Global output:
	error_msg	- A copy of the string constructed using 'msg' and 'str'.
Description:
	This function lets 'error_msg' point to a copy of the string constructed
	using 'msg' and 'str'. If 'error_msg' does not point to NULL, this function
	will free the memory pointed to by 'error_msg'.
	The value of 'error_msg' can be retrieved by calling KTGetErrorMsg().
*/
{
	if (error_msg)
	{
		free (error_msg);
	}
	if (asprintf (&error_msg, msg, str) == -1)
	{
		fprintf (stderr, "keytouch-editor: Did not have enough memory for "
		                 "copying the following error message:\n   ");
		fprintf (stderr, msg, str);
		putc ('\n', stderr);
		error_msg = NULL;
	}
}


char
*KTGetErrorMsg (void)
/*
Global input:
	error_msg	- This pointer will be returned
Returns:
	The error message that was contructed by the latest KTSetErrorMsg() call.
*/
{
	return error_msg;
}
