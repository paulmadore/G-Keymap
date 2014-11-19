/***************************************************************************
                          defaultplugin.h  -  description
                             -------------------
    begin                : 日 10月 26 2003
    copyright            : (C) 2003 by Sheldon Lee Wen
    email                : leewsb@hotmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef DEFAULT_PLUGIN_H
#define DEFAULT_PLUGIN_H

#include <config.h>
#include <lineak/plugin_definitions.h>

class LKbd;
class LConfig;
class LObject;
class displayCtrl;

using namespace lineak_plugins;
 /** For a plugin, we need the following interfaces */
extern "C" int initialize(init_info init);
extern "C" int exec(LObject* imyKey,XEvent xev);
extern "C" macro_info* macrolist();
extern "C" void cleanup();
extern "C" identifier_info* identifier();

extern "C" void initialize_display(displayCtrl *myDisplay);

void macroEAK_SYM(LObject* obj, LCommand &command, int type);
void macroEAK_XSENDKEYS(LObject* obj, LCommand &command);
void macroEAK_OPEN_TRAY(LCommand &command);
void macroEAK_CLOSE_TRAY(LCommand &command);
void macroEAK_MUTE(LCommand &command, int type);
void macroEAK_VOLUP(LCommand &command, int type);
void macroEAK_VOLDOWN(LCommand &command, int type);
void macroEAK_PCM_MUTE(LCommand &command);
void macroEAK_PCM_UP(LCommand &command);
void macroEAK_PCM_DOWN(LCommand &command);
void macroEAK_SLEEP(LCommand &command);
void macroEAK_SCREEN_LOCK(LCommand &command);

#endif
