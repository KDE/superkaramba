/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef SENSOR_H
#define SENSOR_H

#include <QTimer>

#include "sensorparams.h"

class Sensor : public QObject
{
    Q_OBJECT

public:
    Sensor(int msec = 1000);
    void start();
    virtual ~Sensor();
    void addMeter(SensorParams *s);
    SensorParams* hasMeter(const Meter *meter) const;
    void deleteMeter(Meter *meter);
    int isEmpty()
    {
        return objList->isEmpty();
    }
    virtual void setMaxValue(SensorParams *s);

private:
    int msec;
    QTimer timer;

protected:
    QList <QObject*> *objList;

public slots:
    virtual void update() = 0;

};

#endif // SENSOR_H
