/***************************************************************************
                          kmixplugin.cpp  -  description
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

#include <qstringlist.h>
#include <qstring.h>
#include <dcopobject.h>
#include <dcopclient.h>
#include <kapplication.h>
#include <klocale.h>

#include "kmixclient.h"

#include <config.h>
#include <lineak/lconfig.h>
#include <lineak/lkbd.h>
#include <lineak/lkey.h>
#include <lineak/lobject.h>
#include <lineak/plugin_definitions.h>
#include <lineak/displayctrl.h>
#include <lineak/lineak_core_functions.h>

using namespace lineak_core_functions;
// #ifdef HAVE_XOSD
// #include <lineak/xosdctrl.h>
// #endif

#include <iostream>
#include <sstream>
#include <stdlib.h>
using namespace std;

#include "kmixplugin.h"

/** These are required */
#define NUM_MACROS 3
string kmix_symbols[NUM_MACROS] = { "KMIX_VOLUP", "KMIX_VOLDOWN", "KMIX_MUTE" };
macro_info* kmix_macinfo = NULL;
identifier_info* kmix_idinfo = NULL;
string dname = "";
bool verbose = false;
bool enable;
DCOPClient* kmix_dcop;
displayCtrl* kmix_Display = NULL;

extern "C" int initialize(init_info init) {
        verbose = init.verbose;
	enable = init.global_enable;
	kmix_dcop = new DCOPClient();
	if (!(kmix_dcop->registerAs( "kmixlineakdcop" )))
	    msg("Could not register with dcop");
	if (!kmix_dcop->attach()) {
	    error("Could not attach kmix_dcop to dcop server");
        } else
	    msg("Registered");

/*	if (!kmix_dcop->isApplicationRegistered("kmix")) {
		if (KApplication::startServiceByDesktopName("kmix")) {
		cout << i18n("No running instance of KMix found.");
		return false;
		}
	} */

        return true;
}
extern "C" identifier_info* identifier() {
	kmix_idinfo = new (identifier_info);
	kmix_idinfo->description = "KMix Plugin";
	kmix_idinfo->identifier = "kmixplugin";
	kmix_idinfo->type = "MACRO";
	kmix_idinfo->version = VERSION;
	return kmix_idinfo;
}

extern "C" int exec(LObject* imyKey, XEvent xev) {
	LCommand command;
	
	if (imyKey->getType() == CODE || imyKey->getType() == SYM) {
	    command = imyKey->getCommand(xev.xkey.state);
	    dname = imyKey->getCommandDisplayName(xev.xkey.state);   
	}
        if (imyKey->getType() == BUTTON) {
	    command = imyKey->getCommand(xev.xbutton.state);
            dname = imyKey->getCommandDisplayName(xev.xbutton.state);
	}
	if (dname == "" or dname == snull) {
          if (imyKey->isUsedAsToggle())
              dname = imyKey->getNextToggleName();
          else
              dname = imyKey->getName();
        }
	
	/* Give verbose output about the command and display name */
	if (verbose) {
		cout << "----------------------------------------" << endl;
		//cout << "For keycode: " << keycode << endl;
		cout << " Key: " << endl << *(imyKey) << endl;
	  	cout << "	XOSD Display: " << dname << endl;
		cout << "	Command: " << command << endl;
		cout << "----------------------------------------" << endl;
	}
	//cout.setf(ios::boolalpha);
        //cout << "enable = " << enable << endl;
        //cout << "Executing " << imyKey->getCommand() << endl;
        if (command.getMacroType() == "KMIX_VOLUP") { /* Use KMIXers Volume UP */
                macroKMIX_VOLUP(command);
        } else if (command.getMacroType() == "KMIX_VOLDOWN") { /* Use KMIXers Volume Down */
                macroKMIX_VOLDOWN(command);
        } else if (command.getMacroType() == "KMIX_MUTE") { /* Use KMIXers Mute */
                macroKMIX_MUTE(command);
        } else if (command.isMacro())  /** If we are a macro, but we can't find one that matches, return false. */
                return false;
        
        if (imyKey->isUsedAsToggle())
            imyKey->toggleState();
        

        return true;
}
extern "C" macro_info* macrolist() {
	kmix_macinfo = new (macro_info);
	kmix_macinfo->num_macros = NUM_MACROS;
	kmix_macinfo->macro_list = kmix_symbols;
        return kmix_macinfo;
}
extern "C" void cleanup() {
	msg("Cleaning up plugin kmixplugin");
	if (kmix_macinfo != NULL) {
		delete (kmix_macinfo);
		kmix_macinfo = NULL;
	}
	if (kmix_idinfo != NULL) {
		delete (kmix_idinfo);
		kmix_idinfo = NULL;
	}
	if (kmix_dcop != NULL) {
		kmix_dcop->detach();
		delete(kmix_dcop);
		kmix_dcop = NULL;
	}
	msg("Done cleaning up plugin kmixplugin");
}
extern "C" int initialize_display(displayCtrl *ikmix_Display) {
	msg("Initializing on screen display!");
        kmix_Display = ikmix_Display;
	return true;
}

