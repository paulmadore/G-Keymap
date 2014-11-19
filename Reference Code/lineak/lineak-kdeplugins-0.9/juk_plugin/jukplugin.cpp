/***************************************************************************
                          jukplugin.cpp  -  description
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
#include <jukclient.h>

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

#include "jukplugin.h"

/** These are required */
#define NUM_MACROS 13 

string juk_symbols[NUM_MACROS] = { "JUK_PLAY", "JUK_PAUSE", "JUK_STOP", "JUK_PLAYPAUSE", "JUK_BACK", "JUK_FORWARD", "JUK_SEEKBACK", "JUK_SEEKFORWARD", "JUK_VOLUMEUP", "JUK_VOLUMEDOWN", "JUK_VOLUMEMUTE", "JUK_STARTPLAYINGPLAYLIST", "JUK_OPENFILE" };
macro_info* juk_macinfo = NULL;
identifier_info* juk_idinfo = NULL;
bool verbose = false;
bool enable;
DCOPClient* juk_dcop;
displayCtrl* juk_Display = NULL;

extern "C" int initialize(init_info init) {
        verbose = init.verbose;
	enable = init.global_enable;

	juk_dcop = new DCOPClient();
	if (!(juk_dcop->registerAs( "juklineakdcop" )))
            if (verbose) cerr << "Could not register" << endl;
	if (!juk_dcop->attach()) {
            if (verbose) cerr << "Could not attach juk_dcop to dcop server" << endl;
        } else
            if (verbose) cout << "Registered" << endl;
        /*
    	if (!juk_dcop->isApplicationRegistered("juk")) {
            if (KApplication::startServiceByDesktopName("juk")) {
               cout << i18n(QString("No running instance of juk found."));
               return false;
            }
        }
        */
        return true;
}
extern "C" identifier_info* identifier() {
	juk_idinfo = new (identifier_info);
	juk_idinfo->description = "Juk Plugin";
	juk_idinfo->identifier = "jukplugin";
	juk_idinfo->type = "MACRO";
	juk_idinfo->version = VERSION;
	return juk_idinfo;
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
	  	cout << "	XOSD Display: " << dname << endl;
		cout << "	Command: " << command << endl;
		cout << "----------------------------------------" << endl;
	}
	
	JukClient juk(juk_dcop);
        QString comm = QString((command.getCommand()).c_str());
        QString play = "";

	if (dname == "" or dname == snull) {
           if (imyKey->isUsedAsToggle())
                dname = imyKey->getNextToggleName();
           else
                dname = comm.latin1();
        }
        if (comm.upper() == "JUK_PLAY" ) {
            juk.play();
            if ( (play = juk.isPlaying()) != "" ) {
	       dname += ": ";
	       dname += (juk.isPlaying()).latin1();
            }
        }
        else if (comm.upper() == "JUK_PAUSE") {
            juk.pause();
        }
        else if (comm.upper() == "JUK_STOP") {
            juk.stop();
				        }
    	else if (comm.upper() == "JUK_PLAYPAUSE") {
            juk.playPause();
            if ( (play = juk.isPlaying()) != "" ) {
	       dname += ": ";
	       dname += (juk.isPlaying()).latin1();
            }
    	}
	else if (comm.upper() == "JUK_BACK") {
            juk.back();
            if ( (play = juk.isPlaying()) != "" ) {
	       dname += ": ";
	       dname += (juk.isPlaying()).latin1();
            }
        }
	else if (comm.upper() == "JUK_FORWARD") {
            juk.forward();
            if ( (play = juk.isPlaying()) != "" ) {
	       dname += ": ";
	       dname += (juk.isPlaying()).latin1();
            }
	}
	else if (comm.upper() == "JUK_SEEKBACK") {
	    juk.seekBack();
            if ( (play = juk.isPlaying()) != "" ) {
               dname += ": ";
               dname += (juk.isPlaying()).latin1();
            }
	}
	else if (comm.upper() == "JUK_SEEKFORWARD") {
	    juk.seekForward();
            if ( (play = juk.isPlaying()) != "" ) {
               dname += ": ";
               dname += (juk.isPlaying()).latin1();
            }
	}
	else if (comm.upper() == "JUK_VOLUMEUP") {
	    juk.volumeUp();
	}
	else if (comm.upper() == "JUK_VOLUMEDOWN") {
	    juk.volumeDown();
	}
	else if (comm.upper() == "JUK_VOLUMEMUTE") {
	    juk.volumeMute();
	}
	else if (comm.upper() == "JUK_STARTPLAYINGPLAYLIST") {
	    juk.startPlayingPlaylist();
	}
        else if (comm.upper() == "JUK_OPENFILE") {
            const vector<std::string>& args = command.getArgs();
            if (args.size() == 1) {
               QString file = QString((args[0]).c_str());
               juk.openFile(file);
            }
            else {
               QStringList filelist;
               for (vector<std::string>::const_iterator it = args.begin(); it != args.end(); it++)
                   filelist.push_back(QString((*it).c_str()));
               juk.openFile(filelist);
            }
        }
	if (imyKey->isUsedAsToggle())
		imyKey->toggleState();

        if (juk_Display != NULL) {
	        juk_Display->show(dname);
	}

	
        return true;
	
	//"JUK_PLAY"
	//"JUK_PAUSE"
	//"JUK_STOP"
	//"JUK_PLAYPAUSE"
	//"JUK_BACK"
	//"JUK_FORWARD"
	//"JUK_SEEKBACK"
	//"JUK_SEEKFORWARD"
	//"JUK_VOLUMEUP"
	//"JUK_VOLUMEDOWN"
	//"JUK_VOLUMEMUTE"
	//"JUK_STARTPLAYINGPLAYLIST"
	//"JUK_OPENFILE"
        //DCOPObject dcop("juklineakdcop");
}
extern "C" macro_info* macrolist() {
	juk_macinfo = new (macro_info);
	juk_macinfo->num_macros = NUM_MACROS;
	juk_macinfo->macro_list = juk_symbols;
        return juk_macinfo;
}
/** Delete any structs or classes we have new'd here */
extern "C" void cleanup() {
        if (verbose) cout << "Cleaning up plugin jukplugin" << endl;
	if (juk_macinfo != NULL) {
		delete (juk_macinfo);
		juk_macinfo = NULL;
	}
	if (juk_idinfo != NULL) {
		delete (juk_idinfo);
		juk_idinfo = NULL;
	}
	if (juk_dcop != NULL) {
		juk_dcop->detach();
		delete(juk_dcop);
		juk_dcop = NULL;
	}
	if (verbose) cout << "Done cleaning up plugin jukplugin" << endl;
}
extern "C" int initialize_display(displayCtrl *imyDisplay) {
        if (verbose) cout << "Initializing on screen display!" << endl;
        juk_Display = imyDisplay;
	return true;
}

