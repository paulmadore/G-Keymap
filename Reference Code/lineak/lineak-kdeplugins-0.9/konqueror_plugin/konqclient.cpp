/***************************************************************************
                          konqclient.cpp  -  description
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

/** You can see the available konqueror actions by doing this in a shell:
 * dcop konqueror-<pid>  konqueror-mainwindow#1 actions */

#include <unistd.h>
#include <stdio.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <klocale.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <qfile.h>
#include <kurl.h>
#include <kapplication.h>
#include <kdatastream.h>
#include <dcopclient.h>
#include <dcopref.h>
#include <qstring.h>
#include <qstringlist.h>
#include <iostream>
#include "konqclient.h"
//#include "klineaklib.h"

using namespace std;
extern bool verbose;

KonqClient::KonqClient(DCOPClient *idcop){
	dcop = idcop;
        if (isRunning())
		if (verbose) cout << "Found a konqueror ID: " << konqid << endl;
	else
		if (verbose) cout << "Could not get a reference to a konqueror window" << endl;

        //objid = "qt/KXMLGUILClient-KActionCollection/";
        objid = "konqueror-mainwindow#1/action/";
}
KonqClient::~KonqClient(){
}
QCString KonqClient::getActive() {
   QCString objid="konqueror-mainwindow#1";
   QCString function="isActiveWindow()";
   QByteArray data;
   QCString foundObj;
   QCStringList appList;
   QCString app = "konqueror-*";
   QCString konqref;
   konqref = "";

   //Go thorough all of the konqueror objects and look for:
   //dcop konqueror-n konqueror-mainwindow#1 isActiveWindow  to be true.
    if (app.isEmpty())
        app = "*";

    if (app[app.length()-1] == '*')
    {
        int len = app.length()-1;
        QCStringList apps = dcop->registeredApplications();
        for( QCStringList::ConstIterator it = apps.begin();
            it != apps.end();
            ++it)
        {
            if ( strncmp( (*it).data(), app.data(), len) == 0)
                appList.append(*it);
        }
    }
    else
    {
        appList.append(app);
    }
    
     for( QCStringList::ConstIterator it = appList.begin();
         it != appList.end();
         ++it)
    {
        QCString replyType;
        QByteArray replyData;
        bool result;
        if ( dcop->call(*it,objid,function,data,replyType,replyData) ) {
                if ( replyType == "bool") {
                        QDataStream _reply_stream( replyData, IO_ReadOnly );
                        _reply_stream >> result;

                        if (result) {
                           // There should only be one reply.
                          if (verbose) cout << "Found active window = " << *it << endl;
                          konqref = *it;
                        }

                } 
        } 
    }
    return konqref;
}
bool KonqClient::isRunning() {
   QCString tmp = getActive();
   if (tmp != "") {
      konqid = tmp;
      return true;
   }
   else {
      konqid = "";
   }
   return false;
}
bool KonqClient::startKonqueror() {
   KApplication::startServiceByDesktopName("konqueror");
   return (isRunning());
}
void KonqClient::back() {
	objid+="back";
	doDCOP(objid, "activate()");
}
void KonqClient::quit() {
        objid+="quit";
        doDCOP(objid, "activate()");
}
void KonqClient::breakoffcurrenttab() {
        objid+="breakoffcurrenttab";
        doDCOP(objid, "activate()");
}
void KonqClient::duplicate_window() {
        objid+="duplicate_window";
        doDCOP(objid, "activate()");
}
void KonqClient::duplicatecurrenttab() {
        objid+="duplicatecurrenttab";
        doDCOP(objid, "activate()");
}
void KonqClient::forward() {
        objid+="forward";
        doDCOP(objid, "activate()");
}
void KonqClient::fullscreen() {
        objid+="fullscreen";
        doDCOP(objid, "activate()");
}
void KonqClient::konq_sidebartng() {
        objid+="konq_sidebartng";
        doDCOP(objid, "activate()");
}
void KonqClient::home() {
        objid+="home";
        doDCOP(objid, "activate()");
}
void KonqClient::newtab() {
        objid+="newtab";
        doDCOP(objid, "activate()");
}
void KonqClient::nexttab() {
        objid+="activatenexttab";
        doDCOP(objid, "activate()");
}
void KonqClient::previoustab() {
        objid+="activateprevtab";
        doDCOP(objid, "activate()");
}
void KonqClient::print() {
        objid+="print";
        doDCOP(objid, "activate()");
}
void KonqClient::refresh() {
        objid+="reload";
        doDCOP(objid, "activate()");
}
void KonqClient::reload_all_tabs() {
	objid+="reload_all_tabs";
	doDCOP(objid, "activate()");
}
void KonqClient::removecurrenttab() {
        objid+="removecurrenttab";
        doDCOP(objid, "activate()");
}
void KonqClient::removeview() {
        objid+="removeview";
        doDCOP(objid, "activate()");
}
void KonqClient::splitviewh() {
        objid+="splitviewh";
        doDCOP(objid, "activate()");
}
void KonqClient::splitviewv() {
        objid+="splitviewv";
        doDCOP(objid, "activate()");
}
void KonqClient::stop() {
	objid+="stop";
	doDCOP(objid, "activate()");
}
void KonqClient::tab_move_left() {
        objid+="tab_move_left";
        doDCOP(objid, "activate()");
}
void KonqClient::tab_move_right() {
        objid+="tab_move_right";
        doDCOP(objid, "activate()");
}
void KonqClient::trash() {
        objid+="trash";
        doDCOP(objid, "activate()");
}
void KonqClient::undo() {
        objid+="undo";
        doDCOP(objid, "activate()");
}
void KonqClient::up() {
        objid+="up";
        doDCOP(objid, "activate()");
}
void KonqClient::addBookmark() {
	QByteArray data, replyData;
	QCString replyType;
	QDataStream arg( data, IO_WriteOnly );
	QCStringList result;
	arg << "add_bookmark";
        // First find the ID of the addBookmark object.
	cout << "dcop " << konqid << " qt find add_bookmark" << endl; 
	if ( dcop->call(konqid,"qt","find(QCString)",data,replyType,replyData) ) {
		if ( replyType == "QCStringList") {
			QDataStream _reply_stream( replyData, IO_ReadOnly );
			_reply_stream >> result;

			if (!result.empty()) {
			   // There should only be one reply.
			   QCString f = result[0];
			   doDCOP(f, "activate()");
			}
			   
		} else {
			if (verbose) cerr << "Finding the add_bookmark object failed." << endl;
		}
	} else
		if (verbose) cerr << "Finding the add_bookmark object failed." << endl;
	
}
void KonqClient::setURL(string iurl) {
   QString url = QString(iurl.c_str());
   QCString iobjid = "konqueror-mainwindow#1";
   QCString func = "openURL(QString)";
   if (isRunning()) {
           QByteArray data;
           QDataStream arg( data, IO_WriteOnly );
           arg << url;
           if (verbose) cout << konqid << ", " << iobjid << ", " << func << endl;
           if (!dcop->send(konqid,iobjid, func, data)) {
                   if (verbose) cout << "DCOP call failed for: " << konqid << " " << iobjid << " " << func << endl;
           }
    }
}
void KonqClient::doDCOP(QCString iobjid, QCString func) {
   if (isRunning()) {
  	QByteArray data;
   	QDataStream arg( data, IO_WriteOnly );
	arg << "activate";
	if (verbose) cout << konqid << ", " << iobjid << ", " << func << endl;
	if (!dcop->send(konqid,iobjid, func, data)) {
		if (verbose) cout << "DCOP call failed for: " << konqid << " " << iobjid << " " << func << endl;
	}
   }
}


