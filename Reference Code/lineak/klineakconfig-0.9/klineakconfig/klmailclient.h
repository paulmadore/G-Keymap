/***************************************************************************
                          klmailclient.h  -  description
                             -------------------
    begin                : Mon Jul 22 2002
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

#ifndef KLMAILCLIENT_H
#define KLMAILCLIENT_H
#include <qstring.h>

class KURL;
/**Controls the sending of an email via a mail client.
  *@author Sheldon Lee Wen
  */

class KLMailClient {
public: 
	KLMailClient();
	virtual ~KLMailClient();

    bool mailTo(const QString &from, const QString &to, const QString &subject,const KURL &attachment);

  protected:
    /** Send mail with specified from, to and subject field and body as text. If
     * bcc is set, send a blind carbon copy to the sender from */
    bool send(const QString &from,const QString &to,const QString &subject, const QString &body,bool bcc=false, const KURL &attachment = KURL());

    QString createBody();

    int kMailOpenComposer(const QString& to, const QString& cc,
                          const QString& bcc, const QString& subject,
                          const QString& body, int hidden,
                          const QString& attachName, const QCString& attachCte,
                          const QCString& attachData,
                          const QCString& attachType,
                          const QCString& attachSubType,
                          const QCString& attachParamAttr,
                          const QString& attachParamValue,
                          const QCString& attachContDisp );
//    int kMailOpenComposer(const QString& arg0,const QString& arg1,
//                          const QString& arg2,const QString& arg3,
//                          const QString& arg4,int arg5,const KURL& arg6);

};

#endif
