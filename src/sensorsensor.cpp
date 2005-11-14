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
#include <qglobal.h>

SensorSensor::SensorSensor(int interval, char tempUnit) : Sensor( interval )
{
#if defined __FreeBSD__ || defined(Q_OS_NETBSD)
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

void SensorSensor::receivedStdout(KProcess *, char *buffer, int len )
{
    buffer[len] = 0;
    sensorResult += QString( QCString(buffer) );
}

void SensorSensor::processExited(KProcess *)
{
    QStringList stringList = QStringList::split('\n',sensorResult);
    sensorResult = "";
    QStringList::Iterator it = stringList.begin();
#if defined __FreeBSD__ || defined(Q_OS_NETBSD)
    QRegExp rx( "^(\\S+)\\s+:\\s+[\\+\\-]?(\\d+\\.?\\d*)");
#else
    QRegExp rx( "^(.+):\\s+[\\+\\-]?(\\d+\\.?\\d*)");
#endif
    while( it != stringList.end())
    {
        rx.search( *it );

        if ( !rx.cap(0).isEmpty())
        {
            sensorMap[rx.cap(1)] = rx.cap(2);
        }
        it++;
    }

    QString format;
    QString type;
    SensorParams *sp;
    Meter *meter;

    QObjectListIt lit( *objList );
    while (lit != 0)
    {
        sp = (SensorParams*)(*lit);
        meter = sp->getMeter();
        format = sp->getParam("FORMAT");
        type = sp->getParam("TYPE");

        if (type.length() == 0)
            type = "temp2";

        if (format.length() == 0 )
        {
            format = "%v";
        }

#if defined __FreeBSD__ || defined(Q_OS_NETBSD)
        format.replace( QRegExp("%v", false), sensorMap[sensorMapBSD[type]]);
#else
        format.replace( QRegExp("%v", false), sensorMap[type]);
#endif
        meter->setValue(format);
        ++lit;
    }
}

void SensorSensor::update()
{
    ksp.clearArguments();
#if defined __FreeBSD__ || defined(Q_OS_NETBSD)
    ksp << "mbmon -r -c 1" << extraParams;
#else
    ksp << "sensors" << extraParams;
#endif
    ksp.start( KProcess::NotifyOnExit,KProcIO::Stdout);
}


#include "sensorsensor.moc"
