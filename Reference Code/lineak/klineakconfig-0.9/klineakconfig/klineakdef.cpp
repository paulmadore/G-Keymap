/***************************************************************************
                          klineakdef.cpp  -  description
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
// Leave these in this order !!!!!
#include <klocale.h>
#include "klprefs.h"

#include "klineakdef.h"
#include "klmailclient.h"
#include <qpushbutton.h>
#include <qstring.h>
#include <iostream>
#include <lineak/lkbd.h>
#include <lineak/saver.h>
#include <lineak/ldef.h>
#include <lineak/lkey.h>
#include <lineak/defloader.h>


extern "C" {
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/extensions/XKBfile.h>
#include <sys/types.h>
#include <unistd.h>
}


KlineakDef::KlineakDef(QWidget *parent, const char *name ) : KlineakDefUI(parent,name) {
  lcdKeycode->setMode(QLCDNumber::Dec);
  lcdKeycode->setNumDigits(3);
//  btnPicture->setEnabled(false);
  btnSubmit->setEnabled(false);
  btnApply->setEnabled(false);
  //cout << "parsing user def" << endl;
  parseUserDef();
  //cout << "done parsing user def" << endl;
}
KlineakDef::~KlineakDef(){
}
bool KlineakDef::x11Event( XEvent* event) {
   XkbEvent xev;
   xev.core = *event;
   
   if ( this->isVisible() ) {
    if (event->type == KeyPress) {
      XKeyEvent *e = (XKeyEvent *) event;
      if (isEAK(e)) {         
         QString text = currentKeyName; //txtKeyName->text();
         lastKeyCode = e->keycode;
         lcdKeycode->display(lastKeyCode);
         if ( text != "" ) {
            statusLabel->setText(i18n("Key %1 mapped to key name %2").arg(lastKeyCode).arg(txtKeyName->text()));
	    keys2codes[text] = lastKeyCode;
            if (!lstKeyList->findItem(text))
               lstKeyList->insertItem(text);
            if (myKeyboard.brand != "" && myKeyboard.model != "") {
               //btnPicture->setEnabled(true);
               if (!keys2codes.empty())  {
                  btnApply->setEnabled(true);
                  btnSubmit->setEnabled(true);
               }
            }
            txtKeyName->setText("");
         }
      }
      //else
         //cout << "isEAK() is false" << endl;
    }
   }
//    if (event->type == MappingNotify)
//      	cout << "MappingNotify Event Keymap changed\n";
   return false;
}
/** Try and determine if the key is an EAK */
bool KlineakDef::isEAK(XKeyEvent *e) {
    QString ksname, sub;
    KeySym ks;
    int nbytes;
    char str[256+1];

     nbytes = XLookupString (e, str, 256, &ks, NULL);
     if (ks == NoSymbol)
         ksname = "NoSymbol";
     else if (!(ksname = XKeysymToString (ks)))
         ksname = "(no name)";
     if (nbytes < 0) nbytes = 0;
     if (nbytes > 256) nbytes = 256;
     str[nbytes] = '\0';
     
     //cout << endl;
     //cout << "in isEAK()" << endl;
     //cout << "keysym = " << ksname << endl;
     //cout << "e->keycode = " << e->keycode << endl;
     //cout << "nbytes = " << nbytes << endl;
     //cout << "sub = " << ksname.left(3) << endl;
     //cout << endl;
       
     if (e->keycode == 0)
        return false;
     if (ksname == "NoSymbol" || ksname == "(no name)" || ksname == "" ) {
        //cout << "keycode=" << e->keycode << " keysym=" << ksname << " XLookupString gives " << nbytes << " characters: \"" << str << "\"" << endl;
        return true;
     }
     sub = ksname.left(3);
     if (nbytes == 0 && (  sub == "F13" || sub == "F14" || sub == "F15" || sub == "F16" ||
                           sub == "F17" || sub == "F18" || sub == "F19" || sub == "F20" ||
                           sub == "F21" || sub == "F22" || sub == "F23" || sub == "F24" ||
                           sub == "F25" || sub == "F26" ||  sub == "F27" || sub == "F28" ||
                           sub == "F29" || sub == "F30" || sub == "F31" || sub == "F32" )) {
        //cout << "keycode=" << e->keycode << " keysym=" << ksname << " XLookupString gives " << nbytes << " characters: \"" << str << "\"" << endl;
        return true;
     }
     /* X may know about our keyboard, so just get a decent range. */
     if (e->keycode >= 120 && e->keycode <= 255)
        return true;
	
     return false;
}

