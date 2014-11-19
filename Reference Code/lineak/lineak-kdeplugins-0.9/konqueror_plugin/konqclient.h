/***************************************************************************
                          konqclient.h  -  description
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

#ifndef KONQCLIENT_H
#define KONQCLIENT_H
#include <qcstring.h>
#include <dcopclient.h>
#include <string>

/**Controls konqueror via dcop.
  *@author Sheldon Lee Wen
  */
using namespace std;

class KonqClient {
public: 
    KonqClient(DCOPClient *idcop);
    virtual ~KonqClient();
    void addBookmark();
    void back();
    void breakoffcurrenttab(); 
    void duplicate_window(); 
    void duplicatecurrenttab(); 
    void forward();
    void fullscreen(); 
    void konq_sidebartng(); 
    void home();
    bool isRunning();
    void newtab();
    void nexttab();
    void previoustab();
    void print(); 
    void refresh();
    void reload_all_tabs();
    void removecurrenttab(); 
    void removeview(); 
    void setURL(string url);
    void splitviewh(); 
    void splitviewv(); 
    bool startKonqueror();
    void stop();
    void tab_move_left(); 
    void tab_move_right(); 
    void trash(); 
    void undo(); 
    void up();
    void quit();
    QCString getActive();

  protected:
    DCOPClient *dcop;
    QCString konqid, objid;
    virtual void doDCOP(QCString objid, QCString func);

};

#endif
