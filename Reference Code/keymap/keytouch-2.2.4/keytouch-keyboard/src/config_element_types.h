/*---------------------------------------------------------------------------------
Name               : config_element_types.h
Author             : Marvin Raaijmakers
Description        : Contains all ellement types, attribute names and attribute
                     values that appear in the (XML) configuration files.
Date of last change: 19-Aug-2005

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

/* All used attribute names in the configuration files */
#define ATTR_NAME_ACTIONTYPE		"action-type"
#define ATTR_NAME_ISDEFAULT		"isdefault"
#define ATTR_NAME_FORMAT		"format"
#define ATTR_NAME_KEYTYPE		"key-type"

/* All used attribute values in the configuration files */
#define ATTR_VAL_PROGRAM		"program"
#define ATTR_VAL_PLUGIN			"plugin"
#define ATTR_VAL_ACPIHOTKEY		"acpi-hotkey"

/* All used element types in the configuration files */
#define ELEM_TYPE_KEYBOARD		"keyboard"
#define ELEM_TYPE_KEYLIST		"key-list"
#define ELEM_TYPE_DEFAULTACTION		"default-action"
#define ELEM_TYPE_KEY			"key"
#define ELEM_TYPE_KEYNAME		"name"
#define ELEM_TYPE_KEYCODE		"keycode"
#define ELEM_TYPE_SCANCODE		"scancode"
#define ELEM_TYPE_PLUGINNAME		"plugin-name"
#define ELEM_TYPE_PLUGINFUNCTION	"plugin-function"
#define ELEM_TYPE_ACTION		"action"
#define ELEM_TYPE_PREFERENCES		"preferences"
#define ELEM_TYPE_DOCUMENTSDIR		"documents-dir"
#define ELEM_TYPE_HOMEPAGE		"home-page"
#define ELEM_TYPE_SEARCHPAGE		"search-page"
#define ELEM_TYPE_BROWSER		"browser"
#define ELEM_TYPE_EMAILPROG		"email-program"
#define ELEM_TYPE_CHATPROG		"chat-program"
#define ELEM_TYPE_CURRENTKEYBOARD	"current-keyboard"
#define ELEM_TYPE_MODEL			"model"
#define ELEM_TYPE_MANUFACTURER		"manufacturer"
#define ELEM_TYPE_KEYBOARDNAME		"keyboard-name"
#define ELEM_TYPE_KEYBOARDINFO		"keyboard-info"
#define ELEM_TYPE_FILEINFO		"file-info"
#define ELEM_TYPE_AUTHOR		"author"
#define ELEM_TYPE_SYNTAXVERSION		"syntax-version"
#define ELEM_TYPE_LASTCHANGE		"last-change"
#define ELEM_TYPE_ACPI_EVENT_DESCR	"event-descr"
