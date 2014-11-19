/***************************************************************************
                          klineakconfig.cpp  -  description
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

#include "klineakconfig.h"
#include <lineak/msgpasser.h>
#include <lineak/definitions.h>
#include <lineak/lineak_core_functions.h>
#include <lineak/configdirectives.h>
#include <lineak/pluginmanager.h>
#include <lineak/plugin_definitions.h>
#include <lineak/definitions.h>
#include <lineak/cdromctrl.h>
#include <lineak/displayctrl.h>
#include <lineak/saver.h>
#include <lineak/lcommand.h>
#include "advancedkeyoptions.h"

#include <map>
#include <vector>
#include <string>
#include <kstddirs.h>

using namespace std;
using namespace lineak_core_functions;
extern bool verbose;
extern bool very_verbose;
extern KlineakConfig *klineakconfig;

KlineakConfig::KlineakConfig(QWidget *parent, const char *name) : KlineakConfigUI(parent, name)
{
   ApplyUsed=false;
   changeMade=false;
   selected_key=0;
   got_def = false;
   got_conf = false;
   changing_from_signal=false;
   lineakd = NULL;
   plugins = NULL;
   
   QFile filename;
   QString parsefile = QString::null;
   parsefile = QDir::homeDirPath() + CONFFILE;
    
   msg("Loading the plugin manager");
   plugins = new PluginManager;
   vector<string> pluginlist = plugins->scanForPlugins();
   /** Load the plugins */
   plugins->loadPlugins(pluginlist);
   /** Define the list of macros we support */
   plugins->defineMacroLists();
   /** Define the configurate directives we support */
   plugins->defineDirectivesLists();
   /** Get a list of macros we support so that we can inform other classes */
   macrolist = plugins->getMacroList();
   /** Get a list of directives from the plugins and their defaults that we support here */
   dnd = plugins->getDirectivesList();
   msg("Setting dnd!");
   /** Add to the list of directives and defaults that we support implicitly. */
   dnd.addValue(_CD_KEYBOARD_TYPE, snull);
   dnd.addValue(_CD_USERCONFFILE, snull);
   dnd.addValue(_CD_CDROM_DEVICE, DEFAULT_CDROM_DEVICE);
   dnd.addValue(_CD_MIXER_DEVICE, DEFAULT_MIXER_DEVICE);
   dnd.addValue("Screensaver", snull);
   dnd.addValue("Display_plugin",DEFAULT_DISPLAY_PLUGIN);
   dnd.addValue("Display_font", DISPLAY_FONT);
   dnd.addValue("Display_color", DISPLAY_COLOR);
   dnd.addValue("Display_pos", DISPLAY_POS);
   dnd.addValue("Display_align", DISPLAY_ALIGN);
   dnd.addValue("Display_timeout", DISPLAY_TIMEOUT);
   dnd.addValue("Display_hoffset", DISPLAY_HOFFSET);
   dnd.addValue("Display_voffset", DISPLAY_VOFFSET);
   dnd.addValue("Display_soffset", DISPLAY_SOFFSET);
   dnd.addValue("keystate_numlock", snull);
   dnd.addValue("keystate_capslock", snull);
   dnd.addValue("keystate_scrolllock", snull);

   /** Set the macrolist for all LCommand objects */
   LCommand::setMacros(macrolist);
   
   //TODO define these properly so that signals are inherited.
   signal(SIGTERM, signalquit);
   signal(SIGABRT, signalquit);
   signal(SIGINT, signalquit);
   signal(SIGCLD,signalchild);
   /* and for a rehash when we catch SIGHUP */
   signal(SIGHUP,signalhup);
   
   /** Set up the system tray applet */
   systemTrayApp = new Klineak(this);
   connect(systemTrayApp,SIGNAL(toggleApp()),this,SLOT(slotToggleApp()));
   connect(systemTrayApp,SIGNAL(cleanExit()),this,SLOT(quit()));

   /** Set up the advanced keyboard options */
   advancedKbdConfig = new advancedKbd((QWidget*)this, name, true, 0);
   advancedKbdConfig->hide();
   connect(advancedKbdConfig,SIGNAL(saveConfig()),this,SLOT(save_conffile(void)));

   /** Connect signals between advancedKbdConfig and systemTrayApp */
   connect(systemTrayApp,SIGNAL(stopKkeyleds()), advancedKbdConfig, SLOT(stopLeds()));
   
   QString homedir = QDir::homeDirPath() + LINEAKDIR;
   QDir lineakdir;
  
   /** Test to see if the .lineak directory exists in the users home directory */
   lineakdir.setPath(homedir);
   if (!lineakdir.exists())
      lineakdir.mkdir(homedir);

   /** Add the resource dirs */
   resources = KGlobal::dirs();
   // Add ROOTDIR to the prefix we will have KStandardDirs search for the image
   resources->addPrefix(ROOTDIR);
   
   QString userdeffile = QString("%1%2%3").arg(QDir::homeDirPath()).arg(LINEAKDIR).arg(DEFFILE);
   QString userconffile = QString("%1%2").arg(QDir::homeDirPath()).arg(CONFFILE);

   dnd.addValue(_CD_USERDEFFILE, userdeffile.latin1());
   dnd.addValue(_CD_SYSDEFFILE, LINEAKKB_FILE);
   dnd.addValue(_CD_USERCONFFILE, userconffile.latin1());

   /** Parse the user definition file */
   if (!(got_def = parsedef())) {
       QMessageBox::information(this, "KLineakConfig", QString(
          "WARNING: KLineakConfig could parse your keyboard definition file!\n" ));
       quit();
   } 

   /* Make sure we have a config file. */ 
   filename.setName(parsefile);
   if (!filename.exists()) {
      msg("Creating a basic config file.");
      createBasic(parsefile);
   }

   msg("Parsing the config files.");
   /** Parse the users ~/.lineak/lineakd.conf file */
   if (!(got_conf = parseconf()))
      quit();

   msg("Setting commands!");
   usleep(400000);
   myKbd.setCommands(myConfig);

   // Set the controller for the key options.
   keyopts = new advancedKeyOptions((QWidget*)this, name, true, 0);
   keyopts->setMacros(macrolist);
   keyopts->setLConfig(&myConfig);
   /** Set the connections between the advanced key options dialog and
       the main application */
   connect(keyopts,SIGNAL(saveConfig()),this,SLOT(save_conffile(void)));
   connect(keyopts,SIGNAL(apply()),this,SLOT(slotRefreshKeylist(void)));
   
   // Show the panel
   msg("Showing the system tray.");
   systemTrayApp->show();
   // Show all the data in the gui widgets.
   msg("Refreshing GUI data");
   refreshGUIdata();
   // Test and see if there are any lineakd processes running. If there are kill them or exit.
   
   int attempt = 0;
   //Disabled while testing GUI.
   
   while (is_running("lineakd") != false && attempt < 10) {
	//cout << "Killing lineakd" << endl;
   	stopLineakd();
	usleep(400000);
	attempt++;
   }  	
   msg("Starting lineakd");
   startLineakd();
   
}

