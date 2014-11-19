/***************************************************************************
                          kmixplugin.h  -  description
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
#ifndef KMIX_PLUGIN_H
#define KMIX_PLUGIN_H

#include <config.h>
#include <lineak/plugin_definitions.h>

class LKbd;
class LConfig;
class LObject;
class displayCtrl;

 /** For a plugin, we need the following interfaces */
using namespace lineak_plugins;
extern "C" int initialize(init_info init);
extern "C" int exec(LObject* imyKey, XEvent xev);
extern "C" macro_info* macrolist();
extern "C" void cleanup();
extern "C" identifier_info* identifier();
extern "C" int initialize_display(displayCtrl *myDisplay);

void macroKMIX_VOLUP(LCommand& command);
void macroKMIX_VOLDOWN(LCommand& command);
void macroKMIX_MUTE(LCommand& command);


#endif
