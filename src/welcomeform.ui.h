/****************************************************************************
 * ui.h extension file, included from the uic-generated form implementation.
 *
 * If you wish to add, delete or rename functions or slots use
 * Qt Designer which will update this file, preserving your code. Create an
 * init() function in place of a constructor, and a destroy() function in
 * place of a destructor.
 *
 * Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>
 * Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>
 * Copyright (c) 2005 Ryan Nickell <p0z3r@earthlink.net>
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