KlineakConfig::~KlineakConfig() {
   plugins->unloadAllPlugins();
   delete(resources);
   delete(systemTrayApp);
   delete(plugins);
}
void KlineakConfig::slotToggleApp()
{
	if(isVisible())
		hide();
	else {
		show();
		setEnabled(true);
	}
}

bool KlineakConfig::lineakDefStart()
{
    // If it's not already running.....
    if (!klineakdef->isVisible()) {
		stopLineakd();
  		/** Parse the keyboard definitions file */
   		klineakdef->show();
   		//connect klineakdef's ok/apply or close button with lineakDefStop()
   		connect(klineakdef,SIGNAL(quitting()),this,SLOT(lineakDefStop()));
   		//connect klineakdef's ok/apply button to klineakconfig parsedef slot.
   		connect(klineakdef,SIGNAL(applying(QString)),this,SLOT(reparseDef(QString)));
   		// connect klineakdef's keyboardName signal to our setKeyboard slot
                connect(klineakdef,SIGNAL(keyboardName(QString)),this,SLOT(setKeyboard(QString)));
   		// connect(klineakdef,SIGNAL(keyboardName(QString)),this,SLOT(setKeyboard()));
   }
	return true;
}

/** Slot activated when KlineakDef exits. */
bool KlineakConfig::lineakDefStop()
{
   klineakdef->hide();
   startLineakd();
   return true;
}

