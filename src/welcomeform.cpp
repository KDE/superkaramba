/*
 * Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>
 * Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>
 * Copyright (c) 2005 Petri Damsten <damu@iki.fi>
 *
 * This file is part of Superkaramba.
 *
 *  Superkaramba is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Superkaramba is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Superkaramba; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ****************************************************************************/

#include "welcomeform.h"
#include <krun.h>
#include <qtextbrowser.h>
#include <qdir.h>

WelcomeForm::WelcomeForm(QWidget* parent, const char* name,
                         bool modal, WFlags fl)
  : WelcomeFormLayout(parent, name, modal, fl)
{
  //Try to get new themes list without blocking execution
  //    KIO::Scheduler::checkSlaveOnHold(true);

  KIO::Job * job = KIO::file_copy("http://www.superkaramba.com/listing.php",
                                  QDir::home().absPath() +
                                  "/.superkaramba/newestThemes.html",
                                  -1, true, false, false);
  job->setWindow (this);

  connect(job, SIGNAL(result(KIO::Job *)),
          this, SLOT( downloadResult(KIO::Job *)));
}

void WelcomeForm::downloadResult(KIO::Job*)
{
  //do something eventually!
  textLabel1->setSource("file://" + QDir::home().absPath() +
                        "/.superkaramba/newestThemes.html");
  textLabel1->reload();
  //  disp->show();
}

void WelcomeForm::linkClicked ( const QString & link )
{
  KRun::runURL( KURL( link ), "text/html" );
  textLabel1->setSource("file://" + QDir::home().absPath() +
                        "/.superkaramba/newestThemes.html");
}

void WelcomeForm::exitButton_clicked()
{
  close();
}

void WelcomeForm::helpButton_clicked()
{
  KRun::runURL(KURL( "http://netdragon.sourceforge.net/index.php?page=Help" ),
               "text/html" );
}

void WelcomeForm::downloadButton_clicked()
{
  KRun::runURL(KURL( "http://www.superkaramba.com/" ), "text/html" );
}

void WelcomeForm::openButton_clicked()
{
  accept();
}

#include "welcomeform.moc"
