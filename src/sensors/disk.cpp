/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "disk.h"

#include <qfile.h>
#include <QTextStream>
#include <qstring.h>
#include <qregexp.h>
//Added by qt3to4:
#include <QString>
#include <kprocess.h>
#include <kprocio.h>
#include <kdebug.h>

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

int DiskSensor::getFreeSpace(QString mntPt) const
{
    QRegExp rx( "^\\S*\\s*\\d+\\s+\\d+\\s+(\\d+)");
    rx.indexIn(mntMap[mntPt]);
    return rx.cap(1).toInt();
}

int DiskSensor::getUsedSpace(QString mntPt) const
{
    QRegExp rx( "^\\S*\\s*\\d+\\s+(\\d+)\\s+\\d+");
    rx.indexIn(mntMap[mntPt]);
    return rx.cap(1).toInt();
}

int DiskSensor::getTotalSpace(QString mntPt) const
{

    QRegExp rx( "^\\S*\\s*(\\d+)\\s+\\d+\\s+\\d+");
    rx.indexIn(mntMap[mntPt]);

    return rx.cap(1).toInt();

}

int DiskSensor::getPercentUsed(QString mntPt) const
{
    QRegExp rx( "\\s+(\\d+)%\\s+");
    rx.indexIn(mntMap[mntPt]);
    return rx.cap(1).toInt();
}

int DiskSensor::getPercentFree(QString mntPt) const
{
    return ( 100 - getPercentUsed( mntPt ) );
}

void DiskSensor::receivedStdout(KProcess *, char *buffer, int len )
{

    buffer[len] = 0;
    sensorResult += QString( buffer );

}

void DiskSensor::processExited(KProcess *)
{
    QStringList stringList = sensorResult.split('\n');
    sensorResult = "";
    QStringList::Iterator it = stringList.begin();
    //QRegExp rx( "^(/dev/).*(/\\S*)$");
    QRegExp rx( ".*\\s+(/\\S*)$");

    while( it != stringList.end())
    {
        rx.indexIn( *it );
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

    QObject *lit;
    foreach(lit, *objList)
    {
      sp = qobject_cast<SensorParams*>(lit);
      meter = sp->getMeter();
      format = sp->getParam("FORMAT");
      mntPt = sp->getParam("MOUNTPOINT");
      if (mntPt.length() == 0)
        mntPt="/";

      if (format.length() == 0 )
      {
        format = "%u";
      }
      format.replace( QRegExp("%fp", Qt::CaseInsensitive),QString::number(getPercentFree(mntPt)));
      format.replace( QRegExp("%fg",Qt::CaseInsensitive),
                      QString::number(getFreeSpace(mntPt)/(1024*1024)));
      format.replace( QRegExp("%fkb",Qt::CaseInsensitive),
                      QString::number(getFreeSpace(mntPt)*8) );
      format.replace( QRegExp("%fk",Qt::CaseInsensitive),
                      QString::number(getFreeSpace(mntPt)) );
      format.replace( QRegExp("%f", Qt::CaseInsensitive),QString::number(getFreeSpace(mntPt)/1024));

      format.replace( QRegExp("%up", Qt::CaseInsensitive),QString::number(getPercentUsed(mntPt)));
      format.replace( QRegExp("%ug",Qt::CaseInsensitive),
                      QString::number(getUsedSpace(mntPt)/(1024*1024)));
      format.replace( QRegExp("%ukb",Qt::CaseInsensitive),
                      QString::number(getUsedSpace(mntPt)*8) );
      format.replace( QRegExp("%uk",Qt::CaseInsensitive),
                      QString::number(getUsedSpace(mntPt)) );
      format.replace( QRegExp("%u", Qt::CaseInsensitive),QString::number(getUsedSpace(mntPt)/1024));

      format.replace( QRegExp("%tg",Qt::CaseInsensitive),
                      QString::number(getTotalSpace(mntPt)/(1024*1024)));
      format.replace( QRegExp("%tkb",Qt::CaseInsensitive),
                      QString::number(getTotalSpace(mntPt)*8));
      format.replace( QRegExp("%tk",Qt::CaseInsensitive),
                      QString::number(getTotalSpace(mntPt)));
      format.replace( QRegExp("%t", Qt::CaseInsensitive),QString::number(getTotalSpace(mntPt)/1024));

      meter->setValue(format);
    }

    if ( init == 1 )
    {
      emit initComplete();
      init = 0;
    }
}

void DiskSensor::update()
{
    if(ksp.isRunning())
      return;

    ksp.clearArguments();
    ksp << "df";
    ksp.start(KProcIO::NotifyOnExit, KProcIO::Stdout);
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



#include "disk.moc"