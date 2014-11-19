/*---------------------------------------------------------------------------------
Name               : email.c
Author             : Marvin Raaijmakers
Description        : Plugin for keyTouch that provides functions for e-mail
                     programs.
Date of last change: 1-Sept-2005

    Copyright (C) 2005 Marvin Raaijmakers

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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <X11/Xlib.h>

#include <plugin.h>

void email (KTPreferences *preferences);

KTPluginKey key_new_mail[] = {
	{{"Evolution - Mail",  TRUE},  ControlMask,             "N"}, /* Evolution */
	{{"Evolution",         FALSE}, ControlMask | ShiftMask, "M"}, /* Evolution */
	{{"Kontact",           TRUE},  ControlMask,             "N"}, /* Kontact (kmail) */
	{{"Mail & Newsgroups", FALSE}, ControlMask,             "M"}, /* Thunderbird */
	{{NULL,                TRUE},  ControlMask,             "M"}  /* Thunderbird with other title */
};

KTPluginKey key_get_mail[] = {
	{{"Evolution",         FALSE}, 0,                       "F9"}, /* Evolution */
	{{"Kontact",           TRUE},  ControlMask,             "L"},  /* Kontact (kmail) */
	{{"Mail & Newsgroups", FALSE}, ControlMask | ShiftMask, "T"},  /* Thunderbird */
	{{NULL,                TRUE},  ControlMask | ShiftMask, "T"},  /* Thunderbird with other title */
};

KTPluginKey key_reply[] = {
	{{"Kontact", TRUE},  0,           "R"},  /* Kontact (kmail) */
	{{NULL,      FALSE}, ControlMask, "R"},  /* Others */
};

KTPluginKey key_forward[] = {
	{{"Evolution",         FALSE}, ControlMask, "F"}, /* Evolution */
	{{"Kontact",           TRUE},  0,           "F"}, /* Kontact (kmail) */
	{{"Mail & Newsgroups", FALSE}, ControlMask, "L"}, /* Thunderbird */
	{{NULL,                TRUE},  ControlMask, "L"}, /* Thunderbird with other title */
};


KeytouchPlugin plugin_struct = {
	{"E-mail", "Marvin Raaijmakers", "GPL 2", "1.0",
	  "Provides functions for e-mail clients.\n"
	  "The following clients are supported:\n"
	  "- Kontact\n"
	  "- Evolution\n"
	  "- Thunderbird"},
	"email.so",
	5,
	{{"E-mail", KTPluginFunctionType_Function, {.function = email}},
	 {"New mail message", KTPluginFunctionType_Key, {.key_function = {5, key_new_mail}}},
	 {"Send/Receive", KTPluginFunctionType_Key, {.key_function = {4, key_get_mail}}},
	 {"Reply", KTPluginFunctionType_Key, {.key_function = {2, key_reply}}},
	 {"Forward", KTPluginFunctionType_Key, {.key_function = {4, key_forward}}}
	}
};


void
email (KTPreferences *preferences)
{
	if (fork() == 0)
	{
		execlp ("sh", "sh", "-c", preferences->email_program, NULL);
		exit (EXIT_SUCCESS);
	}
} 
