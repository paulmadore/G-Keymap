/***************************************************************************
                          kdeplugin.cpp  -  description
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
#include <string>

#include "konqclient.h"

#include <config.h>
#include <lineak/lconfig.h>
#include <lineak/lkbd.h>
#include <lineak/lkey.h>
#include <lineak/lobject.h>
#include <lineak/plugin_definitions.h>
#include <lineak/displayctrl.h>

#include <iostream>
#include <sstream>
#include <stdlib.h>
using namespace std;

#include "konqplugin.h"

/** These are required */
#define NUM_MACROS 29
string konq_symbols[NUM_MACROS] = { "KONQUEROR", "KONQUEROR_SETURL", "KONQUEROR_BACK", "KONQUEROR_QUIT", "KONQUEROR_BREAKOFF_CURRENT_TAB", "KONQUEROR_DUPLICATE_WINDOW", "KONQUEROR_DUPLICATE_CURRENT_TAB", "KONQUEROR_FORWARD", "KONQUEROR_FULLSCREEN", "KONQUEROR_SIDEBAR", "KONQUEROR_HOME", "KONQUEROR_PRINT", "KONQUEROR_STOP", "KONQUEROR_REFRESH", "KONQUEROR_REMOVE_CURRENT_TAB", "KONQUEROR_REMOVEVIEW", "KONQUEROR_RELOAD", "KONQUEROR_RELOAD_ALL_TABS", "KONQUEROR_SPLITVIEW_H", "KONQUEROR_SPLITVIEW_V", "KONQUEROR_NEWTAB", "KONQUEROR_NEXTTAB", "KONQUEROR_PREVTAB", "KONQUEROR_TABMOVE_LEFT", "KONQUEROR_TABMOVE_RIGHT", "KONQUEROR_TRASH", "KONQUEROR_UNDO", "KONQUEROR_UP", "KONQUEROR_BOOKMARK" };
macro_info* konq_macinfo = NULL;
identifier_info* konq_idinfo = NULL;
bool verbose = false;
bool enable;
string dname = "";
displayCtrl* konq_Display = NULL;
DCOPClient* konq_dcop;

extern "C" int initialize(init_info init) {
        verbose = init.verbose;
	enable = init.global_enable;
	konq_dcop = new DCOPClient();
	if (!(konq_dcop->registerAs( "konqlineakdcop" )))
            if (verbose) cerr << "Could not register" << endl;
	if (!konq_dcop->attach()) {
            if (verbose) cerr << "Could not attach konq_dcop to dcop server" << endl;
        } else
            if (verbose) cout << "Registered" << endl;

	return true;
}
extern "C" identifier_info* identifier() {
	konq_idinfo = new (identifier_info);
	konq_idinfo->description = "Konqueror Plugin";
	konq_idinfo->identifier = "konqplugin";
	konq_idinfo->type = "MACRO";
	konq_idinfo->version = VERSION;
	return konq_idinfo;
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

	/* Give verbose output about the command and display name */
	if (verbose) {
		cout << "----------------------------------------" << endl;
		//cout << "For keycode: " << keycode << endl;
		cout << " Key: " << endl << *(imyKey) << endl;
                /** Note that we aren't using the display name for display here.
                    We will use the macro name instead. */
	  	cout << "	XOSD Display: " << dname << endl;
		cout << "	Command: " << command << endl;
		cout << "----------------------------------------" << endl;
	}
        if ( command.getMacroType() == "KONQUEROR" ||
             command.getMacroType() == "KONQUEROR_BACK" ||
             command.getMacroType() == "KONQUEROR_FORWARD" ||
             command.getMacroType() == "KONQUEROR_HOME" ||
             command.getMacroType() == "KONQUEROR_STOP" ||
             command.getMacroType() == "KONQUEROR_REFRESH" ||
             command.getMacroType() == "KONQUEROR_RELOAD" ||
	     command.getMacroType() == "KONQUEROR_RELOAD_ALL_TABS" ||
             command.getMacroType() == "KONQUEROR_NEWTAB" ||
             command.getMacroType() == "KONQUEROR_NEXTTAB" ||
             command.getMacroType() == "KONQUEROR_PREVTAB" ||
             command.getMacroType() == "KONQUEROR_BOOKMARK" ||
	     command.getMacroType() == "KONQUEROR_QUIT" ||
	     command.getMacroType() == "KONQUEROR_BREAKOFF_CURRENT_TAB" ||
	     command.getMacroType() == "KONQUEROR_DUPLICATE_WINDOW" ||
	     command.getMacroType() == "KONQUEROR_DUPLICATE_CURRENT_TAB" ||
	     command.getMacroType() == "KONQUEROR_FULLSCREEN" ||
	     command.getMacroType() == "KONQUEROR_SIDEBAR" ||
	     command.getMacroType() == "KONQUEROR_PRINT" ||
	     command.getMacroType() == "KONQUEROR_REMOVE_CURRENT_TAB" ||
	     command.getMacroType() == "KONQUEROR_REMOVEVIEW" ||
	     command.getMacroType() == "KONQUEROR_SPLITVIEW_H" ||
	     command.getMacroType() == "KONQUEROR_SPLITVIEW_V" ||
	     command.getMacroType() == "KONQUEROR_TABMOVE_LEFT" ||
	     command.getMacroType() == "KONQUEROR_TABMOVE_RIGHT" ||
	     command.getMacroType() == "KONQUEROR_TRASH" ||
	     command.getMacroType() == "KONQUEROR_UNDO" ||
	     command.getMacroType() == "KONQUEROR_UP" ||
             command.getMacroType() == "KONQUEROR_SETURL"
        ) { 
                macroKONQUEROR(command);
        } else if (command.isMacro())  /** If we are a macro, but we can't find one that matches, return false. */
                return false;

        if (imyKey->isUsedAsToggle())
             imyKey->toggleState();


        return true;
}
extern "C" macro_info* macrolist() {
	konq_macinfo = new (macro_info);
	konq_macinfo->num_macros = NUM_MACROS;
	konq_macinfo->macro_list = konq_symbols;
        return konq_macinfo;
}
extern "C" void cleanup() {
        if (verbose) cout << "Cleaning up plugin konqueror" << endl;
	if (konq_macinfo != NULL) {
		delete (konq_macinfo);
		konq_macinfo = NULL;
	}
	if (konq_idinfo != NULL) {
		delete (konq_idinfo);
		konq_idinfo = NULL;
	}
 	if (konq_dcop != NULL) {
		konq_dcop->detach();
 		delete(konq_dcop);
 		konq_dcop = NULL;
 	}
	if (verbose) cout << "Done cleaning up plugin konqueror" << endl;
}
extern "C" int initialize_display(displayCtrl *imyDisplay) {
        if (verbose) cout << "Initializing on screen display!" << endl;
        konq_Display = imyDisplay;
	return true;
}

