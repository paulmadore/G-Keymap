//
// C++ Implementation: advancedkeyoptions
//
// Description: 
//
//
// Author: Sheldon Lee-Wen <sheldon.leewen@cgi.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <qradiobutton.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qlistbox.h>
#include <qinputdialog.h>
#include <qpushbutton.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <lineak/lineak_core_functions.h>
#include "advancedkeyoptions.h"

advancedKeyOptions::advancedKeyOptions(QWidget* parent, const char* name, bool modal, WFlags fl): frmAdvancedKeyOptionsUI(parent, name, modal, fl) {
   obj = NULL;
   obj_back = NULL;
   config = NULL;
   gotdata = false;
   changed = false;
}

void advancedKeyOptions::setObject(LObject *iobj) {
   obj_back = iobj;
   obj = new LObject();
   *obj = *iobj;
   gotdata = true;
   clear_gui_data();
   update_gui_data();
}

/*!
    \fn advancedKeyOptions::setMacros(vector<string> macrolist)
    Set the list of available macros. We get this list from the pluginmanager.
*/
void advancedKeyOptions::setMacros(vector<string> macrolist) {
   for (vector<string>::iterator it = macrolist.begin(); it != macrolist.end(); it++) {
      cbAction->insertItem(QString(*it));
   }
}

/*!
    \fn advancedKeyOptions::setLConfig(LConfig *iconfig)
    Set the pointer to the LConfig object that we are using to represent the
    users config file.
*/
void advancedKeyOptions::setLConfig(LConfig *iconfig) {
   config = iconfig;
}

advancedKeyOptions::~advancedKeyOptions() {
}

/*!
    \fn advancedKeyOptions::update_gui_data()
 */
void advancedKeyOptions::update_gui_data() {
    if (gotdata) {
       // Get a list of different versions of the key (commands for different modifiers, etc.)
       update_keylist();
       // Update the GUI with the data for the selected key version.
       update_lobject_gui_data();
    }
}

/*!
    \fn advancedKeyOptions::update_lobject_gui_data()
    Update the non-key specific data in the GUI:
     key name and whether or not it is toggleable.
 */
void advancedKeyOptions::update_lobject_gui_data() {
   if (gotdata) {
      cout << *obj << endl;
      lblKeyname->setText(obj->getName());
      //Set the toggleable checkbox
      //chkIsToggleable->setChecked(obj->isUsedAsToggle());
      if (obj->isToggle()) {
         chkIsToggleable->setEnabled(true);
         if (obj->isUsedAsToggle()) {
            chkIsToggleable->setChecked(true);
            btnAddModifier->setText("Add Toggle Name");
            btnRemoveModifier->setText("Remove Toggle Name");
         }
         else {
            chkIsToggleable->setChecked(false);
            btnAddModifier->setText("Add Modifier");
            btnRemoveModifier->setText("Remove Modifier");
         }
      }
      else {
         chkIsToggleable->setChecked(false);
         chkIsToggleable->setEnabled(false);
      }
   }
}

/*!
    \fn advancedKeyOptions::update_keylist()
    Update the list of key commands in the keylist.
 */