void macroKMIX_VOLUP(LCommand& command) {
   if (enable) {
       	const vector<std::string>& args = command.getArgs();
    	int retval = 0;

	KMIXClient kmix(kmix_dcop);
	/** If we are just using the macro plain, no argument */
	if (args.size() == 0 ) {
		// KMIX uses increments of three
		retval = kmix.volumeUp(3,"Mixer0");
	}
	else {
		if (args.size() == 1) {
		   retval = kmix.volumeUp(atoi((args[0]).c_str()), "Mixer0");
		} else {
		/** Assuming we have pairs of values: value, Mixer */
		if (args.size() >=2 && (args.size() % 2 == 0)) {
		   msg("Multiple volume ups.");
			int value = 0;
			std::string device = "";
			for (vector<std::string>::const_iterator it = args.begin(); it != args.end();) {
			   value = atoi(it->c_str());
			   it++;
			   device = *it;
			   it++;
			   if (verbose) cout << device << " adjusted by: " << value << endl;
			   retval = kmix.volumeUp(value,device);
			}
		   }
		}
	}
        if (kmix_Display != NULL && retval != -1) {
		//cout << "Setting the display for kmix volup!" << endl;
		int s = static_cast<int>(kmix_Display->getMaxAudio());
		kmix_Display->setMaxAudio(100);
                kmix_Display->volume(retval);
		kmix_Display->setMaxAudio(s);
	}
   }
}
void macroKMIX_VOLDOWN(LCommand& command) {
   if (enable) {
	const vector<std::string>& args = command.getArgs();
	int retval = 0;

	KMIXClient kmix(kmix_dcop);
	/** If we are just using the macro plain, no argument */
	if (args.size() == 0) {
		// KMIX uses increments of three.
		retval = kmix.volumeDown(3,"Mixer0");
	}
	else {
	if (args.size() == 1) {
		retval = kmix.volumeDown(atoi((args[0]).c_str()), "Mixer0");
	}
	else {
		/** Assuming we have pairs of values: value, Mixer */
		if (args.size() >=2 && (args.size() % 2 == 0)) {
		        msg("Multiple volume downs.");
			int value = 0;
			std::string device = "";
			for (vector<std::string>::const_iterator it = args.begin(); it != args.end();) {
			   value = atoi(it->c_str());
			   it++;
			   device = *it;
			   it++;
			   if (verbose) cout << device << " adjusted by: " << value << endl;
			   retval = kmix.volumeDown(value,device);
			}
		}
	} // else args.size() == 1
	} // else args.size() == 0

        if (kmix_Display != NULL && retval != -1) {
               int s = static_cast<int>(kmix_Display->getMaxAudio());
               kmix_Display->setMaxAudio(100);
               kmix_Display->volume(retval);
               kmix_Display->setMaxAudio(s);
	}
   }
}
void macroKMIX_MUTE(LCommand& command) {
   if (enable) {
	const vector<std::string>& args = command.getArgs();
	int retval = 0;

	KMIXClient kmix(kmix_dcop);
	/** If we are just using the macro plain, no argument */
	if (args.size() == 0) {
		retval = kmix.mute("Mixer0");
	}
	/** One or more arguments, i.e. list of mixers has been given. */
	else {
		std::string device = "";
	        for(unsigned int i=0 ; i<args.size() ; i++) {
		        device = args[i];
			if (verbose) cout << device << " muted." << endl;
			retval = kmix.mute(device);
		}
	}

        if (kmix_Display != NULL && retval != -1) {
		if (retval == 0)
                   kmix_Display->show(dname);
		else {
		   int s = static_cast<int>(kmix_Display->getMaxAudio());
		   kmix_Display->setMaxAudio(100);
		   kmix_Display->volume(retval);
		   kmix_Display->setMaxAudio(s);
		}
	}
   }
}

