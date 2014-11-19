/***************************************************************************
                          mediadetectplugin.cpp  -  description
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
#include "soundctrl.h"
#include "mdloader.h"
#include <lineak/cdromctrl.h>

#include <lineak/plugin_definitions.h>
#include <lineak/displayctrl.h>
#include <lineak/lineak_util_functions.h>
#include <lineak/lineak_core_functions.h>
#include <lineak/pluginmanager.h>
#include <lineak/xmgr.h>
#include <X11/extensions/XTest.h>
#include <algorithm>
#include <cctype>

#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <unistd.h>

using namespace std;
using namespace lineak_core_functions;

#include "mediadetectplugin.h"
#include "mdloader.h"

/** These are required */
#define NUM_MACROS 1

string mediadetect_symbols[NUM_MACROS] = { "EAK_MEDIADETECT" };
macro_info* mediadetect_macinfo = NULL;
identifier_info* idinfo = NULL;
LConfig* myConfig = NULL;
LObject* mdobj = NULL;
extern bool verbose;
extern bool global_enable;
bool enable;
bool trayopen = false;
PluginManager* plugins = NULL;
string dname = "";
displayCtrl* mediadetect_Display = NULL;
//Xmgr mX;

extern "C" int initialize(init_info init) {
        verbose = init.verbose;
	enable = init.global_enable;
	myConfig = init.config;
        plugins = init.plugins;
        mdobj = new LObject();
        msg("Registered");
        return true;
}
extern "C" identifier_info* identifier() {
	idinfo = new (identifier_info);
        idinfo->description = "Media Detect";
        idinfo->identifier = "mediadetect";
        idinfo->type = "MACRO";
	idinfo->version = VERSION;
        return idinfo;
}

