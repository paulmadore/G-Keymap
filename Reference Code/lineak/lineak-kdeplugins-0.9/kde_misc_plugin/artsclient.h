/***************************************************************************
                          artsclient.h  -  description
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

#ifndef ARTSCLIENT_H
#define ARTSCLIENT_H
#include <qstring.h>
#include <arts/soundserver.h>
#include <arts/dispatcher.h>

/**Controls the sending of an email via a mail client.
  *@author Sheldon Lee Wen
  */
using namespace Arts;

class ARTSClient {
public:
    ARTSClient();
    virtual ~ARTSClient();

    bool suspend();
/*    void setVolume(float volume);
    float getVolume();
    void setDecibelVolume(float volume );
    float getDecibelVolume();
    float volumeUp(float delta = 0.05);
    float volumeDown(float delta = 0.05);
    float mute(float value = 0);
*/

  protected:
    Arts::Dispatcher dispatcher;
    Arts::SoundServerV2 server;

};

#endif
