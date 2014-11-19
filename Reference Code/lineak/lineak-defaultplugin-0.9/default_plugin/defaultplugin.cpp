/***************************************************************************
                          defaultplugin.cpp  -  description
                             -------------------
    begin                :  10 26 2003
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
#include <lineak/cdromctrl.h>

#include <lineak/plugin_definitions.h>
#include <lineak/displayctrl.h>
#include <lineak/lineak_util_functions.h>
#include <lineak/lineak_core_functions.h>
#include <lineak/xmgr.h>
#include <X11/extensions/XTest.h>
#include <algorithm>
#include <cctype>

#include <iostream>
#include <sstream>
#include <stdlib.h>
using namespace std;
using namespace lineak_core_functions;

#include "defaultplugin.h"

/** These are required */
#define NUM_MACROS 16 
#define INPUT 1
#define ROOT 2
#define POINTER 3

string default_symbols[NUM_MACROS] = { "EAK_EJECT", "EAK_OPEN_TRAY_SCSI", "EAK_OPEN_TRAY", "EAK_CLOSE_TRAY", "EAK_VOLUP", "EAK_VOLDOWN", "EAK_PCM_UP", "EAK_PCM_DOWN", "EAK_MUTE", "EAK_PCM_MUTE", "EAK_SLEEP", "EAK_SENDKEYS", "EAK_SENDKEYS_ROOT", "EAK_SENDKEYS_WINDOW", "EAK_SYM", "EAK_SCREEN_LOCK" };
macro_info* default_macinfo = NULL;
identifier_info* idinfo = NULL;
LConfig* myConfig = NULL;
bool verbose = false;
bool very_verbose = false;
bool enable;
bool trayopen = false;
PluginManager* plugins = NULL;
string dname = "";
displayCtrl* default_Display = NULL;

// Needed for the XKEYEVENT macros
// 
/*
Display *dpy;
int scr,x,y;
Window win, ret_win;
unsigned int width, height;
unsigned int border_width, depth;
*/

extern "C" int initialize(init_info init) {
        verbose = init.verbose;
	very_verbose = init.very_verbose;
	enable = init.global_enable;
	myConfig = init.config;
        plugins = init.plugins;

	msg("Registered");

        return true;
}
extern "C" identifier_info* identifier() {
	idinfo = new (identifier_info);
        idinfo->description = "Default EAK macros.";
        idinfo->identifier = "default";
        idinfo->type = "MACRO";
	idinfo->version = VERSION;
        return idinfo;
}

