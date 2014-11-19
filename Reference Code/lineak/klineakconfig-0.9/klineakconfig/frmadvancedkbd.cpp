#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './frmadvancedkbd.ui'
**
** Created: Tue Mar 14 11:40:09 2006
**      by: The User Interface Compiler ($Id: frmadvancedkbd.cpp,v 1.3 2006/03/14 16:56:32 sheldonl Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "frmadvancedkbd.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qheader.h>
#include <klistview.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a frmAdvancedKbdUI as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
frmAdvancedKbdUI::frmAdvancedKbdUI( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "frmAdvancedKbdUI" );
    frmAdvancedKbdUILayout = new QGridLayout( this, 1, 1, 11, 6, "frmAdvancedKbdUILayout"); 

    Layout50 = new QHBoxLayout( 0, 0, 6, "Layout50"); 
    Spacer1 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout50->addItem( Spacer1 );

    btnApply = new QPushButton( this, "btnApply" );
    Layout50->addWidget( btnApply );

    btnCancel = new QPushButton( this, "btnCancel" );
    Layout50->addWidget( btnCancel );

    frmAdvancedKbdUILayout->addLayout( Layout50, 1, 0 );

    groupBox3 = new QGroupBox( this, "groupBox3" );
    QFont groupBox3_font(  groupBox3->font() );
    groupBox3->setFont( groupBox3_font ); 
    groupBox3->setColumnLayout(0, Qt::Vertical );
    groupBox3->layout()->setSpacing( 6 );
    groupBox3->layout()->setMargin( 11 );
    groupBox3Layout = new QGridLayout( groupBox3->layout() );
    groupBox3Layout->setAlignment( Qt::AlignTop );

    textLabel2 = new QLabel( groupBox3, "textLabel2" );
    QFont textLabel2_font(  textLabel2->font() );
    textLabel2->setFont( textLabel2_font ); 
    textLabel2->setLineWidth( 2 );

    groupBox3Layout->addWidget( textLabel2, 4, 0 );

    lstConfigDirectives = new KListView( groupBox3, "lstConfigDirectives" );
    lstConfigDirectives->addColumn( tr2i18n( "Property" ) );
    lstConfigDirectives->addColumn( tr2i18n( "Value" ) );
    lstConfigDirectives->setFrameShape( KListView::LineEditPanel );
    lstConfigDirectives->setFrameShadow( KListView::Plain );
    lstConfigDirectives->setLineWidth( 2 );
    lstConfigDirectives->setAllColumnsShowFocus( TRUE );
    lstConfigDirectives->setResizeMode( KListView::AllColumns );
    lstConfigDirectives->setItemsMovable( FALSE );
    lstConfigDirectives->setItemsRenameable( FALSE );
    lstConfigDirectives->setDragEnabled( FALSE );

    groupBox3Layout->addWidget( lstConfigDirectives, 5, 0 );

    chkRunLineakAtExit = new QCheckBox( groupBox3, "chkRunLineakAtExit" );

    groupBox3Layout->addWidget( chkRunLineakAtExit, 0, 0 );

    TextLabel1 = new QLabel( groupBox3, "TextLabel1" );

    groupBox3Layout->addWidget( TextLabel1, 2, 0 );

    layout32 = new QHBoxLayout( 0, 0, 6, "layout32"); 

    txtContributeEmail = new QLineEdit( groupBox3, "txtContributeEmail" );
    txtContributeEmail->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 1, 0, txtContributeEmail->sizePolicy().hasHeightForWidth() ) );
    layout32->addWidget( txtContributeEmail );
    spacer16 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout32->addItem( spacer16 );

    groupBox3Layout->addLayout( layout32, 3, 0 );

    chkLeds = new QCheckBox( groupBox3, "chkLeds" );

    groupBox3Layout->addWidget( chkLeds, 1, 0 );

    frmAdvancedKbdUILayout->addWidget( groupBox3, 0, 0 );
    languageChange();
    resize( QSize(411, 387).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( txtContributeEmail, SIGNAL( textChanged(const QString&) ), this, SLOT( slotContributeEmail(const QString&) ) );
    connect( chkRunLineakAtExit, SIGNAL( clicked() ), this, SLOT( slotRunLineakd() ) );
    connect( chkLeds, SIGNAL( clicked() ), this, SLOT( slotchkLeds() ) );
    connect( btnCancel, SIGNAL( clicked() ), this, SLOT( slotCancel() ) );
    connect( btnApply, SIGNAL( clicked() ), this, SLOT( slotBtnOK() ) );
    connect( lstConfigDirectives, SIGNAL( itemRenamed(QListViewItem*,int,const QString&) ), this, SLOT( slotConfigDirectivesItemRenamed(QListViewItem*,int,const QString&) ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
frmAdvancedKbdUI::~frmAdvancedKbdUI()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void frmAdvancedKbdUI::languageChange()
{
    setCaption( tr2i18n( "Advanced Keyboard Options" ) );
    btnApply->setText( tr2i18n( "Apply" ) );
    btnCancel->setText( tr2i18n( "Close" ) );
    groupBox3->setTitle( tr2i18n( "Advanced Keyboard Configuration Options" ) );
    textLabel2->setText( tr2i18n( "Configuration Directives:" ) );
    lstConfigDirectives->header()->setLabel( 0, tr2i18n( "Property" ) );
    lstConfigDirectives->header()->setLabel( 1, tr2i18n( "Value" ) );
    chkRunLineakAtExit->setText( tr2i18n( "Continue to run lineak after exiting." ) );
    TextLabel1->setText( tr2i18n( "Email address to send my customized keyboard to:" ) );
    chkLeds->setText( tr2i18n( "Use kkeyled to show keyboard LEDs.", "If you do not have LED's on your keyboard, and kkeyled is installed, use kkeyled to show them." ) );
}

void frmAdvancedKbdUI::slotBtnOK()
{
    qWarning( "frmAdvancedKbdUI::slotBtnOK(): Not implemented yet" );
}

void frmAdvancedKbdUI::slotchkLeds()
{
    qWarning( "frmAdvancedKbdUI::slotchkLeds(): Not implemented yet" );
}

void frmAdvancedKbdUI::slotContributeEmail(const QString&)
{
    qWarning( "frmAdvancedKbdUI::slotContributeEmail(const QString&): Not implemented yet" );
}

void frmAdvancedKbdUI::slotRunLineakd()
{
    qWarning( "frmAdvancedKbdUI::slotRunLineakd(): Not implemented yet" );
}

void frmAdvancedKbdUI::slotRunKkeyled()
{
    qWarning( "frmAdvancedKbdUI::slotRunKkeyled(): Not implemented yet" );
}

void frmAdvancedKbdUI::slotConfigDirectivesItemRenamed(QListViewItem*,int,const QString&)
{
    qWarning( "frmAdvancedKbdUI::slotConfigDirectivesItemRenamed(QListViewItem*,int,const QString&): Not implemented yet" );
}

void frmAdvancedKbdUI::slotCancel()
{
    qWarning( "frmAdvancedKbdUI::slotCancel(): Not implemented yet" );
}

#include "frmadvancedkbd.moc"
