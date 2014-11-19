/***************************************************************************
                          kscdplugin.cpp  -  description
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
#include <kscdclient.h>

#include <config.h>
#include <lineak/lconfig.h>
#include <lineak/lkbd.h>
#include <lineak/lkey.h>
#include <lineak/lobject.h>
#include <lineak/plugin_definitions.h>
#ifdef HAVE_XOSD
#include <lineak/xosdctrl.h>
#endif
#include <lineak/displayctrl.h>

#include <iostream>
#include <sstream>
#include <stdlib.h>
using namespace std;

#include "kscdplugin.h"

/** These are required */
#define NUM_MACROS 12 

string kscd_symbols[NUM_MACROS] = { "KSCD_PLAY", "KSCD_PAUSE", "KSCD_STOP", "KSCD_PREVIOUS", "KSCD_NEXT", "KSCD_EJECT", "KSCD_TOGGLE_LOOP", "KSCD_TOGGLE_SHUFFLE", "KSCD_TOGGLE_TIME_DISPLAY", "KSCD_CURRENT_TRACK", "KSCD_CURRENT_ALBUM", "KSCD_CURRENT_ARTIST"};
macro_info* kscd_macinfo = NULL;
identifier_info* kscd_idinfo = NULL;
bool verbose = false;
bool enable;
DCOPClient* kscd_dcop;
displayCtrl* kscd_Display = NULL;

extern "C" int initialize(init_info init) {
        verbose = init.verbose;
	enable = init.global_enable;

	kscd_dcop = new DCOPClient();
	if (!(kscd_dcop->registerAs( "kscdlineakdcop" )))
            if (verbose) cerr << "Could not register" << endl;
	if (!kscd_dcop->attach()) {
            if (verbose) cerr << "Could not attach kscd_dcop to dcop server" << endl;
        } else
            if (verbose) cout << "Registered" << endl;
        /*
    	if (!kscd_dcop->isApplicationRegistered("kscd")) {
            if (KApplication::startServiceByDesktopName("kscd")) {
               cout << i18n(QString("No running instance of kscd found."));
               return false;
            }
        }
        */
        return true;
}
extern "C" identifier_info* identifier() {
	kscd_idinfo = new (identifier_info);
	kscd_idinfo->description = "KSCD Plugin";
	kscd_idinfo->identifier = "kscdplugin";
	kscd_idinfo->type = "MACRO";
	kscd_idinfo->version = VERSION;
	return kscd_idinfo;
}

extern "C" int exec(LObject* imyKey, XEvent xev) {
	LCommand command;
	string dname;

	if (imyKey->getType() == CODE || imyKey->getType() == SYM) {
           command = imyKey->getCommand(xev.xkey.state);
           dname = imyKey->getCommandDisplayName(xev.xkey.state);
	}
	if (imyKey->getType() == BUTTON) {
	   command = imyKey->getCommand(xev.xbutton.state);
           dname = imyKey->getCommandDisplayName(xev.xbutton.state);
        }

	/* Give verbose output about the command and display name */
	if (verbose) {
		cout << "----------------------------------------" << endl;
		//cout << "For keycode: " << keycode << endl;
		cout << " Key: " << endl << *(imyKey) << endl;
	  	cout << "	Display: " << dname << endl;
		cout << "	Command: " << command << endl;
		cout << "----------------------------------------" << endl;
	}
	
	KSCDClient kscd(kscd_dcop);
        QString comm = QString((command.getCommand()).c_str());
	if (dname == "" or dname == snull) {
           if (imyKey->isUsedAsToggle())
                dname = imyKey->getNextToggleName();
           else
                dname = comm.latin1();
        }
        if (comm.upper() == "KSCD_PLAY" ) {
            kscd.play();
	    if ( kscd.currentTrackTitle() != "" ) {
	       dname += ": ";
	       dname += (kscd.currentTrackTitle()).latin1();
	    }
        }
        else if (comm.upper() == "KSCD_PAUSE") {
            kscd.pause();
        }
        else if (comm.upper() == "KSCD_STOP") {
            kscd.stop();
				        }
	else if (comm.upper() == "KSCD_PREVIOUS") {
            kscd.previous();
	    if ( kscd.currentTrackTitle() != "" ) {
	       dname += ": ";
	       dname += (kscd.currentTrackTitle()).latin1();
	    }
        }
	else if (comm.upper() == "KSCD_NEXT") {
            kscd.next();
	    if ( kscd.currentTrackTitle() != "" ) {
	       dname += ": ";
	       dname += (kscd.currentTrackTitle()).latin1();
	    }
	}
	else if (comm.upper() == "KSCD_EJECT") {
	    kscd.eject();
	}
	else if (comm.upper() == "KSCD_TOGGLE_LOOP") {
	    kscd.toggleLoop();
	}
	else if (comm.upper() == "KSCD_TOGGLE_SHUFFLE") {
	    kscd.toggleShuffle();
	}
	else if (comm.upper() == "KSCD_TOGGLE_TIME_DISPLAY") {
	    kscd.toggleTimeDisplay();
	}
        else if (comm.upper() == "KSCD_CURRENT_TRACK") {
	    if ( kscd.currentTrackTitle() != "" ) {
	       cout << kscd.currentTrackTitle();
               dname += ": ";
               dname += kscd.currentTrackTitle().latin1();
	    }
        }
        else if (comm.upper() == "KSCD_CURRENT_ALBUM") {
	    if ( kscd.currentAlbum() != "" ) {
	       cout << kscd.currentAlbum();
               dname += ": ";
               dname += kscd.currentAlbum().latin1();
	    }
        }
        else if (comm.upper() == "KSCD_CURRENT_ARTIST") {
	    if ( kscd.currentArtist() != "" ) {
	       cout << kscd.currentArtist();
               dname += ": ";
               dname += kscd.currentArtist().latin1();
	    }
        }

	if (imyKey->isUsedAsToggle())
		imyKey->toggleState();

        if (kscd_Display != NULL) {
	        kscd_Display->show(dname);
	}

        return true;
}
extern "C" macro_info* macrolist() {
	kscd_macinfo = new (macro_info);
	kscd_macinfo->num_macros = NUM_MACROS;
	kscd_macinfo->macro_list = kscd_symbols;
        return kscd_macinfo;
}
/** Delete any structs or classes we have new'd here */
extern "C" void cleanup() {
        if (verbose) cout << "Cleaning up plugin kscdplugin" << endl;
	if (kscd_macinfo != NULL) {
		delete (kscd_macinfo);
		kscd_macinfo = NULL;
	}
	if (kscd_idinfo != NULL) {
		delete (kscd_idinfo);
		kscd_idinfo = NULL;
	}
	if (kscd_dcop != NULL) {
		kscd_dcop->detach();
		delete(kscd_dcop);
		kscd_dcop = NULL;
	}
	if (verbose) cout << "Done cleaning up plugin kscdplugin" << endl;
}
extern "C" int initialize_display(displayCtrl *imyDisplay) {
        if (verbose) cout << "Initializing on screen display!" << endl;
        kscd_Display = imyDisplay;
	return true;
}

