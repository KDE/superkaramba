/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef XMMSSENSOR_H
#define XMMSSENSOR_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qdatetime.h>
#include <qregexp.h>
#include <qtextcodec.h>

#include "sensor.h"

class XMMSSensor :  public Sensor
{
    Q_OBJECT
public:
    XMMSSensor( int interval, const QString &encoding=QString::null );
    ~XMMSSensor();
   
    void setMaxValue( SensorParams *);
    void addMeter(Meter*);
    static bool isSingleton() { return false; }

signals:
    void xmmsValues(QVariant);

private:
    QTextCodec *codec;
public slots:
    void update();

};


#endif // XMMSSENSOR_H
