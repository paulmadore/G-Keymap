/***************************************************************************
                          klineak.cpp  -  description
                             -------------------
    begin                : Mon Jun 3 2002
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

#include "klineak.h"
#include "klprefs.h"
#include <iostream>
using namespace std;
// kde
#include <kiconloader.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kdebug.h>
#include <kpopupmenu.h>

// qt
#include <qtooltip.h>
#include <qcursor.h>

Klineak::Klineak(QWidget *parent, const char *name ) : KSystemTray(parent,name) {
   KIconLoader load;
   klineakconfigPixmap = load.loadIcon("klineakconfig",KIcon::Small);
   actionCollection = new KActionCollection(this);
   setIcon(load.loadIcon("klineakconfig",KIcon::Small));
   setPixmap(klineakconfigPixmap);
   menu = new KPopupMenu;
   fileQuit = KStdAction::quit(this, SLOT(slotExit()), actionCollection,"quit");
   lineakdRestart = new KAction (i18n("&Restart Lineakd"), "reload", KStdAccel::shortcut(KStdAccel::Reload), parent, SLOT(hupLineakd()), actionCollection,"reload");
   lineakdStart = new KAction (i18n("&Start Lineakd"), "start", KStdAccel::shortcut(KStdAccel::Open), parent, SLOT(startLineakd()), actionCollection,"start");
   lineakdStop = new KAction (i18n("S&top Lineakd"), "stop", KStdAccel::shortcut(KStdAccel::Close), parent, SLOT(stopLineakd()), actionCollection, "stop");
   lineakDef = new KAction(i18n("&Configure Keyboard"), "run", KStdAccel::shortcut(KStdAccel::Close), parent, SLOT(lineakDefStart()), actionCollection, "run");
   lineakPrefs = new KAction(i18n("&Preferences"),"configure",KStdAccel::shortcut(KStdAccel::AccelNone),parent, SLOT(onAdvancedConfigOptionsButtonClicked(void)), actionCollection, "viewsettings");
   lineakPrefs->plug(menu);
   menu->insertSeparator();
   lineakdStart->plug(menu);
   lineakdStop->plug(menu);
   lineakdRestart->plug(menu);
   menu->insertSeparator();
   lineakDef->plug(menu);
   menu->insertSeparator();
   fileQuit->plug(menu);
}

Klineak::~Klineak(){
	//delete(preferencesui);
}
void Klineak::restartLineakd() {
   //cout << "Klineak::restartLineakd()" << endl;
	emit restart();
}
void Klineak::stopLineakd() {
   //cout << "Klineak::stopLineakd()" << endl;
	emit stop();
}
void Klineak::startLineakd() {
   //cout << "Klineak::startLineakd()" << endl;
	emit start();
}
void Klineak::mousePressEvent(QMouseEvent *ev)
{
	if (ev->button() == RightButton)
	{
		menu->exec(QCursor::pos());
	}
	else
	{
		emit toggleApp(); //hide or show the app
	}
	
}
/** Shows the Preferences dialog. */
void Klineak::showPreferences(){
	//if(preferencesui->isVisible())
	//	preferencesui->hide();
	//else {
	//	preferencesui->show();
	//	preferencesui->setEnabled(true);
	//}
}

/** User clicked the quit button */
void Klineak::slotExit(){
   //cout << "Kicker Applet emitting quit\n";
   if (KLPrefs::instance()->mKkeyledAtExit == "No")
    //preferencesui->stopLeds();	
      emit stopKkeyleds();
   
   emit cleanExit();
   //cout << "Kicker Applet emitting quit done\n";
}
#include "klineak.moc"
