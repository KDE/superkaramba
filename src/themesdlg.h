/*
 * Copyright (C) 2005 Petri Damstén <petri.damsten@iki.fi>
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

#ifndef THEMESDLG_H
#define THEMESDLG_H

#include <themes_layout.h>

/**
@author See README for the list of authors
*/
class ThemesDlg : public ThemesLayout
{
    Q_OBJECT

  public:
    ThemesDlg(QWidget *parent = 0, const char *name = 0);
    ~ThemesDlg();

    void addTheme(const QString &appId, const QString &file);
    void removeTheme(const QString &appId, const QString &file);

  public slots:
    virtual void quitSuperKaramba();

  protected slots:
    virtual void addToDesktop();
    virtual void selectionChanged(int);
    virtual void openLocalTheme();
    virtual void getNewStuff();

  protected:
    void populateListbox();
    int themeIndex(QString file);
    void saveUserAddedThemes();
    QStringList themes();
};

#endif