extern "C" int exec(LObject* imyKey,XEvent xev) {
	LCommand command;

        vmsg("in exec");
	
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
                if (very_verbose) cout << "enable = " << enable << endl;
                if (enable) {
                        if (verbose) cout << "... that's the " <<  imyKey->getName() << " key" << endl;
                        /* check if the key has to run a SPECIAL command */
                        if ( command.getMacroType() == "EAK_EJECT" || 
                             command.getMacroType() == "EAK_OPEN_TRAY_SCSI" ||
                             command.getMacroType() == "EAK_OPEN_TRAY" ) { /* Eject */
                                macroEAK_OPEN_TRAY(command);
                        } else if ( command.getMacroType() == "EAK_CLOSE_TRAY" ) { /* Close CD tray */
                                macroEAK_CLOSE_TRAY(command);
                        } else if ( command.getMacroType() == "EAK_VOLUP" ) { /* VolumeUp */
                                macroEAK_VOLUP(command, USE_MIXER);
                        } else if (command.getMacroType() == "EAK_VOLDOWN" ) { /* VolumeDown */
                                macroEAK_VOLDOWN(command, USE_MIXER);
                        } else if (command.getMacroType() == "EAK_MUTE" ) { /* Mute */
                                macroEAK_MUTE(command, USE_MIXER);
			} else if ( command.getMacroType() == "EAK_PCM_UP" ) { /* PCM VolumeUp */
                                macroEAK_VOLUP(command, USE_PCM);
                                //macroEAK_PCM_UP(command);
                        } else if (command.getMacroType() == "EAK_PCM_DOWN" ) { /* PCM VolumeDown */
                                //macroEAK_PCM_DOWN(command);
                                macroEAK_VOLDOWN(command, USE_PCM);
                        } else if (command.getMacroType() == "EAK_PCM_MUTE" ) { /* PCM Mute */
                                //macroEAK_PCM_MUTE(command);
                                macroEAK_MUTE(command,USE_PCM);
                        } else if (command.getMacroType() == "EAK_SLEEP" ) { /* Sleep (not implemented yet) */
                                macroEAK_SLEEP(command);
                        } else if (command.getMacroType() == "EAK_SENDKEYS" ||
				   command.getMacroType() == "EAK_SYM"  ) { /* Key SYM handling */
                                macroEAK_SYM(imyKey, command, INPUT);
			} else if (command.getMacroType() == "EAK_SENDKEYS_ROOT" ) { /* Key event handling */
				macroEAK_SYM(imyKey, command, ROOT);
			} else if (command.getMacroType() == "EAK_SENDKEYS_WINDOW" ) { /* Key event handling */
				macroEAK_SYM(imyKey, command, POINTER);
                        } else if (command.isMacro()) { /** If we are a macro, but we can't find one that matches, return false. */
                                return false;
                        } else if (command.getCommand() == snull) { /* no command defined */
                                if (verbose) cout << "... but it has no command bound to it :(" << endl;
                                cout << *(imyKey);
                        } 
                        /** Rotate the key state if it's a toggleable key */
			if (imyKey->isUsedAsToggle())
				imyKey->toggleState();
                }
        }

	return true;
}
extern "C" macro_info* macrolist() {
	default_macinfo = new (macro_info);
	default_macinfo->num_macros = NUM_MACROS;
	default_macinfo->macro_list = default_symbols;
        return default_macinfo;
}
extern "C" void cleanup() {
	msg("Cleaning up plugin defaultplugin");
	if (default_macinfo != NULL) {
		delete (default_macinfo);
		default_macinfo = NULL;
	}
	if (idinfo != NULL) {
                delete (idinfo);
                idinfo = NULL;
        }
	msg("Done cleaning up plugin defaultplugin");
}
extern "C" void initialize_display(displayCtrl *imyDisplay) {
	msg("Initializing display!");
        default_Display = imyDisplay;
}

void macroEAK_XSENDKEYS(LObject *obj, LCommand &command) {
/*
   const vector<string>& args = command.getArgs();
   string macro = command.getMacroType();

   if (args.size() == 1) {

	   string comm = "bash -c \"xsendkeys ";
	   comm += "\"";
	   comm += args[0];
	   comm += "\"\"";
	   if (!fork()) {
		   //comm += " &";
		   if (verbose) cout << "... running " << comm << endl;
                   if (verbose) cout << "... displaying " << dname << endl;
                   system(comm.c_str());
                   exit(true);
           }
	   
      //
      Display *dpy;
      int scr;
      Window win;
      unsigned int width, height;

      Window ret_win;
      int x, y;
      unsigned int border_width, depth;

      dpy = XOpenDisplay("");
      if (!dpy) {
  	  error("Cannot connect to display!");
      }
      scr = DefaultScreen(dpy);
      win = RootWindow(dpy, scr);
      XGetGeometry(dpy, win, &ret_win, &x, &y, &width, &height, &border_width, &depth);

      int keycode;
      vector<string> elements;
      vector<int>codes;
      KeySym mykeysym;
      KeyCode xkeycode;

      int index = 0;
      string parsed = "";
      string symname = lineak_util_functions::strip_space(args[0]);
      string smodifiers = "";
    
      while (symname.find('+') != string::npos) {
              index = symname.find('+');
              smodifiers = symname.substr(index+1,symname.size()-(index+1));
              parsed = symname.substr(0,index);
              symname = smodifiers;
              if ( parsed == "control" )
                      parsed = "Control_L";
              if ( parsed == "shift" )
                      parsed = "Shift_L";
              if ( parsed == "mod1" || parsed == "alt" )
                      parsed = "Alt_L";
              cout << "element = " << parsed << endl;
              elements.push_back(parsed);
              cout << "remainder = " << smodifiers << endl;
       }
       elements.push_back(smodifiers);

       for (vector<string>::const_iterator it = elements.begin(); it != elements.end(); it++) {
               mykeysym = XStringToKeysym(it->c_str());
               xkeycode = XKeysymToKeycode(dpy, mykeysym);
               cout << *it << " has code of " << (int)xkeycode << endl;
               codes.push_back((int)xkeycode);
       }

       cout << "vector has " << codes.size() << " elements: { ";
       for (vector<int>::const_iterator it = codes.begin(); it != codes.end(); it++)
                       cout << *it << " ";
       cout << "} " << endl;

       for (vector<int>::const_iterator it = codes.begin(); it != codes.end(); it++) {
               keycode = *it;
               if ( !XTestFakeKeyEvent(dpy, keycode, 1, 0) )
		       cout << "Could not send event!" << endl;
               cout << "sending key down: " << keycode << endl;
               XSync(dpy, 1);
       }
       vector<int>::const_iterator it = codes.end();

       do {
            it--;
            keycode = *it;
            XTestFakeKeyEvent(dpy, keycode, 0, 0);
            cout << "sending key up: " << keycode << endl;
            XSync(dpy, 1);
       }
       while (it != codes.begin());
       //
   }
*/
}

