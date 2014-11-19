/***************************************************************************
                          klineak.h  -  description
                             -------------------
    begin                : Mon Jun 3 2002
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

#ifndef KLINEAK_H
#define KLINEAK_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <qwidget.h>
#include <ksystemtray.h>
#include <kstdaction.h>
#include <kaction.h>
#include <qpixmap.h>

// kde include
#include <klocale.h>
#include <kapp.h>
#include <kaccel.h>
#include <kglobal.h>
#include <kstdaction.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <knotifyclient.h>
#include <klineeditdlg.h>
#include <kstatusbar.h>
#include <kpopupmenu.h>
#include <krun.h>
#include <kwin.h>

// qt include
#include <qtooltip.h>

#if ( QT_VERSION < 0x030000 )
#  include <qlist.h>
#else
#  include <qptrlist.h>
#endif

#include <qlineedit.h>
#include <qinputdialog.h>
#include <qlabel.h>

//#include "advancedkbd.h"
//class advancedKbd;

/**System tray class
  *@author Sheldon Lee Wen
  */

class Klineak : public KSystemTray  {
   Q_OBJECT
public: 
	Klineak(QWidget *parent=0, const char *name=0);
	~Klineak();

   QPixmap klineakconfigPixmap;
   KPopupMenu *menu;
   KAction *fileQuit;
   KAction *lineakdRestart;
   KAction *lineakdStop;
   KAction *lineakdStart;
   KAction *lineakDef;
   KAction *lineakPrefs;
   KActionCollection *actionCollection;
   void mousePressEvent(QMouseEvent *ev);

signals:
    virtual void toggleApp();
    virtual void cleanExit();
    virtual void stop();
    virtual void start();
    virtual void restart();
    virtual void stopKkeyleds();

public slots:
    virtual void restartLineakd();
    virtual void startLineakd();
    virtual void stopLineakd();
    virtual void showPreferences();

//private:
    //advancedKbd *preferencesui;
    
public slots: // Public slots
  /** User clicked the quit button */
  virtual void slotExit();
};

#endif

