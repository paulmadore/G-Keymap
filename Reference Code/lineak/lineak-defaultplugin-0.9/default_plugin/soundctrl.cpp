/***************************************************************************
                          soundctrl.cpp  -  description
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

#include "soundctrl.h"
#include <lineak/definitions.h>
#include <string>
#include <iostream>
#include <iomanip>

/* we might need this .. oss emulation under BSD */
#if defined (__NetBSD__) || defined (__OpenBSD__)
#define SOUND_IOCTL(a,b,c)      _oss_ioctl(a,b,c)
#else
#define SOUND_IOCTL(a,b,c)      ioctl(a,b,c)
#endif

#if defined (__FreeBSD__)
# include <sys/soundcard.h>
#else
# if defined (__NetBSD__) || defined (__OpenBSD__)
#  include <soundcard.h>          /* OSS emulation */
#  undef ioctl
# else
/* BSDI, Linux, Solaris */
#  include <sys/soundcard.h>
# endif
#endif

extern "C" {
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
}

using namespace std;
#include <lineak/lineak_core_functions.h>
using namespace lineak_core_functions;
extern bool verbose;


soundCtrl::soundCtrl(string mixdev, int itype){
	ismuted=false;
        mixer = mixdev;
	type = itype;
	init();
}
soundCtrl::soundCtrl(){
	ismuted=false;
}
soundCtrl::~soundCtrl(){
}
void soundCtrl::setType(int itype) {
   if (itype == USE_MIXER )
	   type = USE_MIXER;
   if (itype == USE_PCM )
	   type = USE_PCM;
   if (itype == READ_MIXER_WRITE_ALL)
	   type = READ_MIXER_WRITE_ALL;
   if (itype == READ_PCM_WRITE_ALL)
	   type = READ_PCM_WRITE_ALL;
}
/** Read property of string mixer. */
const string& soundCtrl::getMixer(){
	return mixer;
}
/** Write property of string mixer. */
void soundCtrl::setMixer( const string& _newVal){
	mixer = _newVal;
	init();
}
int soundCtrl::read_device(int fp, int* vol) {
   int ret = 0;
   if ( type == USE_MIXER  || type == READ_MIXER_WRITE_ALL ) {
      ret = SOUND_IOCTL(fp, SOUND_MIXER_READ_VOLUME, vol) ;
   }
   if ( type == USE_PCM || type == READ_PCM_WRITE_ALL ) {
      ret = SOUND_IOCTL(fp, SOUND_MIXER_READ_PCM, vol) ;
   }
   return ret;
}
int soundCtrl::write_device(int fp, int* vol) {
   int ret = 0;
   if ( type == USE_MIXER ) {
      ret = SOUND_IOCTL(fp, SOUND_MIXER_WRITE_VOLUME, vol) ;
   }
   if ( type == USE_PCM ) {
      ret = SOUND_IOCTL(fp, SOUND_MIXER_WRITE_PCM, vol) ;
   }
   if ( type == READ_PCM_WRITE_ALL || type == READ_MIXER_WRITE_ALL ) {
      int iret = 0;
      int jret = 0;
      iret = SOUND_IOCTL(fp, SOUND_MIXER_WRITE_VOLUME, vol);
      jret = SOUND_IOCTL(fp, SOUND_MIXER_WRITE_PCM, vol);
      if (iret == -1 || jret == -1)
	      ret = -1;
      if ( iret == 0 && jret == 0)
	      ret = 0;
      else
	      //ret = (iret+jret)/2;
	      ret = iret;
   }
   return ret;
}
//<< REVISIT
int soundCtrl::toggleMute(int value){
	int fp, left, right, retval = 0;
  	/* open the mixer device */
	if ( (fp = open( mixer.c_str(), O_RDWR|O_NONBLOCK )) != -1 ) {
	   if (!ismuted) { /* mute */
  		/* Store the old volume level */
		//if ( SOUND_IOCTL(fp, SOUND_MIXER_READ_VOLUME, &old_mastervol) != -1) {
		if ( read_device(fp,&old_mastervol) != -1) {
      			msg("... old master volume stored");
                        left = (value & 0xFF);
                        right = ((value >> 8) & 0xFF);
           	        left = (left>MAX_VOLUME) ? MAX_VOLUME : ((left<0) ? 0 : left);
                        right = (right>MAX_VOLUME) ? MAX_VOLUME : ((right<0) ? 0 : right);
                        value = left + (right << 8);
  			/* Mute the volume */
                        //if (SOUND_IOCTL(fp, SOUND_MIXER_WRITE_VOLUME, &value) != -1) {
                        if ( write_device(fp, &value) != -1) {
               		   if (verbose) cout << "... master volume muted to " << value << endl;
			   ismuted = true;
		 	   retval = -1;
              		} else {
		 	   cerr << "... oops! unable to mute the master volume" << endl;
		 	   retval = -2;
              		}
	   	} else {
          		cerr << "... oops! unable to read the volume of " << mixer << endl;
	      		retval = -2;
          	}
	    } else { /* un-mute */
               //if (SOUND_IOCTL(fp, SOUND_MIXER_WRITE_VOLUME, &old_mastervol) != -1) {
               if (write_device(fp, &old_mastervol) != -1) {
        	msg("... master volume restored");
	      	ismuted = false;
	      	retval = old_mastervol;
               } else {
           	cerr << "... oops! unable to restore the master volume" << endl;
	      	retval = -2;
               }
	    }
     	    close(fp);
 	} else {
    	   cerr << "... oops! unable to open the mixer device " << mixer << endl;
    	   retval = -2;
        }
//  cout << "Resulting volume is: " << retval << endl;
  return retval;
}
int soundCtrl::volumeUp(int value){
	return adjustVolume(value);
}
int soundCtrl::volumeDown(int value){
	return adjustVolume(value);
}
int soundCtrl::getVolume() {
	int fp;
  	int retval=0;
  	/* open the mixer device */
  	if ( (fp = open( mixer.c_str(), O_RDONLY|O_NONBLOCK )) != -1 ) {
		//if ( SOUND_IOCTL(fp, SOUND_MIXER_READ_VOLUME, &master_vol) != -1) {
		if ( read_device(fp, &master_vol) != -1) {
                   retval = master_vol;
        	} else
           	   cerr << "... oops! unable to read the volume of " << mixer << endl;
     		close(fp);
 	} else {
    	cerr << "... oops! unable to open the mixer device " << mixer << endl;
    	retval = -2;
  	}
  	return retval;
}
int soundCtrl::setVolume(int value) {
	int fp, left, right,vol;
  	int retval=0;
  	/* open the mixer device */
  	if ( (fp = open( mixer.c_str(), O_RDWR|O_NONBLOCK )) != -1 ) {
        	if (ismuted) {
	   		msg("... but we're muted");
	   		close(fp);
	   		return -1;
		}
	   	/* adjust the master volume */
                //cout << "master_vol = " << master_vol << endl;
                left = (value & 0xFF);
                //cout << "left = " << left << endl;
           	right = ((value >> 8) & 0xFF);
                //cout << "right = " << right << endl;

           	left = (left>MAX_VOLUME) ? MAX_VOLUME : ((left<0) ? 0 : left);
           	right = (right>MAX_VOLUME) ? MAX_VOLUME : ((right<0) ? 0 : right);
           	vol = left + (right << 8);
                //cout << "final master = " << master_vol << endl;

                //if (SOUND_IOCTL(fp, SOUND_MIXER_WRITE_VOLUME, &vol) != -1) {
                if (write_device(fp, &vol) != -1) {
	      		retval = vol;
              		msg("... volume adjusted");
           	} else {
              		retval = -2;
	      		error("... oops! unable to adjust the master volume");
           	}
                close(fp);
 	} else {
    	   cerr << "... oops! unable to open the mixer device " << mixer << endl;
    	   retval = -2;
  	}
  	return retval;
}