void macroEAK_SYM(LObject *obj, LCommand &command, int type) {
   const vector<string>& args = command.getArgs();
   string macro = command.getMacroType();
   if (args.size() == 1) {
      bool root, mouse, input;
      root = mouse = input = false;
      /** Determine the type */
      switch (type) {
	   case INPUT: 
		   input = true;
		   vmsg("Type is input");
		   break;
	   case ROOT: 
		   root = true;
		   vmsg("Type is root");
		   break;
	   case POINTER: 
		   mouse = true;
		   vmsg("Type is pointer");
		   break;
	   default:
		   input = true;
      }
      /** Expecting a KEYSYM string. */
      /** could be something like: "control+alt+S" or "shift+F1" or just "A" 
       *  This is the keysym we want to emit. */

	   /* This doesn't quite work as expected. What we may need to do is
	    * to output each key w/ the correct state in order and then release
	    * them in the proper order. 
	    *
	    * I've discovered that with xev if you press the Alt_L key it outputs normally
	    * but the release event has the modifier state set 0x8. Control is the same
	    * on the release it has the modifier state set to 0x4 (which is the modifier numeric
	    * for the control key.) */
                         
      unsigned int modifiers = 0;
      int index = 0;
      string parsed = "";
      string symname = lineak_util_functions::strip_space(args[0]);
      string smodifiers = "";
      KeySym mykeysym;
      KeySym *keysym;
      char *name;
      
      // parse the symname for modifiers. Strip off the modifier portion. 
      if (symname.find('+') != string::npos) {
          index = symname.rfind('+');
          parsed = symname.substr(index+1,symname.size()-(index+1));
          smodifiers = symname.substr(0,index);
          msg("modifiers = " + smodifiers);
          msg("symname = " + parsed);
          modifiers = lineak_core_functions::getModifierNumericValue(smodifiers);
          if (verbose) cout << "modifiers numerical = " << modifiers << endl;
      }
      else {
          parsed = symname;
          msg("modifiers = none");
	  msg("symname = " + parsed);
      }

      name = (char *) malloc (parsed.size()+1);
      strcpy (name, parsed.c_str());
      name[parsed.size()] = '\0';

      mykeysym = XStringToKeysym(parsed.c_str());
      keysym = &mykeysym;

      if (*keysym == NoSymbol && '0' <= *name && *name <= '9') {
         char *fmt = "%ld";
         char *str = name;
         unsigned long *val = keysym;
         if (*str == '0') {
            str++;
            fmt = "%lo";
            if (*str == '\0') {
               *val = 0;
            }
            if (*str == 'x' || *str == 'X') {
               str++;
               fmt = "%lx";
            }
         }
         if (*val != 0)
            sscanf (str, fmt, val);
      }

      Display *dpy = NULL;
      if ((dpy = XOpenDisplay("")) == NULL)
         error("Could not open the display.");

      XKeyEvent xev;
        
      // Get InputFocus window. Of some reason "InputFocus" dosn't work 
      Window w, win;
      int revertTo;
      int pointer_x, pointer_y, root_x_return, root_y_return, win_x_return, win_y_return;

      pointer_x = pointer_y = win_x_return = win_y_return = 1;
      root_x_return = root_y_return = 1;

      if (root) {
	 vmsg("Doing root window.");
         int scr;
         unsigned int width, height, border_width, depth;
	 scr = DefaultScreen(dpy);
	 win = RootWindow(dpy, scr);
	 XGetGeometry(dpy, win, &w, &win_x_return, &win_y_return, &width, &height, &border_width, &depth);
      }
      else if (input) {
	 vmsg("Doing input window.");
         XGetInputFocus(dpy, &w, &revertTo);
      }
      else if (mouse) {
	 vmsg("Doing mouse input window.");
	 Window child;
	 unsigned int mask_return;
	 XGetInputFocus(dpy, &win, &revertTo);
	 XQueryPointer(dpy, win, &w, &child, &root_x_return, &root_y_return, &win_x_return, &win_y_return, &mask_return);
	 w = child;
      }

      xev.display = dpy; 
      xev.root = DefaultRootWindow(dpy);
      xev.window = w;
      xev.same_screen = True;
      xev.subwindow = None;
      xev.type = KeyPress;
      xev.keycode = XKeysymToKeycode(dpy, mykeysym);
      xev.state = modifiers;
      xev.time = CurrentTime;
      xev.x = 1;
      xev.y = 1;
      xev.x_root = 1;
      xev.y_root = 1;
      //xev.x = win_x_return;
      //xev.y = win_y_return;
      //xev.x_root = root_x_return;
      //xev.y_root = root_y_return;
      xev.same_screen = True;
        
      if (XSendEvent(dpy, w, True, KeyPressMask, ( XEvent* )&xev) == 0)
         error("XSendEvent failed.");

      //xev.type = KeyRelease;
      //if (XSendEvent(dpy, w, True, KeyReleaseMask, ( XEvent* )&xev) == 0)
      //   cerr << "XSendEvent failed." << endl;
      
      XSync(dpy, false);
  }
}

