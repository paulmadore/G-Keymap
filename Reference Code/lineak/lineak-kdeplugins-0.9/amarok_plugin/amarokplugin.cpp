/***************************************************************************
                          amarokplugin.cpp  -  description
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
#include <amarokclient.h>

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

#include "amarokplugin.h"

/** These are required */
#define NUM_MACROS 13 

string amarok_symbols[NUM_MACROS] = { "AMAROK_PLAY", "AMAROK_PAUSE", "AMAROK_STOP", "AMAROK_PLAYPAUSE", "AMAROK_BACK", "AMAROK_FORWARD", "AMAROK_SEEK", "AMAROK_ADDMEDIA", "AMAROK_VOLUMEUP", "AMAROK_VOLUMEDOWN", "AMAROK_VOLUMEMUTE", "AMAROK_TOGGLEPLAYLIST", "AMAROK_ENABLERANDOM" };
macro_info* amarok_macinfo = NULL;
identifier_info* amarok_idinfo = NULL;
bool verbose = false;
bool enable;
DCOPClient* amarok_dcop;
displayCtrl* amarok_Display = NULL;

extern "C" int initialize(init_info init) {
        verbose = init.verbose;
	enable = init.global_enable;

	amarok_dcop = new DCOPClient();
	if (!(amarok_dcop->registerAs( "amaroklineakdcop" )))
            if (verbose) cerr << "Could not register" << endl;
	if (!amarok_dcop->attach()) {
            if (verbose) cerr << "Could not attach amarok_dcop to dcop server" << endl;
        } else
            if (verbose) cout << "Registered" << endl;
        /*
    	if (!amarok_dcop->isApplicationRegistered("amarok")) {
            if (KApplication::startServiceByDesktopName("amarok")) {
               cout << i18n(QString("No running instance of amarok found."));
               return false;
            }
        }
        */
        return true;
}
extern "C" identifier_info* identifier() {
	amarok_idinfo = new (identifier_info);
	amarok_idinfo->description = "Amarok Plugin";
	amarok_idinfo->identifier = "amarokplugin";
	amarok_idinfo->type = "MACRO";
	amarok_idinfo->version = VERSION;
	return amarok_idinfo;
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
	
	AmarokClient amarok(amarok_dcop);
        QString comm = QString((command.getCommand()).c_str());
        const vector<std::string>& args = command.getArgs();
        QString play = "";

	if (dname == "" or dname == snull) {
           if (imyKey->isUsedAsToggle())
                dname = imyKey->getNextToggleName();
           else
                dname = comm.latin1();
        }
        if (comm.upper() == "AMAROK_PLAY" ) {
            amarok.play();
            if ( (play = amarok.isPlaying()) != "" ) {
	       dname += ": ";
	       dname += play.latin1();
            }
        }
        else if (comm.upper() == "AMAROK_PAUSE") {
            amarok.pause();
        }
        else if (comm.upper() == "AMAROK_STOP") {
            amarok.stop();
				        }
    	else if (comm.upper() == "AMAROK_PLAYPAUSE") {
            amarok.playPause();
            if ( (play = amarok.isPlaying()) != "" ) {
	       dname += ": ";
	       dname += play.latin1();
            }
    	}
	else if (comm.upper() == "AMAROK_BACK") {
            amarok.back();
            if ( (play = amarok.isPlaying()) != "" ) {
	       dname += ": ";
	       dname += play.latin1();
            }
        }
	else if (comm.upper() == "AMAROK_FORWARD") {
            amarok.forward();
            if ( (play = amarok.isPlaying()) != "" ) {
	       dname += ": ";
	       dname += play.latin1();
	       
            }
	}
	else if (comm.upper() == "AMAROK_SEEK") {
            if (args.size() == 1) {
               int s = atoi((args[0]).c_str());
	       amarok.seek(s);
               if ( (play = amarok.isPlaying()) != "" ) {
                  dname += ": ";
                  dname += play.latin1();
               }
            }
	}
	else if (comm.upper() == "AMAROK_ENABLERANDOM") {
            if (args.size() == 1) {
               QString sarg = QString((args[0]).c_str());
               bool arg;
               if ( sarg == "true" ) arg = true;
               if ( sarg == "false" ) arg = false;
	       amarok.enableRandomMode(arg);
               if ( (play = amarok.isPlaying()) != "" ) {
                  dname += ": ";
                  dname += play.latin1();
               }
            }
	}
	else if (comm.upper() == "AMAROK_VOLUMEUP") {
	    amarok.volumeUp();
	}
	else if (comm.upper() == "AMAROK_VOLUMEDOWN") {
	    amarok.volumeDown();
	}
	else if (comm.upper() == "AMAROK_VOLUMEMUTE") {
	    amarok.volumeMute();
	}
	else if (comm.upper() == "AMAROK_TOGGLEPLAYLIST") {
	    amarok.togglePlaylist();
	}
        else if (comm.upper() == "AMAROK_ADDMEDIA") {
            if (args.size() == 1) {
               QString file = QString((args[0]).c_str());
               amarok.addMedia(file);
            }
            else {
               QStringList filelist;
               for (vector<std::string>::const_iterator it = args.begin(); it != args.end(); it++)
                   filelist.push_back(QString((*it).c_str()));
               amarok.addMedia(filelist);
            }
        }
	if (imyKey->isUsedAsToggle())
		imyKey->toggleState();

        if (amarok_Display != NULL) {
	        amarok_Display->show(dname);
	}

	
        return true;
	
	//"AMAROK_PLAY"
	//"AMAROK_PAUSE"
	//"AMAROK_STOP"
	//"AMAROK_PLAYPAUSE"
	//"AMAROK_BACK"
	//"AMAROK_FORWARD"
	//"AMAROK_SEEK"
	//"AMAROK_ADDMEDIA"
        //"AMAROK_ENABLERANDOM"
	//"AMAROK_VOLUMEUP"
	//"AMAROK_VOLUMEDOWN"
	//"AMAROK_VOLUMEMUTE"
	//"AMAROK_TOGGLEPLAYLIST"
	//"AMAROK_OPENFILE"
        //DCOPObject dcop("amaroklineakdcop");
}
extern "C" macro_info* macrolist() {
	amarok_macinfo = new (macro_info);
	amarok_macinfo->num_macros = NUM_MACROS;
	amarok_macinfo->macro_list = amarok_symbols;
        return amarok_macinfo;
}
/** Delete any structs or classes we have new'd here */
extern "C" void cleanup() {
        if (verbose) cout << "Cleaning up plugin amarokplugin" << endl;
	if (amarok_macinfo != NULL) {
		delete (amarok_macinfo);
		amarok_macinfo = NULL;
	}
	if (amarok_idinfo != NULL) {
		delete (amarok_idinfo);
		amarok_idinfo = NULL;
	}
	if (amarok_dcop != NULL) {
		amarok_dcop->detach();
		delete(amarok_dcop);
		amarok_dcop = NULL;
	}
	if (verbose) cout << "Done cleaning up plugin amarokplugin" << endl;
}
extern "C" int initialize_display(displayCtrl *imyDisplay) {
        if (verbose) cout << "Initializing on screen display!" << endl;
        amarok_Display = imyDisplay;
	return true;
}

