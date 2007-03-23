/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <QStringList>

#include "program.h"

ProgramSensor::ProgramSensor(const QString &progName, int interval, QString encoding)
        : Sensor(interval)
{
    if (!encoding.isEmpty()) {
        codec = QTextCodec::codecForName(encoding.toAscii().constData());
        if (codec == 0)
            codec = QTextCodec::codecForLocale();
    } else
        codec = QTextCodec::codecForLocale();


    programName = progName;
    //update();
    connect(&ksp, SIGNAL(receivedStdout(KProcess *, char *, int)),
            this, SLOT(receivedStdout(KProcess *, char *, int)));
    connect(&ksp, SIGNAL(processExited(KProcess *)),
            this, SLOT(processExited(KProcess *)));
}

ProgramSensor::~ProgramSensor()
{}

void ProgramSensor::receivedStdout(KProcess *, char *buffer, int len)
{
    buffer[len] = 0;
    sensorResult += codec->toUnicode(buffer);
}

void ProgramSensor::processExited(KProcess *)
{
    int lineNbr;
    SensorParams *sp;
    Meter *meter;
    QVector<QString> lines;
    QStringList stringList = sensorResult.split('\n');
    QStringList::ConstIterator end(stringList.end());
    for (QStringList::ConstIterator it = stringList.begin(); it != end; ++it) {
        lines.push_back(*it);
    }

    int count = (int) lines.size();
    QObject *object;
    foreach(object, *objList) {
        sp = (SensorParams*)(object);
        meter = sp->getMeter();
        if (meter != 0) {
            lineNbr = (sp->getParam("LINE")).toInt();
            if (lineNbr >= 1  && lineNbr <= (int) count) {
                meter->setValue(lines[lineNbr-1]);
            }
            if (-lineNbr >= 1 && -lineNbr <= (int) count) {
                meter->setValue(lines[count+lineNbr]);
            }
            if (lineNbr == 0) {
                meter->setValue(sensorResult);
            }
        }
    }

    sensorResult = "";
}

void ProgramSensor::update()
{
    ksp.clearArguments();
    ksp << programName;

    ksp.start(KProcIO::NotifyOnExit, KProcIO::Stdout);
}

#include "program.moc"
