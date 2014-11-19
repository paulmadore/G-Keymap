/*----------------------------------------------------------------------------------
Name               : getscancode.c
Author             : Marvin Raaijmakers
Description        : For getting a scancode and/or keycode from an event device
Date of last change: 22-Aug-2007
History            : 22-Aug-2007 poll_keycode_scancode():
                                  device now MUST report a scancode
                     24-Jun-2007 Compiler warnings fixes
                     30-Aug-2006 Added poll_event_descr() and get_acpi_event_descr()
                     27-Apr-2006 poll_keycode_scancode() will return TRUE when no
                                 scancode and keycode were received or when a mouse
                                 button was clicked
                     05-Apr-2006 The functions now also wait for a possible keycode

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
------------------------------------------------------------------------------------*/
#include <gtk/gtk.h>
#include <glib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "interface.h"
#include "support.h"

#include <keytouch-editor.h>
#include <getscancode.h>
#include <evdev.h>
#include <acpi.h>
#include <ud_socket.h>

static gboolean
poll_keycode_scancode (POLL_SCANCODE_STRUCT *poll)
/*
Input:
	poll->fd		- Contains the file descriptor to the event device
Output:
	poll->keycode_scancode	- The scancode member will be the received scancode
				  and/or the keycode member will be the received
				  keycode.
Returns:
	FALSE if data was read from the event device and this data contains
	information about a key press, otherwise TRUE.
Description:
	This function waits EVDEV_TIMEOUT microseconds for data from the event
	device. If the data contains a scancode poll->keycode_scancode->scancode
	will be equal to it, otherwise poll->keycode_scancode->scancode will remain
	unchanged. If the data contains a keycode poll->keycode_scancode->keycode
	will be equal to it, otherwise poll->keycode_scancode->keycode will remain
	unchanged.
*/
{
	fd_set			set;
	struct timeval		timeout;
	int			readen_size,
				i,
				select_returnval;
	struct input_event	ev[64]; /* Event list */
	
	/* Initialize the file descriptor set. */
	FD_ZERO (&set);
	FD_SET (poll->fd, &set);
	
	/* Initialize the timeout data structure. */
	timeout.tv_sec = 0;
	timeout.tv_usec = EVDEV_TIMEOUT;
	
	/* Wait until there is data available on poll->fd and stop
	 * waiting after EVDEV_TIMEOUT microseconds */
	select_returnval = select (FD_SETSIZE, &set, NULL, NULL, &timeout);
	/* If there is data available */
	if (select_returnval == 1)
	{
		/* Read the data */
		readen_size = read (poll->fd, ev, sizeof(struct input_event) * 64);
		if (readen_size < (int) sizeof(struct input_event))
		{
			KTError (_("Failed to read events."), "");
		}
		else
		{
			/* Read the events and stop after a key event (if there is one in the event list). */
			for (i = 0; i < readen_size / sizeof(struct input_event); i++)
			{
				/* If we receive a sync event */
				if (ev[i].type == EV_SYN)
				{
					break;
				}
				/* If the event contains a scancode */
				else if (ev[i].type == EV_MSC &&
					(ev[i].code == MSC_RAW || ev[i].code == MSC_SCAN))
				{
					poll->keycode_scancode->scancode = ev[i].value;
				}
				/* If the event contains a keycode */
				else if (ev[i].type == EV_KEY)
				{
					poll->keycode_scancode->keycode = ev[i].code;
				}
			}
			/* If the key has no scancode */
			if (!poll->keycode_scancode->scancode)
			{
				poll->keycode_scancode->keycode = 0;
				/*KTError (_("The event device did not report a scancode."), "");*/
				return TRUE;
			}
		}
		gtk_main_quit();
		return FALSE;
	}
	/* If there is no data */
	else
	{
		return TRUE;
	}
}


Boolean
get_keycode_scancode (	char			*evdev_filename,
			char			*msg,
			KTKeycodeScancode	*keycode_scancode )
