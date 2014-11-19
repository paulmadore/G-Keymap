/***************************************************************************
                          kmailplugin.cpp  -  description
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

#include "klmailclient.h"

#include <config.h>
#include <lineak/lconfig.h>
#include <lineak/lkbd.h>
#include <lineak/lkey.h>
#include <lineak/lobject.h>
#include <lineak/plugin_definitions.h>
#include <lineak/displayctrl.h>
// #ifdef HAVE_XOSD
// #include <lineak/xosdctrl.h>
// #endif

#include <iostream>
#include <sstream>
#include <stdlib.h>
using namespace std;

#include "kmailplugin.h"

/** These are required */
#define NUM_MACROS 1
string kmail_symbols[NUM_MACROS] = { "KMAIL_COMPOSE" };
macro_info* kmail_macinfo = NULL;
identifier_info* kmail_idinfo = NULL;
bool verbose = false;
bool enable;
string dname = "";
DCOPClient* kmail_dcop;
displayCtrl* kmail_Display = NULL;

extern "C" int initialize(init_info init) {
        verbose = init.verbose;
	enable = init.global_enable;
	kmail_dcop = new DCOPClient();
	if (!(kmail_dcop->registerAs( "kmaillineakdcop" )))
            if (verbose) cerr << "Could not register" << endl;
	if (!kmail_dcop->attach()) {
            if (verbose) cerr << "Could not attach kmail_dcop to dcop server" << endl;
        } else
            if (verbose) cout << "Registered" << endl;

	return true;
}
extern "C" identifier_info* identifier() {
	kmail_idinfo = new (identifier_info);
	kmail_idinfo->description = "KMail Plugin";
	kmail_idinfo->identifier = "kmailplugin";
	kmail_idinfo->type = "MACRO";
	kmail_idinfo->version = VERSION;
	return kmail_idinfo;
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
        //cout << "enable = " << enable << endl;
        //cout << "Executing " << imyKey->getCommand() << endl;
        if ( command.getMacroType() == "KMAIL_COMPOSE") { /* Open kmail's composer window */
                macroKMAIL_COMPOSE(command);
        } else if (command.isMacro())  /** If we are a macro, but we can't find one that matches, return false. */
                return false;

        if (imyKey->isUsedAsToggle())
              imyKey->toggleState();


        return true;
}
extern "C" macro_info* macrolist() {
	kmail_macinfo = new (macro_info);
	kmail_macinfo->num_macros = NUM_MACROS;
	kmail_macinfo->macro_list = kmail_symbols;
        return kmail_macinfo;
}
extern "C" void cleanup() {
        if (verbose) cout << "Cleaning up plugin kmail" << endl;
	if (kmail_macinfo != NULL) {
		delete (kmail_macinfo);
		kmail_macinfo = NULL;
	}
	if (kmail_idinfo != NULL) {
		delete (kmail_idinfo);
		kmail_idinfo = NULL;
	}
 	if (kmail_dcop != NULL) {
		kmail_dcop->detach();
 		delete(kmail_dcop);
 		kmail_dcop = NULL;
 	}
	if (verbose) cout << "Done cleaning up plugin kmail" << endl;
}
extern "C" int initialize_display(displayCtrl *imyDisplay) {
        if (verbose) cout << "Initializing on screen display!" << endl;
        kmail_Display = imyDisplay;
	return true;
}

bool macroKMAIL_COMPOSE(LCommand& command) {
   if (enable) {
        if (kmail_Display != NULL)
		if (dname == "" || dname == snull || dname == "KMAIL_COMPOSE")
                   kmail_Display->show(string("Composing an EMail"));
	        else
		   kmail_Display->show(dname);

	const vector<std::string>& args = command.getArgs();
	KLMailClient mail(kmail_dcop);
	/** If we are just using the macro plain, no argument */
	if (args.size() == 0) {
		mail.mailTo(QString(""), QString(""),QString(""),QString("") ,KURL(""));
	}
	else {
		if (args.size() <= 5) {
		switch (args.size()) {
		case 1:
			return (mail.mailTo(QString((args[0]).c_str()), QString(""),QString(""),QString("") ,KURL("")));
			break;
		case 2:
			return (mail.mailTo(QString((args[0]).c_str()), QString((args[1]).c_str()),QString(""),QString("") ,KURL("")));
			break;
		case 3:
			return (mail.mailTo(QString((args[0]).c_str()), QString((args[1]).c_str()),QString((args[2]).c_str()),QString("") ,KURL("")));
			break;
		case 4:
			return (mail.mailTo(QString((args[0]).c_str()), QString((args[1]).c_str()),QString((args[2]).c_str()),QString((args[3]).c_str()) ,KURL("")));
			break;
		case 5:
			return (mail.mailTo(QString((args[0]).c_str()), QString((args[1]).c_str()),QString((args[2]).c_str()),QString((args[3]).c_str()) ,KURL(QString((args[4]).c_str()))));
			break;
		}
	} else if (args.size() > 5) {
		/** Someone is getting ridiculous */
		return (mail.mailTo(QString((args[0]).c_str()), QString((args[1]).c_str()),QString((args[2]).c_str()),QString((args[3]).c_str()) ,KURL( QString((args[4]).c_str()) )));
	}
	}
	return false;
   }
   return false;
}

