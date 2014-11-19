/*---------------------------------------------------------------------------------
Name               : evdev.h
Author             : Marvin Raaijmakers
Description        : Header file for evdev.c
Date of last change: 16-Mar-2008
History            :

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
#ifndef _EVDEV_H
#define _EVDEV_H

#include <linux/input.h>
#ifndef MSC_RAW
#	define MSC_RAW	0x03
#endif
#ifndef MSC_SCAN
#	define MSC_SCAN	0x04
#endif
#ifndef EV_SYN
#	define EV_SYN 0
#endif

#ifndef _BOOL_DEF
 #define _BOOL_DEF
 typedef int Boolean;
 #define TRUE	1
 #define FALSE	0
#endif

#define EVDEV_DIR	"/dev/input/"

typedef struct {
	char *file_name,
	     *complete_file_name,
	     *device_name;
	int driver_version,
	    bus_id,
	    vendor_id,
	    product_id,
	    version_id;
} EVENT_DEVICE;

typedef struct _evdev_list_entry {
	EVENT_DEVICE *evdev;
	struct _evdev_list_entry *next;
} EVDEV_LIST_ENTRY;

typedef struct {
	EVDEV_LIST_ENTRY *head, *tail;
} EVDEV_LIST;


#define evdev_file_name(_evdev) ((_evdev)->file_name)
#define evdev_complete_file_name(_evdev) ((_evdev)->complete_file_name)
#define evdev_device_name(_evdev) ((_evdev)->device_name)
#define evdev_bus_id(_evdev) ((_evdev)->bus_id)
#define evdev_bus_string(_evdev) ((_evdev)->bus_id == BUS_I8042 ? "PS/2" : ((_evdev)->bus_id == BUS_USB ? "USB" : ""))


extern Boolean read_evdev_list (EVDEV_LIST *evdev_list, char *dir);
extern char *load_evdev_module (void);
extern void clear_evdev_list (EVDEV_LIST *evdev_list);

#endif
