/***************************************************************************
 *   Copyright (C) 2003 by Ralph M. Churchill                              *
 *   mrchucho@yahoo.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef RSSSENSOR_H
#define RSSSENSOR_H

#include <sensor.h>
#include <qstring.h>
#include <qtextcodec.h>

/**
 *
 * Ralph M. Churchill
 **/
class RssSensor : public Sensor
{
    Q_OBJECT
public:
    RssSensor( const QString &source, int interval, const QString &format, const QString &encoding=QString::null );

    ~RssSensor();

    void update();
private:
    QTextCodec *codec;
    QString source;
    QString format;
    QString encoding;

};

#endif // RSSSENSOR_H
