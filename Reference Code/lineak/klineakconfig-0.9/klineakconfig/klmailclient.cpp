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
#include "klprefs.h"
#include "klmailclient.h"

using namespace std;

KLMailClient::KLMailClient(){
}
KLMailClient::~KLMailClient(){
}
// Interface to send an email.
bool KLMailClient::mailTo(const QString &from, const QString &to, const QString &subject,const KURL &attachment) {
  int bcc = false;
  QString body = createBody();
  kdDebug () << "KLMailClient::mailTo " << to << endl;
  return send(from,to,subject,body,bcc,attachment);
}
// Private method to send the email.
bool KLMailClient::send(const QString &from,const QString &to, const QString &subject,const QString &body,bool bcc, const KURL &attachment) {
  kdDebug() << "KLMailClient::sendMail():\nFrom: " << from << "\nTo: " << to << "\nSubject: " << subject << "\nBody: \n" << body
            			<< "\nAttachment:\n" << attachment.path() << endl;

  if (KLPrefs::instance()->mMailClient == KLPrefs::MailClientSendmail) {
  	bool needHeaders = true;

    QString command = KStandardDirs::findExe(QString::fromLatin1("sendmail"),QString::fromLatin1("/sbin:/usr/sbin:/usr/lib"));
    if (!command.isNull())
    	command += QString::fromLatin1(" -oi -t");
    else {
      command = KStandardDirs::findExe(QString::fromLatin1("mail"));
      if (command.isNull()) return false; // give up

      command.append(QString::fromLatin1(" -s \x22"));
      command.append(subject);
      command.append(QString::fromLatin1("\x22"));

      if (bcc) {
        command.append(QString::fromLatin1(" -b "));
        command.append(from);
      }

      command.append(" ");
      command.append(to);

      needHeaders = false;
    }

    FILE * fd = popen(command.local8Bit(),"w");
    if (!fd)
    {
      kdError() << "Unable to open a pipe to " << command << endl;
      return false;
    }

    QString textComplete;
    if (needHeaders)
    {
      textComplete += QString::fromLatin1("From: ") + from + '\n';
      textComplete += QString::fromLatin1("To: ") + to + '\n';
      if (bcc) textComplete += QString::fromLatin1("Bcc: ") + from + '\n';
      textComplete += QString::fromLatin1("Subject: ") + subject + '\n';
      textComplete += QString::fromLatin1("X-Mailer: KlineakConfig") + VERSION + '\n';
    }
    textComplete += '\n'; // end of headers
    textComplete += body;
    textComplete += '\n';
    textComplete += attachment.path();

    fwrite(textComplete.local8Bit(),textComplete.length(),1,fd);
    pclose(fd);
  } else {
    if (!kapp->dcopClient()->isApplicationRegistered("kmail")) {
        if (KApplication::startServiceByDesktopName("kmail")) {
        	KMessageBox::error(0,i18n("No running instance of KMail found."));
        	return false;
        }
    }

    if (attachment.path().isEmpty()) {
//    	if (!kMailOpenComposer(to,"",from,subject,body,0,attachment))
		cerr << "Cannot attach keyboard definition file." << endl;
     		return false;
    } else {
      	if (!kMailOpenComposer(to,"",from,subject,body,0,"lineakkb.def","7bit",attachment.path().utf8(),"text","config","method","publish", "attachment"))
       		return false;
    }
  }
  return true;
}
//// Send the def file as body of the message.
//int KLMailClient::kMailOpenComposer(const QString& arg0,const QString& arg1, const QString& arg2,const QString& arg3,const QString& arg4,int arg5, const KURL& arg6) {
//  int result = 0;
//
//   QString attfn = QString(arg6.path().utf8());
//   QFile attfile;
//    attfile.setName(attfn);
//    if (attfile.open(IO_ReadOnly) == false)
//    	cout << "Could not open file: " << attfile.name();
//  QByteArray data, replyData;
//  QCString replyType;
//  QDataStream arg( data, IO_WriteOnly );
//  arg << arg0; // to
//  arg << arg1;  // cc
//  arg << arg2; // bcc
//  arg << arg3; //subject
//  arg << arg4; //body
//  arg << arg5; // hidden=0
//  arg << attfile.readAll(); // KURL messageFile
//  cout << "In kMailOpenComposer version 1: arg6=" << arg6.path().utf8() << endl;
//  cout << "In kMailOpenComposer version 1: data=" << data << endl;
//  if (kapp->dcopClient()->call("kmail","KMailIface","openComposer(QString,QString,QString,QString,QString,int,KURL)", data, replyType, replyData ) ) {
//    if ( replyType == "int" ) {
//      QDataStream _reply_stream( replyData, IO_ReadOnly );
//      _reply_stream >> result;
//    } else {
//      kdDebug() << "kMailOpenComposer() call failed." << endl;
//    }
//  } else {
//    kdDebug() << "kMailOpenComposer() call failed." << endl;
//  }
//  return result;
//}
// Create an email and add the def file as an attachment.
int KLMailClient::kMailOpenComposer( const QString& arg0, const QString& arg1, const QString& arg2, const QString& arg3,
                                     const QString& arg4, int arg5, const QString& arg6, const QCString& arg7, const QCString& arg8,
                                     const QCString& arg9, const QCString& arg10, const QCString& arg11, const QString& arg12, const QCString& arg13 ) {

    int result = 0;
    QString attfn = QString(arg8);
    QFile attfile;
    attfile.setName(attfn);
    if (attfile.open(IO_ReadOnly) == false)
    	cout << "Could not open file: " << attfile.name();
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
    if ( kapp->dcopClient()->call("kmail","KMailIface","openComposer(QString,QString,QString,QString,QString,int,QString,QCString,QCString,QCString,QCString,QCString,QString,QCString)", data, replyType, replyData ) ) {
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


QString KLMailClient::createBody()
{
  QString body = "Lineak Keyboard Description file attached.";
  
  return body;
}