/** Triggered when the user presses the "Close" button */
void KlineakDef::onQuitButtonClicked(){
   emit quitting();
   //emit keyboardName(hashKbIdent());
   hide();
}
/** Triggered when the user hits the Apply button */
void KlineakDef::onApplyButtonClicked(){
  
  /** Applying should send back the name of the file to parse */
  emit applying(saveDefFile());	
  /** Applying sends back the name of the keyboard to set as the default. */
  emit keyboardName(myKeyboard.name);
  onQuitButtonClicked();
}
QString KlineakDef::hashKbIdent() {
  QString hash;
  QStringList hMod;

  // Set the defaults.
  if (myKeyboard.brand == "")
     myKeyboard.brand = "User Defined Brand";
  if (myKeyboard.model == "")
     myKeyboard.model = "User Defined Model";
  
  hash = "USR"; // Start user defined keyboards with USR
  hMod = getenv("USER"); // Fourth letter in the hash is the first letter of the userid
  hMod += QStringList::split(" ",QString(myKeyboard.brand.c_str())); // Next letter is the first letter of every word in the brandname.
  hMod += QStringList::split(" ",QString(myKeyboard.model.c_str())); // Next letter is the first letter of every word in the modelname.
  for ( QStringList::Iterator it = hMod.begin(); it != hMod.end(); ++it ) {
        hash+= QString(*it)[0];
  }
  //cout << "hash is : " << hash << endl;
  return hash;
}
/** Save the Def File */
QString KlineakDef::saveDefFile() {
   QFile userdeffile;
   QString userdeffilename = QString("%1%2%3").arg(getenv("HOME")).arg(LINEAKDIR).arg(DEFFILE);
   userdeffile.setName(userdeffilename);
   
   // If the hash name is not set the keyboard will not save. It will show up as empty.
   myKeyboard.name = hashKbIdent().latin1();
   
   if (userdeffile.exists() && (keys2codes.size() != 0)) {
   // show message box that we are overwriting the existing file.
   // if that's ok then continue.
   // else return QString::null;
      switch( QMessageBox::warning( this, "KLineak",
         i18n("A user defined keyboard definition file already exists.\n"
           "Do you want to replace that definition with a new one?\n"),
           i18n("Replace"),
           i18n("Don't Replace"), 0, 0, 1 ) ) {
        case 0: // The user clicked the "Replace" button or pressed Enter
           break;
        case 1: // The user clicked the "Don't Replace" or pressed Escape
           return ("");
           break;
      }
  }
  cout << "Saving deffile\n";
  
  for(map<QString,int>::iterator it = keys2codes.begin();it!=keys2codes.end(); it++) {
  
     LKey *tmp = new LKey((it->first).latin1(), it->second);
     if (tmp != NULL) {
        myKeyboard.removeObject(it->first.latin1());   
        myKeyboard.addObject(tmp);
        cout << "Added object: " << endl;
        cout << *tmp << endl;
     }
     tmp = NULL;
  }
  //cout << "Saving the LKbd" << endl;
  //cout << myKeyboard << endl;	       
  LDef def(userdeffilename.latin1());
  Saver tmp(userdeffilename.latin1());
  def.addKeyboard(myKeyboard);
  //cout << "Saving definition!" << endl;
  //cout << def << endl;
  tmp.saveFile(def);

  return userdeffile.name();

}

