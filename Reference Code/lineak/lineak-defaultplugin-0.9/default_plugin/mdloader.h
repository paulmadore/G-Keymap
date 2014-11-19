/***************************************************************************
                          mdloader.h  -  description
                             -------------------
    begin                : Fri Oct 22 2004
    copyright            : (C) 2004 by Sheldon Lee Wen
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

#ifndef MDLOADER_H
#   define MDLOADER_H

#   ifdef HAVE_CONFIG_H
#      include <config.h>
#   endif

#   include <lineak/loader.h>
#   include <lineak/configdirectives.h>
#   include <map>
#   include <string>
#   include <iostream>
#   include <vector>

#define TYPE "TYPE"
#define TITLE "TITLE"
#define PROGRAM "PROGRAM"
#define CONTROLLER "CONTROLLER"
#define NAME "NAME"
#define PLAY "PLAY"
#define STOP "STOP"
#define PAUSE "PAUSE"
#define PLAYPAUSE "PLAYPAUSE"
#define NEXT "NEXT"
#define PREVIOUS "PREVIOUS"
#define PRIORITY "PRIORITY"
#define CLASS "CLASS"

// Valid Types:
#define MACRO "MACRO"
#define PROGRAM "PROGRAM"
#define DCOP "DCOP"

// Valid Classes:
#define MEDIA_PLAYER "MEDIA_PLAYER"
#define WEB_BROWSER "WEB_BROWSER"

using namespace std;

class ConfigDirectives;

/**Loads keyboard definition files.
  *@author Sheldon Lee Wen
  */

class MDLoader:public Loader
{
 public:
   MDLoader ();
   MDLoader (string filename);
   virtual ~ MDLoader ();

   /** Load a keyboard definition file */
   map < string, ConfigDirectives* >* loadDef ();
   /** Load the config for the file filename */
   map < string, ConfigDirectives* >* loadDef (string filename);
 private:
   vector < string > *processMulti (vector < string > *rawData);
   /** Return a keyboard definition object. */
   map < string, ConfigDirectives* >*getDefObj (vector < string > *results);

};

#endif