void advancedKeyOptions::update_keylist() {   
   if (gotdata) {
      QListViewItem *qlv, *flv;
      flv = qlv = 0;

      //Fill the keylist with command names of the form 
      if (!obj->isUsedAsToggle()) {
         vector<unsigned int> modifiers = obj->getModifiers();
         QString str;

         lstKeyCommands->setColumnText(0,"Modifier");
         lstKeyCommands->setColumnWidthMode(0,QListView::Maximum);
         for (vector<unsigned int>::iterator m = modifiers.begin();
              m != modifiers.end(); m++) 
         {
            // Create a QListViewItem and add it to the list lstKeyCommands
            cout << "modifier: " << *m << " command: " << obj->getCommand(*m) << endl;
            // Get the string version of the modifier
            str = QString(obj->getModifierString(*m).c_str());
            if ( str == "" ) str = "default";
            qlv = new QListViewItem(lstKeyCommands, str, obj->getCommand(*m).getCommand());
            // Set flv to point at the default object.
            if ( str == "default" ) flv = qlv;
            //object_data[str] = obj;
         }
         // Set the default selected key.
         // If we have a default modifier then set that as selected.
         if ( flv != 0 ) {
            lstKeyCommands->setSelected(flv,true);
            slotSelectKeyCommand(flv);
            // Else set the last created modifier as the selected one.
         } else if ( qlv != 0 ) {
            lstKeyCommands->setSelected(qlv,true);
            slotSelectKeyCommand(qlv);
         } else
            cout << "advancedKeyOptions::update_keylist() 146: cannot make a key selected b/c there are no valid modifiers for this key, not even a default." << endl;
            
      }
      else {
            string s;
            vector<string> names = obj->getToggleNames();

            // Change the lstKeyCommands column 0 heading to Toggle Name
            lstKeyCommands->setColumnText(0,"Toggle Name");
            lstKeyCommands->setColumnWidthMode(0,QListView::Maximum);
            for (unsigned int i=0; i < names.size(); i++) {
               cout << "toggle name: " << obj->getNextToggleName();
               cout << " command: " << obj->getCommand();

               // Create a QListViewItem and add it to the list lstKeyCommands
               qlv = new QListViewItem(lstKeyCommands, obj->getNextToggleName(), obj->getCommand().getCommand());
               //toggle_data[obj->getNextToggleName()]= &obj->getCommand();
               //object_data[obj->getNextToggleName()] = obj;

               obj->toggleState(); 
            }
            if ( qlv != 0 ) {
               lstKeyCommands->setSelected(qlv,true);
               slotSelectKeyCommand(qlv);
            } else
               cout << "advancedKeyOptions::update_keylist() 170: cannot make a key selected b/c there are no valid toggle names for this key, not even a default." << endl;
      }
      lstKeyCommands->setColumnWidthMode(0,QListView::Maximum);
   }
}

/*!
    \fn advancedKeyOptions::slotRadioToggle
    Method to switch between the Run and Macro boxes.
    It enables one set and disables the others, etc.
 */
void advancedKeyOptions::slotRadioToggle() {
    /// User has clicked on the radio button.
    if (radioRun->isOn()) {
       cbAction->setEnabled(false);
       txtCommand->setEnabled(true);
       txtMacroArguments->setEnabled(false);
       // If the user has clicked on the radio button while editing a command.
       if (lstKeyCommands->selectedItem() != 0) {
          if ( txtCommand->text() != "" ) {
             //cout << "Updating command to: " << txtCommand->text() << endl;
             slotSetCommand(txtCommand->text());
          }
       }
    }
    if (radioAction->isOn()) {
       cbAction->setEnabled(true);
       txtCommand->setEnabled(false);
       txtMacroArguments->setEnabled(true);
       // If the user has clicked on the radio button while editing a command.
       if (lstKeyCommands->selectedItem() != 0) {
          if ( cbAction->currentText() != "" ) {
             //cout << "Updating command to: " << cbAction->currentText() << endl;
             slotSetMacro(cbAction->currentText());
          }
       }
    }
}

/*!
   Set the run text to "text" and turn on the Run
      radio button. */
void advancedKeyOptions::setRadioRun(QString text) {
    radioRun->setChecked(true);
    txtCommand->setEnabled(true);
    radioAction->setChecked(false);
    cbAction->setEnabled(false);
    txtMacroArguments->setText("");
    txtMacroArguments->setEnabled(false);

    if (txtCommand->text() != text)
       txtCommand->setText(text);
}

/*!
   Set the macro action "action" and turn on the action 
      radio button and combo box. */
void advancedKeyOptions::setRadioAction(QString action, QString args) {
    radioAction->setChecked(true);
    cbAction->setEnabled(true);
    radioRun->setChecked(false);
    txtCommand->setEnabled(false);
    txtMacroArguments->setEnabled(true);

    bool found=false;

    if (action != "") {
       //cout << "searching for action" << endl;
       //cout << "searching through: " << cbAction->count() << " items." << endl;

       for (int i=0; i < cbAction->count(); i++) {
          if (action == cbAction->text(i)) {
             cout << action << "==" << cbAction->text(i) << endl;
             cbAction->setCurrentItem(i);
             found = true;
             //cout << "Found item: " << endl;
             //txtMacroArguments->setText(args);
             slotSetMacroArguments(args);
          }
          
       }
       if (!found) {
           //cbAction->insertItem(action);
          cout << "Item not found." << endl;
       }
    }
}