/** Slot activated by KlineakDef's apply button. It is passed the name of the file to parse. If it is not null, then parse it. */
bool KlineakConfig::reparseDef(QString parsefile){
   
   /** The changing from signal flag should stop the setKeyboard method
   until the new file has been reparsed.*/
   
   changing_from_signal = true;
   msg("reparsing");
   bool res = false;
     
   if (parsefile != QString::null)
      res = parsedef(parsefile);
   
   changing_from_signal = false;
   return res;
}

/** Parse the definition file. */
bool KlineakConfig::parsedef(QString parsefile) {
  QFile filename;
  
  //cout << "parsing deffile" << endl;
  /** If parsefile is null, then set the default to the system lineakkb.def */
  if (parsefile == QString::null) {
     if (verbose) cout << dnd << endl;
     return (parsedeffile(dnd, myDef));
  }
  else {
     filename.setName(parsefile);
     if (!filename.exists()) {
        cerr << "The filename: " << parsefile << " that you want to parse does not exist!" << endl;
        return false;
     } else {
        //ConfigDirectives tmp = prefs;
	string file = "";
	if (parsefile == QString::null)
	   file = "";
	else
           file = parsefile.latin1();
	dnd.addValue(_CD_USERDEFFILE, file);
        //cout << "Parsing the def file." << endl;
        //cout << dnd << endl;
	/* parse .def files */
        if(!parsedeffile(dnd, myDef)) {
           return false;
        }
        if (got_conf) {
           //changing_text_from_signal = true;
           /* refresh EAK types */
           refresh_kbtypes ();
           //changing_text_from_signal = false;
        }
     }
  }
  //cout << "Done parsing deffile" << endl;
  return true;
}

/** Parse the confile file */
bool KlineakConfig::parseconf(QString parsefile) {
  QString returnMsg;

  //cout << "parsing conffile\n";	
  
  /** Parse the .conf file. If there was some error, display it and return false. */
  if(!parseconffile(dnd, myConfig)) {
     QMessageBox::critical(this, "KLineakConfig", "Failed to parse the configuration file.");
     return false;
  }
  //msg("Do we have a def yet?");
  /** If we parsed a definition's file */
  if (got_def) {
     string kbdtxt = myConfig[_CD_KEYBOARD_TYPE];

     if (myConfig.isEmpty())
        error("myConfig is empty!");
	   
     if (myDef.hasKeyboard(kbdtxt)) {
        msg("Returning the keyboard.");
        myKbd = myDef.getKeyboard(kbdtxt);
     }
     else {
        cerr << "myDef reports that it does not have keyboard: " << kbdtxt << endl;
	return false;
     }
     
     //msg("is myKbd empty?");
     if (myKbd.isEmpty()) {
        msg("OH OH! it's an empty keyboard definition!");
        QString homedir = QDir::homeDirPath(); //getenv("HOME");
        homedir+=LINEAKDIR;
        homedir+=DEFFILE;
       
        QMessageBox::information(this, "KLineakConfig",
           i18n("WARNING: KLineakConfig could not find any keys defined for your keyboard!\n"
                "You probably have a corrupt or incorrect keyboard definition file in either:\n"
                "%1\n"
                "or\n"
                "%2\n").arg(LINEAKKB_FILE).arg(homedir));
           quit();
     } else
        msg("myKbd is not empty");
  }
  else {
     error("Attepting to load the user .conf setting before we have keyboard data!");
     return false;
  }
  msg("Returning from parseConf");
  return true;
}

