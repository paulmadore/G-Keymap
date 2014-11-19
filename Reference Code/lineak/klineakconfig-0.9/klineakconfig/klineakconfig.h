/***************************************************************************
                          klineakconfig.h  -  description
                             -------------------
    begin                : Tue Apr  9 18:33:44 EDT 2002
    copyright            : (C) 2002 by Sheldon Lee Wen
    email                : tormak@rogers.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KLINEAKCONFIG_H
#define KLINEAKCONFIG_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/** Keep these in this order!!!! */
#include "klineak.h"
#include "klprefs.h"
#include <kstddirs.h>
#include <kiconloader.h>
#include "klineakdef.h"
#include "advancedkbd.h"
#include "advancedkeyoptions.h"

/** END Keep these in this order!!! */

#include <qwidget.h>
#include <klineakconfigui.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
//#include <qlistbox.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qmessagebox.h>
#include <kmessagebox.h>
#include <qstring.h>
#include <klineedit.h>

#include <kprocess.h>
#include <qfileinfo.h>
#include <kurl.h>

#include <lineak/configloader.h>
#include <lineak/lineak_core_functions.h>
#include <lineak/lconfig.h>
#include <lineak/lkbd.h>
#include <lineak/ldef.h>
#include <lineak/lobject.h>

class KlineakDef;
class Klineak;

class ConfigDirectives;
class PluginManager;

using namespace lineak_core_functions;

/** KlineakConfig is the base class of the project */
class KlineakConfig : public KlineakConfigUI
{
  Q_OBJECT 
  public:
       /** construtor */
       KlineakConfig(QWidget* parent=0, const char *name=0);
       /** destructor */
       ~KlineakConfig();

       LConfig* getConfig() { return &myConfig; }
       

  private:
       bool ApplyUsed;
       bool changeMade;
       bool got_def;
       bool got_conf;
       bool changing_from_signal;
       QListViewItem * selected_key;

       
       LConfig myConfig;
       LDef myDef;
       LKbd myKbd;
       Klineak *systemTrayApp;
       advancedKbd *advancedKbdConfig;
       advancedKeyOptions *keyopts;
       KStandardDirs *resources;
       KIconLoader load;
       
       KProcess *lineakd;
       KlineakDef  *klineakdef;
       ConfigDirectives dnd;
       PluginManager *plugins;
       vector<string> macrolist;
       QMap<string,bool> dirty;
       
   public slots:
       //virtual void updateKeyList(int index);
       //virtual void updateKeyList(const QString& inText);
       virtual void updateKeyboardModels(const QString& inText);
       //virtual void updateDisplayName(const QString& inText);
       virtual void onKeyListSelect(QListViewItem *box);
       virtual void updateAvailableKeys(const QString &keyboard);
       //virtual void updateCDRom(const QString& inText);
       //virtual void updateMixer(const QString& inText);
       virtual void quit();
       virtual bool onApplyButtonClicked();
       virtual void slotToggleApp();
       virtual bool hupLineakd();
       virtual bool startLineakd();
       virtual bool stopLineakd();
       virtual bool lineakDefStart();
       virtual bool lineakDefStop();
       virtual bool parsedef(QString parsefile=QString::null);
       virtual bool parseconf(QString parsefile= QString::null);
       /** This member function set's KlineakConfig's definition widget. */
       virtual void setLineakDef(KlineakDef *def);
       /** Slot activated by KlineakDef's apply button. It is passed the name of the file to parse. If it is not null, then parse it. */
       virtual bool reparseDef(QString parsefile = QString::null);
       virtual bool backup_conffile(void);
       virtual bool restore_conffile(void);
       virtual bool save_conffile(void);
       virtual void onAdvancedConfigOptionsButtonClicked(void);
       virtual void onAdvancedKeyOptionsButtonClicked(void);
       virtual void slotRefreshKeylist(void);
       virtual void setKeyboard(QString);

   private:
       /* refresh *ALL* the data on the GUI (this is called on startup) */
       bool refreshGUIdata();
       /* refresh EAK types */
       bool refresh_kbtypes();
       /* refresh CD-ROM entry */
       //bool refresh_cdromdev();
       /* refresh keyboard image */
       //bool refresh_kbimage();
       /* refresh key list */
       bool refresh_keylist();
       /* refresh command/action */
       //bool refresh_keycommand(QListBoxItem *box);
       /* refresh the display name text box */
       bool refresh_displayname(QListViewItem *box);
       /* refresh the special commands list */
       //bool refresh_specials();
       /** Is this command a special action? */
       //bool is_special_action (const QString &command);
       /* select a new keyboard */
       void select_new_keyboard(const QString &ndata);
       /* change the key command data to the users input */
       //void change_key_command(LObject *thiskey, const QString &entrytext);
       /* wrapper for change_key_command, lookups the selected key */
       //void change_selectedkey_command(const QString &entrytext);
       /** Present the user with a choice of supported keyboards and get the keyboard type back as a return value. */
       void createBasic(const QString &filename);
       /** Change the display name of the key. */
       //bool change_key_displayname(LObject *thiskey, const QString &entrytext);
       /** Reconcile the myKbd object with the configuration file. */
       void reconcile_config();
       

};

/** Handle signals properly */
void signalchild(int sig_num);
void signalhup(int sig_num);
void signalquit(int sig_num);
#endif
