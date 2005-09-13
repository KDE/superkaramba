/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "sensorsensor.h"
#include <QByteArray>

SensorSensor::SensorSensor(int interval, char tempUnit) : Sensor( interval )
{
#ifdef __FreeBSD__
    sensorMapBSD["VCore 1"] = "VC0";
    sensorMapBSD["VCore 2"] = "VC1";
    sensorMapBSD["+3.3V"] = "V33";
    sensorMapBSD["+5V"] = "V50P";
    sensorMapBSD["+12V"] = "V12P";
    sensorMapBSD["-12V"] = "V12N";
    sensorMapBSD["-5V"] = "V50N";
    sensorMapBSD["fan1"] = "FAN0";
    sensorMapBSD["fan2"] = "FAN1";
    sensorMapBSD["fan3"] = "FAN2";
    sensorMapBSD["temp1"] = "TEMP0";
    sensorMapBSD["temp2"] = "TEMP1";
    sensorMapBSD["temp3"] = "TEMP2";
#endif
    if(tempUnit == 'F')
      extraParams = " -f";
    connect(&ksp, SIGNAL(receivedStdout(KProcess *, char *, int )),
            this,SLOT(receivedStdout(KProcess *, char *, int )));
    connect(&ksp, SIGNAL(processExited(KProcess *)),
            this,SLOT(processExited( KProcess * )));

    // readValues();
}


SensorSensor::~SensorSensor()
{
}

void SensorSensor::addMeter(Meter* meter)
{
    connect(this, SIGNAL(sensorValues(QVariant)), meter, SLOT(update(QVariant)));
}

void SensorSensor::receivedStdout(KProcess *, char *buffer, int len )
{
    buffer[len] = 0;
    sensorResult += QString( QByteArray(buffer) );
}

void SensorSensor::processExited(KProcess *)
{
    QStringList stringList = QStringList::split('\n',sensorResult);
    sensorResult = "";
    QStringList::Iterator it = stringList.begin();
#ifdef __FreeBSD__
    QRegExp rx( "^(\\S+)\\s+:\\s+[\\+\\-]?(\\d+\\.?\\d*)");
#else
    QRegExp rx( "^(.+):\\s+[\\+\\-]?(\\d+\\.?\\d*)");
#endif
    while( it != stringList.end())
    {
        rx.search( *it );

        if ( !rx.cap(0).isEmpty())
        {
            sensorMap[rx.cap(1)] = QVariant(rx.cap(2));
        }
        it++;
    }

#ifdef __FreeBSD__
    //format.replace( QRegExp("%v", false), sensorMap[sensorMapBSD[type]]);
    emit sensorValues(QVariant(sensorMap));
#else
    emit sensorValues(QVariant(sensorMap));
#endif
}

void SensorSensor::update()
{
    ksp.clearArguments();
#ifdef __FreeBSD__
    ksp << "mbmon -r -c 1" << extraParams;
#else
    ksp << "sensors" << extraParams;
#endif
    ksp.start( KProcess::NotifyOnExit,KProcIO::Stdout);
}


#include "sensorsensor.moc"