/*!
    \fn advancedKeyOptions::slotClose()
    Hide the window on a close.
 */
void advancedKeyOptions::slotClose() {
    // Set the defaults for the obj pointers and close.
    obj = NULL;
    obj_back = NULL;
    changed = false;

    if ( this->isVisible() )
       this->hide();
}

/*!
    \fn advancedKeyOptions::slotApplyClicked()

    Apply the changes when the apply button is clicked.
    Changes are applied to internal data structures but 
      are not written to the config file until the 'Apply'
      button is clicked in the main GUI.
 */
void advancedKeyOptions::slotApplyClicked() {
    // If we have changed the object.
    // Return the new obj
    // Destroy the old one.
    if (changed) {
       // Reconcile the changes in obj to the LConfig object.
       //FIXME: If you have changed a key from toggleable to not or vice versa, you
       // will have to remove the old key entries and add new ones from the lconfig object.
       /** For debugging
       vector<string> keys = config->getKeycommKeys();
       // For all of the keys in the config object.
       for (vector<string>::iterator it = keys.begin(); it != keys.end(); it++) {
          cout << "keys = " << *it << endl;
       }
       vector<keycommand_info>& info = config->getKeycomm(obj_back->getName());
       cout << endl;
       cout << "keycommand_info vector has " << info.size() << " elements." << endl;
       for (vector<keycommand_info>::const_iterator it = info.begin(); it != info.end(); it++) {
          cout << "config_name = "  << it->config_name << endl; 
          cout << "parsed_name = "  << it->parsed_name << endl;
          cout << "display_name = " << it->display_name << endl;
          cout << "modifiers = "    << lineak_core_functions::getModifierString(it->modifiers) << endl;
          cout << "command = "      << it->command << endl;
       }
       */
       // Look through all of the original keys. If we do not find one that matches our obj->getName()
       // and our key is a toggleable key, we will have to search for separate entries 
       // for each toggle key command.
       // Probably the easiest way to handle toggleable keys is to remove all toggle keys and
       // forms of the toggle keys from the lconfig object and replace those entries with 
       // new entries from our obj.
       if ( obj->isToggle() ) {
          //cout << "This is a toggleable key" << endl;
          cout << *obj_back;
          
          vector<string> names = obj_back->getToggleNames();
          //cout << " obj_back has " << names.size() << " toggle names" << endl;
          // If we have toggle names.
          if ( names.size() != 0) {
             // For each toggle name, see if the config file has an entry for it.
             for (vector<string>::iterator it = names.begin(); it != names.end(); it++) 
             {
                config->removeKey(*it);
             }
          }
       }
       config->removeKey(obj_back->getName());
       
       /** For debugging 
       keys = config->getKeycommKeys();
       // For all of the keys in the config object.
       for (vector<string>::iterator it = keys.begin(); it != keys.end(); it++) {
          cout << "keys = " << *it << endl;
       }
       */
       // At this point we have removed all of the configuration from the lconfig for the key.
       
       // Updating original copy of obj.
       *obj_back = *obj;

       if (!obj->isUsedAsToggle()) {
          //cout << "Object is not being used as a toggle" << endl;
          string name,smod;
          int mod = 0;
          keycommand_info inf;
          vector<keycommand_info> infvec;
          // Get a list of all modifiers.
          vector<unsigned int> mods = obj->getModifiers();
          
          // If there are no modifiers we need to at least enter a default modifier.
          if (mods.size() == 0) {
             cout << "Zero modifiers" << endl;
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
               // Add the configuration information
               infvec.push_back(inf);
               // Clear the data structure.
               inf.parsed_name = "";
               inf.modifiers = 0;
               inf.display_name = "";
               inf.command = LCommand("");
               inf.config_name = "";
            }
          }
          config->updateKeycommData(obj->getName(), infvec);
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
             // Add the vector here bc with a toggleable key there will be no modifiers.
             config->updateKeycommData(tname, infvec);
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

       cout << "Showing new config" << endl;
       cout << *config << endl;
       cout << "Showing new obj" << endl;
       cout << *obj_back << endl;
       
       delete(obj);
       obj = NULL;
       obj_back = NULL;
       changed = false;
       
       emit apply();
    }
    
    
    if ( this->isVisible() )
       this->hide();
}