/*
Input:
	evdev_filename		- The filename of the evdev device to listen for a
				  scancode
	msg			- The message to display while while waiting for a
				  scancode
Output:
	keycode_scancode	- The scancode member will be the received scancode
				  or 0 if no scancode was received. The keycode
				  member will be the received keycode or 0 if no
				  keycode was received.
Returns:
	TRUE if a keycode and/or scancode was retrieved, otherwise FALSE.
Description:
	This function creates a dialog with a "Cancel" button, displays the text of
	pointed to by 'msg' and wait for a scancode and/or keycode from the event
	device whos filename is 'evdev_filename'. If the user clicks the "Cancel"
	button or the device could not be opened or did not send a scancode or
	keycode, this function will return 0.
	If the device did not send a keycode or scancode following error message
	will be shown: "The event device did not report a scancode or keycode."
*/
{
	POLL_SCANCODE_STRUCT	poll;
	GtkWidget		*dialog;
	guint			poll_tag;
	
	keycode_scancode->keycode = 0;
	keycode_scancode->scancode = 0;
	poll.keycode_scancode = keycode_scancode;
	
	/* If opening the event device failed */
	if ((poll.fd = open( evdev_filename, O_RDONLY )) < 0)
	{
		KTError (_("Could not open event device '%s'."), evdev_filename);
	}
	else
	{
		dialog = GTK_WIDGET( create_get_scancode_dialog() );
		/* Set the text to show */
		gtk_label_set_text (GTK_LABEL(lookup_widget(dialog, "msg_label")), msg);
		gtk_widget_show (dialog);
		/* Poll every POLL_INTERVAL milisecond for a scancode and/or keycode */
		poll_tag = g_timeout_add (POLL_INTERVAL, (GSourceFunc) poll_keycode_scancode, &poll);
		gtk_main ();
		gtk_widget_destroy (dialog);
		/* Stop polling */
		g_source_remove (poll_tag);
		close (poll.fd);
	}
	return (keycode_scancode->scancode || keycode_scancode->keycode);
}



static gboolean
poll_event_descr (POLL_EVENT_DESCR_STRUCT *poll)
/*
Input:
	poll->fd		- Contains the file descriptor to the event device
Output:
	poll->event_descr	- Will point to new allocated memory that contains
				  the readen event description, or to NULL if no
				  event description was read.
Returns:
	FALSE if data was read from the ACPI socket file, otherwise TRUE.
Description:
	This function waits EVDEV_TIMEOUT microseconds for data from the ACPI socket
	file.
*/
{
	fd_set		set;
	struct timeval	timeout;
	int		select_returnval;
	
	/* Initialize the file descriptor set. */
	FD_ZERO (&set);
	FD_SET (poll->fd, &set);
	
	/* Initialize the timeout data structure. */
	timeout.tv_sec = 0;
	timeout.tv_usec = EVDEV_TIMEOUT;
	
	/* Wait until there is data available on poll->fd and stop
	 * waiting after EVDEV_TIMEOUT microseconds */
	select_returnval = select (FD_SETSIZE, &set, NULL, NULL, &timeout);
	/* If there is data available */
	if (select_returnval == 1)
	{
		/* Read the data */
		poll->event_descr = keytouch_strdup (read_eventdescr (poll->fd));
		gtk_main_quit();
		return FALSE;
	}
	/* If there is no data */
	else
	{
		return TRUE;
	}
}


char
*get_acpi_event_descr (char *msg)
/*
Input:
	msg	- The message to display while while waiting for a event description
Returns:
	NULL if no event description was read. If an event description was read then
	a address to this event description will be returned. If the memory pointed
	to by this address is no longer needed, it should be freed.
Description:
	This function creates a dialog with a "Cancel" button, displays the text of
	pointed to by 'msg' and wait for an event description from the ACPI socket.
	If the user clicks the "Cancel" button or the device could not be opened or
	did not send a event description, this function will return NULL.
*/
{
	POLL_EVENT_DESCR_STRUCT	poll;
	GtkWidget		*dialog;
	guint			poll_tag;
	
	poll.event_descr = NULL;
	
	/* If opening the event device failed */
	if ((poll.fd = ud_connect (ACPI_SOCKETFILE)) < 0)
	{
		KTError (_("Could not open ACPI socket file '%s'."), ACPI_SOCKETFILE);
	}
	else
	{
		dialog = GTK_WIDGET( create_get_scancode_dialog() );
		/* Set the text to show */
		gtk_label_set_text (GTK_LABEL(lookup_widget(dialog, "msg_label")), msg);
		gtk_widget_show (dialog);
		/* Poll every POLL_INTERVAL milisecond for a scancode and/or keycode */
		poll_tag = g_timeout_add (POLL_INTERVAL, (GSourceFunc) poll_event_descr, &poll);
		gtk_main ();
		gtk_widget_destroy (dialog);
		/* Stop polling */
		g_source_remove (poll_tag);
		close (poll.fd);
	}
	return poll.event_descr;
}
