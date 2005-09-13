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
#include <QVector>
#include <QByteArray>

#include "programsensor.h"
#include "programsensor.moc"

ProgramSensor::ProgramSensor(const QString &progName, int interval, QString encoding )
: Sensor( interval )
{
     if( !encoding.isEmpty())
    {
        codec = QTextCodec::codecForName( encoding.ascii() );
        if ( codec == 0)
            codec = QTextCodec::codecForLocale();
    }
    else
        codec = QTextCodec::codecForLocale();


    programName = progName;
    //update();
    connect(&ksp, SIGNAL(receivedStdout(KProcess *, char *, int )),
            this,SLOT(receivedStdout(KProcess *, char *, int )));
    connect(&ksp, SIGNAL(processExited(KProcess *)),
            this,SLOT(processExited( KProcess * )));
}

ProgramSensor::~ProgramSensor()
{}

void ProgramSensor::addMeter(Meter* meter)
{
    disconnect(0, 0, meter, SLOT(update(QVariant)));
    connect(this, SIGNAL(programValues(QVariant)), meter, SLOT(update(QVariant)));
}

void ProgramSensor::receivedStdout(KProcess *, char *buffer, int len)
{
    buffer[len] = 0;
    sensorResult += codec->toUnicode( QByteArray(buffer) );
}

void ProgramSensor::processExited(KProcess *)
{
    QStringList lines;
    QStringList stringList = QStringList::split('\n',sensorResult,true);
    QStringList::ConstIterator end( stringList.end() );
    for ( QStringList::ConstIterator it = stringList.begin(); it != end; ++it )
    {
        lines << *it;
    }

    emit programValues(QVariant(lines));

    sensorResult = "";
}

void ProgramSensor::update()
{
    ksp.clearArguments();
    ksp << programName;


    ksp.start( KProcIO::NotifyOnExit,KProcIO::Stdout);
}

