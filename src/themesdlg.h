/*
 * Copyright (C) 2005 Petri Damst� <petri.damsten@iki.fi>
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 ****************************************************************************/

#ifndef THEMESDLG_H
#define THEMESDLG_H

#include <KNS/Engine>

#include "ui_themes_layout.h"
#include "karambaapp.h"
#include "themewidget.h"

/**
@author See README for the list of authors
*/
class KArchiveDirectory;

class ThemesDlg : public QDialog, public Ui::ThemesLayout
{
    Q_OBJECT

public:
    explicit ThemesDlg(QWidget *parent = 0, const char *name = 0);
    ~ThemesDlg();

    int addTheme(const QString &appId, const QString &file);
    void removeTheme(const QString &appId, const QString &file, int instance);
    int addThemeToList(const QString &file);
    void addSkzThemeToDialog(const QString &file);
    void addThemeToDialog(const KArchiveDirectory *archiveDir, const QString& destDir);
    void writeNewStuffConfig(const QString &file);
    void configSanityCheck();
    bool isDownloaded(const QString &path);
    void saveUserAddedThemes();
    QStringList runningThemes();

protected slots:
    virtual void addToDesktop();
    virtual void addToDesktop(QPoint, ThemeWidget*);
    virtual void selectionChanged(int);
    virtual void openLocalTheme();
    virtual void getNewStuff();
    virtual void search(const QString& text);
    virtual void uninstall();
    virtual void installNewTheme(const QString &newTheme);

protected:
    static bool filter(int index, QWidget* widget, void* data);
    void populateListbox();
    int themeIndex(const QString &file);
    QStringList themes();

private:
    bool m_newStuffInitialized;
    QStringList m_newStuffStatus;
};

#endif