void macroEAK_OPEN_TRAY(LCommand &command) {
        const vector<string>& args = command.getArgs();
  /** If there is just the macro, and no arguments, use the globally defined cdrom device, and
      eject that */
        string macro = command.getMacroType();
        if (args.size() == 0) {
           cdromCtrl myCdrom(myConfig->getValue("CdromDevice"));
                if (macro == "EAK_EJECT" || macro == "EAK_OPEN_TRAY")
                   myCdrom.openTray();
                if (macro == "EAK_OPEN_TRAY_SCSI") {
                   if (verbose) cout << "Calling the openTrayScsi() interface" << endl;
                   myCdrom.openTrayScsi();
                }
        }
        /** If however, the macro contains arguments, then attempt to eject all of those */
        else {
          cdromCtrl cd;
          for (vector<string>::const_iterator it = args.begin(); it != args.end(); it++) {
                cd.setCdromdev(*it);
        /** Output the command */
                //if (default_Display != NULL) {
                //   default_Display->show(string("Ejecting CDROM"));
                //}
                if (macro == "EAK_EJECT" || macro == "EAK_OPEN_TRAY")
                   cd.openTray();
                if (macro == "EAK_OPEN_TRAY_SCSI") {
                   vmsg("Calling the openTrayScsi() interface");
                   cd.openTrayScsi();
                }
    }
  }
  if ( default_Display != NULL ) {
     if (dname == "" || dname == snull) default_Display->show(string("Ejecting CDROM"));
     else
        default_Display->show(dname);
  }
}
void macroEAK_CLOSE_TRAY(LCommand &command) {
        const vector<string>& args = command.getArgs();
  /** If there is just the macro, and no arguments, use the globally defined cdrom device, and
      eject that */
        string macro = command.getMacroType();
        if (args.size() == 0) {
           cdromCtrl myCdrom(myConfig->getValue("CdromDevice"));
                   vmsg("Calling the closeTray() interface");
                   myCdrom.closeTray();
                
        }
        /** If however, the macro contains arguments, then attempt to eject all of those */
        else {
          cdromCtrl cd;
          for (vector<string>::const_iterator it = args.begin(); it != args.end(); it++) {
                cd.setCdromdev(*it);
        /** Output the command */
                if (default_Display != NULL) {
                   default_Display->show(string("Closing the CDROM tray"));
                }
                vmsg("Calling the closeTray() interface");
                cd.closeTray();

          }
        }
  if ( default_Display != NULL ) {
     if (dname == "" || dname == snull) default_Display->show(string("Closing the CDROM tray"));
     else
        default_Display->show(dname);
  }

}
/** macroEAK_Mute. In order to support using lots of soundCtrl devices in a disposable
        manner. i.e. create one when you want, then throw it away when you don't, and supporting
        lots of user mixer devices, we need to keep track of soundCtrl objects for each mixer we
        may have used. They get added on a per use basis. This is b/c the toggleMute() method of
        the soundCtrl class depends on a private variable that tracks the state of the mixer (i.e. muted
        or not). If we just instantiate and destroy the objects as we need them we loose the
        state of this variable. I support I could have created a static vector of mute states for the
        class, but I felt that was getting really beyond the function of this class. */
