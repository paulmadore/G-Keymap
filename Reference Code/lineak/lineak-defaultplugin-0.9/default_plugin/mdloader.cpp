/***************************************************************************
                          mdloader.cpp  -  description
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
#include <mdloader.h>
#include <lineak/lineak_util_functions.h>
#include <lineak/ldef.h>
#include <lineak/lkbd.h>
#include <lineak/lkey.h>
#include <lineak/lobject.h>
#include <lineak/lbutton.h>
#include <map>

using namespace lineak_util_functions;

MDLoader::MDLoader() {
}
MDLoader::MDLoader(string filename) : Loader(filename) {
}
MDLoader::~MDLoader(){
}
/** Process a file with sub sections */
vector<string>* MDLoader::processMulti(vector<string>* rawData) {
        if (rawData->empty()) {
    		return rawData;
	}

       vector<string> *parsedData = new vector<string>;
       string tmpStore = snull;
       string key=snull;
       string data=snull;
       for(vector<string>::size_type i = 0; i < rawData->size(); i++) {
          	tmpStore = (*rawData)[i];
            data = strip_space(tmpStore);
	    //cout << "raw data: " << data << endl;
            parsedData->push_back(data);
       }
      delete (rawData);
      return parsedData;
}
/** Load a keyboard definition file */
map<string,ConfigDirectives*>* MDLoader::loadDef() {
      return getDefObj(processMulti(loadFile()));
}
/** Load the config for the file filename */
map<string,ConfigDirectives*>* MDLoader::loadDef(string filename) {
	if ( setFile(filename) )
		return (loadDef());
	return	 NULL;
}

/** Return a ConfigDirectives object. */
/** We take the name value pairs from the map,
	We then add those pairs to a ConfigDirectives object */
map<string,ConfigDirectives*>* MDLoader::getDefObj(vector<string> *results){

	map<string,ConfigDirectives*> *deftable = new map<string,ConfigDirectives*>;
	// Initialize the table.
	deftable->clear();
         // If we don't have any values loaded. Return empty keyboard table.
        if (results == pnull )
    	   return deftable;
        if (results->empty()) {
		delete (results);
		return deftable;
	}

	ConfigDirectives *def = new ConfigDirectives();
	string tmp,tmp2,tmp3=snull, heading = snull;
 	string key=snull,data=snull;
	vector<string>::size_type loc = 0;
   for (vector<string>::size_type i=0; i < results->size(); i++) {
        tmp = (*results)[i];
	// If this line of the file has brackets, it's a beginning or end of a section.
        if ( tmp.find('[') != string::npos && tmp.find(']') != string::npos ) {
           // The Keyboard Name, i.e. it's a section heading w/o the keyword KEY,BUTTON or END
	   if ( tmp.find("END") == string::npos ) {
	      // Assign the keyboard identifier. i.e. ACEAKV12
              def->addValue(TITLE, strip_space(tmp.substr(1,tmp.size()-2)));
	      heading = TITLE;
	   }
           /** create a new keyboard object */
           // End of Keyboard Definition Marker i.e. there is an END with no KEY or BUTTON
           if ( tmp.find("END") != string::npos ) {
              heading = "END";
              /**  set heading = snull;
                   add the keyboard config object to a map<string,ConfigDirectives>
                   create a new keyboard config object to add to */
              heading = snull;
              (*deftable)[(def->getValue(TITLE))]= def;
	      //cout << "Outputing definition" << endl;
	      //cout << *def << endl;
              def = new ConfigDirectives();
           }
        }
        else {
	   // Handle empty lines.
	   loc = tmp.find('=');
           // empty line
   	   if (loc == (unsigned int)string::npos)
   	     continue;

	   // key == the name of the key.
           key = strip_space(tmp.substr(0,loc));
	   // data == the keycode of the key.
           data = strip_space(tmp.substr(loc+1, tmp.size()-(loc+1)));
	   //cout << "data = " << data << endl;
	   if ( heading == TITLE ) {
              // found a global entry
              if (loc == 0) continue;
              if ( key == PROGRAM)
                 def->addValue(PROGRAM, strip(data,"\""));
              if ( key == CONTROLLER)
                 def->addValue(CONTROLLER, strip(data,"\""));
              if ( key == NAME)
                 def->addValue(NAME, strip(data,"\""));
	      if ( key == TYPE)
                 def->addValue(TYPE, strip(data,"\""));
	      if ( key == PLAY)
                 def->addValue(PLAY, strip(data,"\""));
	      if ( key == STOP)
                 def->addValue(STOP, strip(data,"\""));
	      if ( key == PAUSE)
                 def->addValue(PAUSE, strip(data,"\""));
	      if ( key == PLAYPAUSE)
                 def->addValue(PLAYPAUSE, strip(data,"\""));
	      if ( key == NEXT)
                 def->addValue(NEXT, strip(data,"\""));
	      if ( key == PREVIOUS)
                 def->addValue(PREVIOUS, strip(data,"\""));
	      if ( key == PRIORITY)
                 def->addValue(PRIORITY, strip(data,"\""));
	      if ( key == CLASS)
		 def->addValue(CLASS, strip(data,"\""));
           }
        }
   }
   // Initialize and fill in the values for the config object here.
   delete (results);
   return (deftable);
}