/*!
    \fn advancedKeyOptions::slotSetCommand(const QString&)

    Set a text string as the command for a key.
 */
void advancedKeyOptions::slotSetCommand(const QString& icommand) {
    /// Set the modified flag.
    changed = true;

    /// Set the command when the string is changed in the command
    /// text box.
    QString name;
    LCommand comm(icommand);

    if (lstKeyCommands->selectedItem() != 0) {
       name = lstKeyCommands->selectedItem()->text(0);
       if ( ! obj->isUsedAsToggle() ) {
          unsigned int mod = lineak_core_functions::getModifierNumericValue(name.latin1());
          if (obj->hasModifier(mod)) {
             cout << "Setting command for modifier: " << name << endl;
             obj->setCommand(comm, mod);
             lstKeyCommands->selectedItem()->setText(1,icommand);
             setCommandGUI(&comm);
          }
          else
             cout << "The modifier: " << name << " does not exist! Cannot assign a command to it." << endl;
       }
       else {
          if ( obj->ownsName(name.latin1())) {
             cout << "Setting command for toggle name: " << name << endl;
             obj->setCommand(comm, name.latin1());
             lstKeyCommands->selectedItem()->setText(1,icommand);
             setCommandGUI(&comm);
          }
          else
             cout << "The toggle name: " << name << " does not exist! Cannot assign a command to it." << endl;
       }
    }
}

/*!
    \fn advancedKeyOptions::slotSetMacro(const QString&)
    
    Set a macro as the command for a key.
 */
void advancedKeyOptions::slotSetMacro(const QString& imacro) {
    /// Set the modified flag.
    changed = true;

    cout << "Set macro to: " << imacro << endl;
    /// Set the command when the macro is changed in the command
    /// macro box.
    QString name, args, macro;
    macro = imacro;

    
    if ( txtMacroArguments->text() != "" ) {
       args = txtMacroArguments->text();
       macro+="(" + args + ")";
       cout << "Parsed macro command: " << macro << endl;
    }

    LCommand comm(macro);

    if (comm.isMacro() && (lstKeyCommands->selectedItem() != 0)) {
       name = lstKeyCommands->selectedItem()->text(0);
	if ( ! obj->isUsedAsToggle() ) {
	   unsigned int mod = lineak_core_functions::getModifierNumericValue(name.latin1());
	   if (obj->hasModifier(mod)) {
		cout << "Setting macro command for modifier: " << name << endl;
		obj->setCommand(comm, mod);
                lstKeyCommands->selectedItem()->setText(1,comm.getMacroType().c_str());
	   }
	   else
		cout << "The modifier: " << name << " does not exist! Cannot assign a macro to it." << endl;
	        //obj->setCommandDisplayName("", imods);
	   }
	else {
	   if ( obj->ownsName(name.latin1()) ) {
		cout << "Setting macro command for toggle name: " << name << endl;
		obj->setCommand(comm, name.latin1());
                lstKeyCommands->selectedItem()->setText(1,comm.getMacroType().c_str());
	   }
	   else
		cout << "The toggle name: " << name << " does not exist! Cannot assign a macro to it." << endl;
	        //obj->setToggleCommandDisplayName("", text.latin1());
	}
    }
}

/*!
    \fn advancedKeyOptions::slotSetToggleable()
 */
