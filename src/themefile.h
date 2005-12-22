/****************************************************************************
*  themefile.h - Theme file handling
*
*  Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>
*  Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>
*  Copyright (c) 2004 Petri Damstén <damu@iki.fi>
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

#include <kurl.h>
#include <qstring.h>
#include <qcstring.h>
#include <qpixmap.h>
#include <qfile.h>
#include <qvaluevector.h>

class LineParser;
class QTextStream;
class ThemeLocale;
class ZipFile;

/**
@author See README for the list of authors
*/
class ThemeFile
{
  public:
    typedef QValueVector<ThemeFile> List;

    ThemeFile(const KURL& url = KURL());
    ~ThemeFile();

    bool isZipTheme() const { return m_zipTheme; };
    const QString& name() const { return m_name; };
    const QString& version() const { return m_version; };
    const QString& license() const { return m_license; };
    const QString& id() const { return m_id; };
    const QString& mo() const { return m_mo; };
    const QString& file() const { return m_file; };
    const QString& pythonModule() const { return m_python; };
    bool pythonModuleExists() const;
    const QString& path() const { return m_path; };
    const QString& description() const { return m_description; };
    const QString& author() const { return m_author; };
    const QString& authorEmail() const { return m_authorEmail; };
    const QString& homepage() const { return m_homepage; };
    QPixmap icon() const;
    bool exists() const;
    bool isThemeFile(const QString& filename) const;
    bool isValid() const;
    QByteArray readThemeFile(const QString& filename) const;
    bool fileExists(const QString& filename) const;
    const ThemeLocale* locale() const { return m_locale; };
    bool canUninstall() const;

    bool set(const KURL& url);
    bool open();
    bool nextLine(LineParser& parser);
    bool close();

    static bool isZipFile(const QString& filename);
    static QString canonicalFile(const QString& file);

  private:
    void parseXml();
    void mkdir(QDir dir);

    QString m_path;
    bool m_zipTheme;
    QString m_file;
    QString m_id;
    QString m_mo;
    QString m_name;
    QString m_theme;
    QString m_python;
    QString m_icon;
    QString m_version;
    QString m_license;
    QTextStream* m_stream;
    QByteArray m_ba;
    QFile m_fl;
    QString m_description;
    QString m_author;
    QString m_authorEmail;
    QString m_homepage;
    ThemeLocale* m_locale;
    ZipFile* m_zip;
};

#endif