/** Update the list of available keys in the list box when a new keyboard
	  is selected from the Keyboard Type combo box */
void KlineakConfig::updateAvailableKeys(const QString &keyboard) {
    //string kbd = keyboard.latin1();
    QString model = myKbd.model.c_str();
    //cout << "myKbd.name = " << myKbd.name << endl;
    //cout << "new kbd = " << kbd << endl;
    if (model != keyboard ) {
       select_new_keyboard(keyboard);
       // Update the keyboard in the config file.
       //myConfig[_CD_KEYBOARD_TYPE] = keyboard;
       //changeMade = true;
    }
}

/** Update the list of keyboard in the Keyboard Types combo box
      when the user selects a new brand */
void KlineakConfig::updateKeyboardModels(const QString &itxt) {
   string inText = itxt.latin1();
   if (myKbd.brand != inText) {
      // Load the appropriate models.
      map<string,LKbd*> models = myDef.getModels((cbKeyboardBrand->currentText()).latin1());
      map<string,LKbd*>::iterator it = models.begin();
      
      // Clear the combo box.
      cbKeyboard->clear();
      // For each model of keyboard
      for(int m=0;it!=models.end();it++, m++)  {
         cbKeyboard->insertItem(QString((it->second)->model.c_str()));
         if(it->first == myKbd.model)
            cbKeyboard->setCurrentItem(m);
      }
      // We should now have a new keyboard in the Keyboard Type box, so update the list of keys.
      select_new_keyboard(cbKeyboard->currentText());
   }
}

/** called when an item is selected in the keylist */
void KlineakConfig::onKeyListSelect(QListViewItem *box) {

  if ( lstAvailableKeys->currentItem() != 0 ) {
    //changing_text_from_signal = true;
    //refresh_keycommand(box);
    //refresh_displayname(box);
    if (btnAdvancedKeyOptions->isEnabled() == false )
       btnAdvancedKeyOptions->setEnabled(true);

    //changing_text_from_signal = false;
    selected_key = lstAvailableKeys->currentItem();
  }
}

/** Slot called when you click the Apply button */
bool KlineakConfig::onApplyButtonClicked() {
  /* first time apply? -> backup conf file */
   if (!ApplyUsed) {
      QString returnMsg;
      
    ApplyUsed = true;
    // If there was some error message, display it.
    backup_conffile();
   }
   
  if (changeMade) {
     /* save the changes to a new conf file */
     if (!save_conffile()) {
  	QMessageBox::information(this,"KlineakConfig",i18n("ERROR: The keyboard configuration could not be saved."));	
        return false;
     }
     else
        changeMade = false;
  }
  // Hide the main gui.
  hide();
  /* restart lineakd */
  hupLineakd();
  return true;
}

/** Hup the lineakd daemon.
		Return true if we hupped the daemon.
  		Return false if the daemon was not hupped */
bool KlineakConfig::hupLineakd() {
   msg("hupLineakd()");
   msgPasser message;
   message.start();
   message.sendMessage(msgPasser::HUP,"hup");
   return (true);
}

/** Start the lineakd daemon.
		If lineakd is running standalone, kill it, and start under the control of klineakconfig.
		Return true if lineakd has been started by klineakconfig or if we started lineakd.
    	Return false if lineakd could not be started */
bool KlineakConfig::startLineakd() {
      /* If lineakd is not running */   		
    if (lineakd != NULL) {
    	stopLineakd();
     	delete (lineakd);
        lineakd = NULL;
     }
    	
    lineakd = new KShellProcess();
    lineakd->clearArguments();
  	*lineakd << "lineakd";
        
        if (verbose)
	   *lineakd << "-v";
        
 	lineakd->start(KProcess::DontCare,KProcess::NoCommunication);
 	if (lineakd->isRunning()) {
           if (verbose) cout << "Starting lineakd, pid is: " << lineakd->pid() << endl;
//		/** At some point where I figure out how to get root priv's renice to -10 */
//		PControl pc;
//		pc.renice(lineakd->pid());
 		return true;
   }
 	else {
           QMessageBox::information(this, "KLineakConfig", QString(
        	i18n("WARNING: KLineakConfig could not start the easy access keyboard driver!\n")));
 			cerr << "Failed to start lineakd" << endl;
  		return false;
 	}
   return false;
}