void advancedKeyOptions::slotSetToggleable() {
   if (gotdata) {
      
      //cout << "Not implemented yet!" << endl;
      if (obj->isToggle() ) {
         if ( obj->isUsedAsToggle() ) {
            // Change the key to be not a toggleable key.
            cout << "Set key as not toggleable" << endl;
            obj->setUsedAsToggle(false);
            obj->clearToggleData();
            
            // Add the default modifier
            if (obj->hasModifier(0)) 
               cout << "Already has a default modifier." << endl;
            else
               obj->addModifier(0);

            obj->setCommand(LCommand(""));
            obj->setCommandDisplayName("");
            changed = true;
         }
         else {
            // Change the key to be toggleable.
            cout << "Set key as toggleable" << endl;
            int index = 0;
            string tmp;
            string name = obj->getName();
            // Add a bar at the end so that our while loop parses all entries.
            name+='|';
            obj->setUsedAsToggle(true);
            obj->clearModifierData();
            obj->clearToggleData();
            cout << *obj;
            // For each part of the toggleable name
            // add a toggle name.
            while (name.find('|') != string::npos) {
               index = name.find('|');
               tmp = name.substr(0,index);
               obj->addToggleName(tmp);
               obj->setCommand(LCommand(""),tmp);
               obj->setToggleCommandDisplayName("",tmp);
               name.erase(0, index+1);
            }
            cout << *obj;
            changed = true;
         }

         clear_gui_data();
         update_gui_data();
      }
      else
         cout << "Attempting to change the toggle characteristics of a non-togglable key. You shouldn't be able to do this!" << endl;
   }
}

/*!
    \fn advancedKeyOptions::slotSetMacroArguments(const QString& args)
    
    This function is the slot that is connected to the trigger for the
    textChanged() signal from the txtMacroArguments text box in the gui.
 */
void advancedKeyOptions::slotSetMacroArguments(const QString& args ) {
   if (gotdata && (lstKeyCommands->selectedItem() != 0)) {
      /// Set the modified flag.
      changed = true;

      if (!obj->isUsedAsToggle()) {
         QString name = lstKeyCommands->selectedItem()->text(0);
         LCommand & comm = obj->getCommand(lineak_core_functions::getModifierNumericValue(name.latin1()));
         // Probably easier to get the macro type from getMacroType()
         QString scomm = comm.getMacroType();
         // rebuild the command string MACRO(args)
         QString sname = scomm;
         if (args != "" ) {
            sname+="(";
            sname+=args;
            sname+=")";
         }
            
         // set the string comm->setCommand(scomm)
         cout << "Setting the command to: " << sname << endl;
         comm.setCommand(sname);
         // Update the display text.
         lstKeyCommands->selectedItem()->setText(1,sname);
      }
      else {
         QString name = lstKeyCommands->selectedItem()->text(0);
         LCommand & comm = obj->getToggleCommand(name.latin1());
         // Probably easier to get the macro type from getMacroType()
         QString scomm = comm.getMacroType();
         // rebuild the command string MACRO(args)
         QString sname = scomm;
         if (args != "" ) {
            sname+="(";
            sname+=args;
            sname+=")";
         }
            
         // set the string comm->setCommand(scomm)
         cout << "Setting the command to: " << sname << endl;
         comm.setCommand(sname);
         // Update the display text.
         lstKeyCommands->selectedItem()->setText(1,sname);
      }
      //cout << "Not implemented yet!" << endl;
   }
}

/*!
    \fn advancedKeyOptions::slotSelectKeyCommand(QListViewItem*)
 */
void advancedKeyOptions::slotSelectKeyCommand(QListViewItem* item) {
   if (gotdata && item != NULL && item != 0) {
       /// If the key is not a toggleable key.
       if ( ! obj->isUsedAsToggle() ) {
          QString modifier_name;
          LCommand *m_command;
          unsigned int modifier;
          modifier_name = item->text(0);

          // Get the numeric modifier and string modifier name so that we can get command details.
          if (modifier_name == "default")
             modifier = 0;
          else
             modifier = lineak_core_functions::getModifierNumericValue(modifier_name);

          // Set the modifier text in the combobox.
          //txtModifiers->setText(modifier_name);
          // Set the display name in the display name text box.
          cout << "getting object information for display." << endl;
          txtDisplayName->setText(obj->getCommandDisplayName(modifier));
          // Get a pointer to the LCommand object.
          m_command = &(obj->getCommand(modifier));
          setCommandGUI(m_command);
       }
       // If this is a toggleable key.
       else {
          QString toggle_name = item->text(0);
          LCommand *t_command = NULL;
          t_command = &(obj->getToggleCommand(toggle_name));

          if (t_command != NULL) {
             //txtModifiers->setText("");
             txtDisplayName->setText(obj->getToggleCommandDisplayName(toggle_name));
          }
          setCommandGUI(t_command);
       }
   }
   else {
     cout << "slotSelectKeyCommand: item is null" << endl;
     txtDisplayName->setText("");
     setCommandGUI(0);
   }
}

