/*---------------------------------------------------------------------------------
Name               : string_to_keycode.h
Author             : Marvin Raaijmakers
Description        : Header file for string_to_keycode.c
Date of last change: 01-Jul-2006

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

    You can contact me at: marvinr(at)users(dot)sf(dot)net
    (replace (at) by @ and (dot) by .)
-----------------------------------------------------------------------------------*/
#ifndef INCLUDE_STRING_TO_KEYCODE
#define INCLUDE_STRING_TO_KEYCODE

#include <keys.h>

#define NUM_KEYCODES	256

#define kernel_keycode_to_string(_keycode) (key_name[(_keycode)])

extern const char *key_name[KEY_MAX + 1];

extern unsigned int string_to_kernel_keycode (char *string);
extern unsigned int string_to_xkeycode (char *string);

#endif
