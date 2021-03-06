/*---------------------------------------------------------------------------------
Name               : check_desktop.h
Author             : Marvin Raaijmakers
Description        : Headerfile for check_desktop.c
Date of last change: 18-Aug-2005

    Copyright (C) 2004-2005 Marvin Raaijmakers

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

typedef enum {
	UNKNOWN_DESKTOP,
	KDE,
	GNOME
} DESKTOP;

extern DESKTOP check_desktop(void);