bool macroKONQUEROR(LCommand& command) {
   if (enable) {
	const vector<std::string>& args = command.getArgs();

	QCString objid;
    	QCString function;
        QByteArray data;
	QCString foundApp;
	QCString foundObj;

	KonqClient konq(konq_dcop);
	if (!konq.isRunning()) {
	   if (!konq.startKonqueror()) {
              if (KApplication::startServiceByDesktopName("konqueror")) {
	         cout << i18n("No running instance of konqueror found.");
              }
           }
	}
        QString comm;
	if (args.size() == 1) {
           if (command.getMacroType() == "KONQUEROR_SETURL") {
              comm = "SETURL";
           } else
	      comm = QString((args[0]).c_str());
	}
	else if (args.size() == 0) {
	   string tmp = command.getCommand();
	   int loc = tmp.find("_");
	   comm = QString(tmp.substr(loc+1,tmp.size()).c_str());
	   //comm = QString(tmp.substr(loc+1,tmp.size()));
	}
	else
	   return false;
	   
	   if (dname == "" || dname == snull)
              dname = comm.latin1();

	   if ( comm.upper() == "BACK" ) {
	      konq.back();
	   }
	   else if ( comm.upper() == "FORWARD" ) {
	      konq.forward();
	   }
	   else if ( comm.upper() == "HOME" ) {
	      konq.home();
	   }
	   else if ( comm.upper() == "STOP" ) {
	      konq.stop();
	   }
	   else if ( comm.upper() == "REFRESH" || comm.upper() == "RELOAD" ) {
	      konq.refresh();
	   }
	   else if ( comm.upper() == "RELOAD_ALL_TABS" ) {
	      konq.reload_all_tabs();
	   }
	   else if ( comm.upper() == "NEWTAB" ) {
	      konq.newtab();
	   }
	   else if ( comm.upper() == "NEXTTAB") {
	      konq.nexttab();
	   }
	   else if ( comm.upper() == "PREVIOUSTAB") {
	      konq.previoustab();
	   }
	   else if ( comm.upper() == "BOOKMARK" ) {
	      konq.addBookmark();
	   }
	   else if ( comm.upper() == "QUIT" ) {
	      konq.quit();
	   }
	   else if ( comm.upper() == "BREAKOFF_CURRENT_TAB" ) {
	      konq.breakoffcurrenttab();
	   }
           else if ( comm.upper() == "DUPLICATE_WINDOW" ) {
	      konq.duplicate_window();
	   }
           else if ( comm.upper() == "DUPLICATE_CURRENT_TAB" ) { 
	      konq.duplicatecurrenttab();
	   }
	   else if ( comm.upper() == "FULLSCREEN" ) {
	      konq.fullscreen();
	   }
	   else if ( comm.upper() == "SIDEBAR" ) {
              konq.konq_sidebartng();
	   }
	   else if ( comm.upper() == "PRINT" ) {
              konq.print();
	   }
	   else if ( comm.upper() == "REMOVE_CURRENT_TAB" ) {
              konq.removecurrenttab();
	   }
	   else if ( comm.upper() == "REMOVEVIEW" ) {
              konq.removeview();
	   }
	   else if ( comm.upper() == "SPLITVIEW_H" ) {
              konq.splitviewh();
	   }
	   else if ( comm.upper() == "SPLITVIEW_V" ) {
	      konq.splitviewv();
           }	   
	   else if ( comm.upper() == "TABMOVE_LEFT" ) {
              konq.tab_move_left();
	   }	   
	   else if ( comm.upper() == "TABMOVE_RIGHT" ) {
              konq.tab_move_right();
	   }	   
	   else if ( comm.upper() == "TRASH" ) {
              konq.trash();
	   }	   
	   else if ( comm.upper() == "UNDO" ) {
              konq.undo();
	   }	   
	   else if ( comm.upper() == "UP") {
              konq.up();
           }
           else if ( comm.upper() == "SETURL") {
              konq.setURL(args[0]);
	   } else {
	      if (verbose) {
		cerr << comm << " is an unknown KONQUEROR macro argument." << endl;
	      }
	      return false;
	   }
	konq_Display->show(dname);
   }
   return true;
   
//   return false;
}

