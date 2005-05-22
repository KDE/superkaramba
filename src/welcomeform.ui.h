/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/
#include <qstringlist.h>
#include "karamba.h"

void WelcomeForm::exitButton_clicked()
{
  close();
}

void WelcomeForm::helpButton_clicked()
{
  KRun::runURL( KURL( "http://netdragon.sourceforge.net/index.php?page=Help" ), "text/html" );
}

void WelcomeForm::downloadButton_clicked()
{
  KRun::runURL( KURL( "http://www.superkaramba.com/" ), "text/html" );
}

void WelcomeForm::openButton_clicked()
{
  
  accept();
}
