/*
    This file is part of KOrganizer.
    Copyright (c) 2001 Cornelius Schumacher <schumacher@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    As a special exception, permission is given to link this program
    with any edition of Qt, and distribute the resulting executable,
    without including the source code for Qt in the source distribution.
*/

// $Id: klprefs.cpp,v 1.3 2006/03/14 16:56:32 sheldonl Exp $

#include <time.h>
#include <unistd.h>

#include <qdir.h>
#include <qstring.h>
#include <qfont.h>
#include <qcolor.h>
#include <qstringlist.h>

#include <kglobal.h>
#include <kconfig.h>
#include <klocale.h>
#include <kdebug.h>
#include <kemailsettings.h>
#include <kstaticdeleter.h>
//#include "lineakparser.h"
#include <lineak/definitions.h>

#include "klprefs.h"
#define CONTRIB  i18n("lineak-developers@lists.sourceforge.net")

KLPrefs *KLPrefs::mInstance = 0;
static KStaticDeleter<KLPrefs> insd_p;

KLPrefs::KLPrefs() : KPrefs("klineakconfigrc") {
  KPrefs::setCurrentGroup("General");
  addItemBool("First Run",&m1Run,true);
  addItemBool("Run On Startup",&mRunOnStartup,true);

  addItemString("Contribution Email",&mContribEmail, CONTRIB);

  KPrefs::setCurrentGroup("Personal Settings");
  addItemString("User Config File",&mKlineakConfFile, QDir::homeDirPath() + CONFFILE );
  addItemString("User Definition File",&mKlineakDefFile,QString("%1%2%3").arg(QDir::homeDirPath()).arg(LINEAKDIR).arg(DEFFILE));
  addItemBool("Keyboard LEDs",&mLEDs,false);
  addItemInt("Mail Client",&mMailClient,MailClientKMail);
  addItemBool("Use Control Center Email",&mEmailControlCenter,true);

 KPrefs::setCurrentGroup("Notification Messages");  
  addItemString("Lineakd At Exit", &mLineakAtExit, "No");
  addItemString("Kkeyled At Exit", &mKkeyledAtExit, "Yes");
}

KLPrefs::~KLPrefs() {
  //kdDebug() << "KLPrefs::~KLPrefs()" << endl;
  //if (mInstance == this)
      //mInstance = insd_p.setObject(0;
}

KLPrefs *KLPrefs::instance() {
  if (!mInstance) {
      mInstance = insd_p.setObject(mInstance, new KLPrefs());
      mInstance->readConfig();
  }

  return mInstance;
}

void KLPrefs::usrSetDefaults()
{
  // Default should be set a bit smarter, respecting username and locale
  // settings for example.
  fillMailDefaults();
  mLineakAtExit="No";
  mContribEmail = CONTRIB;
  mKkeyledAtExit="Yes";
//  mRunOnStartup=true;
//  mLEDs=false;
//  KMessageBox::enableAllMessages();

}

void KLPrefs::fillMailDefaults() {
  mName = fullName(); 
  mEmail = email();
  mContribEmail = CONTRIB;
}

void KLPrefs::usrReadConfig()
{
  config()->setGroup("Personal Settings");
  mName = config()->readEntry("User Name","");
  mEmail = config()->readEntry("User Email","");
  fillMailDefaults();
  config()->setGroup("Notification Messages");
  mLineakAtExit=config()->readEntry("Lineakd At Exit");
  mKkeyledAtExit=config()->readEntry("Kkeyled At Exit");
}


void KLPrefs::usrWriteConfig()
{
  config()->setGroup("General");
  config()->writeEntry("Contribution Email",mContribEmail);
  // write personal settings stuff
  config()->setGroup("Personal Settings");
  config()->writeEntry("User Name",mName);
  config()->writeEntry("User Email",mEmail);
  // write nofitication messages
  config()->setGroup("Notification Messages");
  config()->writeEntry("Lineakd At Exit",mLineakAtExit);
  config()->writeEntry("Kkeyled At Exit",mKkeyledAtExit);
}

void KLPrefs::setFullName(const QString &name)
{
  mName = name;
}

void KLPrefs::setEmail(const QString &email)
{
  mEmail = email;
}

QString KLPrefs::fullName()
{
  if (mEmailControlCenter) {
    KEMailSettings settings;
    return settings.getSetting(KEMailSettings::RealName);
  } else {
  	if (mName.isEmpty()) mName = i18n("Anonymous");
    return mName;
  }
}

QString KLPrefs::email()
{
  if (mEmailControlCenter) {
    KEMailSettings settings;
    return settings.getSetting(KEMailSettings::EmailAddress);
  } else {
  	if (mEmail.isEmpty()) mEmail = i18n("nobody@localhost.localdomain");
    return mEmail;
  }
}
