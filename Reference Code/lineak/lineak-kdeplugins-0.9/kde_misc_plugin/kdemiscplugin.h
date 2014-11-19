/***************************************************************************
                          kdeplugin.h  -  description
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
#ifndef LINEAK_PLUGIN_H
#define LINEAK_PLUGIN_H

#include "../config.h"
#include <lineak/plugin_definitions.h>

class LKbd;
class LConfig;
class LKey;
#ifdef HAVE_XOSD
class xosdCtrl;
#endif

 /** For a plugin, we need the following interfaces */
using namespace lineak_plugins;

extern "C" int initialize(init_info init);
extern "C" int exec(LKey* imyKey);
extern "C" macro_info* macrolist();
extern "C" void cleanup();
extern "C" const char* identifier();

#ifdef HAVE_XOSD
extern "C" void initialize_xosd(xosdCtrl *myXosd);
#endif

void macroARTS_SUSPEND(LCommand& command);
bool macroADHOC_DCOP(LCommand& command);
//void macroARTS_VOLUP(LCommand& command);
//void macroARTS_VOLDOWN(LCommand& command);
//void macroARTS_MUTE(LCommand& command);


#endif
