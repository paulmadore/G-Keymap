/***************************************************************************
                          kmixclient.h  -  description
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

#ifndef KMIXCLIENT_H
#define KMIXCLIENT_H
#include <string>
#include <dcopclient.h>

/**Controls the kmix appplication via dcop.
  *@author Sheldon Lee Wen
  */
using namespace std;

class KMIXClient {
public: 
    KMIXClient(DCOPClient *idcop);
    virtual ~KMIXClient();

    int volumeUp(int value = 1, string mixer = "");
    int volumeDown(int value = 1, string mixer = "");
    int mute(string mixer = "");
    int masterVolume(string mixer);
    int setVolume(int value = 1, string mixer="");
    bool startKMIX();
    bool isRunning();

  protected:
    DCOPClient *dcop;
    bool muted;
    bool running;
};

#endif