void macroEAK_MUTE(LCommand &command, int type) {
  const vector<string>& args = command.getArgs();
  static map<string,soundCtrl> soundDevices;
  int retval = 0;
  string mixer = myConfig->getValue("MixerDevice");

  /** If there is just the macro, and no arguments, use the default volume stepping, and
      adjust by that */
  if (args.size() == 0) {
     /* Set the mixer device */
     /** If the soundDevice object for the corresponding mixer is not in the map. Add it */
      if ((soundDevices[mixer]).getMixer() != mixer) {
            soundCtrl mySound(mixer,USE_MIXER);
            soundDevices[mixer] = mySound;
      }
      if ( type == USE_PCM )
         (soundDevices[mixer]).setType(USE_PCM);
      if ( type == USE_MIXER )
         (soundDevices[mixer]).setType(USE_MIXER);

      retval = (soundDevices[mixer]).toggleMute();
      if (default_Display != NULL) {
        if (retval == -1)
           default_Display->volume(displayCtrl::MUTE);
        else
           default_Display->volume(retval);
      }
  }
  else {
  /** We should have either an integer, or integer-string pairs, which correspond to the increment
        value, and the device to adjust. */
    // Assume we have just an increment value.
    int value = 0;
        if (args.size() == 1) {
          value = atoi(args[0].c_str());
     /** If the soundDevice object for the corresponding mixer is not in the map. Add it */
      if ((soundDevices[mixer]).getMixer() != mixer) {
            soundCtrl mySound(mixer,USE_MIXER);
        soundDevices[mixer] = mySound;
      }
      if ( type == USE_PCM )
             (soundDevices[mixer]).setType(USE_PCM);
      if ( type == USE_MIXER )
             (soundDevices[mixer]).setType(USE_MIXER);

      retval = (soundDevices[mixer]).toggleMute(value);
      if (default_Display != NULL) {
         if (retval == -1)
            default_Display->volume(displayCtrl::MUTE);
         else
            default_Display->volume(retval);
      }
    }
    // Assume we have integer and value pairs.
    else {
        if (args.size() >=2 && (args.size() % 2 == 0)) {
//          soundCtrl sound;
            for (vector<string>::const_iterator it = args.begin(); it != args.end();) {
                value = atoi(it->c_str());
                it++;
                string device = *it;
                it++;
                /** If there is no soundCtrl object for this mixer, add one */
                if ((soundDevices[device]).getMixer() != device) {
                    soundCtrl mySound(device,USE_MIXER);
                    soundDevices[device] = mySound;
                }
		if ( type == USE_PCM )
                   (soundDevices[mixer]).setType(USE_PCM);
       	        if ( type == USE_MIXER )
	           (soundDevices[mixer]).setType(USE_MIXER);

                retval = (soundDevices[mixer]).toggleMute(value);
                if (default_Display != NULL) {
                    if (retval == -1)
                        default_Display->volume(displayCtrl::MUTE);
                    else
                        default_Display->volume(retval);
                }
            }
        }
    }
  }
}

