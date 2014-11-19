/*-------------------------------------------------------------------------------
Name               : keytouch-plugin.h
Author             : Marvin Raaijmakers
Description        : Header file for the keytouch plugin handler or keytouch
                     plugins
Date of last change: 8-August-2004
History            :
                     8-August-2004  Created this file

    Copyright (C) 2004 Marvin Raaijmakers

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    You can contact me at: marvin_raaijmakers(at)lycos(dot)nl
    (replace (at) by @ and (dot) by .)
---------------------------------------------------------------------------------*/

#define MAX_PLUGIN_NAME_SIZE	(40)
#define MAX_PROG_SIZE		(99)
#define MAX_FILE_SIZE		(299)
#define MAX_LINE_SIZE		(299)

typedef struct {
	char	documents_dir[MAX_FILE_SIZE+1],
		home_page[MAX_FILE_SIZE+1],
		search_page[MAX_FILE_SIZE+1],
		browser[MAX_PROG_SIZE+1],
		email_program[MAX_PROG_SIZE+1],
		messenger[MAX_PROG_SIZE+1];
} PREFERENCES;