/*!
   \fn advancedKeyOptions::clear_gui_data()
     clear the data from the gui.
     clear the list of key commands.
*/
void advancedKeyOptions::clear_gui_data() {
   lstKeyCommands->clear();
   //toggle_data.clear();
   //object_data.clear();
}

/*!
    \fn advancedKeyOptions::slotDisplayName(const QString&)
    Slot is called to change the display name of a key when text is
    entered or changed in the txtDisplayName text box.
 */
void advancedKeyOptions::slotDisplayName(const QString& idname) {
    /// Set the display name for an action.
    QListViewItem *lv = lstKeyCommands->selectedItem();
    
    if (lv == 0)
       cout << "lv is null!" << endl;
    else {
       /// Set the modified flag.
       changed = true;
       if (!obj->isUsedAsToggle()) {
       
          obj->setCommandDisplayName(idname.latin1(), lineak_core_functions::getModifierNumericValue(lv->text(0).latin1()));
       }
       else
          obj->setToggleCommandDisplayName(idname.latin1(), lv->text(0).latin1());
    }
    //cout << "Displaying object" << endl;
    //cout << *obj << endl;
}

/*!
    \fn advancedKeyOptions::setCommandGUI(LCommand *command)
    Setup the correct values for the widgets that show the command
    of the lobject.
 */
void advancedKeyOptions::setCommandGUI(LCommand *command) {
   if (command == 0) {
      setRadioRun("");
   }
   else {
    /// This method sets the gui options for the command itself.
          if ( command->isMacro() ) {
             cout << command->getCommand() <<  " is a macro of type: " << command->getMacroType() << endl;
             // Parse out the string arguments.
             string s_command = command->getCommand();
             string::size_type b,c;
             b = s_command.find('(');
             // If there are no macro arguments
             if (b == string::npos )
                setRadioAction( command->getMacroType() );
             else {
                // If there are macro arguments then parse them and pass them in.
                c = s_command.rfind(')');
                QString args = s_command.substr(b+1,c-(b+1)).c_str();
                cout << "command args: " << args << endl;
                setRadioAction( command->getMacroType(), args );
             }
          }
          else {
             setRadioRun( QString(command->getCommand().c_str()) );
             cout << command->getCommand() <<  " is not a macro" << endl;
          }
   }
}

/*!
    \fn advancedKeyOptions::slotAddModifier()
 */