extern "C" int exec(LObject* imyKey,XEvent xev) {
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
	  	cout << "	XOSD Display: " << dname << endl;
		cout << "	Command: " << command << endl;
		cout << "----------------------------------------" << endl;
	}
	        /* lookup the key in our EAKeylist */
        if (!command.isEmpty())    {
                if (verbose) cout << "enable = " << enable << endl;
                if (enable) {
                        if (verbose) cout << "... that's the " <<  imyKey->getName() << " key" << endl;
                        /* check if the key has to run a SPECIAL command */
                        if ( command.getMacroType() == "EAK_MEDIADETECT" ) 
                        { /* Eject */
                                macroEAK_MediaDetect(imyKey, command, xev);
                        } else if (command.getCommand() == snull) { /* no command defined */
                                if (verbose) cout << "... but it has no command bound to it :(" << endl;
                                if (verbose) cout << *(imyKey);
                        } 
                        /** Rotate the key state if it's a toggleable key */
			if (imyKey->isUsedAsToggle())
				imyKey->toggleState();
                }
        }

	return true;
}
extern "C" macro_info* macrolist() {
	mediadetect_macinfo = new (macro_info);
	mediadetect_macinfo->num_macros = NUM_MACROS;
	mediadetect_macinfo->macro_list = mediadetect_symbols;
        return mediadetect_macinfo;
}
extern "C" void cleanup() {
	msg("Cleaning up plugin mediadetectplugin");
	if (mediadetect_macinfo != NULL) {
		delete (mediadetect_macinfo);
		mediadetect_macinfo = NULL;
	}
	if (idinfo != NULL) {
                delete (idinfo);
                idinfo = NULL;
        }
        if (mdobj != NULL) {
                delete (mdobj);
                mdobj = NULL;
        }
	msg("Done cleaning up plugin mediadetectplugin");
}
extern "C" void initialize_display(displayCtrl *imyDisplay) {
	msg("Initializing display!");
        mediadetect_Display = imyDisplay;
}
void macroEAK_MediaDetect(LObject *obj, LCommand &command, XEvent xev) {
 const vector<string>& args = command.getArgs();
 if (args.size() != 0) {
   map<int, ConfigDirectives*> heur;
   string macro = command.getMacroType();
   string home = getenv("HOME");
   string usrconffile = home + LINEAKDIR;
   string sysconffile = string(CONFDIR) + "/";
   string arg = args[0];
   string carg = "";
   
   if (args.size() == 2)
	   carg = args[1];

   usrconffile += "media-detect.conf";
   sysconffile += "media-detect.conf";

   map < string, ConfigDirectives* >* sysdefs = NULL;
   map < string, ConfigDirectives* >* defs = NULL;
   ConfigDirectives *dir = NULL;

   if (lineak_util_functions::file_exists(sysconffile)) {
      MDLoader ldr(sysconffile);
      sysdefs = ldr.loadDef();
   }

   if (lineak_util_functions::file_exists(usrconffile)) {
      MDLoader ldr(usrconffile);
      defs = ldr.loadDef();
   }

   // If there are use and system wide configurations merge them
   // together.
   if (sysdefs != NULL && defs != NULL) {
      map < string, ConfigDirectives* >::iterator it = sysdefs->begin();
      while (it != sysdefs->end()) {
	  if ( defs->find(it->first) != defs->end() ) {
	     free((*defs)[it->first]);
	  }
	  (*defs)[it->first] = it->second;
	  it++;
      }
   }
   //If sysdefs is not empty, but the user defs are the use the system configuration.
   else if (defs == NULL && sysdefs != NULL)
	   defs = sysdefs;
   
   if (defs != NULL ) {
   map < string, ConfigDirectives* >::iterator it = defs->begin();

   /** Determine which of the configured apps are running and
    *  add them to a list heur of the running programs. From
    *  this list, look at the application priorities to try
    *  and determine which one to act on. */
   for (;it != defs->end(); it++) {
        //    cout << "Definition for: " << it->first << endl;
        //    cout << *(it->second);
        //    cout << endl;
        dir = it->second;
        string name = dir->getValue(PROGRAM);
	// If carg is empty (meaning that there is only 1 argument to the macro)
	// then proceed without caring about application classes. However if it is not empty
	// (meaning that there is an argument to the macro for application class) check the
	// class of the entry before deciding to do this. The classes must match.
	if (carg == "" || carg == dir->getValue(CLASS) ) {
           if (verbose) cout << "Checking to see if: " << name << " is running." << endl;
           if (is_running(name)) {
	       int pri = atoi((dir->getValue(PRIORITY)).c_str());
	       heur[pri+1] = dir;
               if (verbose) cout << "Found: " << name << " running." << endl;
           }
	}
   }
   dir = NULL;

   /** heur should have a list of the programs that are running. We should determine
    *  which one to pick and then act accordingly. */
    if (heur.size() == 1) {
       dir = (heur.begin())->second;
    }
    else if (heur.size() > 1) {
       map<int, ConfigDirectives*>::iterator ita = heur.begin();
       int pria = 0;
       int prib = 0;
       string name = "";

       for (; ita != heur.end(); ita++) {
       // If there isn't an object pointed to yet, point to the first one.
           if (dir == NULL) {
               dir = ita->second;
               pria = ita->first;
           }
           else {
               prib = ita->first;
               if ( prib < pria ) {
                    pria = prib;
                    dir = ita->second;
               }
           }
        }
    }
    /** dir should now point to the configuration that we want.
     *  The next step is to carry out the execution of the desired action.
     *  */
    if ( dir != NULL && arg != "" && heur.size() >= 1 ) {
	    string type = dir->getValue(TYPE);
	    if (type == "dcop" || type == "DCOP" )
		dispatch_dcop(obj, command, dir);
	    if (type == "program" || type == "PROGRAM" )
		dispatch_program(obj, command, dir);
	    if (type == "macro" || type == "MACRO" )
		dispatch_macro(obj, command, dir, xev);
    }
    }
 }  
}

