#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>

#include <keytouch-editor.h>
#include <acpi.h>

#include "ud_socket.h"

static char *read_line (int fd);


Boolean
check_acpi (void)
/*
Returns:
	TRUE if ACPI_SOCKETFILE is a valid and accesible ACPI socket, otherwise FALSE
	will be returned.
*/
{
	int fd;
	
	fd = ud_connect (ACPI_SOCKETFILE);
	if (fd >= 0)
	{
		close (fd);
	}
	return fd >= 0;
}


char
*read_eventdescr (int fd)
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
