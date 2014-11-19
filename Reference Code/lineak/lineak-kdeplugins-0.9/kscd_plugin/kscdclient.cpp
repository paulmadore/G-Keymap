/***************************************************************************
                          kscdclient.cpp  -  description
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
#include <qdatastream.h>
#include <qcstring.h> 
#include <iostream>
#include "kscdclient.h"

using namespace std;
extern bool verbose;

KSCDClient::KSCDClient(DCOPClient *idcop){
	dcop = idcop;

	/* if (!dcop->isApplicationRegistered("kscd")) {
	   running = false;
           if (KApplication::startServiceByDesktopName("kscd")) {
	       cout << i18n("No running instance of kscd found.");
           }
        } */
}
KSCDClient::~KSCDClient(){
}
bool KSCDClient::isRunning() {
      if (!dcop->isApplicationRegistered("kscd")) {
         return false;
      }
      else {
	 return true;
      }
   return true;
}
bool KSCDClient::startKSCD() {
   if (!isRunning()) {
      KApplication::startServiceByDesktopName("kscd");
   }
   return (dcop->isApplicationRegistered("kscd"));
}
void KSCDClient::play() {
   dcopSend("play()");
}
void KSCDClient::pause() {
   dcopSend("pause()");
}
void KSCDClient::stop() {
   dcopSend("stop()");
}
void KSCDClient::previous() {
   dcopSend("previous()");
}
void KSCDClient::next() {
   dcopSend("next()");
}
void KSCDClient::eject() {
   dcopSend("eject()");
}
void KSCDClient::toggleLoop() {
   dcopSend("toggleLoop()");
}
void KSCDClient::toggleShuffle() {
   dcopSend("toggleShuffle()");
}
void KSCDClient::toggleTimeDisplay() {
   dcopSend("toggleTimeDisplay()");
}
QString KSCDClient::currentTrackTitle() {
   return dcopCall("currentTrackTitle()");
}
QString KSCDClient::currentAlbum() {
   return dcopCall("currentAlbum()");
}
QString KSCDClient::currentArtist() {
   return dcopCall("currentArtist()");
}

void KSCDClient::dcopSend(QString command) {
   if (!isRunning())
      startKSCD();

   if (isRunning()) {
      QByteArray data;
      QDataStream arg( data, IO_WriteOnly );
      if (!dcop->send("kscd","CDPlayer",QCString(command),data)) {
        if (verbose) cerr << "kscd Player " << command << " call failed." << endl;
      }
   }
}

QString KSCDClient::dcopCall(QString command) {
   QString result = "";
   if (!isRunning())
      startKSCD();

   if (isRunning()) {
      QByteArray data, replyData;
      QCString replyType;
      QDataStream arg( data, IO_WriteOnly );
      if ( dcop->call("kscd","CDPlayer",QCString(command), data, replyType, replyData ) ) {
          if ( replyType == "QString" ) {
              QDataStream _reply_stream( replyData, IO_ReadOnly );
              _reply_stream >> result;
          } else {
              kdDebug() << "dcopCall() call failed." << endl;
          }
      } else {
           kdDebug() << "dcopCall() call failed." << endl;
      }
   }
  return result;
}

