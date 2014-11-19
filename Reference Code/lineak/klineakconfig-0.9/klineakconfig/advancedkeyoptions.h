//
// C++ Interface: advancedkeyoptions
//
// Description: 
//
//
// Author: Sheldon Lee-Wen <sheldon.leewen@cgi.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ADVANCEDKEYOPTIONS_H
#define ADVANCEDKEYOPTIONS_H

#include <frmadvancedkeyoptionsui.h>
#include <qlistbox.h>
#include <qmap.h>
#include <lineak/lobject.h>
#include <lineak/lconfig.h>
#include <lineak/lcommand.h>

/**
Advanced options for each key.

	@author Sheldon Lee-Wen <sheldon.leewen@cgi.com>
*/
class advancedKeyOptions : public frmAdvancedKeyOptionsUI
{
   Q_OBJECT
public:
    advancedKeyOptions(QWidget* parent, const char* name, bool modal, WFlags fl);
    ~advancedKeyOptions();

public:
    void setObject(LObject *iobj);
    void setMacros(vector<string> macrolist);
    void setLConfig(LConfig *iconfig);
    void setRadioRun(QString text);
    void setRadioAction(QString text, QString args="");

public slots:
    void slotRadioToggle();
    void slotApplyClicked();
    void slotClose();
    void slotSetCommand(const QString&);
    void slotSetMacro(const QString&);
    void slotSetToggleable();
    void slotSelectKeyCommand(QListViewItem*);
    //void slotSetModifiers(const QString&);
    void slotDisplayName(const QString&);
    void slotAddModifier();
    void slotRemoveModifier();
    void slotSetMacroArguments(const QString&);

signals:
    virtual void apply();
    virtual void saveConfig();
    
private:
    bool gotdata;
    bool changed;
    LObject *obj;
    LObject *obj_back;
    LConfig *config;
    // Toggle names to toggle commands
    //QMap<string,LCommand*> toggle_data;
    //QMap<string,LObject*> object_data;
    // Modifier number to modifier command
    //QMap<unsigned int,LCommand*> modifier_data;

protected:
    void update_lobject_gui_data();
    void update_keylist();
    void update_gui_data();
    void clear_gui_data();

protected:
    void setCommandGUI(LCommand *command);
};

#endif
