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

void WelcomeForm::downloadResult(KIO::Job*)
{
  //do something eventually!
  textLabel1->setSource("file://" + QDir::home().absPath() + "/.superkaramba/newestThemes.html");
  textLabel1->reload();
  //  disp->show();
}

void WelcomeForm::linkClicked ( const QString & link )
{
  KRun::runURL( KURL( link ), "text/html" );
  textLabel1->setSource("file://" + QDir::home().absPath() + "/.superkaramba/newestThemes.html");

}