/** Stop the lineakd daemon. If we killed it return true. Else if it could not be killed
		return false */
bool KlineakConfig::stopLineakd() {
   msg("Stopping Lineakd");
   msgPasser message;
   message.start();
   message.sendMessage(msgPasser::EXIT,"exit");
   
   if (lineakd != NULL) {
      delete (lineakd);
      lineakd = NULL;
   }
   
   return (true);
}

/** When the klineak quit button is clicked. */
void KlineakConfig::quit() {
  //cout << "In Quit\n";
  if (ApplyUsed) {
     hupLineakd();
  }
  else {
    	if(changeMade) {
          switch( QMessageBox::warning( this, "KLineak",
                  i18n("Do you want to save your changes?\n"),
                  i18n("Apply"),
                  i18n("Quit"), 0, 0, 1 ) )  {
              case 0: // The user clicked the Apply button or pressed Enter
                  onApplyButtonClicked(); // Apply
 		  void showPreferences();
                  break;
              case 1: // The user clicked the Quit or pressed Escape
                  restore_conffile();
                  break;
          }
       }
  }
  //cout << "In Quit: See if lineakd is running. Leave running or exit?\n";

  if (lineak_core_functions::is_running("lineakd")) {
     switch( KMessageBox::questionYesNo( this,  i18n(     //parent
                  "The lineakd daemon is still running.\n"
                  "Do you want to continue running lineakd in the background?\n"
                  "(If not, you will be unable to use your easy access keys.)\n\n"), //text
                  i18n("Leave lineakd running?"),  //caption
                  i18n("Run in background"),  //yes button
                  i18n("Stop lineakd"), //no button
                  "Lineakd At Exit"  ) ) {  // name of don't show again
     case KMessageBox::Yes : // The user clicked the "Run in background" button or pressed Enter
          lineakd->detach();
          break;
     case KMessageBox::No : // The user clicked the Quit or pressed Escape
          stopLineakd();
          break;
     }
  }
  //cout << "In Quit: p.cleanexit()\n";

  exit(1);
}

/* refresh *ALL* the data on the GUI (this is called on startup) */
bool  KlineakConfig::refreshGUIdata() {
  //changing_text_from_signal = true;
  /* refresh EAK types */
  refresh_kbtypes ();
  /* refresh key list */
  refresh_keylist ();
  /* refresh the display name */
  //refresh_displayname(lstAvailableKeys->selectedItem());
  //changing_text_from_signal = false;

  return (true);
}
/** refresh the Keyboard types */
bool  KlineakConfig::refresh_kbtypes() {

    vector<string> brands = myDef.getBrands();
    vector<string>::iterator it2 = brands.begin();
    
    cbKeyboardBrand->clear();
    // Load the brands of Keyboards.
    for(int b=0;it2!=brands.end(); it2++, b++) {
       cbKeyboardBrand->insertItem(QString((*it2).c_str()));
       if (*it2 == myKbd.brand)
          cbKeyboardBrand->setCurrentItem(b);
    }
    
    cbKeyboard->clear();
    
    // Load the appropriate models.
    map<string,LKbd*> models = myDef.getModels((cbKeyboardBrand->currentText()).latin1());
    map<string,LKbd*>::iterator it = models.begin();
      
    // For each model of keyboard
    //for(int m=0;it!=p.bmmap[cbKeyboardBrand->currentText()].end();it++, m++)  {
    for(int m=0;it!=models.end();it++, m++)  {
       cbKeyboard->insertItem(QString(it->second->model.c_str()));
       if(it->second->model == myKbd.model && it->second->brand == myKbd.brand)
          cbKeyboard->setCurrentItem(m);
    }
    
   return (true);
}

