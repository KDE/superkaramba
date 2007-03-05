/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "sensor.h"

#include <kdebug.h>

Sensor::Sensor(int iMsec)
{
    objList = new QList<QObject*>();
    msec = iMsec;
}

void Sensor::start()
{
    if (!timer.isActive()) {
        connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
        timer.start((msec == 0) ? 1000 : msec);
    }
}

Sensor::~Sensor()
{
    timer.stop();

    while (!objList->isEmpty())
        delete objList->takeFirst();

    delete objList;
}

void Sensor::addMeter(SensorParams *sp)
{
    objList->append(sp);
}

SensorParams* Sensor::hasMeter(Meter *meter)
{
    QObject *it;
    foreach(it, *objList) {
        if ((qobject_cast<SensorParams*>(it))->getMeter() == meter) {
            return qobject_cast<SensorParams*>(it);
        }
    }

    return NULL;
}

void Sensor::deleteMeter(Meter *meter)
{
    SensorParams* sp = hasMeter(meter);

    if (sp) {
        objList->removeAll(sp);
        delete sp;
    }
}

void Sensor::setMaxValue(SensorParams*)
{}

#include "sensor.moc"
