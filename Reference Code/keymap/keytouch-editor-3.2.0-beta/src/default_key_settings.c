/*---------------------------------------------------------------------------------
Name               : default_key_settings.c
Author             : Marvin Raaijmakers
Description        : Contains default names and actions for known keys with a
                     keycode.
Date of last change: 01-Jul-2006
History            : 01-Jul-2006 Added defaults for keycodes KEY_NEW, KEY_REDO,
                                 KEY_SEND, KEY_REPLY, KEY_FORWARDMAIL, KEY_SAVE and
                                 KEY_DOCUMENTS
                     08-Apr-2006 Created this file

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
#include <keytouch-editor.h>
#include <keys.h>

const KTKeyDefaults key_defaults[KEY_MAX + 1] = {
	[0 ... KEY_MAX] = {.name = NULL},
	[KEY_MUTE] = {"Mute", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "Amixer", .function_name = "Mute"}}},
	[KEY_VOLUMEDOWN] = {"Volume Down", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "Amixer", .function_name = "Volume decrease"}}},
	[KEY_VOLUMEUP] = {"Volume Up", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "Amixer", .function_name = "Volume increase"}}},
	[KEY_POWER] = {"Power", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "Lock Screen", .function_name = "Lock Screen"}}},
	[KEY_COMPOSE] = {"Compose", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "E-mail", .function_name = "New mail message"}}},
	[KEY_STOP] = {"Stop", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "WWW Browser", .function_name = "Stop"}}},
	[KEY_UNDO] = {"Undo", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "Common actions", .function_name = "Undo"}}},
	[KEY_REDO] = {"Redo", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "Common actions", .function_name = "Redo"}}},
	[KEY_COPY] = {"Copy", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "Common actions", .function_name = "Copy"}}},
	[KEY_OPEN] = {"Open", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "Common actions", .function_name = "Open"}}},
	[KEY_PASTE] = {"Paste", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "Common actions", .function_name = "Paste"}}},
	[KEY_FIND] = {"Find", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "Common actions", .function_name = "Find"}}},
	[KEY_CUT] = {"Cut", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "Common actions", .function_name = "Cut"}}},
	[KEY_HELP] = {"Help", {.program = {.type = KTActionTypeProgram, .command = "khelpcenter || gnome-help"}}},
	[KEY_CALC] = {"Help", {.program = {.type = KTActionTypeProgram, .command = "kcalc || gnome-calculator || xcalc"}}},
	[KEY_SETUP] = {"Setup", {.program = {.type = KTActionTypeProgram, .command = "kcontrol || gnome-control-center"}}},
	[KEY_SLEEP] = {"Sleep", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "Lock Screen", .function_name = "Lock Screen"}}},
	[KEY_WAKEUP] = {"Wake up", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "Lock Screen", .function_name = "Lock Screen"}}},
	[KEY_PROG1] = {"Prog 1", {.program = {.type = KTActionTypeProgram, .command = "keytouch"}}},
	[KEY_PROG2] = {"Prog 2", {.program = {.type = KTActionTypeProgram, .command = "keytouch"}}},
	[KEY_WWW] = {"WWW", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "WWW Browser", .function_name = "Home"}}},
	[KEY_MAIL] = {"Mail", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "E-mail", .function_name = "E-mail"}}},
	[KEY_BOOKMARKS] = {"Favorites", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "WWW Browser", .function_name = "Add Bookmark"}}},
	[KEY_COMPUTER] = {"Computer", {.program = {.type = KTActionTypeProgram, .command = ""}}},
	[KEY_BACK] = {"Back", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "WWW Browser", .function_name = "Back"}}},
	[KEY_FORWARD] = {"Forward", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "WWW Browser", .function_name = "Forward"}}},
	[KEY_CLOSECD] = {"Close CD", {.program = {.type = KTActionTypeProgram, .command = "eject -t"}}},
	[KEY_EJECTCD] = {"Eject CD", {.program = {.type = KTActionTypeProgram, .command = "eject"}}},
	[KEY_EJECTCLOSECD] = {"Eject/Close CD", {.program = {.type = KTActionTypeProgram, .command = "eject -T"}}},
	[KEY_NEXTSONG] = {"Next song", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "XMMS", .function_name = "Next"}}},
	[KEY_PLAYPAUSE] = {"Play/Pause", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "XMMS", .function_name = "Play/Pause"}}},
	[KEY_PREVIOUSSONG] = {"Previous song", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "XMMS", .function_name = "Previous"}}},
	[KEY_STOPCD] = {"Stop CD", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "XMMS", .function_name = "Stop"}}},
	[KEY_RECORD] = {"Record", {.program = {.type = KTActionTypeProgram, .command = "k3b"}}},
	[KEY_REWIND] = {"Rewind", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "XMMS", .function_name = "Previous"}}},
	[KEY_CONFIG] = {"Config", {.program = {.type = KTActionTypeProgram, .command = "kcontrol || gnome-control-center"}}},
	[KEY_HOMEPAGE] = {"Homepage", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "WWW Browser", .function_name = "Home"}}},
	[KEY_REFRESH] = {"Reload", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "WWW Browser", .function_name = "Reload"}}},
	[KEY_PLAYCD] = {"Play CD", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "XMMS", .function_name = "Play/Pause"}}},
	[KEY_PAUSECD] = {"Pause CD", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "XMMS", .function_name = "Play/Pause"}}},
	[KEY_PROG3] = {"Prog 3", {.program = {.type = KTActionTypeProgram, .command = "keytouch"}}},
	[KEY_PROG4] = {"Prog 4", {.program = {.type = KTActionTypeProgram, .command = "keytouch"}}},
	[KEY_SUSPEND] = {"Suspend", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "Lock Screen", .function_name = "Lock Screen"}}},
	[KEY_CLOSE] = {"Close", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "Common actions", .function_name = "Close/Quit"}}},
	[KEY_PLAY] = {"Play", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "XMMS", .function_name = "Play/Pause"}}},
	[KEY_FASTFORWARD] = {"Fast forward", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "XMMS", .function_name = "Next"}}},
	[KEY_PRINT] = {"Print", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "Common actions", .function_name = "Print"}}},
	[KEY_EMAIL] = {"E-mail", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "E-mail", .function_name = "E-mail"}}},
	[KEY_CHAT] = {"Chat", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "Chat", .function_name = "Chat"}}},
	[KEY_SEARCH] = {"Search", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "WWW Browser", .function_name = "Search"}}},
	[KEY_MEDIA] = {"Media", {.program = {.type = KTActionTypeProgram, .command = "gmplayer"}}},
	[KEY_SEND] = {"Send", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "E-mail", .function_name = "Send/Receive"}}},
	[KEY_REPLY] = {"Reply mail", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "E-mail", .function_name = "Reply"}}},
	[KEY_FORWARDMAIL] = {"Forward mail", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "E-mail", .function_name = "Forward"}}},
	[KEY_SAVE] = {"Save", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "Common actions", .function_name = "Save"}}},
	[KEY_DOCUMENTS] = {"Documents", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "Filemanager", .function_name = "Documents"}}},
	[KEY_NEW] = {"New", {.plugin = {.type = KTActionTypePlugin, .plugin_name = "Common actions", .function_name = "New (file)"}}}
};
