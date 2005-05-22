/****************************************************************************
** Form implementation generated from reading ui file 'themelistwindow.ui'
**
** Created: Wed Jun 30 00:16:00 2004
**      by: The User Interface Compiler ($Id: themelistwindow.cpp,v 1.4 2005/05/19 04:39:22 p0z3r Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "karambaapp.h"
#include "dcopinterface_stub.h"
#include "karamba.h"
#include "karambalistboxitem.h"
#include "themelistwindow.h"
#include "karambainterface.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <klistbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a ThemeListWindow as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
ThemeListWindow::ThemeListWindow( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
  setName( "ThemeListWindow" );
    ThemeListWindowLayout = new QGridLayout( this, 1, 1, 11, 6, "ThemeListWindowLayout");

    layout5 = new QVBoxLayout( 0, 0, 6, "layout5");

    textLabel1 = new QLabel( this, "textLabel1" );
    layout5->addWidget( textLabel1 );

    lbxThemes = new KListBox( this, "lbxThemes" );
    layout5->addWidget( lbxThemes );

    layout4 = new QHBoxLayout( 0, 0, 6, "layout4");

    btnOpen = new QPushButton( this, "btnOpen" );
    layout4->addWidget( btnOpen );

    btnCloseTheme = new QPushButton( this, "btnCloseTheme" );
    layout4->addWidget( btnCloseTheme );
    layout5->addLayout( layout4 );

    layout3 = new QHBoxLayout( 0, 0, 6, "layout3");
    QSpacerItem* spacer = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout3->addItem( spacer );

    btnQuit = new QPushButton( this, "btnQuit" );
    layout3->addWidget( btnQuit );
    layout5->addLayout( layout3 );

    ThemeListWindowLayout->addLayout( layout5, 0, 0 );
    languageChange();
    resize( QSize(270, 337).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );

    // signals and slots connections
    connect( btnCloseTheme, SIGNAL( clicked() ), this, SLOT( closeTheme() ) );
    connect( lbxThemes, SIGNAL( selected(int) ), this, SLOT( themeSelected() ) );
    connect( btnQuit, SIGNAL( clicked() ), this, SLOT( quitSuperKaramba() ) );
    connect( btnOpen, SIGNAL( clicked() ), this, SLOT( openTheme() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
ThemeListWindow::~ThemeListWindow()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void ThemeListWindow::languageChange()
{
    setCaption( tr( "SuperKaramba" ) );
    textLabel1->setText( tr( "Loaded Themes:" ) );
    btnOpen->setText( tr( "Open Theme..." ) );
    btnCloseTheme->setText( tr( "Close Selected" ) );
    btnQuit->setText( tr( "Quit" ) );
}

void ThemeListWindow::openTheme()
{
    QStringList fileNames;
    fileNames = KFileDialog::getOpenFileNames(QString::null, 
                                              "*.theme *.ctheme *.skz|Themes",
                                              0,
                                              "Open Themes");
    for ( QStringList::Iterator it = fileNames.begin(); it != fileNames.end(); ++it )
    {
        QFileInfo file( *it );
        if( file.exists() )
        {
            (new karamba( *it, false ))->show();
        }
    }
}

void ThemeListWindow::closeTheme()
{
  if (lbxThemes->currentItem() != -1)
  {
    KarambaIface* iface = ((KarambaApplication*)qApp)->dcopIface();
    QString appId = ((KarambaListBoxItem*)(lbxThemes->selectedItem()))->appId;
    dcopIface_stub dcop(appId.ascii(), iface->objId());

    dcop.closeTheme(lbxThemes->selectedItem()->text());
  }
}

void ThemeListWindow::themeSelected()
{
}

void ThemeListWindow::quitSuperKaramba()
{
  KarambaApplication* app = (KarambaApplication*)qApp;
  QStringList apps = app->getKarambas();
  QStringList::Iterator it;

  for (it = apps.begin(); it != apps.end(); ++it)
  {
    dcopIface_stub dcop((*it).ascii(), app->dcopIface()->objId());
    dcop.quit();
  }
}

void ThemeListWindow::addTheme(QString appId, QString name)
{
  KarambaListBoxItem* newItem = new KarambaListBoxItem(lbxThemes, name);
  newItem->appId = appId;
}

void ThemeListWindow::removeTheme(QString, QString name)
{
  QListBox *lb = lbxThemes;

  for ( int i = 0; i < (int)lb->count(); ++i ) {
    QString appId = ((KarambaListBoxItem*)(lb->item( i )))->appId;
    if (appId == appId && lb->item(i)->text() == name) {
      lb->removeItem(i);
      break;
    }
  }
}
