/*****************************************************************

Copyright (c) 1996-2000 the kicker authors. See file AUTHORS.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#include <qfileinfo.h>
#include <kdesktopfile.h>
#include <kapplication.h>

#include "themeinfo.h"

class ThemeInfo::Private
{
public:
    Private()
      : type(Undefined),
        unique(true),
        hidden(false)
    {}

    QString name;
    QString comment;
    QString icon;
    QString lib;
    QString desktopFile;
    QString configFile;
    QString desktopFilePath;
    ThemeType type;
    bool unique;
    bool hidden;
};

ThemeInfo::ThemeInfo( const QString& deskFile, const QString& configFile, const ThemeInfo::ThemeType type)
{
    d = new Private;
    d->type = type;
    QFileInfo fi(deskFile);
    d->desktopFilePath = fi.absFilePath();
    d->desktopFile = fi.fileName();

    KDesktopFile df(deskFile);

    // set the themessimple attributes
    setName(df.readName());
    setComment(df.readComment());
    setIcon(df.readIcon());

    // library
    setLibrary(df.readEntry("X-KDE-Library"));

    // is it a unique theme?
    setIsUnique(df.readBoolEntry("X-KDE-UniqueTheme", false));

    // should it be shown in the gui?
    d->hidden = df.readBoolEntry("Hidden", false);

    if (configFile.isEmpty())
    {
        // generate a config file base name from the library name
        d->configFile = d->lib.lower();

        if (d->unique)
        {
            d->configFile.append("rc");
        }
        else
        {
            d->configFile.append("_")
                        .append(kapp->randomString(20).lower())
                        .append("_rc");
        }
    }
    else
    {
        d->configFile = configFile;
    }
}

ThemeInfo::ThemeInfo(const ThemeInfo &copy)
{
    d = new Private;
    *d = *copy.d;
}

ThemeInfo::~ThemeInfo()
{
    delete d;
}

ThemeInfo& ThemeInfo::operator=(const ThemeInfo &rhs)
{
    *d = *rhs.d;

    return *this;
}

QString ThemeInfo::name() const
{
    return d->name;
}

QString ThemeInfo::comment() const
{
    return d->comment;
}

QString ThemeInfo::icon() const
{
    return d->icon;
}

ThemeInfo::ThemeType ThemeInfo::type() const
{
    return d->type;
}

QString ThemeInfo::library() const
{
    return d->lib;
}

QString ThemeInfo::desktopFilePath() const
{
    return d->desktopFilePath;
}

QString ThemeInfo::desktopFile() const
{
    return d->desktopFile;
}

QString ThemeInfo::configFile() const
{
    return d->configFile;
}

bool ThemeInfo::isUniqueTheme() const
{
    return d->unique;
}

bool ThemeInfo::isHidden() const
{
    return d->hidden;
}

void ThemeInfo::setConfigFile(const QString &cf)
{
    d->configFile = cf;
}

void ThemeInfo::setType(ThemeType type)
{
    d->type = type;
}

void ThemeInfo::setName(const QString &name)
{
    d->name = name;
}

void ThemeInfo::setComment(const QString &comment)
{
    d->comment = comment;
}

void ThemeInfo::setIcon(const QString &icon)
{
    d->icon = icon;
}

void ThemeInfo::setLibrary(const QString &lib)
{
   d->lib = lib; 
}

void ThemeInfo::setIsUnique(bool u)
{
    d->unique = u;
}

bool ThemeInfo::operator!=( const ThemeInfo& rhs) const
{
    return configFile() != rhs.configFile();
}

bool ThemeInfo::operator==( const ThemeInfo& rhs) const
{
    return configFile() == rhs.configFile();
}

bool ThemeInfo::operator<( const ThemeInfo& rhs ) const
{
    return name().lower() < rhs.name().lower();
}

bool ThemeInfo::operator> ( const ThemeInfo& rhs ) const
{
    return name().lower() > rhs.name().lower();
}

bool ThemeInfo::operator<= ( const ThemeInfo& rhs ) const
{
    return name().lower() <= rhs.name().lower();
}

