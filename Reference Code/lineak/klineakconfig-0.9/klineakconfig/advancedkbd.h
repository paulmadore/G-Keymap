//
// C++ Interface: advancedkbd
//
// Description:
//
//
// Author: Sheldon Lee-Wen <sheldon.leewen@cgi.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ADVANCEDKBD_H
#define ADVANCEDKBD_H

#include <frmadvancedkbd.h>
#include <lineak/lconfig.h>

#include <qwidget.h>
#include <klineakconfigui.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistview.h>
#include <klistview.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qmessagebox.h>
#include <kmessagebox.h>
#include <qstring.h>
#include <klineedit.h>
#include <kprocess.h>

#include <kprocess.h>
#include <qfileinfo.h>
#include <kurl.h>

#include <vector>
#include <string>

/**
	@author Sheldon Lee-Wen <sheldon.leewen@cgi.com>
*/
class advancedKbd : public frmAdvancedKbdUI
{
   Q_OBJECT

public:
  advancedKbd(QWidget* parent, const char* name, bool modal, WFlags fl);
  ~advancedKbd();
  void setData(LConfig &myConfig);

signals:
  void saveConfig();

protected slots:
  virtual void languageChange();
  virtual void slotBtnOK();
  virtual void slotCancel();
  virtual void slotchkLeds();
  virtual void slotContributeEmail(const QString& arg1);
  //virtual void slotRunKkeyled();
  virtual void slotRunLineakd();
  virtual void slotConfigDirectivesItemRenamed(QListViewItem*,int,const QString &);
  virtual void startLeds();
  virtual void stopLeds();
  
private:
  void load_gui_data();
  void refresh_gui_data();
  LConfig *config;
  bool gotdata;
  KProcess leds;

};

#endif
