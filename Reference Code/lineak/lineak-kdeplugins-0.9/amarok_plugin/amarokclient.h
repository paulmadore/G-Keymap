/***************************************************************************
                          amarokclient.h  -  description
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

#ifndef AMAROKCLIENT_H
#define AMAROKCLIENT_H
#include <string>
#include <dcopclient.h>
#include <qstring.h>
#include <qstringlist.h>

/**Controls the amarok appplication via dcop.
  *@author Sheldon Lee Wen
  */
using namespace std;

class AmarokClient {

  public: 
    AmarokClient(DCOPClient *idcop);
    virtual ~AmarokClient();
    void addMedia(QString s);
    void addMedia(QStringList list);
    void seek(int s);
    void enableRandomMode(bool enable);
    void play();
    void pause();
    void stop();
    void playPause();
    void back();
    void forward();
    void volumeUp();
    void volumeDown();
    void volumeMute();
    void togglePlaylist();
    bool startAmarok();
    bool isRunning();
    QString isPlaying();


  protected:
    void dcopSend(QString);
    DCOPClient *dcop;
};

#endif
