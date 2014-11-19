/*---------------------------------------------------------------------------------
Name               : email.c
Author             : Marvin Raaijmakers
Description        : For sending text files via e-mail
Date of last change: 24-Jun-2007
History            : 24-Jun-2007 Compiler warnings fixes
                     27-Apr-2006  init_sockaddr() does not exit the program when it
                                  fails, but returns FALSE instead
                     11-Mar-2006  Wrote this file

    Copyright (C) 2006-2007 Marvin Raaijmakers

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
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#include <email.h>


static Boolean init_sockaddr (struct sockaddr_in *name, const char *hostname, uint16_t port);
static int smtp_get_reply (int sock);
static Boolean smtp_send_string (int sock, char *string);
static Boolean smtp_connect (char *hostname, int *sock);
static Boolean copy_file_to_socket (char *filename, int sock);


Boolean
init_sockaddr (	struct sockaddr_in	*name,
		const char		*hostname,
		uint16_t		port)
/*
Input:
	hostname	- The host name to connect to
	port		- The port to use for the connection
Output:
	name		- Will be initialized with the socket address for a
			  connection to 'hostname' on port 'port'.
Returns:
	TRUE if 'name' is successfully initialized, otherwise FALSE.
*/
{
	struct hostent *hostinfo;
	
	name->sin_family = AF_INET;
	name->sin_port = htons (port);
	hostinfo = gethostbyname (hostname);
	if (hostinfo == NULL)
	{
		fprintf (stderr, "Unknown host %s.\n", hostname);
		return FALSE;
	}
	name->sin_addr = *(struct in_addr *) hostinfo->h_addr;
	return TRUE;
}


int
smtp_get_reply (int sock)
/*
Input:
	sock	- Connection to the SMTP server
Returns:
	The reply code received from the SMTP server and -1 on timeout.
Description:
	This function reads the reply code from the SMTP server and returns it. This
	function will return -1 when nothing is received after SMTP_TIMEOUT seconds.
*/
{
	fd_set		set;
	struct timeval	timeout;
	int		reply_code;
	char		connection_buffer[100],
			*endptr;
	
	
	reply_code = -1;
	/* Initialize the file descriptor set. */
	FD_ZERO (&set);
	FD_SET (sock, &set);
	
	/* Initialize the timeout data structure. */
	timeout.tv_sec = SMTP_TIMEOUT;
	timeout.tv_usec = 0;
	
	/* Wait until there is data available on socket and stop
	 * waiting after SMTP_TIMEOUT microseconds */
	if (select (FD_SETSIZE, &set, NULL, NULL, &timeout) && /* If there is data available */
	    read (sock, connection_buffer, 99) )               /* If the data was successfully read */
	{
		connection_buffer[99] = '\0';
		reply_code = (int) strtol (connection_buffer, &endptr, 10);
		if (endptr == NULL)
		{
			reply_code = -1;
		}
	}
	
	return reply_code;
}


Boolean
smtp_send_string (	int	sock,
			char	*string )
/*
Input:
	sock	- Connection to the SMTP server
	string	- The string to send
Returns:
	TRUE if the string was successfully sent, otherwise FALSE.
Description:
	This function sends 'string' (without '\0' character) to sock.
*/
{
	if (write (sock, string, strlen(string)) < 0)
	{
		perror (__FUNCTION__);
		return FALSE;
	}
	return TRUE;
}


Boolean
smtp_connect (	char	*hostname,
		int	*sock )
