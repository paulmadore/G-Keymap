/***************************************************************************
                          amarokclient.cpp  -  description
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
#include "amarokclient.h"

using namespace std;
extern bool verbose;

AmarokClient::AmarokClient(DCOPClient *idcop){
	dcop = idcop;

	/* if (!dcop->isApplicationRegistered("amarok")) {
	   running = false;
           if (KApplication::startServiceByDesktopName("amarok")) {
	       cout << i18n("No running instance of amarok found.");
           }
        } */
}
AmarokClient::~AmarokClient(){
}
bool AmarokClient::isRunning() {
      if (!dcop->isApplicationRegistered("amarok")) {
         return false;
      }
      else {
	 return true;
      }
   return true;
}
bool AmarokClient::startAmarok() {
   if (!isRunning()) {
      KApplication::startServiceByDesktopName("amarok");
   }
   return (dcop->isApplicationRegistered("amarok"));
}
void AmarokClient::addMedia(QString s) {
   if (!isRunning())
      startAmarok();

   if (isRunning()) {
      QByteArray data;
      QDataStream arg( data, IO_WriteOnly );
      KURL url(s);
      if (url.isValid()) {
        arg << url;
        if (!dcop->send("amarok","player","addMedia(KURL)",data)) {
          if (verbose) cerr << "amarok player addMedia(KURL) call failed." << endl;
        }
      }
      else if (verbose) cerr << "amarok addMedia, url is invalid." << endl;
   }
}
void AmarokClient::addMedia(QStringList list) {
   if (!isRunning())
      startAmarok();

   if (isRunning()) {
      QByteArray data;
      QDataStream arg( data, IO_WriteOnly );
      KURL::List url(list);
      arg << url;
      if (!dcop->send("amarok","player","addMediaList(KURL::List)",data)) {
         if (verbose) cerr << "amarok player addMediaList(KURL::List) call failed." << endl;
      }
   }
}

void AmarokClient::seek(int s) {
   if (!isRunning())
      startAmarok();

   if (isRunning()) {
      QByteArray data;
      QDataStream arg( data, IO_WriteOnly );
      arg << s;
      if (!dcop->send("amarok","player","seek(int)",data)) {
        if (verbose) cerr << "amarok player seek(int) call failed." << endl;
      }
   }
}
void AmarokClient::enableRandomMode(bool enable) {
   if (!isRunning())
      startAmarok();

   if (isRunning()) {
      QByteArray data;
      QDataStream arg( data, IO_WriteOnly );
      arg << enable;
      if (!dcop->send("amarok","player","enableRandomMode(bool)",data)) {
        if (verbose) cerr << "amarok player enableRandomMode(bool) call failed." << endl;
      }
   }
}
void AmarokClient::play() {
   if (! isPlaying() )
      dcopSend("play()");
}
void AmarokClient::pause() {
   dcopSend("pause()");
}
void AmarokClient::stop() {
   dcopSend("stop()");
}
void AmarokClient::playPause() {
   dcopSend("playPause()");
}
void AmarokClient::back() {
   dcopSend("prev()");
}
void AmarokClient::forward() {
   dcopSend("next()");
}
void AmarokClient::volumeUp() {
   dcopSend("volumeUp()");
}
void AmarokClient::volumeDown() {
   dcopSend("volumeDown()");
}
void AmarokClient::volumeMute() {
   dcopSend("mute()");
}
QString AmarokClient::isPlaying() {
   QString result = "";

   //if (!isRunning())
   //   startAmarok();

   if (isRunning()) {
      QByteArray data, replyData;
      QCString replyType;
      QDataStream arg( data, IO_WriteOnly );
      if ( dcop->call("amarok","player","nowPlaying()", data, replyType, replyData ) ) {
	  if ( replyType == "QString" ) {
              QDataStream _reply_stream( replyData, IO_ReadOnly );
              _reply_stream >> result;
          } else {
              kdDebug() << "nowPlaying() call failed." << endl;
          }
      } else {
           kdDebug() << "nowPlaying() call failed." << endl;
      }
   }
   
  return result; 
}
void AmarokClient::togglePlaylist() {
   dcopSend("togglePlaylist()");
}
void AmarokClient::dcopSend(QString command) {
   if (!isRunning())
      startAmarok();

   if (isRunning()) {
      QByteArray data;
      QDataStream arg( data, IO_WriteOnly );
      if (!dcop->send("amarok","player",QCString(command),data)) {
        if (verbose) cerr << "amarok player " << command << " call failed." << endl;
      }
   }
}

