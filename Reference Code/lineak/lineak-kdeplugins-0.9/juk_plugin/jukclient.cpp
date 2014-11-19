/***************************************************************************
                          jukclient.cpp  -  description
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
#include "jukclient.h"

using namespace std;
extern bool verbose;

JukClient::JukClient(DCOPClient *idcop){
	dcop = idcop;

	/* if (!dcop->isApplicationRegistered("juk")) {
	   running = false;
           if (KApplication::startServiceByDesktopName("juk")) {
	       cout << i18n("No running instance of juk found.");
           }
        } */
}
JukClient::~JukClient(){
}
bool JukClient::isRunning() {
      if (!dcop->isApplicationRegistered("juk")) {
         return false;
      }
      else {
	 return true;
      }
   return true;
}
bool JukClient::startJuk() {
   if (!isRunning()) {
      KApplication::startServiceByDesktopName("juk");
   }
   return (dcop->isApplicationRegistered("juk"));
}
void JukClient::openFile(QString s) {
   if (!isRunning())
      startJuk();

   if (isRunning()) {
      QByteArray data;
      QDataStream arg( data, IO_WriteOnly );
      arg << s;
      if (!dcop->send("juk","Player","openFile(QString)",data)) {
        if (verbose) cerr << "juk Player openFile(QString) call failed." << endl;
      }
   }
}
void JukClient::openFile(QStringList file) {
   if (!isRunning())
      startJuk();

   if (isRunning()) {
      QByteArray data;
      QDataStream arg( data, IO_WriteOnly );
      arg << file;
      if (!dcop->send("juk","Player","openFile(QStringList)",data)) {
         if (verbose) cerr << "juk Player openFile(QStringList) call failed." << endl;
      }
   }
}
void JukClient::play() {
   dcopSend("play()");
}
void JukClient::pause() {
   dcopSend("pause()");
}
void JukClient::stop() {
   dcopSend("stop()");
}
void JukClient::playPause() {
   dcopSend("playPause()");
}
void JukClient::back() {
   dcopSend("back()");
}
void JukClient::forward() {
   dcopSend("forward()");
}
void JukClient::seekBack() {
   dcopSend("seekBack()");
}
void JukClient::seekForward() {
   dcopSend("seekForward()");
}
void JukClient::volumeUp() {
   dcopSend("volumeUp()");
}
void JukClient::volumeDown() {
   dcopSend("volumeDown()");
}
void JukClient::volumeMute() {
   dcopSend("volumeMute()");
}
QString JukClient::isPlaying() {
   QString result = "";
   if (!isRunning())
      startJuk();

   if (isRunning()) {
      QByteArray data, replyData;
      QCString replyType;
      QDataStream arg( data, IO_WriteOnly );
      if ( dcop->call("juk","Player","playingString()", data, replyType, replyData ) ) {
	  if ( replyType == "QString" ) {
              QDataStream _reply_stream( replyData, IO_ReadOnly );
              _reply_stream >> result;
          } else {
              kdDebug() << "playingString() call failed." << endl;
          }
      } else {
           kdDebug() << "playingString() call failed." << endl;
      }
   }
  return result; 
}
/*
void JukClient::setVolume(float volume) {
} */
void JukClient::startPlayingPlaylist() {
   dcopSend("startPlayingPlaylist()");
}
void JukClient::dcopSend(QString command) {
   if (!isRunning())
      startJuk();

   if (isRunning()) {
      QByteArray data;
      QDataStream arg( data, IO_WriteOnly );
      if (!dcop->send("juk","Player",QCString(command),data)) {
        if (verbose) cerr << "juk Player " << command << " call failed." << endl;
      }
   }
}