/*
Input:
	hostname	- The hostname of the SMTP server
Output:
	sock		- Will the contain the socket of the connection (if TRUE is
			  returned)
Returns:
	TRUE if the connection was successfully established, otherwise FALSE.
Description:
	This function tries to connect to the SMTP server named 'hostname'. FALSE
	will be returned when connecting failed. Otherwise a HELO command will be
	sent to the server and TRUE will be returned if the server accepts the
	connection.
*/
{
	struct sockaddr_in	address;
	Boolean			connected = FALSE;
	
	/* Create the socket. */
	*sock = socket (PF_INET, SOCK_STREAM, 0);
	if (*sock >= 0)
	{
		/* If a socket address has been made */
		if (init_sockaddr (&address, hostname, SMTP_PORT))
		{
			/* If de connection was successfully established */
			connected = connect (*sock, (struct sockaddr *) &address, sizeof(address)) >= 0 &&
			            smtp_get_reply(*sock) == SMTP_REPLY_GREETING &&
			            smtp_send_string(*sock, "HELO bla\n") && smtp_get_reply(*sock) == SMTP_REPLY_OK;
		}
		else
		{
			connected = FALSE;
		}
	}
	return (connected);
}


Boolean
copy_file_to_socket (	char	*filename,
			int	sock )
/*
Input:
	filename	- The file to read from
Output:
	sock		- The contents of file will be written to this socket.
Returns:
	TRUE if the file was successfully copied, otherwise FALSE.
Description:
	This function opens the file named 'filename' and reads it. The readen
	content will be written to 'sock'.
*/
{
	FILE *file;
	char c;
	
	file = fopen (filename, "r");
	if (file == NULL)
	{
		return FALSE;
	}
	while ((c = getc(file)) != EOF)
	{
		if (write (sock, &c, 1) != 1)
		{
			return FALSE;
		}
	}
	return TRUE;
}



Boolean
email_file (	char		*to,
		char		*from,
		char		*subject,
		char		*filename,
		const char	*smtp_hostnames[] )
/*
Input:
	to		- The e-mail address to send the file to
	from		- The sender e-mail address
	subject		- The subject of the message
	filename	- The name of the file that will be body of the message
	smtp_hostnames	- An array of pointers to hostnames of SMTP servers. The
			  last element in the array must point to NULL.
Returns:
	TRUE (!= 0 actually) if the message was sent successfully, otherwise FALSE.
Description:
	This function sends an e-mail to the e-mail address 'to'. The message will
	contain the contents of the file, name 'filename'. This function tries to
	send the message via the first hostname in smtp_hostnames. It tries every
	next hostname in the array until it finds one where it can successfully
	send to.
*/
{
	char **hostname;
	Boolean success;
	int sock;
	
	success = FALSE;
	/* Find an SMTP server where we can connect to: */
	for (hostname = (char **) smtp_hostnames; *hostname; hostname++)
	{
		if (smtp_connect (*hostname, &sock)) /* If we are connected */
		{
			success = smtp_send_string (sock, "MAIL FROM: <") &&
				smtp_send_string (sock, from) &&
				smtp_send_string (sock, ">\n") &&
				smtp_get_reply (sock) == SMTP_REPLY_OK &&
				smtp_send_string (sock, "RCPT TO: <") &&
				smtp_send_string (sock, to) &&
				smtp_send_string (sock, ">\n") &&
				smtp_get_reply (sock) == SMTP_REPLY_OK &&
				smtp_send_string (sock, "DATA\n") &&
				smtp_get_reply (sock) == SMTP_REPLY_START_MAIL_INPUT &&
				smtp_send_string (sock, "From: <") &&
				smtp_send_string (sock, from) &&
				smtp_send_string (sock, ">\n") &&
				smtp_send_string (sock, "Subject: ") &&
				smtp_send_string (sock, subject) &&
				smtp_send_string (sock, "\n\n") &&
				copy_file_to_socket (filename, sock) &&
				smtp_send_string (sock, "\r\n.\r\n") &&
				smtp_get_reply (sock) == SMTP_REPLY_OK &&
				smtp_send_string (sock, "QUIT\n") &&
				smtp_get_reply (sock) == SMTP_REPLY_GOODBYE;
			if (success)
			{
				break;
			}
		}
	}
	
	return success;
}
