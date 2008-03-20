/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "disksensor.h"

#include <qfile.h>
#include <qtextstream.h>
#include <qstring.h>
#include <qregexp.h>
#include <kprocess.h>
#include <kprocio.h>

DiskSensor::DiskSensor( int msec ) : Sensor( msec )
{
    connect(&ksp, SIGNAL(receivedStdout(KProcess *, char *, int )),
            this,SLOT(receivedStdout(KProcess *, char *, int )));
    connect(&ksp, SIGNAL(processExited(KProcess *)),
            this,SLOT(processExited( KProcess * )));

    // update values on startup
    ksp.clearArguments();
    ksp << "df";
    ksp.start( KProcIO::Block,KProcIO::Stdout);
    init = 1;
}
DiskSensor::~DiskSensor()
{}

long DiskSensor::getFreeSpace(QString mntPt) const
{
    QRegExp rx( "^\\S*\\s*\\d+\\s+\\d+\\s+(\\d+)");
    rx.search(mntMap[mntPt]);
    return rx.cap(1).toLong();
}

long DiskSensor::getUsedSpace(QString mntPt) const
{
    QRegExp rx( "^\\S*\\s*\\d+\\s+(\\d+)\\s+\\d+");
    rx.search(mntMap[mntPt]);
    return rx.cap(1).toLong();
}

long DiskSensor::getTotalSpace(QString mntPt) const
{

    QRegExp rx( "^\\S*\\s*(\\d+)\\s+\\d+\\s+\\d+");
    rx.search(mntMap[mntPt]);

    return rx.cap(1).toLong();

}

int DiskSensor::getPercentUsed(QString mntPt) const
{
    QRegExp rx( "\\s+(\\d+)%\\s+");
    rx.search(mntMap[mntPt]);
    return rx.cap(1).toInt();
}

int DiskSensor::getPercentFree(QString mntPt) const
{
    return ( 100 - getPercentUsed( mntPt ) );
}

void DiskSensor::receivedStdout(KProcess *, char *buffer, int len )
{

    buffer[len] = 0;
    sensorResult += QString( QCString(buffer) );

}

void DiskSensor::processExited(KProcess *)
{
    QStringList stringList = QStringList::split('\n',sensorResult);
    sensorResult = "";
    QStringList::Iterator it = stringList.begin();
    //QRegExp rx( "^(/dev/).*(/\\S*)$");
    QRegExp rx( ".*\\s+(/\\S*)$");

    while( it != stringList.end())
    {
        rx.search( *it );
        if ( !rx.cap(0).isEmpty())
        {
            mntMap[rx.cap(1)] = *it;
        }
        it++;
    }
    stringList.clear();

    QString format;
    QString mntPt;
    SensorParams *sp;
    Meter *meter;

    QObjectListIt lit( *objList );
    while (lit != 0)
    {
        sp = (SensorParams*)(*lit);
        meter = sp->getMeter();
        format = sp->getParam("FORMAT");
        mntPt = sp->getParam("MOUNTPOINT");
        if (mntPt.length() == 0)
            mntPt="/";

        if (format.length() == 0 )
        {
            format = "%u";
        }
        format.replace( QRegExp("%fp", false),QString::number(getPercentFree(mntPt)));
        format.replace( QRegExp("%fg",false),
                        QString::number(getFreeSpace(mntPt)/(1024*1024)));
        format.replace( QRegExp("%fkb",false),
                        QString::number(getFreeSpace(mntPt)*8) );
        format.replace( QRegExp("%fk",false),
                        QString::number(getFreeSpace(mntPt)) );
        format.replace( QRegExp("%f", false),QString::number(getFreeSpace(mntPt)/1024));
        
        format.replace( QRegExp("%up", false),QString::number(getPercentUsed(mntPt)));
        format.replace( QRegExp("%ug",false),
                        QString::number(getUsedSpace(mntPt)/(1024*1024)));
        format.replace( QRegExp("%ukb",false),
                        QString::number(getUsedSpace(mntPt)*8) );
        format.replace( QRegExp("%uk",false),
                        QString::number(getUsedSpace(mntPt)) );
        format.replace( QRegExp("%u", false),QString::number(getUsedSpace(mntPt)/1024));

        format.replace( QRegExp("%tg",false),
                        QString::number(getTotalSpace(mntPt)/(1024*1024)));
        format.replace( QRegExp("%tkb",false),
                        QString::number(getTotalSpace(mntPt)*8));
        format.replace( QRegExp("%tk",false),
                        QString::number(getTotalSpace(mntPt)));
        format.replace( QRegExp("%t", false),QString::number(getTotalSpace(mntPt)/1024));
        meter->setValue(format);
        ++lit;
    }
    if ( init == 1 )
    {
        emit initComplete();
        init = 0;
    }
}

void DiskSensor::update()
{
    ksp.clearArguments();
    ksp << "df";
    ksp.start( KProcIO::NotifyOnExit,KProcIO::Stdout);
}

void DiskSensor::setMaxValue( SensorParams *sp )
{
    Meter *meter;
    meter = sp->getMeter();
    const QString mntPt = sp->getParam( "MOUNTPOINT" );

    QString f;
    f = sp->getParam("FORMAT");
    if( f == "%fp" || f == "%up" )
        meter->setMax( 100 );
    else
        meter->setMax( getTotalSpace( mntPt ) / 1024 );
}



#include "disksensor.moc"
