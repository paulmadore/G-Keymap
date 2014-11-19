/***************************************************************************
                          kscdclient.h  -  description
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

#ifndef KSCDCLIENT_H
#define KSCDCLIENT_H
#include <string>
#include <dcopclient.h>
#include <qstring.h>
#include <qstringlist.h>

/**Controls the kscd appplication via dcop.
  *@author Sheldon Lee Wen
  */
using namespace std;

class KSCDClient {

  public: 
    KSCDClient(DCOPClient *idcop);
    virtual ~KSCDClient();
    void play();
    void pause();
    void stop();
    void previous();
    void next();
    bool startKSCD();
    bool isRunning();
    void eject();
    void toggleLoop();
    void toggleShuffle();
    void toggleTimeDisplay();
    QString currentTrackTitle();
    QString currentAlbum();
    QString currentArtist();


  protected:
    void dcopSend(QString);
    QString dcopCall(QString);
    DCOPClient *dcop;
};

#endif
