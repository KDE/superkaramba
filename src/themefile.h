/****************************************************************************
*  themefile.h - Theme file handling
*
*  Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>
*  Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>
*  Copyright (c) 2004 Petri Damstén <damu@iki.fi>
*
*  This file is part of Superkaramba.
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
#ifndef THEMEFILE_H
#define THEMEFILE_H

#include <qstring.h>
#include <qcstring.h>
#include <qfile.h>

class LineParser;
class QTextStream;
class KURL;

/**
@author See README for the list of authors
*/
class ThemeFile
{
  public:
    ThemeFile();
    ~ThemeFile();

  public:
    bool isZipTheme() const { return m_zipTheme; };
    const QString& name() const { return m_name; };
    const QString& file() const { return m_file; };
    const QString& path() const { return m_path; };
    bool exists() const;
    bool isThemeFile(const QString& filename) const;
    QByteArray readThemeFile(const QString& filename) const;

    bool set(const KURL& url);
    bool open();
    bool nextLine(LineParser& parser);
    bool close();

    static bool isZipFile(const QString& filename);

  private:
    QByteArray readZipFile(const QString& filename) const;

    QString m_path;
    bool m_zipTheme;
    QString m_file;
    QString m_name;
    QTextStream* m_stream;
    QByteArray m_ba;
    QFile m_fl;
};

#endif
