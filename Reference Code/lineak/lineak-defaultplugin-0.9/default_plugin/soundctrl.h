/***************************************************************************
                          soundctrl.h  -  description
                             -------------------
    begin                : Sat Feb 22 2003
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

#ifndef SOUNDCTRL_H
#define SOUNDCTRL_H

#include <string>
#include <lineak/definitions.h>

#define USE_MIXER 0
#define USE_PCM 1
#define READ_MIXER_WRITE_ALL 10
#define READ_PCM_WRITE_ALL 11

using namespace std;
/**Initialize, and modify the volume levels.
  *@author Sheldon Lee Wen
  */

class soundCtrl {
public: 
	soundCtrl(string mixdev, int itype = USE_MIXER);
	soundCtrl();
	~soundCtrl();
  	/** Write property of string mixer. */
  	void setMixer( const string& _newVal);
  	/** Read property of string mixer. */
  	const string& getMixer();
   	/** Toggle the mute */
        int toggleMute(int value = VOLMUTE_VALUE);
        int volumeUp(int value = VOLUP_VALUE );
        int volumeDown(int value = VOLDOWN_VALUE);
        int setVolume(int value);
        int getVolume();
	void setType(int itype);
private: // Private attributes
        /** Initialize the mixer */
	void init();
  	/** Adjust the master volume. */
  	int adjustVolume(int value);
	/** Read the device according to type. */
	int read_device(int fp, int* vol);
	/** Write to the device according to type. */
	int write_device(int fp, int* vol);
  	/** Old value of the master volume. */
  	int old_mastervol;
  	/** Currentl master volume level. */
  	int master_vol;
  	/** Name of the mixer device. */
  	string mixer;
  	/** State of muted */
  	bool ismuted;
	/** Track which device we are using */
	int type;
};

#endif
