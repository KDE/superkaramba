/*
 * Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>
 * Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>
 * Copyright (c) 2005 Ryan Nickell <p0z3r@earthlink.net>
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
#include "lineparser.h"

#include <QRegExp>

LineParser::LineParser(const QString& line)
{
    set(line);
}

LineParser::~LineParser()
{}

void LineParser::set(const QString& line)
{
    QRegExp rx("^\\s*(\\S+)");
    m_line = line;

    rx.indexIn(m_line);
    m_meter = rx.cap(1).toUpper();
}

int LineParser::getInt(const QString &w, int def) const
{
    QRegExp rx("\\W+" + w + "=([-]?\\d+)", Qt::CaseInsensitive);
    if (rx.indexIn(m_line) != -1)
        return rx.cap(1).toInt();
    else
        return def;
}

QColor LineParser::getColor(const QString &w, const QColor &def) const
{
    QRegExp rx("\\W+" + w + "=([-]?\\d+),([-]?\\d+),([-]?\\d+)(,([-]?\\d+))?", Qt::CaseInsensitive);
    if (rx.indexIn(m_line) != -1) {
        int alpha = 255;
        QString alphaString = rx.cap(4).remove(0, 1);
        if (!alphaString.isEmpty()) {
            alpha = alphaString.toInt();
        }
        return QColor(rx.cap(1).toInt(), rx.cap(2).toInt(), rx.cap(3).toInt(), alpha);
    } else {
        return def;
    }
}

QString LineParser::getString(const QString &w, const QString &def) const
{
    QString result;
    QRegExp rx("\\W+" + w + "=\"([^\"]*)\"", Qt::CaseInsensitive);

    bool found = (rx.indexIn(m_line) == -1) ? false : true;
    if (rx.cap(1).isEmpty()) {
        rx = QRegExp(w + "=(\\S+)", Qt::CaseInsensitive);
        found = (rx.indexIn(m_line) == -1) ? false : true;
        result = rx.cap(1);
    } else {
        result = rx.cap(1);
    }

    result.replace("%quote", "\"");

    if (found)
        return result;
    else
        return def;
}

bool LineParser::getBoolean(const QString &w, bool def) const
{
    QString boolean = getString(w, "-").toLower();
    if (boolean == "-")
        return def;
    else if (boolean == "true") // true / false
        return true;
    else if (boolean == "1") // 1 / 0
        return true;
    else if (boolean == "on") // on / off
        return true;
    return false;
}
