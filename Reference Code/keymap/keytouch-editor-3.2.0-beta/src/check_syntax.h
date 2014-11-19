/*---------------------------------------------------------------------------------
Name               : check_syntax.h
Author             : Marvin Raaijmakers
Description        : Header file for check_syntax.c
Date of last change: 23-Jun-2006
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
#ifndef INCLUDE_CHECK_SYNTAX
#define INCLUDE_CHECK_SYNTAX

#define MAX_NUM_KEYS	300

#define SYNTAX_VERSION_MAJOR	1
#define SYNTAX_VERSION_MINOR	2
#define SYNTAX_VERSION_STRING	"1.2"

extern Boolean check_keyboard_file (XmlDocument *keyboard_file);

#endif
