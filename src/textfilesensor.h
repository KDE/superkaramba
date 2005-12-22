/*
 * Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>
 * Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>
 * Copyright (c) 2005 Ryan Nickell <p0z3r@earthlink.net>
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
#ifndef TEXTFILESENSOR_H
#define TEXTFILESENSOR_H


#include <sensor.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qstring.h>
#include <qtextcodec.h>
#include <qvaluevector.h>
/**
 *
 * Hans Karlsson
 **/
class TextFileSensor : public Sensor
{

Q_OBJECT
public:
    TextFileSensor( const QString &fileName, bool rdf, int interval, const QString &encoding=QString::null );

    ~TextFileSensor();

    void update();

private:
QTextCodec *codec;
QString fileName;
bool rdf;
};

#endif // TEXTFILESENSOR_H
