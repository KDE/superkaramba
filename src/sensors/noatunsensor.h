/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef NOATUNSENSOR_H
#define NOATUNSENSOR_H

#include "sensor.h"
#include <QRegExp>
#include <QDataStream>
#include <qstringlist.h>
#include <QByteArray>

/**
@author Hans Karlsson
*/
class NoatunSensor : public Sensor
{
    Q_OBJECT
public:
    NoatunSensor( int interval, DCOPClient *client);

    ~NoatunSensor();
    void setMaxValue( SensorParams *);
    void addMeter(Meter*);
    static bool isSingleton() { return false; }

signals:
    void noatunValues(QVariant);


private:
    QByteArray noatunID;

    bool isRunning();
    QString getTitle();
    int getTime();
    int getLength();


    DCOPClient *client;
    QByteArray appId;
public slots:
    void update();
    
};

#endif
