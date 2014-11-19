/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Wed May 15 17:51:34 EDT 2002
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

#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

#include "klineakconfig.h"
#include "klineakdef.h"
#include "klprefs.h"
#include <lineak/lineak_core_functions.h>

extern bool verbose;

static const char *description =
	I18N_NOOP("KlineakConfig");
// INSERT A DESCRIPTION FOR YOUR APPLICATION HERE
KlineakConfig *klineakconfig = NULL;	
	
static KCmdLineOptions options[] =
{
  { "verbose", I18N_NOOP("Show verbose output"), 0 }
  // INSERT YOUR COMMANDLINE OPTIONS HERE
};

int main(int argc, char *argv[])
{
  verbose = false;
   
  KAboutData aboutData( "klineakconfig", I18N_NOOP("KlineakConfig"),
    VERSION, description, KAboutData::License_GPL,
    "(c) 2002, Sheldon Lee Wen", 0, 0, "leewsb@hotmail.com");
  aboutData.addAuthor("Sheldon Lee Wen",0, "leewsb@hotmail.com");
  KCmdLineArgs::init( argc, argv, &aboutData );
  KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

  KApplication a;
  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
  if (args->isSet("verbose"))
     verbose = true;
  
  klineakconfig = new KlineakConfig();
  
  KlineakDef *def = new KlineakDef();
  
  a.setMainWidget(klineakconfig);
  
  a.installX11EventFilter(def);
  
//  a.installX11EventFilter(klineakconfig);
  klineakconfig->setLineakDef(def);
  
  /** Set our preferences */
  if (KLPrefs::instance()->m1Run) {
       // This must be before the m1Run line as it sets m1Run (for first run) to true
       KLPrefs::instance()->setDefaults();
       // Set first run to false.
       KLPrefs::instance()->m1Run = false;
       // Write out our changes.
       KLPrefs::instance()->writeConfig();
   } 
  
  return a.exec();
}
