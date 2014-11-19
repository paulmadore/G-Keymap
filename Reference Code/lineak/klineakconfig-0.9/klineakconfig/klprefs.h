/*
    This file is part of KOrganizer.
    Copyright (c) 2000,2001 Cornelius Schumacher <schumacher@kde.org>

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
#ifndef _KLPREFS_H
#define _KLPREFS_H

#include <qdict.h>

#include "kprefs.h"

class KConfig;
class QFont;
class QColor;
class QStringList;

class KLPrefs : public KPrefs
{
  public:
    enum { MailClientKMail, MailClientSendmail };
    enum { Yes, No };
    
    virtual ~KLPrefs();
  
    /** Get instance of KOPrefs. It is made sure that there is only one instance. */
    static KLPrefs *instance();
    /** Set preferences to default values */
    void usrSetDefaults();
    /** Read preferences from config file */
    void usrReadConfig();
    /** Write preferences to config file */
    void usrWriteConfig();

  protected:
    /** Fill empty mail fields with default values. */
    void fillMailDefaults();

  private:
    /** Constructor disabled for public. Use instance() to create a KLPrefs object. */
    KLPrefs();
    static KLPrefs *mInstance;

  public:
    // preferences data
    void setFullName(const QString &);
    QString fullName();
    void setEmail(const QString &);
    QString email();

    bool    mEmailControlCenter;
    bool    mLEDs;
    bool    m1Run;
    bool    mRunOnStartup;
    int     mDefaultFormat;
    int     mMailClient;

    QString mLineakAtExit;
    QString mKkeyledAtExit;
    QString mContribEmail;
    QString mKlineakConfFile;
    QString mKlineakDefFile;

  private:
    QString mName;
    QString mEmail;
};

#endif