/** When the user clicks on a key in the key list */
void KlineakDef::updateKeyinfo(const QString &key){
  //int keycode = (myKeyboard.getObject(key.latin1())->getKeyCode();
  int keycode = keys2codes[key];
  txtKeyName->setText(key);
  lcdKeycode->display(keycode);
  currentKeyName = key;
  statusLabel->setText(i18n("Key %1 mapped to key name %2").arg(keycode).arg(key));
}
/** Update the brand for our keyboard */
void KlineakDef::updateBrand(const QString &brand){
  myKeyboard.brand = brand.latin1();
  if (myKeyboard.brand != "" && myKeyboard.model != "") {
     //btnPicture->setEnabled(true);
     if (!keys2codes.empty())
        btnApply->setEnabled(true);
  }
    	
}
/** Update the name of the current key */
void KlineakDef::updateKeyname(const QString &keyname){
  currentKeyName = keyname;
}
/** Update the model text of our keyboard. */
void KlineakDef::updateModel(const QString &model){
  myKeyboard.model = model.latin1();
  if (myKeyboard.brand != "" && myKeyboard.model != "") {
     //btnPicture->setEnabled(true);
     if (!keys2codes.empty())
        btnApply->setEnabled(true);
  }
}
/** Triggered when the user hits the Clear Key button */
void KlineakDef::onClearKeyButtonClicked(){
  //map<QString,int>::iterator it = keys2codes.find(lstKeyList->currentText());

  txtKeyName->setText("");
  lcdKeycode->display(0);
  currentKeyName = "";
  statusLabel->setText("");
  keys2codes.erase(lstKeyList->currentText());
  
  //LObject * obj = myKeyboard.getObject(lstKeyList->currentItem());
  myKeyboard.removeObject(lstKeyList->currentText().latin1());
  //if (obj->getType() == SYM || obj->getType() == CODE) {
  //   LKey *key = static_cast<LKey*>(*obj);
  //   key->clear();
  lstKeyList->removeItem(lstKeyList->currentItem());   
  
}
/** Triggered when the user hits the Clear All button */
void KlineakDef::onClearAllButtonClicked(){
  txtKeyName->setText("");
  lcdKeycode->display(0);
  currentKeyName = "";
  statusLabel->setText("");
  lstKeyList->clear();
  keys2codes.clear();
  const map<string,LObject*> &objects = myKeyboard.getObjects();
  map<string,LObject*>::iterator it = const_cast<map<string,LObject*> &>(objects).begin();
  for (; it != objects.end(); it++) {
     myKeyboard.removeObject(it->first);
  }
  
  
}
/** Triggered when the user hits the "Submit my Keyboard" button */
void KlineakDef::onSubmitButtonClicked() {
   /** Write out the config file so we can attach it. */
   onApplyButtonClicked();

   const QString dest_email = KLPrefs::instance()->mContribEmail;
   const QString source_email = KLPrefs::instance()->email();
   const QString subject = "lineakd user defined keyboard contribution.";
   const KURL attach(QString("%1%2%3").arg(getenv("HOME")).arg(LINEAKDIR).arg(DEFFILE));
   KLMailClient mail;
   mail.mailTo(source_email,dest_email,subject,attach);
}
/** Triggered when the user hits the "Choose Picture" button 
void KlineakDef::onPictureButtonClicked() {
   KURL desturl;
   KURL url = KFileDialog::getOpenURL( ":load_image", KImageIO::pattern( KImageIO::Reading ), this );
   QString destname;
   QDir lineakdir;
   QString picsdir = QString(QDir::homeDirPath()) + PICSDIR;
   lineakdir.setPath(picsdir);

   destname = lineakdir.path() + "/";
   destname += hashKbIdent() + ".";
   destname += QString(QPixmap::imageFormat (url.path()) ).lower();
   desturl.setPath(destname);

   if ( !url.isValid() )
       kdWarning( 4610 ) << "malformed URL " << url.prettyURL() << endl;
   if ( !desturl.isValid() )
       kdWarning( 4610 ) << "malformed URL " << desturl.prettyURL() << endl;

   cout << "copying " << url.path() << " to " << desturl.path() << endl;
   KIO::file_copy(url,desturl, 0644, true);
   picture = desturl;
   emit setPicture(desturl);
} */

/** Parse the user defined keyboard file if it exists and load it into the gui. */
void KlineakDef::parseUserDef(){
   
   QFile userdeffile;
   QString userdeffilename = QString("%1%2%3").arg(getenv("HOME")).arg(LINEAKDIR).arg(DEFFILE);
   userdeffile.setName(userdeffilename);
   //cout << "Parsing userdef\n";
   
   if (userdeffile.exists() && userdeffile.size() != 0) {
      LDef udef;
          
      DefLoader defldr(userdeffilename.latin1());
      udef = defldr.loadDef();
      
      if (udef.isEmpty() ) {
         cerr << "*** Error occured while loading the user keyboard definition file from ";
	 cerr << userdeffilename << endl;
      }
      /** Clear the existing map and add the complete data */
      keys2codes.clear();
      /* Fill the keys2codes map */
      QString kbtype, kbname, model;
      map<string,LKbd*> table = udef.getTable();
      map<string,LKbd*>::iterator it = table.begin();
      myKeyboard = *(it->second);
      // Update the brand and model fields.
      QString qbrand = myKeyboard.brand.c_str();
      QString qmodel = myKeyboard.model.c_str();
      txtBrand->setText(qbrand);
      updateBrand(qbrand);
      txtModel->setText(qmodel);
      updateModel(qmodel);
      
      // Get and update the keys
      QString text;
      int code = 0;
      vector<string> names = myKeyboard.getNames();
      for (vector<string>::iterator it = names.begin(); it != names.end(); it++) {
         text = QString(it->c_str());
	 code = (static_cast<LKey*>(myKeyboard.getObject(*it)))->getKeyCode();
         keys2codes[text] = code; 
         // Add the key item to the gui
         if (!lstKeyList->findItem(text))
            lstKeyList->insertItem(text);
         // Update the keyinfo
         updateKeyinfo(text);
      }
      // If keys are defined then enable the apply and submit buttons.
      if (!keys2codes.empty())  {
         btnApply->setEnabled(true);
         btnSubmit->setEnabled(true);
      }
   }
   userdeffile.close();
   
   //cout << "Parsed userdef\n";

}
