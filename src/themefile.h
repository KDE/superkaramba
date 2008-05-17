/****************************************************************************
*  themefile.h - Theme file handling
*
*  Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>
*  Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>
*  Copyright (c) 2004 Petri Damst� <damu@iki.fi>
*
*  This file is part of SuperKaramba.
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
#ifndef THEMEFILE_H
#define THEMEFILE_H

#include <KUrl>

#include <QTextStream>
#include <QDir>

#include "superkaramba_export.h"
class LineParser;
class ThemeLocale;
class ZipFile;

/**
@author See README for the list of authors
*/

class SUPERKARAMBA_EXPORT ThemeFile
{
public:
    typedef QVector<ThemeFile> List;

    ThemeFile(const KUrl& url = KUrl());
    ~ThemeFile();

    bool isZipTheme() const;
    const QString& name() const;
    const QString& version() const;
    const QString& license() const;
    const QString& id() const;
    const QString& mo() const;
    const QString& file() const;
    const QString& scriptModule() const;
    bool scriptModuleExists() const;
    const QString& path() const;
    const QString& description() const;
    const QString& author() const;
    const QString& authorEmail() const;
    const QString& homepage() const;
    QPixmap icon() const;
    QString iconName() const;
    bool exists() const;
    bool isThemeFile(const QString& filename) const;
    bool isValid() const;
    QByteArray readThemeFile(const QString& filename) const;
    bool fileExists(const QString& filename) const;
    const ThemeLocale* locale() const;
    bool canUninstall() const;
    KUrl getUrlPath();

    bool set(const KUrl& url);
    bool open();
    bool nextLine(LineParser& parser);
    bool close();

    QString extractArchive() const;
    bool extractArchiveTo(const QString& path);
    bool extractArchiveFileTo(const QString& file, const QString& path);
    bool copyArchiveTo(const QString& path);

    static bool isZipFile(const QString& filename);
    static QString canonicalFile(const QString& file);

private:
    void parseXml();
    void mkdir(QDir dir);

    class Private;
    Private* const d;
};

#endif