/** refresh the keylist when the user selected some other keyboard */
bool  KlineakConfig::refresh_keylist() {
  QString tmp;
  QListViewItem *lvp = 0;
  const map <string, LObject*> &objects = myKbd.getObjects();
  map <string, LObject*>::iterator it = const_cast<map <string, LObject*> &>(objects).begin();
  
  lstAvailableKeys->clear();
  for (; it!= objects.end(); it++) {
	tmp = QString(it->second->getName().c_str());
        lvp = new QListViewItem(lstAvailableKeys, tmp, QString(it->second->getCommandDisplayName().c_str()));
	//lstAvailableKeys->insertItem(tmp);
  }
  lstAvailableKeys->setSelected(0,true);
  selected_key = 0;
  return (true);
}

/** This slot is called by the advanced key options gui when the apply
    button is clicked and a change has been made. */
void KlineakConfig::slotRefreshKeylist(void) {
   //flag that we have made changes.
   changeMade = true;
   // Refresh the gui.
   refresh_keylist();
}

/** This slot is called when the klineakdef emits the keyboardName(QString)
    signal. This signal is emitted when the user is saving his custom 
    definition file. */
void KlineakConfig::setKeyboard(QString name) {
   LKbd kb;
   
   while (changing_from_signal);
   kb = myDef.getKeyboard(name.latin1());

   // Update the GUI with the new data. First set the appropriate brand.
   for (int i=0; i != cbKeyboardBrand->count(); i++) {
      if ( kb.brand == string(cbKeyboardBrand->text(i).latin1()) ) {
         updateKeyboardModels(kb.brand);
         cbKeyboardBrand->setCurrentItem(i);
         break;
      }
   }
   // Now set the appropriate keyboard.
   //cout << "myKbd.model = " << kb.model << endl;
   //cout << "keyboard we are setting to has name = " << name << endl;
   
   for (int i=0; i != cbKeyboard->count(); i++) {
      //cout << "cbKeyboard->text(" << i << ").latin1() = " << string(cbKeyboard->text(i).latin1()) << endl;
      //cout << "myDef.getKeyboard(" << cbKeyboardBrand->currentText() << ", " << cbKeyboard->text(i) << ").name == " << myDef.getKeyboard(cbKeyboardBrand->currentText().latin1(),cbKeyboard->text(i).latin1()).name << endl;
      //cout << endl;
      if ( kb.model == string(cbKeyboard->text(i).latin1()) ) {
         // Make sure that the keyboard is the actual one we want by
         // comparing the keyboard hash (internal) name codes.
         if (myDef.getKeyboard(cbKeyboardBrand->currentText().latin1(),cbKeyboard->text(i).latin1()).name == string(name.latin1())) {
            //cout << "Setting model to item: " << i << endl;
            cbKeyboard->setCurrentItem(i);
            updateAvailableKeys(kb.model);
            //myKbd = myDef.getKeyboard(name.latin1());
            break;
         }
      }
   }
   
   changeMade = true;
  
  //changing_text_from_signal = false;
   selected_key = 0;
   //msg("leaving setKeyboard");
}

/** select a new keyboard by model name. The brand name is obtained by
    the current value in the GUI. */
void  KlineakConfig::select_new_keyboard(const QString &ndata) {
  myKbd.clear();
  myKbd = myDef.getKeyboard((cbKeyboardBrand->currentText()).latin1(), ndata.latin1());
  // Reconcile the configuration file.
  reconcile_config();
  // This must be called after reconcile_config b/c it assumes that
  // the lconfig object is consistent with the myKbd object in terms
  // of the keys defined.
  myKbd.setCommands(myConfig);
  if (verbose) {
     msg("SELECTED NEW KEYBOARD");
     cout << myKbd << endl;
     msg("--END SELECTED NEW KEYBOARD--");
  }
  
  //changing_text_from_signal = true;
  refresh_keylist();
  //refresh_keycommand(lstAvailableKeys->item(0)); /* refresh_keylist selects 1st key, hence index 0 */
  //refresh_kbimage();
  
  changeMade = true;
  
  //changing_text_from_signal = false;
  selected_key = 0;
}

