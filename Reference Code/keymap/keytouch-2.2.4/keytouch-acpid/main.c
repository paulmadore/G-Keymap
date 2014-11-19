/*----------------------------------------------------------------------------------
Name               : main.c
Author             : Marvin Raaijmakers
Description        : The main source file of keytouchd, that contains the
                     event-loop. This program grabs the key press events of the
                     extra function keys and performs the appropriate action when
                     such key is pressed.
Date of last change: 21-Sep-2006
History            : 21-Sep-2006 Added support for IBM/Lenovo notebooks (modified
                                 read_event())
                     03-Jan-2006 - Added signalhandler clean_exit()
                                 - Runs clear_acpi_key_list() when the current
                                   keyboard changes
                                 - The unused keyboard name will be freed after
                                   calling get_current_keyboard(&tmp_keyboard_name)

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
------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <signal.h>
#include <sys/wait.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <X11/XF86keysym.h>

#include <mxml.h>
#include <keytouch-acpid.h>
#include "ud_socket.h"


static char *read_line (int fd);
static char *read_event (int fd);
static void clean_exit (int sig);

/* The variables below are global because
 * they are needed by clean_exit()
 */
static KTAcpiKeyList	key_list;
static KTKeyboardName	keyboard_name;
static Display		*display;


int
main (int argc, char *argv[])
{
	KTKeyboardName	tmp_keyboard_name;
	int		sock_fd, i;
	unsigned int	keycode,
			nerrs = 0;
	char		*event_descr;
	
	
	/* Open the acpi socket */
	sock_fd = ud_connect(ACPI_SOCKETFILE);
	if (sock_fd < 0)
	{
		fprintf (stderr, "%s: can't open socket %s: %s\n",
			 argv[0], ACPI_SOCKETFILE, strerror(errno));
		return (EXIT_FAILURE);
	}
	fcntl(sock_fd, F_SETFD, FD_CLOEXEC);
	
	/* Connect to the server specified in the DISPLAY environment variable */
	display = XOpenDisplay(NULL);
	if (display == NULL)
	{
		KTError ("Cannot connect to X server %s.", XDisplayName(NULL));
		return (EXIT_FAILURE);
	}
	/* We need the XTest extension to send keyboard events */
	if (!XQueryExtension (display, "XTEST", &i, &i, &i))
	{
		KTError ("Extension XTest unavailable on display '%s'.\n", XDisplayName(NULL));
		XCloseDisplay (display);
		return (EXIT_FAILURE);
	}
	
	get_current_keyboard (&keyboard_name);
	read_configuration (&key_list, &keyboard_name);
	
	/* Trap key signals */
	signal (SIGINT, clean_exit);
	signal (SIGQUIT, clean_exit);
	signal (SIGTERM, clean_exit);
	
	while (1)
	{
		event_descr = read_event(sock_fd);
		if (event_descr)
		{
			get_current_keyboard (&tmp_keyboard_name);
			/* If the name of the current keyboard changed */
			if (strcmp(tmp_keyboard_name.model, keyboard_name.model) ||
			    strcmp(tmp_keyboard_name.manufacturer, keyboard_name.manufacturer))
			{
				/* We don't need the old keyboard name anymore */
				XmlFree (keyboard_name.model);
				XmlFree (keyboard_name.manufacturer);
				keyboard_name = tmp_keyboard_name;
				/* Clear the old configuration */
				clear_acpi_key_list (&key_list);
				/* Read the new configuration */
				read_configuration (&key_list, &keyboard_name);
			}
			else
			{
				XmlFree (tmp_keyboard_name.model);
				XmlFree (tmp_keyboard_name.manufacturer);
			}
			
			keycode = get_keycode(event_descr, &key_list);
			/* Is the event bound to a keycode? */
			if (keycode)
			{
				XTestFakeKeyEvent(display, keycode, True, 0);
				XTestFakeKeyEvent(display, keycode, False, 0);
				XFlush (display);
			}
		}
		else if (errno == EPIPE)
		{
			KTError ("events file connection closed", "");
			return (EXIT_FAILURE);
		}
		else if (++nerrs >= ACPI_MAX_ERRS)
		{
			KTError ("too many errors reading events file - aborting", "");
			return (EXIT_FAILURE);
		}
	}
}


void
clean_exit (int sig)
/*
Input:
	-
Output:
	-
Globals:
	key_list
	keyboard_name
	display
Returns:
	-
Description:
	This function prepares the program to exit and then exits.
*/
{
	XmlFree (keyboard_name.model);
	XmlFree (keyboard_name.manufacturer);
	clear_acpi_key_list (&key_list);
	XCloseDisplay (display);
	
	exit (EXIT_SUCCESS);
}


char
*read_event (int fd)
/*
Input:
	fd	- The socket to read from
Output:
	-
Returns:
	A pointer to the readen event description or NULL if reading the event
	failed.
Description:
	read_event() reads a line from fd and puts it in a string. The last space
	character in this string will be replaced by '\0' if the string does not
	begin with "ibm". A pointer to the resulting string will be returned. The
	string may not be modified and may not be used after calling this function
	again.
*/
{
	char *line, *last_space, *c;
	
	line = read_line(fd);
	if (line && strncmp ("ibm", line, 3))
	{
		last_space = NULL;
		for (c = line; *c != '\0'; c++)
		{
			if (isspace(*c))
			{
				last_space = c;
			}
		}
		if (last_space != NULL)
		{
			*last_space = '\0';
		}
	}
	return (line);
}


/* The code below was borrowed from:
 *  acpi_listen.c - ACPI client for acpid's UNIX socket
 *
 *  Portions Copyright (C) 2003 Sun Microsystems (thockin@sun.com)
 *  Some parts (C) 2003 - Gismo / Luca Capello <luca.pca.it> http://luca.pca.it
 */
#define MAX_BUFLEN	1024
static char *
read_line(int fd)
{
	static char *buf;
	int buflen = 64;
	int i = 0;
	int r;
	int searching = 1;

	while (searching) {
		buf = realloc(buf, buflen);
		if (!buf) {
			fprintf(stderr, "ERR: malloc(%d): %s\n",
				buflen, strerror(errno));
			return NULL;
		}
		memset(buf+i, 0, buflen-i);

		while (i < buflen) {
			r = read(fd, buf+i, 1);
			if (r < 0 && errno != EINTR) {
				/* we should do something with the data */
				fprintf(stderr, "ERR: read(): %s\n",
					strerror(errno));
				return NULL;
			} else if (r == 0) {
				/* signal this in an almost standard way */
				errno = EPIPE;
				return NULL;
			} else if (r == 1) {
				/* scan for a newline */
				if (buf[i] == '\n') {
					searching = 0;
					buf[i] = '\0';
					break;
				}
				i++;
			}
		}
		if (buflen >= MAX_BUFLEN) {
			break;
		}
		buflen *= 2;
	}

	return buf;
}
