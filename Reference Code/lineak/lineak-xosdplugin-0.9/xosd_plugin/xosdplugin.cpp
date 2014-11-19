/***************************************************************************
                          xosdplugin.cpp  -  description
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
#include <config.h>
#include <lineak/lconfig.h>
#include <lineak/lkbd.h>
#include <lineak/lkey.h>
#include <lineak/lobject.h>

#include <lineak/plugin_definitions.h>
#include <lineak/displayctrl.h>
#include <lineak/lineak_util_functions.h>
#include <lineak/lineak_core_functions.h>
#include "xosdctrl.h"

#include <algorithm>
#include <cctype>

#include <iostream>
#include <sstream>
#include <stdlib.h>
using namespace std;
using namespace lineak_core_functions;
#include "xosdplugin.h"

//macro_info* default_macinfo = NULL;
directive_info* dir_info = NULL;
identifier_info* idinfo = NULL;
init_info* initinfo = NULL;
bool verbose = false;
bool very_verbose = false;
bool enable;

//xosdCtrl myxosd;
xosdCtrl* default_display = NULL;

extern "C" int initialize(init_info init) {
        initinfo = &init;
        verbose = init.verbose;
	enable = init.global_enable;
	if (default_display == NULL)  
		default_display = new xosdCtrl(*(init.config));

        return true;
}
extern "C" identifier_info* identifier() {
        idinfo = new (identifier_info);
	idinfo->description = "XOSD On Screen Display Plugin";
	idinfo->identifier = "xosd";
	idinfo->type = "DISPLAY";
	idinfo->version = VERSION;
	return idinfo;
}
extern "C" void cleanup() {
       msg("Cleaning up plugin defaultplugin");
	/*if (default_macinfo != NULL) {
		delete (default_macinfo);
		default_macinfo = NULL;
	} */
        if (idinfo != NULL) {
		delete (idinfo);
		idinfo = NULL;
	}
	if (default_display != NULL) {
	        msg("Cleaning up XOSD");
		default_display->cleanup();
		msg("DONE CLEANING UP XOSD");
		delete(default_display);
		default_display = NULL;
	}
        if (dir_info->directives != NULL) {
                delete(dir_info->directives);
                dir_info->directives = NULL;
        }
        if (dir_info != NULL) {
                delete (dir_info);
                dir_info = NULL;
        }
	msg("Done cleaning up plugin defaultplugin");
}
extern "C" displayCtrl* get_display() {
        vmsg("Returning pointer to xosdCtrl");
        if (default_display == NULL)
            default_display = new xosdCtrl(*(initinfo->config)); 
	return (default_display);
}

extern "C" void initialize_display(displayCtrl *imyDisplay) {
        if (verbose) cout << "Initializing display!" << endl;
	if ( idinfo->type == "DISPLAY" )
		if (verbose) cout << "This is a display plugin, it is already initialized." << endl;
	//if (default_display == NULL)
        //   default_display = (xosdCtrl*)imyDisplay;
}

extern "C" directive_info* directivelist() {
        if (dir_info == NULL) {
           dir_info = new (directive_info);
           dir_info->directives = NULL;
        }
        if ( dir_info->directives == NULL ) {
           dir_info->directives = new ConfigDirectives();
        }
        (dir_info->directives)->addValue("Display_plugin","xosd");
        return dir_info;
}
