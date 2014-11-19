/***************************************************************************
                          kdesktopplugin.cpp  -  description
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

#include "kdesktopplugin.h"
#include <lineak/lconfig.h>
#include <lineak/lkbd.h>
#include <lineak/lkey.h>
#include <lineak/lobject.h>
#include <lineak/plugin_definitions.h>
#include <lineak/msgpasser.h>
#include <lineak/lcommand.h>
#include <lineak/lineak_util_functions.h>
#include <lineak/displayctrl.h>

#include <iostream>
#include <sstream>
#include <stdlib.h>
using namespace std;
#include "kdesktopplugin.h"

/** These are required */
#define NUM_MACROS 6

string kdesktop_symbols[NUM_MACROS] = { "KDE_LOCK_DESKTOP", "KMENU", "KDESKTOP_NEXT", "KDESKTOP_PREVIOUS", "KDESKTOP_EXECUTE", "KDE_LOGOUT" };
macro_info* kdesktop_macinfo = NULL;
identifier_info* kdesktop_idinfo = NULL;
bool verbose = false;
bool enable;
string dname = "";
DCOPClient* kdesktop_dcop = NULL;
displayCtrl* kdesktop_Display = NULL;

extern "C" int initialize(init_info init) {
	verbose = init.verbose;
	enable = init.global_enable;

	kdesktop_dcop = new DCOPClient();
	if (!(kdesktop_dcop->registerAs( "kdesktoplineakdcop" )))
            if (verbose) cerr << "Could not register" << endl;
	if (!kdesktop_dcop->attach()) {
            if (verbose) cerr << "Could not attach kdesktop_dcop to dcop server" << endl;
        } else
            if (verbose) cout << "Registered" << endl;
	return true;
}
extern "C" identifier_info* identifier() {
	kdesktop_idinfo = new (identifier_info);
	kdesktop_idinfo->description = "KDesktop Plugin";
	kdesktop_idinfo->identifier = "kdesktopplugin";
	kdesktop_idinfo->type = "MACRO";
	kdesktop_idinfo->version = VERSION;
	return kdesktop_idinfo;
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
        if ( command.getMacroType() == "KDE_LOCK_DESKTOP") { /* Invoke the screensaver. */
                macroKDE_LOCK_DESKTOP(command);
	} else if (command.getMacroType() == "KMENU") { /* Invoke he kmenu */
		macroKMENU(command);
        } else if (command.getMacroType() == "KDESKTOP_NEXT") { /* Go to the next desktop */
                macroKDESKTOP(command);
        } else if (command.getMacroType() == "KDESKTOP_PREVIOUS") { /* Go to the previous desktop */
                macroKDESKTOP(command);
        } else if (command.getMacroType() == "KDESKTOP_EXECUTE") { /* Open the run dlg. */
                macroKDESKTOP(command);
        } else if (command.getMacroType() == "KDE_LOGOUT") { /* Logout */
                macroKSMSERVER(command);
        } else if (command.isMacro())  /** If we are a macro, but we can't find one that matches, return false. */
                return false;

   	if (imyKey->isUsedAsToggle())
              imyKey->toggleState();
		   
        return true;
}
extern "C" macro_info* macrolist() {
	kdesktop_macinfo = new (macro_info);
	kdesktop_macinfo->num_macros = NUM_MACROS;
	kdesktop_macinfo->macro_list = kdesktop_symbols;
        return kdesktop_macinfo;
}
extern "C" void cleanup() {
        if (verbose) cout << "Cleaning up plugin kdesktop" << endl;
        if (verbose) cout << "cleaning up kdesktop_macinfo" << endl;
	if (kdesktop_macinfo != NULL) {
		delete (kdesktop_macinfo);
		kdesktop_macinfo = NULL;
	}
        if (verbose) cout << "cleaning up kdesktop_idinfo" << endl;
	if (kdesktop_idinfo != NULL) {
		delete (kdesktop_idinfo);
		kdesktop_idinfo = NULL;
	}
        if (verbose) cout << "cleaning up kdesktop_dcop" << endl;
	if (kdesktop_dcop != NULL) {
		kdesktop_dcop->detach();
		delete(kdesktop_dcop);
		kdesktop_dcop = NULL;
	}
	if (verbose) cout << "Done cleaning up plugin kdesktop" << endl;
}
extern "C" int initialize_display(displayCtrl *ikdesktop_Display) {
        if (verbose) cout << "Initializing on screen display!" << endl;
        kdesktop_Display = ikdesktop_Display;
	return true;
}

