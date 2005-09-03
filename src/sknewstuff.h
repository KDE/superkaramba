/*
 * Copyright (C) 2005 Ryan Nickell <p0z3r @ earthlink . net>
 *
 * This file is part of SuperKaramba.
 *
 *  SuperKaramba is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  SuperKaramba is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SuperKaramba; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ****************************************************************************/

#ifndef SKNEWSTUFF_H
#define SKNEWSTUFF_H

#include "karambaapp.h"
#include "knewstuff/knewstuff.h"
#include "knewstuff/entry.h"

class ThemesDlg;
class KArchiveDirectory;

class SKNewStuff : public KNewStuff
{
  public:
    SKNewStuff( ThemesDlg * );

    bool install( const QString &fileName );
    bool createUploadFile( const QString &fileName );
    QString downloadDestination( KNS::Entry *entry );

  protected:
    void addThemes(const KArchiveDirectory *archiveDir,
                   const QString& destDir);

  private:
    ThemesDlg *mDlg;
};

#endif //KDE_3_3
