/***************************************************************************
                          klineaklib.h  -  description
                             -------------------
    begin                : December 4 2003
    copyright            : (C) 2003 by Sheldon Lee Wen
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
#ifndef KLINEAK_LIB_H
#define KLINEAK_LIB_H
/*
#include <ctype.h>
#include <dcopclient.h>
#include <dcopobject.h>
#include <dcopref.h>
#include <iostream>
#include <kapplication.h>
#include <kdatastream.h>
#include <klocale.h>
#include <kurl.h>
#include <qcolor.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qvariant.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
*/
#include <qstring.h>
#include <qstringlist.h>
#include <config.h>
#include <qcolor.h>
#include <qrect.h>
#include <qpoint.h>
#include <qsize.h>
#include <qcolor.h>
#include <lineak/plugin_definitions.h>
QCStringList findObject(DCOPClient* dcop, const char* app, const char* obj, const char* func, QCStringList args, bool bAppIdOnly );
bool launchApp(DCOPClient* dcop, QString app);
void startApp(DCOPClient* dcop, const char *_app, int argc, const char **args);
bool mkBool( const QString& s );
QPoint mkPoint( const QString &str );
QSize mkSize( const QString &str );
QRect mkRect( const QString &str );
QColor mkColor( const QString& s );
QCString demarshal( QDataStream &stream, const QString &type );
void marshall( QDataStream &arg, QCStringList args, uint &i, QString type );

#endif
