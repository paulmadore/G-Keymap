/***************************************************************************
                          klmailclient.cpp  -  description
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
#include <unistd.h>
#include <stdio.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <klocale.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <qfile.h>
#include <kmessagebox.h>
#include <kurl.h>
#include <kapplication.h>
#include <dcopclient.h>
#include <iostream>
#include "klmailclient.h"

using namespace std;
extern bool verbose;

KLMailClient::KLMailClient(DCOPClient *idcop){
	dcop = idcop;
	running = false;
	running = isRunning();
}
KLMailClient::~KLMailClient(){
}
bool KLMailClient::isRunning() {
   if (!running) {
      if (!dcop->isApplicationRegistered("kmail"))
         return false;
      else {
         running = true;
	 return true;
      }
   }
   return true;
}
bool KLMailClient::startKmail() {
   KApplication::startServiceByDesktopName("kmail");
   return (dcop->isApplicationRegistered("kmail"));
}
// Interface to send an email.
bool KLMailClient::mailTo(const QString &from, const QString &to, const QString &cc, const QString &subject,const KURL &attachment) {
   if (!isRunning())
      if (!startKmail()) {
         if (verbose) cerr << "Kmail could not be started." << endl;
	 return false;
      }

/*	if (!dcop->isApplicationRegistered("kmail")) {
            if (KApplication::startServiceByDesktopName("kmail")) {
               cout << i18n(QString("No running instance of kmail found."));
            } else
	    	return false;
        } */

  int bcc = false;
  QString body = "";
  if (verbose) cout << "KLMailClient::mailTo " << to << endl;
  return send(from,to,cc,subject,body,bcc,attachment);
}
// Private method to send the email.
bool KLMailClient::send(const QString &from,const QString &to, const QString &cc, const QString &subject,const QString &body,bool bcc, const KURL &attachment) {

    if (verbose) cout << "KLMailClient::sendMail():\nFrom: " << from << "\nTo: " << to << "\nSubject: " << subject << "\nBody: \n" << body
            			<< "\nAttachment:\n" << attachment.path() << endl;
    if (!isRunning())
       if (!startKmail()) {
           if (verbose) cerr << "Kmail could not be started." << endl;
           return false;
       }

/*    if (!dcop->isApplicationRegistered("kmail")) {
        if (KApplication::startServiceByDesktopName("kmail")) {
       	   cout << i18n("No running instance of KMail found.");
       	   return false;
        }
    } */

    if (!kMailOpenComposer(to,cc,from,subject,body,bcc,"","7bit",attachment.path().utf8(),"text","config","method","publish", "attachment"))
       	return false;

  return true;
}
int KLMailClient::kMailOpenComposer( const QString& arg0, const QString& arg1, const QString& arg2, const QString& arg3,
                                     const QString& arg4, int arg5, const QString& arg6, const QCString& arg7, const QCString& arg8,
                                     const QCString& arg9, const QCString& arg10, const QCString& arg11, const QString& arg12, const QCString& arg13 ) {

    int result = 0;
    QString attfn = QString(arg8);
    QFile attfile;
    attfile.setName(attfn);
    if (attfile.open(IO_ReadOnly) == false)
    	cerr << "Could not open file: " << attfile.name();
    QByteArray data, replyData;
    QCString replyType;
    QDataStream arg( data, IO_WriteOnly );
    arg << arg0; //to
    arg << arg1; // cc
    arg << arg2; // from (bcc)
    arg << arg3; //subject
    arg << arg4; //body
    arg << arg5; // hidden=0
    arg << arg6; // attachName
    arg << arg7; // "7bit" (attachCte)
    arg << attfile.readAll(); //attachment (attachData)
    arg << arg9; // "text" (attachType)
    arg << arg10; // "calendar" (attachSubType)
    arg << arg11; // "method" (attachParamAttr)
    arg << arg12; // "publish" (attachParamValue)
    arg << arg13; // "attachment" (attachContDisp)
    if ( dcop->call("kmail","KMailIface","openComposer(QString,QString,QString,QString,QString,int,QString,QCString,QCString,QCString,QCString,QCString,QString,QCString)", data, replyType, replyData ) ) {
        if ( replyType == "int" ) {
            QDataStream _reply_stream( replyData, IO_ReadOnly );
            _reply_stream >> result;
        } else {
            kdDebug() << "kMailOpenComposer() call failed." << endl;
        }
    } else {
        kdDebug() << "kMailOpenComposer() call failed." << endl;
    }
    attfile.close();
    return result;
}
