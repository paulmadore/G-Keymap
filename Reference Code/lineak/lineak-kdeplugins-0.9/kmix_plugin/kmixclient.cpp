/***************************************************************************
                          kmixclient.cpp  -  description
                             -------------------
    begin                : Mon Jul 22 2002
    copyright            : (C) 2002 by Sheldon Lee Wen
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
#include <unistd.h>
#include <stdio.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <klocale.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <qfile.h>
#include <kurl.h>
#include <kapplication.h>
#include <dcopclient.h>
#include <iostream>
#include <lineak/lineak_core_functions.h>
#include "kmixclient.h"

using namespace std;
using namespace lineak_core_functions;

extern bool verbose;
extern bool very_verbose;

KMIXClient::KMIXClient(DCOPClient *idcop){
	dcop = idcop;
	muted = false;
        running = false;
	running = isRunning();
	/*
	if (!dcop->isApplicationRegistered("kmix")) {
           if (KApplication::startServiceByDesktopName("kmix")) {
	       cout << i18n("No running instance of KMix found.");
           }
        }
	*/
}
KMIXClient::~KMIXClient(){
}
bool KMIXClient::isRunning() {
   if (!running) {
      if (!dcop->isApplicationRegistered("kmix"))
         return false;
      else {
	 running = true;
	 return true;
      }
   }
   return true;
}
bool KMIXClient::startKMIX() {
   KApplication::startServiceByDesktopName("kmix");
   return (dcop->isApplicationRegistered("kmix"));
}
int KMIXClient::volumeUp(int value, string mixer) {
   if (!running) 
      isRunning();
   if (running) {
     int master = masterVolume(mixer);

     if (value == 0)
	master++;
     else
	master += abs(value);

     return setVolume(master,mixer);
   }
   else
      error("KMIX is not running!");
 
   return -1; 
}
int KMIXClient::volumeDown(int value, string mixer) {
   if (!running)
      isRunning();
   if (running) {
     int master = masterVolume(mixer);
	
     if (value == 0)
        master--;
     else
	master -= abs(value);
	
     return setVolume(master,mixer);
   }
   else
      error("KMIX is not running!");

   return -1;
}
int KMIXClient::mute(string mixer) {
   if (!running)
      isRunning();

   if (running) {
	   
      bool result = false;
      QByteArray data, replyData;
      QCString replyType;
      QDataStream arg( data, IO_WriteOnly );

      // Get the last digit from the mixer name.
      int imixer = atoi(mixer.substr((mixer.size()-1),1).c_str());
      arg << imixer;
      if (dcop->call(QCString("kmix"),QCString(mixer.c_str()),QCString("mute(int)"),data, replyType, replyData)) {
         if ( replyType == "bool" ) {
            QDataStream _reply_stream( replyData, IO_ReadOnly );
            _reply_stream >> result;
         } else
	    error("kmix mute(int) call failed.");
      } else
	 error("kmix mute(int) call failed.");

      QByteArray data2, replyData2;
      QDataStream arg2( data2, IO_WriteOnly );
      arg2 << imixer;
      // If we are muted
      if (result) {
         // Unmute
         arg2 << false;
      }
      else {
         // mute
         arg2 << true;
      }

      bool retval = dcop->call(QCString("kmix"),QCString(mixer.c_str()),QCString("setMute(int,bool)"),data2,replyType,replyData2);
      if (!retval) {
	 error("kmix setMute(int,bool) call failed.");
         // If we were muted, and an unmute failed.
         if (result)
            return (0);
         else
	    // If we were unmuted, and a mute failed.
	    return (masterVolume(mixer));
      } else {
         // If we were muted.
         if (result)
	    return (masterVolume(mixer));
         else
	    return (0);
      }
   }
   else
      error("KMIX is not running!");
	 
   return -1;
}
int KMIXClient::masterVolume(string mixer) {
   if (!running)
      isRunning();
   if (running) {
	   
      int result = 0;
      QByteArray data, replyData;
      QCString replyType;
      if (dcop->call(QCString("kmix"),QCString(mixer.c_str()),QCString("masterVolume()"),data, replyType, replyData)) {
         if ( replyType == "int" ) {
            QDataStream _reply_stream( replyData, IO_ReadOnly );
            _reply_stream >> result;
	    return result;
         } else
	    error("kmix masterVolume() call failed.");
      } else
         error("kmix masterVolume() call failed.");
      return 0;
   }
   else {
      error("KMIX is not running!");
      return -1;
   }

   return 0;
}
int KMIXClient::setVolume(int value, string mixer) {	

   int oldmaster = masterVolume(mixer);
   int newmaster;

   if (!running)
      isRunning();
   if (running) {

     if (value > 100 || value < 0)
        return oldmaster;

     QByteArray data, replyData;
     data.truncate(0);
     QCString replyType;
     QDataStream arg( data, IO_WriteOnly );

     if (very_verbose) cout << "Start Volume is: " << oldmaster << endl;
     if (very_verbose) cout << "Start Setting volume to: " << value << endl;
     if (very_verbose) cout << "Mixer Name " <<  QCString(mixer.c_str()) << endl;
     arg << 0;

     if (very_verbose) cout << "Size " <<  data.size()  << endl;
	
     if (value < oldmaster) {
	if (very_verbose) cout << "decrease volume from " << oldmaster << " to " << value;
	for(int i=oldmaster;i>value;i--) {
		if (very_verbose) cout << "-";
		dcop->call(QCString("kmix"),QCString(mixer.c_str()),QCString("decreaseVolume(int)"),data, replyType, replyData, true);
	}
     } else {
	if (very_verbose) cout << "increase volume from " << oldmaster << " to " << value;
	for(int i=oldmaster;i<value;i++) {
		if (very_verbose) cout << "+";
		dcop->call(QCString("kmix"),QCString(mixer.c_str()),QCString("increaseVolume(int)"),data, replyType, replyData, true);
	}
     }
     if (very_verbose) cout << endl;

     
     int loop=300;
     while(oldmaster == masterVolume(mixer)) {
	   if (very_verbose) cout << ".";
	   newmaster = masterVolume(mixer);
	   if (loop-- <= 0) break;
	}
     newmaster = masterVolume(mixer);

     if (very_verbose) cout << " waitloops = " << 300 - loop << endl << "New Mastervalue " << newmaster << endl;
     return newmaster;
   }
   else
      error("KMIX is not running!");
   return -1;
}