void advancedKeyOptions::slotAddModifier() {
    cout << "slotAddModifier: pop up a dialog box to get the modifier name." << endl;
    bool ok = false;
    bool ibr = false;
    QString text = QString::null;

    /// Set the modified flag.
    changed = true;

    while ((!(ok && !text.isEmpty())) || ibr==false) {

       if (obj->isUsedAsToggle()) 
          text = QInputDialog::getText("Add a Toggle Name", "Name:", QLineEdit::Normal, QString::null, &ok, this );
       else
          text = QInputDialog::getText("Add a Modifier", "Modifier:", QLineEdit::Normal, QString::null, &ok, this );
       // user entered something and pressed OK
       if ( ok && !text.isEmpty() ) {
          // This is a toggleable key
          if (!obj->isUsedAsToggle()) {
		unsigned int imods = lineak_core_functions::getModifierNumericValue(text.latin1());
                // Modifier was invalid or we already have it.
		if (obj->hasModifier(imods)) {
		   switch (KMessageBox::warningContinueCancel(this, i18n("The modifier you have selected already exists or is invalid.\n"
									"If you wish to redefine this modifier please remove it first.\n"
									"Valid modifiers are: control, shift, alt.\n"),
								i18n("Modifier error."),
								i18n("Add a new modifier") ) ) {
			case KMessageBox::Continue :
			ok = false;
			text = "";
			break;
			case KMessageBox::Cancel :
			ibr = true;
			break;
		   }
		}
		// Is a valid modifier and we don't have this modifier yet.
		else {
			cout << "Do the right thing and add modifier: " << text << endl;
			QListViewItem *lv = new QListViewItem(lstKeyCommands, text, "");
                        cout << "adding modifier to the obj" << endl;
			obj->addModifier(imods);
			cout << "setting an empty command in the obj" << endl;
			obj->setCommand(LCommand(""), imods);
			cout << "setting the display name to null" << endl;
			obj->setCommandDisplayName("", imods);
                        ibr=true;
			cout << "calling slotSelectKeyCommand" << endl;
                        lstKeyCommands->setSelected(lv,true);
			//lstKeyCommands->clicked(lv);
                        slotSelectKeyCommand(lv);
                         
                        
		}
          }
          // For a toggleable key.
          else {
                // We have this toggle name already
                if (obj->ownsName(text.latin1())) { 
		   switch (KMessageBox::warningContinueCancel(this, i18n("The name you have selected already exists.\n"
									"If you wish to redefine this name please remove it first.\n"),
								i18n("Duplicate Name."),
								i18n("Add a new toggle name") ) ) {
			case KMessageBox::Continue :
			ok = false;
			text = "";
			break;
			case KMessageBox::Cancel :
			ibr = true;
			break;
		   }
                }
                else {
			cout << "Do the right thing and add name: " << text << endl;
			QListViewItem *lv = new QListViewItem(lstKeyCommands, text, "");
			obj->addToggleName(text.latin1());
			obj->setCommand(LCommand(""), text.latin1());
			obj->setToggleCommandDisplayName("", text.latin1());
                        ibr=true;
                        lstKeyCommands->setSelected(lv,true);
                        //lstKeyCommands->clicked(lv);
			slotSelectKeyCommand(lv);
                }
          }
       } else {
          // user entered nothing or pressed Cancel
          cout << "Nothing entered or cancel pressed." << endl;
          ibr = true;
          ok = false;
       }
       
   }
    
}

/*!
    \fn advancedKeyOptions::slotRemoveModifier()
 */
void advancedKeyOptions::slotRemoveModifier() {
    if (lstKeyCommands->selectedItem() != 0) {

    /// Set the modified flag.
    changed = true;

    cout << "slotRemoveModifier: " << lstKeyCommands->selectedItem()->text(0) << endl;
    /// Set the modifiers for a key.
    QListViewItem *lv = lstKeyCommands->selectedItem();
    QString origmod = "";
    QString mods = lstKeyCommands->selectedItem()->text(0);
    unsigned int origimod = 0;

    if (!obj->isUsedAsToggle()) { 
       cout << "Checking for valid modifier." << endl;
       // First, determine if the modifier entered is valid. If not, do nothing.
       unsigned int imods = lineak_core_functions::getModifierNumericValue(mods.latin1());
       // if imods is zero, and mods is not "" or default then we have invalid input.
       //if (imods == 0 && ( mods != "" && mods != "default" ) ) valid = false;
       // if imods is zero and mods is "" or "default then we have valid input.
       //if (imods == 0 && ( mods == "" || mods == "default" ) ) valid = true;

       // if imods is non-zero then we have valid input.
       if ( (imods != 0) || (imods == 0 && ( mods == "" || mods == "default" ) ) ) {
          LCommand command;
          cout << "We have a valid modifier." << endl;
          // store the original modifier name.
          origmod = lv->text(0);
          origimod = lineak_core_functions::getModifierNumericValue(origmod.latin1());
          command = obj->getCommand(origimod);
          cout << "Removing old items." << endl;
          // Remove the old entry from obj and the list view. Check for duplicates.
          obj->removeCommand(origimod);
          lstKeyCommands->takeItem(lv);
          //delete(lv);
          lv = 0;
       }
       
       QListViewItemIterator it( lstKeyCommands );
       while (it.current())
          it++;
       slotSelectKeyCommand(it.current());
    }
    }
}

