/*---------------------------------------------------------------------------------
Name               : amixer.h
Author             : Marvin Raaijmakers
Description        : Header file for the keyTouch amixer plugin
Date of last change: 24-Sep-2006
History            : 07-Feb-2006 Added definition of MSGQ_AMIXER_PROJ_ID

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
-----------------------------------------------------------------------------------*/
#ifndef _AMIXER_H
#define _AMIXER_H

#define UPDATE_INTERVAL		50	/* miliseconds*/
#define SHOW_WINDOW_TIME	1	/* seconds */
#define MSGQ_PERMISSIONS	0600
#define MSGQ_AMIXER_PROJ_ID	'm'

#define CHANGE_VOL_CMD(_vol)	("amixer set Master " _vol " > /dev/null")
#define VOL_DEFAULT_INCR	"2%+"
#define VOL_DEFAULT_DECR	"2%-"
#define VOL_10PERCENT_INCR	"10%+"
#define VOL_10PERCENT_DECR	"10%-"

static char *get_keytouch_user_dir (void);

typedef struct {
		long	mtype;
		time_t	time;
} MSGBUF;

typedef struct {
	GtkWidget	*window,
			*volume_bar;
	time_t		close_time;
	int		msgqid;
} VOLUMEBAR_INFO; 

#endif