bool macroKDE_LOCK_DESKTOP(LCommand& command) {
        if (kdesktop_Display != NULL) {
		if (dname == "" || dname == snull || dname == "KDE_LOCK_DESKTOP" )
                   kdesktop_Display->show(string("Locking the desktop"));
	        else
	           kdesktop_Display->show(dname);
        }

	//const vector<std::string>& args = command.getArgs();
	if (!kdesktop_dcop->isApplicationRegistered("kdesktop")) {
	/*	if (KApplication::startServiceByDesktopName("kdesktop")) {
		cout << i18n("No running instance of KDesktop found.");
		return false;
		}
	*/
		return false;
	}
	bool result = false;

	QByteArray data, replyData;
	QCString replyType;
	QDataStream arg( data, IO_WriteOnly );
	//arg << arg0; //to
	/*  bool call(const QCString &remApp, const QCString &remObj,
			const QCString &remFun, const QByteArray &data,
			QCString& replyType, QByteArray &replyData,
			bool useEventLoop=false);
	*/
	/** Should probably fork a thread here, and do this. Then in this thread,
	*   enable the screensaver, set enable = false, and
	*   while the screensaver is enabled loop until it's not. Then set the
	*   enable variable to true */
	if (kdesktop_dcop->call(QCString("kdesktop"),QCString("KScreensaverIface"),QCString("isBlanked()"), data, replyType, replyData ) ) {
	if ( replyType == "bool" ) {
		QDataStream _reply_stream( replyData, IO_ReadOnly );
		_reply_stream >> result;
	}
	else
		if (verbose) cerr << "dcop call kdesktop KScreensaverIface isBlanked() could not find return type." << endl;
	}
	else
	if (verbose) cerr << "dcop call kdesktop KScreensaverIface isBlanked() failed." << endl;

	// If the desktop lock is already enabled, return false.
	if (result) {
	return false;
	}
	// Else, lock and wait until it's disabled.
	else {
	if (!kdesktop_dcop->send("kdesktop","KScreensaverIface","lock()",data )) {
		if (verbose) cerr << "lock() call failed." << endl;
		return false;
	}
	else {
		// Loop until it's off.
		enable = false;
		msgPasser message;
		message.start();
		message.sendMessage(msgPasser::DISABLE,"disable");
		while (true) {
		sleep(1);
		if (kdesktop_dcop->call(QCString("kdesktop"),QCString("KScreensaverIface"),QCString("isBlanked()"), data, replyType, replyData ) ) {
		if ( replyType == "bool" ) {
			QDataStream _reply_stream( replyData, IO_ReadOnly );
			_reply_stream >> result;
			if (!result) {
			message.sendMessage(msgPasser::ENABLE,"enable");
			enable = true;
			return (true);
			} //else
			//cout << "Disabled" << endl;
		} else
			if (verbose) cerr << "isBlanked() call failed." << endl;
		} else
		if (verbose) cerr << "isBlanked() call failed." << endl;
	}
	return true;
	}
	}
	return false;

}

bool macroKMENU(LCommand& command) {
   if (enable) {
	if (!kdesktop_dcop->isApplicationRegistered("kicker")) {
		/* if (KApplication::startServiceByDesktopName("kicker")) {
		cout << i18n(QString("No running instance of kicker found."));
		return false;
		} */
		cout << "KMenu failed" << endl;
		return false;
	}
	int root_x, root_y, win_x, win_y;
	root_x = 0;
	root_y = 0;
	QByteArray data;
	QDataStream arg( data, IO_WriteOnly );

	const vector<std::string>& args = command.getArgs();
	if (args.size() == 2) {
		root_x = atoi((args[0]).c_str());
		root_y = atoi((args[1]).c_str());
		QPoint point(root_x,root_y);
		arg << point;
		if (!kdesktop_dcop->send("kicker","kicker","popupKMenu(QPoint)",data )) {
			if (verbose) cerr << "popupKMenu(QPoint) call failed." << endl;
			return false;
		}
	}
	else {
		//cout << "CALLING KICKER TO OPEN KMENU" << endl;
		/* Try open the kmenu on kicker. If there isn't one, or it fails. Open it
		under the mouse */
//		if (!kdesktop_dcop->send("kicker","kicker","firstOpenPopupKMenu()",data)) {
//			if (verbose) cerr << "firstOpenPopupKMenu() call failed." << endl;

			QPoint point(root_x,root_y);
			arg << point;
			if (!kdesktop_dcop->send("kicker","kicker","popupKMenu(QPoint)",data )) {
				if (verbose) cerr << "popupKMenu(QPoint) call failed." << endl;
				return false;
			}
//		}
	}
	if (dname == "" || dname == snull || dname == "KMENU" )
               kdesktop_Display->show(string("KMenu"));
        else
               kdesktop_Display->show(dname);

	return true;

   }
   return false;
}
bool macroKSMSERVER(LCommand& command) {
   if (enable) {
        QByteArray data;
        QDataStream arg( data, IO_WriteOnly );
        arg << "0";
        arg << "0";
        arg << "0";
        if (command.getCommand() == "KDE_LOGOUT") {
            cout << "Sending DCOP command ksmserver ksmserver logout 0 0 0" << endl;
            if (!kdesktop_dcop->send("ksmserver","ksmserver","logout",data )) {
                if (verbose) cerr << "logout() call failed." << endl;
                return false;
            }
            return true;
        }
   }
   return false;
}

bool macroKDESKTOP(LCommand& command) {
   if (enable) {
	if (!kdesktop_dcop->isApplicationRegistered("kwin")) {
		/* if (KApplication::startServiceByDesktopName("kwin")) {
		cout << i18n(QString("No running instance of kwin found."));
		return false;
		} */
		return false;
	}
	QByteArray data;

	if (command.getCommand() == "KDESKTOP_NEXT") {
	   if (!kdesktop_dcop->send("kwin","KWinInterface","nextDesktop()",data )) {
		if (verbose) cerr << "nextDesktop() call failed." << endl;
		return false;
	   }
	   kdesktop_Display->show(dname);
           return true;
	}
	else if (command.getCommand() == "KDESKTOP_PREVIOUS") {
  	   if (!kdesktop_dcop->send("kwin","KWinInterface","previousDesktop()",data )) {
		if (verbose) cerr << "previousDesktop() call failed." << endl;
		   return false;
	   }
	   kdesktop_Display->show(dname);
           return true;
	}
	else if (command.getCommand() == "KDESKTOP_EXECUTE") {
	   if (!kdesktop_dcop->send("kdesktop","KDesktopIface","popupExecuteCommand()",data )) {
		if (verbose) cerr << "popupExecuteCommand() call failed." << endl;
		return false;
	   }
	   kdesktop_Display->show(dname);
           return true;
	}
	return true;
   }
   return false;
}

