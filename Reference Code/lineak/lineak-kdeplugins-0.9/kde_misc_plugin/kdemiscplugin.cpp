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

#include "artsclient.h"

#include <config.h>
#include <lineak/lconfig.h>
#include <lineak/lkbd.h>
#include <lineak/lkey.h>
#include <lineak/plugin_definitions.h>
#ifdef HAVE_XOSD
#include <lineak/xosdctrl.h>
#endif

#include <iostream>
#include <sstream>
#include <stdlib.h>
using namespace std;

#include "kdemiscplugin.h"

/** These are required */
#define NUM_MACROS 2
string misc_symbols[NUM_MACROS] = { "ARTS_SUSPEND","ADHOC_DCOP" };
macro_info* misc_macinfo = NULL;
bool verbose = false;
bool enable;
xosdCtrl* misc_Xosd = NULL;
ARTSClient *arts = NULL;
DCOPClient* misc_dcop;

extern "C" int initialize(init_info init) {
        verbose = init.verbose;
	enable = init.global_enable;
	misc_dcop = new DCOPClient();
	if (!(misc_dcop->registerAs( "misclineakdcop" )))
            if (verbose) cerr << "Could not register" << endl;
	if (!misc_dcop->attach()) {
            if (verbose) cerr << "Could not attach misc_dcop to dcop server" << endl;
        } else
            if (verbose) cout << "Registered" << endl;

	return true;
}
extern "C" const char* identifier() {
	const char* pluginname = "kdemiscplugin";
	return pluginname;
}

extern "C" int exec(LKey* imyKey) {
	LCommand command = imyKey->getCommand();
	string dname = imyKey->getDisplayName();
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
        if (command.getMacroType() == "ARTS_SUSPEND") { /* Suspend ARTS */
                if (arts == NULL)
                        arts = new ARTSClient();
                macroARTS_SUSPEND(command);
        } else if (command.getMacroType() == "ADHOC_DCOP") { /* Process a DCOP query, adhoc, with dcop */
                macroADHOC_DCOP(command);
/*        } else if (command.getMacroType() == "ARTS_VOLDOWN") {
                if (arts == NULL)
                        arts = new ARTSClient();
                macroARTS_VOLDOWN(command);
        } else if (command.getMacroType() == "ARTS_MUTE") {
                if (arts == NULL)
                        arts = new ARTSClient();
                macroARTS_MUTE(command); */
        } else if (command.isMacro())  /** If we are a macro, but we can't find one that matches, return false. */
                return false;

	if (imyKey->isUsedAsToggle())
		imyKey->toggleKeyState();

        return true;
}
extern "C" macro_info* macrolist() {
	misc_macinfo = new (macro_info);
	misc_macinfo->num_macros = NUM_MACROS;
	misc_macinfo->macro_list = misc_symbols;
        return misc_macinfo;
}
extern "C" void cleanup() {
        if (verbose) cout << "Cleaning up plugin kde_misc" << endl;
	if (misc_macinfo != NULL) {
		delete (misc_macinfo);
		misc_macinfo = NULL;
	}
 	if (misc_dcop != NULL) {
		misc_dcop->detach();
 		delete(misc_dcop);
 		misc_dcop = NULL;
 	}
        if (arts != NULL) {
                delete(arts);
		arts = NULL;
	}
	if (verbose) cout << "Done cleaning up plugin kde_misc" << endl;
}
#ifdef HAVE_XOSD
extern "C" void initialize_xosd(xosdCtrl *imyXosd) {
        if (verbose) cout << "Initializing xosd!" << endl;
        misc_Xosd = imyXosd;
}
#endif

