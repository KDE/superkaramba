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
#include <qstring.h>
#include <qobject.h>
#include <qstringlist.h>
#include <qmap.h>
#include <qtimer.h>
#include <QList>

#include "sensorparams.h"

class Sensor : public QObject
{
    Q_OBJECT

public:
    Sensor(int msec=2000);
    virtual ~Sensor();

//    void start();
    /* we will discuss if addMeter/deleteMeter has any benefit. Right now after attachToSensor(), it is only a burden */
    void deleteMeter( Meter *meter );
    virtual void addMeter( Meter * );
    virtual int isEmpty()
    {
        return 0;
    };

signals:
    void valueChanged(QVariant);

protected:
    int m_msec;
    QTimer m_timer;

public slots:
//    virtual void update()=0;

};

#endif // SENSOR_H
