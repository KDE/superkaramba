/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef UPTIMESENSOR_H
#define UPTIMESENSOR_H
#include "sensor.h"

#include <qfile.h>
#include <qtextstream.h>
#include <qstring.h>
#include <qregexp.h>
#include <qdatetime.h>
#include <kstaticdeleter.h>

class UptimeSensor :  public Sensor
{
    Q_OBJECT
public:
    void update();
    void addMeter(Meter*);
    static UptimeSensor* self();
    static bool isSingleton() { return true; }

signals:
    void uptimeValues(QVariant);
private:
    friend class KStaticDeleter<UptimeSensor>;
    QVariantMap data;
    UptimeSensor(int interval=2000);
    virtual ~UptimeSensor();
    static UptimeSensor* m_self;
};

#endif // UPTIMESENSOR_H
