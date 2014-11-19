/*---------------------------------------------------------------------------------
Name               : evdev.c
Author             : Marvin Raaijmakers
Description        : Provides functions to related to the evdev module
Date of last change: 16-Mar-2008
History            : 16-Mar-2008 Added clear_evdev_list()
                     24-Jun-2007 Compiler warnings fixes
                     13-Mar-2006 Wrote load_evdev_module()

    Copyright (C) 2006-2008 Marvin Raaijmakers

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
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#include <evdev.h>
#include <keytouch-editor.h>


static int always_true (const struct dirent *file);
static EVENT_DEVICE *create_event_device (const char *dir, const char *file_name);


int
always_true (const struct dirent *file)
{
	return (TRUE);
}


EVENT_DEVICE
*create_event_device (	const char	*dir,
			const char	*file_name )
/*
Input:
	dir		- The directory where the event device is located in
	file_name	- The file name of the event device
Returns:
	A new EVENT_DEVICE or NULL if reading the device failed.
Description:
	This function reads information from the event device, named file_name and
	located in dir, creates a new EVENT_DEVICE and fills in the members of it
	with the readen information.
	
*/
{
	char *complete_file_name;
	int event_device;
	EVENT_DEVICE *evdev;
	int version;
	unsigned short id[4];
	char name[256] = "Unknown";
	
	if (asprintf(&complete_file_name, "%s/%s", dir, file_name) == -1)
	{
		KTError (_("Could not allocate memory."), "");
		exit (EXIT_FAILURE);
	}
	
	event_device = open(complete_file_name, O_RDONLY);
	/* If opening the event device failed */
	if (event_device < 0)
	{
		free (complete_file_name);
		return NULL;
	}
	/* Get the driver version */
	if (ioctl(event_device, EVIOCGVERSION, &version))
	{
		free (complete_file_name);
		close (event_device);
		return NULL;
	}
	/* Get the id's */
	ioctl (event_device, EVIOCGID, id);
	/* Get the device name */
	ioctl (event_device, EVIOCGNAME(sizeof(name)), name);
	
	evdev = keytouch_malloc (sizeof(EVENT_DEVICE));
	evdev->file_name = keytouch_strdup (file_name);
	evdev->complete_file_name = complete_file_name;
	evdev->driver_version = version;
	evdev->bus_id = id[ID_BUS];
	evdev->vendor_id = id[ID_VENDOR];
	evdev->product_id = id[ID_PRODUCT];
	evdev->version_id = id[ID_VERSION];
	evdev->device_name = keytouch_strdup(name);
	
	close (event_device);
	return evdev;
}


void
clear_evdev_list (EVDEV_LIST *evdev_list)
/*
Input:
	evdev_list
Output:
	evdev_list  - All elements are removed from the list and the allocated
	              memory of these elements is freed.
*/
{
	EVDEV_LIST_ENTRY *next, *entry;
	
	entry = evdev_list->head;
	while (entry)
	{
		next = entry->next;
		free (entry->evdev->file_name);
		free (entry->evdev->complete_file_name);
		free (entry->evdev->device_name);
		free (entry->evdev);
		free (entry);
		entry = next;
	}
	evdev_list->head = evdev_list->tail = NULL;
}


Boolean
read_evdev_list (EVDEV_LIST *evdev_list,
                 char       *dir )
/*
Input:
	dir		- The path where the event devices are located in
Output:
	evdev_list	- The list containing all readen devices
returns:
	TRUE if the devices where successfully read, otherwise FALSE.
description:
	This function reads information about all event devices in dir and puts it
	in evdev_list.
*/
{
	struct dirent	**file_list;
	int		num_files,
			count;
	EVENT_DEVICE	*new_evdev;
	EVDEV_LIST_ENTRY *entry;
	
	evdev_list->head = evdev_list->tail = NULL;
	num_files = scandir (dir, &file_list, always_true, alphasort);
	if (num_files >= 0)
	{
		for (count = 0; count < num_files; count++)
		{
			new_evdev = create_event_device(dir, file_list[count]->d_name);
			if (new_evdev)
			{
				entry = keytouch_malloc(sizeof(EVDEV_LIST_ENTRY));
				entry->evdev = new_evdev;
				entry->next = NULL;
				if (evdev_list->head)
				{
					evdev_list->tail->next = entry;
					evdev_list->tail = entry;
				}
				else
				{
					evdev_list->head = evdev_list->tail = entry;
				}
			}
			free (file_list[count]);
		}
		free (file_list);
	}
	return (num_files >= 0 && evdev_list->head);
}


char
*load_evdev_module (void)
/*
Returns:
	NULL if evdev was successfully loaded, otherwise the address of the error
	message will be returned. The error message will be the first line of the
	output of "modprobe evdev" and should be freed with if it is no longer
	needed.
Description:
	This function runs "modprobe evdev 2>&1" and returns an error message if
	modprobe failed.
*/
{
	FILE	*modprobe;
	char	output_buffer[100],
		*error = NULL;
	Boolean	output_available;
	
	modprobe = popen ("modprobe evdev 2>&1", "r");
	if (modprobe)
	{
		if (!feof( modprobe ))
		{
			output_available = (Boolean) fgets (output_buffer, 99, modprobe);
		}
		else
		{
			output_available = FALSE;
		}
		if (pclose( modprobe))
		{
			if (output_available)
			{
				error = strdup (output_buffer);
			}
			else
			{
				error = strdup (_("Unknown reason"));
			}
			if (error == NULL)
			{
				KTError (_("Could not allocate memory."), "");
				exit (EXIT_FAILURE);
			}
		}
	}
	else
	{
		error = strdup ("see output on stderr");
		if (error == NULL)
		{
			KTError (_("Could not allocate memory."), "");
			exit (EXIT_FAILURE);
		}
		perror ("keytouch-editor");
	}
	
	return error;
}