bool macroADHOC_DCOP(LCommand& command) {
   if (enable) {
	QString comm = (command.getCommand()).c_str();
	command.setSeparator(" ");
	const vector<std::string>& args = command.getArgs();

	if (args.size() < 3)
		return false;

	// The application
	QCString app = (args[0]).c_str();
	// The interface
	QCString objid = (args[1]).c_str();
	// The dcop command.
	QCString function = (args[2]).c_str();
	// The rest of the arguments.
	QCStringList params;
	for (unsigned int i = 3; i != (args.size()-1); i++)
		params.append( (args[i]).c_str());

	if (!misc_dcop->isApplicationRegistered(app)) {
		if (KApplication::startServiceByDesktopName(app)) {
			cout << i18n(QString("No running instance of %1 found.").arg(app));
			return false;
		}
	}
	QByteArray data, replyData;
	QCString replyType;
	QDataStream arg( data, IO_WriteOnly );
	//arg << arg0; //to
	/*  bool call(const QCString &remApp, const QCString &remObj,
			const QCString &remFun, const QByteArray &data,
			QCString& replyType, QByteArray &replyData,
			bool useEventLoop=false);
	*/
	if (verbose) cout << "Executing dcop->send(" << app << "," << objid << "," << function << "," << data << ")" << endl;
	if (!misc_dcop->send(app, objid, function, data ) )
	   if (verbose) cerr << "dcop call " << app << " " << objid << " " << function << " " << data << " failed." << endl;
        return true;
   }
   return false;
}

void macroARTS_SUSPEND(LCommand& command) {
   if (enable) {
        #ifdef HAVE_XOSD
        if (misc_Xosd != NULL)
                misc_Xosd->show(string("Suspending ARTS Sound System."));
        #endif
        ARTSClient arts;
        arts.suspend();
   }
}

/*
void macroARTS_VOLUP(LCommand& command) {
   if (enable) {
        float ret = 0;
        const vector<std::string>& args = command.getArgs();
        if (args.size() == 0) {
                ret = arts->volumeUp();
        } else {
                float value = atol(args[0].c_str());
                ret = arts->volumeUp(fabsf(value));
        }
        #ifdef HAVE_XOSD
        if (misc_Xosd != NULL) {
                //float maxaudio = misc_Xosd->getMaxAudio();
                //misc_Xosd->setMaxAudio(100);
                //misc_Xosd->volume(ret);
                //misc_Xosd->setMaxAudio(maxaudio);
		int sign;
		int dp;
		string s = "Volume ";
		string ss= string(fcvt(ret,3,&dp,&sign));
		if (sign == 0)
                   s+="+";
                else
                   s+="-";
		if (dp >= 0) {
		   string rm = ss.substr(ss.size()-3,3);
		   string dec = ss.substr(0,ss.size()-3);
		   if (dec == "") dec = "0";
		   s+=dec;
		   s+='.';
		   s+=rm;
		} else {
		  s+= "0.";
		  s+= ss;
		}
		misc_Xosd->show(s);
        }
        #endif
   }
}
void macroARTS_VOLDOWN(LCommand& command) {
   if (enable) {
        const vector<std::string>& args = command.getArgs();
        float ret = 0;
        if (args.size() == 0) {
                ret = arts->volumeDown();
        } else {
                float value = atol(args[0].c_str());
                ret = arts->volumeDown(fabsf(value));
        }
        #ifdef HAVE_XOSD
        if (myXosd != NULL) {
                // float maxaudio = myXosd->getMaxAudio();
                //myXosd->setMaxAudio(10);
                //myXosd->volume(ret);
                //myXosd->setMaxAudio(maxaudio);
		int sign;
		int dp;
		string s = "Volume ";
		string ss = string(fcvt(ret,3,&dp,&sign));
	        if (sign == 0)
			s+="+";
		else
			s+="-";
		cout << "Getting there\n";
		if (dp >= 0) {
                   string rm = ss.substr(ss.size()-3,3);
	           string dec = ss.substr(0,ss.size()-3);
		   cout << "Almost" << endl;
		   if (dec == "") dec = "0";
	           s+=dec;
                   s+='.';
                   s+=rm;
		} else {
		   s+= "0.";
		   s+= ss;
		}
		myXosd->show(s);
        }
        #endif
   }
}
void macroARTS_MUTE(LCommand& command) {
   if (enable) {
         const vector<std::string>& args = command.getArgs();
         float ret = 0;
        if (args.size() == 0) {
                ret = arts->mute();
        } else {
                float vol = atol(args[0].c_str());
                ret = arts->mute(fabsf(vol));
        }
         #ifdef HAVE_XOSD
        if (myXosd != NULL) {
                if (ret == xosdCtrl::MUTE)
                        myXosd->volume(xosdCtrl::MUTE);
		else
			myXosd->volume(ret);

        }
         #endif
   }
}
*/
