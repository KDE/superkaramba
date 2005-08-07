/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "programsensor.h"

#include <qstringlist.h>
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

void ProgramSensor::receivedStdout(KProcess *, char *buffer, int len)
{
    buffer[len] = 0;
    sensorResult += codec->toUnicode( QCString(buffer) );
}

void ProgramSensor::processExited(KProcess *)
{
    int lineNbr;
    SensorParams *sp;
    Meter *meter;
    QValueVector<QString> lines;
    QStringList stringList = QStringList::split('\n',sensorResult,true);
    QStringList::ConstIterator end( stringList.end() );
    for ( QStringList::ConstIterator it = stringList.begin(); it != end; ++it )
    {
        lines.push_back(*it);
    }

    int count = (int) lines.size();
    QObjectListIt it( *objList );
    while (it != 0)
    {
        sp = (SensorParams*)(*it);
        meter = sp->getMeter();
        if( meter != 0)
        {
            lineNbr = (sp->getParam("LINE")).toInt();
            if ( lineNbr >= 1  && lineNbr <=  (int) count )
            {
                meter->setValue(lines[lineNbr-1]);
            }
            if ( -lineNbr >= 1 && -lineNbr <= (int) count )
            {
                meter->setValue(lines[count+lineNbr]);
            }
            if (lineNbr == 0)
            {
                meter->setValue(sensorResult);
            }
        }
        ++it;
    }

    sensorResult = "";
}

void ProgramSensor::update()
{
    ksp.clearArguments();
    ksp << programName;


    ksp.start( KProcIO::NotifyOnExit,KProcIO::Stdout);
}

#include "programsensor.moc"
