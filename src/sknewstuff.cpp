/*
 * Copyright (C) 2005 Ryan Nickell <p0z3r @ earthlink . net>
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

#include <kapplication.h>
#include <kdebug.h>

#include "themesdlg.h"
#include "sknewstuff.h"

SKNewStuff::SKNewStuff( ThemesDlg *dlg ) :
  KNewStuff( "superkaramba/themes", dlg ),
  mDlg( dlg )
{
}

bool SKNewStuff::install( const QString &fileName )
{
  kdDebug() << "SKNewStuff::install(): " << fileName << endl;
  return false;
}

bool SKNewStuff::createUploadFile( const QString &fileName )
{
  kdDebug() << "SKNewStuff::createUploadFile(): " << fileName << endl;
  return false;
}