void macroEAK_VOLUP(LCommand &command,int type) {
  const vector<string>& args = command.getArgs();
  int retval = 0;
  string mixer = myConfig->getValue("MixerDevice");
  /** If there is just the macro, and no arguments, use the default volume stepping, and
      adjust by that */
  msg("EAK_VOLUP");
  if (args.size() == 0) {
        msg("doing default volume up");
        /* Set the mixer device */
        soundCtrl mySound(mixer,type);
        retval = mySound.volumeUp();
        if (default_Display != NULL)
            default_Display->volume(retval);
  }
  /** We should have either an integer, or integer-string pairs, which correspond to the increment
      value, and the device to adjust. */
  // Assume we have just an increment value.
  if (args.size() == 1) {
      int value = 0;
      msg("single volume up");
      value = atoi(args[0].c_str());
      soundCtrl mySound(mixer,type);
      retval = mySound.volumeUp(value);
      if (default_Display != NULL)
            default_Display->volume(retval);
  }
  // Assume we have integer and value pairs.
  if (args.size() >=2 && (args.size() % 2 == 0)) {
      int value = 0;
      msg("multiple volume ups");
      soundCtrl sound;
      for (vector<string>::const_iterator it = args.begin(); it != args.end();) {
          value = atoi(it->c_str());
          it++;
          string device = *it;
          it++;
          if (verbose) cout << device << " adjusted by: " << value << endl;
          sound.setMixer(device);
          retval = sound.volumeUp(value);
          if (default_Display != NULL)
              default_Display->volume(retval);
      }
  } 
}

void macroEAK_VOLDOWN(LCommand &command, int type) {
  const vector<string>& args = command.getArgs();
  int retval = 0;
  string mixer = myConfig->getValue("MixerDevice");
  /** If there is just the macro, and no arguments, use the default volume stepping, and
      adjust by that */

  if (args.size() == 0) {
      msg("default volumeDown");
      /* Set the mixer device */
      soundCtrl mySound(mixer,type);
      retval = mySound.volumeDown();
      if (default_Display != NULL)
         default_Display->volume(retval);
  }
  /** We should have either an integer, or integer-string pairs, which correspond to the increment
      value, and the device to adjust. */
  // Assume we have just an increment value.
  if (args.size() == 1) {
        int value = 0;
        value = atoi(args[0].c_str());
        // If the value is positive, make it negative.
        if ( value > 0)
            value = 0 - value;
        if (verbose) cout << "single volume down by: " << value << endl;
        soundCtrl mySound(mixer,type);
        retval = mySound.volumeDown(value);
        if (default_Display != NULL)
            default_Display->volume(retval);
  }
  // Assume we have integer and value pairs.
  if (args.size() >=2 && (args.size() % 2 == 0)) {
	int value = 0;
        soundCtrl sound;
        for (vector<string>::const_iterator it = args.begin(); it != args.end();) {
                value = atoi(it->c_str());
                it++;
                string device = *it;
                it++;
                if ( value > 0)
                   value = 0 - value;
                if (verbose) cout << device << " adjusted by: " << value << endl;
                sound.setMixer(device);
                retval = sound.volumeDown(value);
                if (default_Display != NULL)
                    default_Display->volume(retval);
        }
  }
}

void macroEAK_SLEEP(LCommand &command) {
        if (default_Display != NULL)
          default_Display->show ("Sleep");
}

void macroEAK_SCREEN_LOCK(LCommand &command) {
        const vector<string>& args = command.getArgs();
        if (!args.empty()) {
    		string method = args[0];
	    	// Transform the method to uppercase so we can do a comparison.
    		int (*pf)(int)=toupper;
    		std::transform(method.begin(), method.end(), method.begin(), pf);
                string comm;
                if (!fork()) {
                        if (method == "KDE")
                                comm = "dcop kdesktop KScreensaverIface lock";
                	if (method == "GNOME" || method == "XSCREENSAVER" )
                                comm = "xscreensaver-command -lock";

          		//comm = lineak_util_functions::unescape(comm,"#");
                        comm += " &";
                 	// child process that tries to run the command, and then exits
                	// all specials done, let's go for it... ;)
                	msg("Locking screen for desktop " + method);
                                system(comm.c_str());
                                exit(1);
                 }
                 // NOTE: no, we don't wait() for the child. we just ignore the SIGCLD signal
        }
        else
                error("EAK_SCREEN_LOCK macro requires an argument");

}

