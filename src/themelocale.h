/*
 * Copyright (c) 2005 Petri Damsten <damu@iki.fi>
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
#ifndef THEMELOCALE_H
#define THEMELOCALE_H

#include <qstring.h>
#include <qstringlist.h>

class ThemeFile;

/**
@author See README for the list of authors
*/

struct sk_kde_loaded_l10nfile
{
  int decided;
  const void *data;
  sk_kde_loaded_l10nfile() : decided(0), data(0) {}
};

class ThemeLocale
{
  public:
    ThemeLocale(ThemeFile* theme);
    ~ThemeLocale();

    QString translate(QString text) const;
    void setLanguage(const QStringList &languages);
    QString language() const { return m_language; };

    static QStringList languageList();

  private:
    sk_kde_loaded_l10nfile m_domain;
    ThemeFile* m_theme;
    QString m_language;

    void unload();
};



#endif
