/***************************************************************************
                          klineakdef.h  -  description
                             -------------------
    begin                : Sun Jun 9 2002
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

#ifndef KLINEAKDEF_H
#define KLINEAKDEF_H

#include <qwidget.h>
#include <qframe.h>
#include <qlistbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qbutton.h>
#include <qimage.h>
#include <qmessagebox.h>
#include <qlcdnumber.h>
#include <qstringlist.h>
#include <kfiledialog.h>
#include <kurl.h>
#include <kimageio.h>
#include <kdebug.h>
#include <kio/job.h>
#include <kimageio.h>
#include <ktempfile.h>
#include "klineakdefui.h"
#include <lineak/definitions.h>
#include <lineak/lkbd.h>

extern "C" {
  #include <sys/types.h>
  #include <dirent.h>
  #include <signal.h>
  #include <X11/Xlib.h>
  #include <X11/XKBlib.h>
  #include <X11/Xutil.h>
  #include <X11/extensions/XKBfile.h>
}


/**
  *@author Sheldon Lee Wen
  */
  
class KlineakDef : public KlineakDefUI  {
   Q_OBJECT
public: 
   KlineakDef(QWidget *parent=0, const char *name=0);
   ~KlineakDef();
   bool x11Event( XEvent* event);
   QString hashKbIdent();

private:
   int lastKeyCode;
   QString currentKeyName;
   LKbd myKeyboard;
   //EAKeyboard myKeyboard;
   //KURL picture;
   bool isEAK(XKeyEvent *e);
   /** map for the kbtype to the full name */
   map<QString,int> keys2codes;

public slots: // Public slots
  /** Triggered when the user hits the Apply button */
  virtual void onApplyButtonClicked();
  /** Triggered when the user presses the "Close" button */
  virtual void onQuitButtonClicked();
  /** Triggered when the user hits the Clear Key button */
  virtual void onClearKeyButtonClicked();
  /** Triggered when the user hits the Clear All button */
  virtual void onClearAllButtonClicked();
  /** Triggered when the user hits the "Submit my Keyboard" button */
  virtual void onSubmitButtonClicked();
  /** Triggered when the user hits the "Choose Picture" button */
  //virtual void onPictureButtonClicked();
  /** Update the model text of our keyboard. */
  virtual void updateModel(const QString &model);
  /** Update the name of the current key */
  virtual void updateKeyname(const QString &keyname);
  /** Update the brand for our keyboard */
  virtual void updateBrand(const QString &brand);
  /** Update the information for the selected key */
  virtual void updateKeyinfo(const QString &key);
  /** Write out the definition file */
  virtual QString saveDefFile() ;


signals: // Signals
  /** Emitted when the user clicks the Apply button. */
  void applying(QString);
  /** Emitted when the user clicks Quit. */
  void quitting();
  /** Emitted when the user sets the picture for the keyboard */
  void setPicture(KURL);
  /** Emitted to relay the hashed name of the keyboard so that lineakconfig
  			can set it as the default */
  void keyboardName(QString);
public slots: // Public slots
  /** Parse the user defined keyboard file if it exists and load it into the gui. */
  void parseUserDef();
};

#endif
