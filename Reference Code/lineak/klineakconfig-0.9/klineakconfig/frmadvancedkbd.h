/****************************************************************************
** Form interface generated from reading ui file '/home/sheldonl/development/klineakconfig/klineakconfig/frmadvancedkbd.ui'
**
** Created: Mon Mar 6 14:05:39 2006
**      by: The User Interface Compiler ($Id: frmadvancedkbd.h,v 1.2 2006/03/13 17:55:22 sheldonl Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef FRMADVANCEDKBDUI_H
#define FRMADVANCEDKBDUI_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QPushButton;
class QGroupBox;
class QLabel;
class KListView;
class QListViewItem;
class QCheckBox;
class QLineEdit;

class frmAdvancedKbdUI : public QDialog
{
    Q_OBJECT

public:
    frmAdvancedKbdUI( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~frmAdvancedKbdUI();

    QPushButton* btnApply;
    QPushButton* btnCancel;
    QGroupBox* groupBox3;
    QLabel* textLabel2;
    KListView* lstConfigDirectives;
    QCheckBox* chkRunLineakAtExit;
    QLabel* TextLabel1;
    QLineEdit* txtContributeEmail;
    QCheckBox* chkLeds;

public slots:
    virtual void slotBtnOK();
    virtual void slotchkLeds();
    virtual void slotContributeEmail(const QString&);
    virtual void slotRunLineakd();
    virtual void slotRunKkeyled();
    virtual void slotConfigDirectivesItemRenamed(QListViewItem*,int,const QString&);
    virtual void slotCancel();

protected:
    QGridLayout* frmAdvancedKbdUILayout;
    QHBoxLayout* Layout50;
    QSpacerItem* Spacer1;
    QGridLayout* groupBox3Layout;
    QHBoxLayout* layout32;
    QSpacerItem* spacer16;

protected slots:
    virtual void languageChange();

};

#endif // FRMADVANCEDKBDUI_H
