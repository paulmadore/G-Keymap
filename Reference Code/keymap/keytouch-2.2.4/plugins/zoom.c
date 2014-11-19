/*---------------------------------------------------------------------------------
Name               : zoom.c
Author             : Marvin Raaijmakers
Description        : Zoom plugin that can be used with graphic applications or
                     browsers.
Date of last change: 03-Aug-2006
History            : 03-Aug-2006 Created this plugin

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
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <unistd.h>

#include <plugin.h>

KTPluginKey key_in[] = {
	{{"Firefox", TRUE}, ControlMask, "equal"},
	{{"Konqueror", TRUE}, ControlMask, "equal"},
	{{"KPDF", TRUE}, ShiftMask | ControlMask, "plus"},
	{{"KView", TRUE}, ShiftMask | ControlMask, "plus"},
	{{"digiKam", TRUE}, ShiftMask | ControlMask, "plus"},
	{{NULL, TRUE}, ShiftMask, "plus"}
};

KTPluginKey key_out[] = {
	{{"Firefox", TRUE}, ControlMask, "minus"},
	{{"Konqueror", TRUE}, ControlMask, "minus"},
	{{"KPDF", TRUE}, ControlMask, "minus"},
	{{"KView", TRUE}, ControlMask, "minus"},
	{{"digiKam", TRUE}, ControlMask, "minus"},
	{{NULL, TRUE}, 0, "minus"}
};

KTPluginKey key_hundred_percent[] = {
	{{"Firefox", TRUE}, ControlMask, "0"},
	{{"Kuickshow", TRUE}, 0, "o"},
	{{NULL, TRUE}, 0, "1"}
};

KTPluginKey key_fit[] = {
	{{"digiKam", TRUE}, 0, "a"},
	{{NULL, TRUE}, ControlMask | ShiftMask, "e"}
};


KeytouchPlugin plugin_struct = {
	{"Zoom", "Marvin Raaijmakers", "GPL 2", "1.0", "Zoom plugin that can be used with graphic applications or browsers."},
	"zoom.so",
	4,
	{{"In",            KTPluginFunctionType_Key,      {.key_function = {6, key_in}}},
	 {"Out",           KTPluginFunctionType_Key,      {.key_function = {6, key_out}}},
	 {"100%",          KTPluginFunctionType_Key,      {.key_function = {3, key_hundred_percent}}},
	 {"Fit in window", KTPluginFunctionType_Key,      {.key_function = {2, key_fit}}}
	}
};