void dispatch_dcop(LObject *obj, LCommand &command, ConfigDirectives *dir) {
   const vector<string>& args = command.getArgs();
   string arg = args[0];
   string type = dir->getValue(TYPE);
   string comm = type + " ";
   comm += dir->getValue(PROGRAM) + " ";
   comm += dir->getValue(arg);
   
   msg("Executing on command: " + arg);
   //obj->getType() == SYM
   if (global_enable) {
         if (!fork()) {
            //string comm = lineak_util_functions::unescape(command.getCommand(), "\"#");
            comm += " &";
            /* child process that tries to run the command, and then exits */
            /* all specials done, let's go for it... ;) */
            msg("... running " + comm);
            msg("... displaying " + comm);
            system(comm.c_str());
            exit(true);
        }
        /* NOTE: no, we don't wait() for the child. we just ignore the SIGCLD signal */
        /** Use the OSD to show the output */
        if ( mediadetect_Display != NULL ) {
	     if (dname == "" || dname == snull) {
	        string lname = dir->getValue(NAME) + ": ";
                lname += arg;
		mediadetect_Display->show(lname);
             } 
             else 	     
                mediadetect_Display->show(dname);
	}
    }
}

void dispatch_program(LObject *obj, LCommand &command, ConfigDirectives *dir) {
   const vector<string>& args = command.getArgs();
   string arg = args[0];
   string comm;
   string prog = dir->getValue(PROGRAM) + " ";
   string controller = dir->getValue(CONTROLLER) + " ";

   if (controller == " ")
	   comm = prog;
   else
	   comm = controller;
   
   comm += dir->getValue(arg);

   msg("Executing on command: " + comm);
   //obj->getType() == SYM
   if (global_enable) {
      if (!fork()) {
         //string comm = lineak_util_functions::unescape(command.getCommand(), "\"#");
         comm += " &";
         /* child process that tries to run the command, and then exits */
         /* all specials done, let's go for it... ;) */
         if (verbose) cout << "... running " << comm << endl;
        //if (verbose) cout << "... displaying " << comm << endl;
         system(comm.c_str());
         exit(true);
      }
      /* NOTE: no, we don't wait() for the child. we just ignore the SIGCLD signal */
      /** Use the OSD to show the output */
      if ( mediadetect_Display != NULL ) {
           if (dname == "" || dname == snull) {
               string lname = dir->getValue(NAME) + ": ";
               lname += arg;
	       mediadetect_Display->show(lname);
           }
           else
               mediadetect_Display->show(dname);
      }
   }
}

void dispatch_macro(LObject *obj, LCommand &command, ConfigDirectives *dir, XEvent xev) {
   const vector<string>& args = command.getArgs();
   string arg = args[0];
   string comm = dir->getValue(arg);

   exec_t execute = NULL;

   msg("Executing on command: " + arg);
   /** Replace the LCommand object of the LObject with one that contains the macro to run. */
   // do it here.
   LCommand newcomm = command;
   newcomm.setCommand(comm);
   // Copy the LObject to our our internal LObject. We do not want to alter the 
   // users configuration permanently, so may a copy and use it.
   *mdobj = *obj;

   if ( !obj->isUsedAsToggle() ) {
      if (obj->getType() == CODE || obj->getType() == SYM) {
         mdobj->setCommand(newcomm,xev.xkey.state);
      }
      if (obj->getType() == BUTTON) {
         mdobj->setCommand(newcomm,xev.xbutton.state);
      }
   }
   else {
      string name = mdobj->getNextToggleName();
      msg("Setting command for toggle name: " + name);
      mdobj->setCommand(newcomm, name);
   }	   
   if (verbose) cout << "dispatch_macro: " << *mdobj << endl;
   
   if (global_enable) {
      execute = plugins->exec(mdobj, xev);
      execute(mdobj,xev); 

    /** Use the OSD to show the output */
      /** We should not show the display here. Instead, we should allow
       * the macro to handle the display itself as it may have contextual
       * information to display that we cannot get here. */
      /*
    if ( mediadetect_Display != NULL ) {
         if (dname == "" || dname == snull) {
             string lname = dir->getValue(NAME) + ": ";
             lname += arg;
             mediadetect_Display->show(lname);
         }
         else
             mediadetect_Display->show(dname);
    }*/

   }
}
