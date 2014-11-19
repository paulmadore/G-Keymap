/***************************************************************************
                          artsclient.cpp  -  description
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
#include <config.h>
#ifdef HAVE_XOSD
#include <lineak/xosdctrl.h>
#endif
#include <iostream>
#include "artsclient.h"
//#include <arts/soundserver.h>
//#include <arts/dispatcher.h>

using namespace std;
//using namespace Arts;
extern bool verbose;

ARTSClient::ARTSClient() : dispatcher(), server(Reference("global:Arts_SoundServer"))  {
    if (server.isNull())
    {
       if (verbose)
         cerr << "unable to connect to sound server" << endl;
    }
//    muted = false;
}
ARTSClient::~ARTSClient() {
}
/** Suspend the sound server.
        return true if the server is or has been suspended.
        return false if the server is busy. or cannot be suspended. */
bool ARTSClient::suspend() {
    if (!server.isNull()) {
        switch (server.secondsUntilSuspend())
        {
        case 0:
                if (verbose)
                cerr << "sound server was already suspended"  << endl;
                return true;
                break;

        case -1:
                if (verbose)
                cerr << "sound server is busy"  << endl;
                return false;
                break;

        default:
                if (server.suspend() == true)
                {
                if (verbose)
                        cerr << "sound server suspended"  << endl;
                return true;
                } else {
                if (verbose)
                        cerr << "unable to suspend sound server" << endl;
                return false;
                }
        }
        return true;
        }

        return false;
}
/*
void ARTSClient::setVolume(float volume) {
        if (!server.isNull())
                server.outVolume().scaleFactor(fabsf(volume));
}
float ARTSClient::getVolume() {
        if (!server.isNull())
              return server.outVolume().scaleFactor();
        else
              return -1;
}
// set the output volume in dB
void ARTSClient::setDecibelVolume(float volume)
{
    setVolume(pow( 10, volume/20 ) );
}

// get the output volume in dB
float ARTSClient::getDecibelVolume()
{
    return 20*log10( getVolume() );
}
float ARTSClient::volumeUp(float delta) {
    if (!server.isNull()) {
        float vol = getVolume();
        float del = fabsf(delta);
        if (verbose) cout << "Volume is: " << vol << " setting vol to: " << vol << " + " << del << " = " << (vol + del) << endl;
        setVolume((vol + del));
        vol = getVolume();
        return (vol);
    } else
        return -1;
}
float ARTSClient::volumeDown(float delta) {
    if (!server.isNull()) {

        float vol = getVolume();
        float del = fabsf(delta);
	float ret = vol - del;
        if (verbose) cout << "Volume is: " << vol << " setting vol to: " << vol << " - " << del << " = " << ret << endl;
	if (ret < 0.01) ret = 0.01;
        setVolume(ret);
        vol = getVolume();
        return (vol);
    } else
        return -1;
}
float ARTSClient::mute(float value) {
    static float old_volume;
    if (!server.isNull()) {

        if (muted) {
                setVolume(old_volume);
                muted = false;
                return (old_volume);
        }
        else {
                old_volume = getVolume();
                setVolume(fabsf(value));
                muted = true;
                // xosd needs a value to show that it's muted.
                #ifdef HAVE_XOSD
                return xosdCtrl::MUTE;
                #else
		return -1;
                #endif
        }
    }
    else
        return -1;
}
*/

