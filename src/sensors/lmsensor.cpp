/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "lmsensor.h"

SensorSensor::SensorSensor(int interval, char tempUnit) : Sensor(interval)
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
    if (tempUnit == 'F')
        extraParams = " -f";
    connect(&ksp, SIGNAL(receivedStdout(K3Process *, char *, int)),
            this, SLOT(receivedStdout(K3Process *, char *, int)));
    connect(&ksp, SIGNAL(processExited(K3Process *)),
            this, SLOT(processExited(K3Process *)));

    // readValues();
}


SensorSensor::~SensorSensor()
{}

void SensorSensor::receivedStdout(K3Process *, char *buffer, int len)
{
    buffer[len] = 0;
    sensorResult += QString(buffer);
}

void SensorSensor::processExited(K3Process *)
{
    QStringList stringList = sensorResult.split('\n');
    sensorResult = "";
    QStringList::Iterator it = stringList.begin();
#if defined __FreeBSD__ || defined(Q_OS_NETBSD)
    QRegExp rx("^(\\S+)\\s+:\\s+[\\+\\-]?(\\d+\\.?\\d*)");
#else
    QRegExp rx("^(.+):\\s+[\\+\\-]?(\\d+\\.?\\d*)");
#endif
    while (it != stringList.end()) {
        rx.indexIn(*it);

        if (!rx.cap(0).isEmpty()) {
            sensorMap[rx.cap(1)] = rx.cap(2);
        }
        it++;
    }

    QString format;
    QString type;
    SensorParams *sp;
    Meter *meter;

    QObject *lobject;
    foreach(lobject, *objList) {
        sp = (SensorParams*)(lobject);
        meter = sp->getMeter();
        format = sp->getParam("FORMAT");
        type = sp->getParam("TYPE");

        if (type.length() == 0)
            type = "temp2";

        if (format.length() == 0) {
            format = "%v";
        }

#if defined __FreeBSD__ || defined(Q_OS_NETBSD)
        format.replace(QRegExp("%v", Qt::CaseInsensitive),
                       sensorMap[sensorMapBSD[type]]);
#else
        format.replace(QRegExp("%v", Qt::CaseInsensitive),
                       sensorMap[type]);
#endif
        meter->setValue(format);
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
    ksp.start(K3Process::NotifyOnExit, K3ProcIO::Stdout);
}


#include "lmsensor.moc"
