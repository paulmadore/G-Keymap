//
// C++ Implementation: advancedkbd
//
// Description:
//
//
// Author: Sheldon Lee-Wen <sheldon.leewen@cgi.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "advancedkbd.h"
#include "klineakconfig.h"
#include "klprefs.h"
#include <lineak/lineak_core_functions.h>
//#include <kprocess.h>
#include <qcheckbox.h>
#include <dcopclient.h>



advancedKbd::advancedKbd(QWidget* parent, const char* name, bool modal, WFlags fl): frmAdvancedKbdUI(parent, name, modal, fl)
{
   KlineakConfig* klconfig = static_cast<KlineakConfig*>(parent);
   gotdata = false;
   lstConfigDirectives->setAllColumnsShowFocus(true); 
   connect(lstConfigDirectives, SIGNAL(itemRenamed(QListViewItem*,int,const QString &)), this, SLOT(slotConfigDirectivesItemRenamed(QListViewItem*,int,const QString &)));

   if (KLPrefs::instance()->mLEDs) {
      chkLeds->setChecked(true);
      startLeds();
   }
}
advancedKbd::~advancedKbd()
{
   KLPrefs::instance()->writeConfig();
   gotdata = false;
}

void advancedKbd::setData(LConfig &myConfig) {
   config = &myConfig;
   gotdata = true;
   
   load_gui_data();
}

void advancedKbd::load_gui_data() {

  if (gotdata) {  
	// Populate the configuration directives listview.
	QListViewItem *lvp = 0;
	cout << "Refreshing key list" << endl;
	vector<string> keys = config->getDirectiveKeys();
	
	lstConfigDirectives->clear();
	for (vector<string>::iterator it = keys.begin(); it!= keys.end(); it++) {
		lvp = new QListViewItem(lstConfigDirectives, *it, config->getValue(*it));
		lvp->setRenameEnabled(0,false);
		lvp->setRenameEnabled(1,true);
		lvp->setSelectable(true);
		//lstAvailableKeys->insertItem(tmp);
	}
	lstConfigDirectives->setSelected(0,true);
  }
  // Set the contribute email dialog
  txtContributeEmail->setText(KLPrefs::instance()->mContribEmail);
  // Set the use kkeyleds checkbox
  chkLeds->setChecked(KLPrefs::instance()->mLEDs);

  // Set the run lineak at exist checkbox
  if (KLPrefs::instance()->mLineakAtExit == "Yes")
     chkRunLineakAtExit->setChecked(true);
  else
     chkRunLineakAtExit->setChecked(false);

  // Set the use kkeyleds checkbox
//  if (KLPrefs::instance()->mKkeyledAtExit == "Yes")
//     chkRunKkeyledAtExit->setChecked(true);
//  else
//     chkRunKkeyledAtExit->setChecked(false);

}

//void advancedKbd::refresh_contrib_email() {}
void advancedKbd::refresh_gui_data() {}

void advancedKbd::slotConfigDirectivesItemRenamed(QListViewItem* lv, int col, const QString &txt) {
    cout << "Item : " << lv->text(0) << " Value: " << lv->text(1) << endl;
    config->setValue(lv->text(0),lv->text(1));
    //emit saveConfig();
}

void advancedKbd::languageChange()
{
   frmAdvancedKbdUI::languageChange();
}
void advancedKbd::slotCancel() {
   load_gui_data();
   if (isVisible())
      hide();
}
/** Triggered when the user clicks the OK button */
void advancedKbd::slotBtnOK(){
         KLPrefs::instance()->writeConfig();
         emit saveConfig();
         if (isVisible())
                hide();
}
/** Triggered when the user clicks the LED's check box. */
void advancedKbd::slotchkLeds(){
        if (chkLeds->isChecked()) {
                 KLPrefs::instance()->mLEDs = true;
                 startLeds();
        }
        else {
                 KLPrefs::instance()->mLEDs = false;
                 stopLeds();
        }
         //KLPrefs::instance()->writeConfig();
}

void advancedKbd::slotContributeEmail(const QString& arg1)
{
   KLPrefs::instance()->mContribEmail = arg1;
   //KLPrefs::instance()->writeConfig();
}

/** Starts kkeyled */
void advancedKbd::startLeds(){
        leds.clearArguments();
        leds << "kkeyled";
        leds.start(KProcess::DontCare);
        if (!leds.isRunning())
        {
                        QMessageBox::information(0, "KlineakControl", i18n(
                        "WARNING: KlineakControl could not start the keyboard led display (kkeyled)!\n"));
                        cout << "Failed to start kkeyled" << endl;
        }

}
/** Stops kkeyled */
void advancedKbd::stopLeds(){
        // Use pcontrol to kill the process. kkeyled forks itself into the background do KProcess contains the wrong PID.
        if (lineak_core_functions::is_running("kkeyled")) {
                cout << "Should kill kkeyled here!" << endl;
                DCOPClient *dcop = new DCOPClient();
                dcop->registerAs("klineakconfig");
                dcop->attach();
                QByteArray data;
                dcop->send("kkeyled", "MainApplication-Interface", "quit()", data);
                //leds.normalExit();
        }

}

/** User toggled Run Lineakd after exit checkbox. */
void advancedKbd::slotRunLineakd(){
        if (chkRunLineakAtExit->isChecked())
                 KLPrefs::instance()->mLineakAtExit = "Yes";
        else
                 KLPrefs::instance()->mLineakAtExit = "No";

         KLPrefs::instance()->writeConfig();
}
/** User toggled Run kkeyled after exit button. */
// void advancedKbd::slotRunKkeyled(){
//         if (chkRunKkeyledAtExit->isChecked())
//                  KLPrefs::instance()->mKkeyledAtExit = "Yes";
//         else
//                  KLPrefs::instance()->mKkeyledAtExit = "No";
// 
//          KLPrefs::instance()->writeConfig();
// }
