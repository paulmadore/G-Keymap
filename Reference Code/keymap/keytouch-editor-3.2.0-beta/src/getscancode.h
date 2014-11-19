/*---------------------------------------------------------------------------------
Name               : getscancode.h
Author             : Marvin Raaijmakers
Description        : Header file for getscancode.c
Date of last change: 05-Apr-2006
History            :

    Copyright (C) 2006 Marvin Raaijmakers

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
#ifndef _GETSCANCODE_H
#define _GETSCANCODE_H

#include <keytouch-editor.h>

#define POLL_INTERVAL	10	/* miliseconds*/
#define EVDEV_TIMEOUT	1000	/* # of microseconds to wait for input from the evdev file */

#define BTN_MIN 0x100
#define BTN_MAX 0x151

typedef struct {
	KTKeycodeScancode *keycode_scancode;
	int fd;
} POLL_SCANCODE_STRUCT;

typedef struct {
	char *event_descr;
	int fd;
} POLL_EVENT_DESCR_STRUCT;

#endif