/** Adjust the master volume. */
int soundCtrl::adjustVolume(int value){
	int fp, left, right;
  	int retval=0;
  	/* open the mixer device */
  	if ( (fp = open( mixer.c_str(), O_RDWR|O_NONBLOCK )) != -1 ) {
        	if (ismuted) {
	  		msg("... but we're muted");
	   		close(fp);
	   		return -1;
		}
		//if ( SOUND_IOCTL(fp, SOUND_MIXER_READ_VOLUME, &master_vol) != -1) {
		if ( read_device(fp, &master_vol) != -1) {
	   	   /* adjust the master volume */
                   //cout << "master_vol = " << master_vol << endl;
                   left = (master_vol & 0xFF) + value;
                   //cout << "left = " << left << endl;
           	   right = ((master_vol >> 8) & 0xFF) + value;
                   //cout << "right = " << right << endl;

           	   left = (left>MAX_VOLUME) ? MAX_VOLUME : ((left<0) ? 0 : left);
           	   right = (right>MAX_VOLUME) ? MAX_VOLUME : ((right<0) ? 0 : right);
           	   master_vol = left + (right << 8);
                   //cout << "final master = " << master_vol << endl;

                   //if (SOUND_IOCTL(fp, SOUND_MIXER_WRITE_VOLUME, &master_vol) != -1) {
                   if (write_device(fp, &master_vol) != -1) {
	      		retval = master_vol;
              		msg("... volume adjusted");
           	   } else {
              		retval = -2;
	      		cerr << "... oops! unable to adjust the master volume" << endl;
           	   }
        	} else
           		cerr << "... oops! unable to read the volume of " << mixer << endl;
     		close(fp);
 	} else {
    	   cerr << "... oops! unable to open the mixer device " << mixer << endl;
    	   retval = -2;
  	}
  	return retval;
}

void soundCtrl::init() {
  	int fp;
  	msg("Sound init, using " + mixer + " as the mixer device");
  	if ( (fp = open( mixer.c_str(), O_RDWR|O_NONBLOCK )) != -1 ) {
    	//if ( SOUND_IOCTL(fp, SOUND_MIXER_READ_VOLUME, &old_mastervol) != -1) {
    	if ( read_device(fp, &old_mastervol) != -1) {
	    		msg("... master volume stored");
		} else
      		cerr << "... oops! unable to read the volume of " << mixer << " (sound init)" << endl;
                close(fp);
  	} else
    	cerr << "... oops! unable to open the mixer device " << mixer << " (sound init)" << endl;
}