/** Present the user with a choice of supported keyboards and get the keyboard type back as a return value. */
void KlineakConfig::createBasic(const QString &filename) {
  /** Get the first keyboard parsed from the .def file */
  map<string,LKbd*>::iterator it = myDef.getTable().begin();
  it++;
  string kbtype = it->first;
  dnd.addValue(_CD_USERCONFFILE, filename.latin1());
  dnd.addValue(_CD_KEYBOARD_TYPE,kbtype);
  create_new_conf(dnd, myDef);
  changeMade = true;
}

/** This member function set's KlineakConfig's definition widget. */
void KlineakConfig::setLineakDef(KlineakDef *def) {
	klineakdef = def;	
}

/* backup the conf file so we can cancel the changes */
bool KlineakConfig::backup_conffile (void) {
   KURL source, dest;
   QString name;
   QString returnMsg;
   msg("Backup conffile");
   name = (myConfig[_CD_USERCONFFILE]).c_str();
   if (verbose) cout << name << " to " << name + ".backup" << endl;
   source.setPath(name);
   dest.setPath(name + ".backup");
   KIO::file_copy(source,dest, 0755, true, true, false);
  
  return true;
}

/* restore the conf file from backup */
bool KlineakConfig::restore_conffile (void) {
   

   KURL source, dest;
   QString name;
   name = (myConfig[_CD_USERCONFFILE]).c_str();
   source.setPath(name + ".backup");
   dest.setPath(name);
   KIO::file_copy(source,dest, 0755, true, true, false);
   return true;
}

bool KlineakConfig::save_conffile (void) {
   // FIXME: Reconcile the changes in the lobjects/lkbd to the config file.
   reconcile_config();
   // Now save the thing.
   Saver tmp(myConfig.getFilename());
   if (verbose) {
      msg("Saving!");
      cout << myConfig << endl;
   }
   
   bool r = tmp.saveFile(myConfig);
   
   if (r)
      msg("Saved!");
   else
      error("Could not save the config file.");
   
   return (r);
}

void KlineakConfig::onAdvancedConfigOptionsButtonClicked(void) {
   msg("Showing Advanced Configuration Options");
   advancedKbdConfig->setData(myConfig);
   advancedKbdConfig->show();
   advancedKbdConfig->raise();
}

void KlineakConfig::onAdvancedKeyOptionsButtonClicked(void) {
   msg("onAdvancedKeyOptionsButtonClicked()");
   if (lstAvailableKeys->currentItem() == 0) 
      msg("The current item is null.");
   else {
      string key = lstAvailableKeys->currentItem()->text(0).latin1();
      //cout << "Outputting myKbd" << endl;
      //cout << myKbd << endl;
      // Get the key object
      LObject *obj = myKbd.getObject(key);
      // If we do not have a valid object, print an error.
      if (obj==NULL) {
         cerr << "Cannot find the selected key: " << key << endl;
      }
      else {
         keyopts->setObject(obj);
         if (!keyopts->isVisible())
            keyopts->show();
      }
   }
}

void KlineakConfig::reconcile_config(void) {
   //cout << "In reconcile_config()" << endl;
   // Update the keyboard in the config file.
   myConfig[_CD_KEYBOARD_TYPE] = myKbd.name;
   
   /** Remove all key configurations */
   //cout << "removing existing key configurations" << endl;
   myConfig.clearKeycomms();
   //cout << myConfig << endl;
   
   vector<string> names = myKbd.getNames();
   /** For each object in the myKbd object update the config file. */
   for (vector<string>::iterator it=names.begin(); it != names.end(); it++ ) {
      LObject * obj = myKbd.getObject(*it);
      
      // Reconcile the changes in obj to the LConfig object.
      if (!obj->isUsedAsToggle()) {
         //cout << "Object is not being used as a toggle" << endl;
         string name,smod;
         int mod = 0;
         keycommand_info inf;
         vector<keycommand_info> infvec;
          // Get a list of all modifiers.
         vector<unsigned int> mods = obj->getModifiers();
         
         if (mods.size() == 0) {
            //cout << "Zero modifiers" << endl;
            // Clear the data structure.
            inf.parsed_name = obj->getName();
            inf.modifiers = 0;
            inf.display_name = "";
            inf.command = LCommand("");
            inf.config_name = obj->getName();
            infvec.push_back(inf);
         }
         else {
            // Build a new vector<keycommand_info> with the information for each modifier
            for (vector<unsigned int>::const_iterator it = mods.begin(); it!= mods.end(); it++) {
               // get the modifier
               mod = *it;
               smod = lineak_core_functions::getModifierString(*it);
               // translate the modifier into a string
               inf.parsed_name = obj->getName();
               inf.modifiers = *it;
               inf.display_name = obj->getCommandDisplayName(*it);
               inf.command = obj->getCommand(*it);
               if ( inf.display_name != "" ) {
                  inf.config_name = "[" + inf.display_name;
                  inf.config_name +="] ";
               }
               inf.config_name += inf.parsed_name;
               if (smod != "") {
                  inf.config_name+="+";
                  inf.config_name+=smod;
               }
               infvec.push_back(inf);
               // Clear the data structure.
               inf.parsed_name = "";
               inf.modifiers = 0;
               inf.display_name = "";
               inf.command = LCommand("");
               inf.config_name = "";
            }
         }
         //cout << "Added key: " << obj->getName() << endl;
         myConfig.updateKeycommData(obj->getName(), infvec);
      }
      else {
         //cout << "Object is being used as a toggle" << endl;
         string name,tname;
         keycommand_info inf;
         vector<keycommand_info> infvec;
          // Get a list of all toggle names.
         vector<string> names = obj->getToggleNames();
          // Build a new vector<keycommand_info> with the information for each toggle name.
         for (vector<string>::const_iterator it = names.begin(); it!= names.end(); it++) {
             // get the toggle name.
             //cout << "Building a keycommand_info map for toggle key name: " << *it << endl;
            tname = *it;
             // Build the keycommand_info structure.
            inf.parsed_name = tname;
            inf.modifiers = 0;
            inf.display_name = obj->getToggleCommandDisplayName(*it);
            inf.command = obj->getToggleCommand(*it);
            if ( inf.display_name != "" ) {
               inf.config_name = "[" + inf.display_name;
               inf.config_name +="] ";
            }
            inf.config_name += inf.parsed_name;
             // Following code commented out b/c toggleable keys do not support modifiers yet.
             //if (smod != "") {
             //   inf.config_name+="+";
             //   inf.config_name+=smod;
             //}
            infvec.push_back(inf);
            //cout << "Added key: " << tname << endl;
             // Add the vector here bc with a toggleable key there will be no modifiers.
            myConfig.updateKeycommData(tname, infvec);
             // Clear the data structure.
            inf.parsed_name = "";
            inf.modifiers = 0;
            inf.display_name = "";
            inf.command = LCommand("");
            inf.config_name = "";
             // Clear the vector b/c there is only one keycommand_info structure per
             // toggleable key.
            infvec.clear();
             
             //cout << *config;
             //cout << endl;
         }
      }

      //delete(obj);
      obj = NULL;
   }
   if (verbose) {
      cout << myKbd << endl;
      cout << myConfig << endl;
   }
   //cout << "leaving reconcile_config()" << endl;
}

void signalchild(int sig_num) {
  pid_t child;

  /*   If more than one child exits at approximately the same time, the signals */
  /*   may get merged. Handle this case correctly. */
  while ((child = waitpid(-1, NULL, WNOHANG)) > 0)
  {
     if (verbose)
        cout << "Catch CHLD signal -> pid " << child << " terminated" << endl;
  }
}

void signalhup(int sig_num) {
	klineakconfig->hupLineakd();
}

void signalquit(int sig_num) {
        klineakconfig->quit();
}
