/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <QByteArray>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QRegExp>

#include <kprocess.h>
#include <kprocio.h>

#include "disksensor.h"
#include "disksensor.moc"

static KStaticDeleter<DiskSensor> diskSensorDeleter;
DiskSensor* DiskSensor::m_self = 0;

DiskSensor* DiskSensor::self()
{
    if (!m_self)
    {
        diskSensorDeleter.setObject(m_self, new DiskSensor());
    }

    return m_self;
}

DiskSensor::DiskSensor(int msec)
        :   Sensor(msec)
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

void DiskSensor::addMeter(Meter* meter)
{
    disconnect(this, SIGNAL(diskValues(QVariant )), meter, SLOT(storeData(QVariant)));
    connect(this, SIGNAL(diskValues(QVariant)), meter, SLOT(storeData(QVariant)));
}

int DiskSensor::getFreeSpace(QString mntPt) const
{
    QRegExp rx("^\\S*\\s*\\d+\\s+\\d+\\s+(\\d+)");
    rx.search(mntMap[mntPt]);

    return rx.cap(1).toInt();
}

int DiskSensor::getUsedSpace(QString mntPt) const
{
    QRegExp rx("^\\S*\\s*\\d+\\s+(\\d+)\\s+\\d+");
    rx.search(mntMap[mntPt]);

    return rx.cap(1).toInt();
}

int DiskSensor::getTotalSpace(QString mntPt) const
{
    QRegExp rx("^\\S*\\s*(\\d+)\\s+\\d+\\s+\\d+");
    rx.search(mntMap[mntPt]);

    return rx.cap(1).toInt();
}

int DiskSensor::getPercentUsed(QString mntPt) const
{
    QRegExp rx("\\s+(\\d+)%\\s+");
    rx.search(mntMap[mntPt]);

    return rx.cap(1).toInt();
}

int DiskSensor::getPercentFree(QString mntPt) const
{
    return (100 - getPercentUsed( mntPt ));
}

void DiskSensor::receivedStdout(KProcess *, char *buffer, int len )
{
    buffer[len] = 0;
    sensorResult += QString(QByteArray(buffer));
}

void DiskSensor::processExited(KProcess *)
{
    //QRegExp rx( "^(/dev/).*(/\\S*)$");
    QRegExp rx( ".*\\s+(/\\S*)$");
    sensorResult = "";
    data.clear();

    QStringList stringList = QStringList::split('\n',sensorResult);
    foreach (QString it, stringList)
    {
        rx.indexIn(it);
        if (!rx.cap(0).isEmpty())
        {
            mntMap[rx.cap(1)] = it;
            data[rx.cap(1)]=QVariant();
        }
    }
    stringList.clear();

    const QList<QString> mntPts= data.keys();
    foreach(QString mntPt, mntPts)
    {
        QMap<QString,QVariant> map;
        map["fp"] = getPercentFree(mntPt);
        map["fg"] = getFreeSpace(mntPt)/(1024*1024);
        map["fkb"] = getFreeSpace(mntPt)*8;
        map["fk"] = getFreeSpace(mntPt);
        map["f"] = getFreeSpace(mntPt)/1024;
        map["up"] = getPercentUsed(mntPt);
        map["ug"] = getUsedSpace(mntPt)/(1024*1024);
        map["ukb"] = getUsedSpace(mntPt)*8;
        map["uk"] = getUsedSpace(mntPt);
        map["u"] = getUsedSpace(mntPt)/1024;
        map["tg"] = getTotalSpace(mntPt)/(1024*1024);
        map["tkb"] = getTotalSpace(mntPt)*8;
        map["tk"] = getTotalSpace(mntPt);
        map["t"] = getTotalSpace(mntPt)/1024;
        data[mntPt]=QVariant(map);
    }

    emit diskValues(QVariant(data));

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
/*
void DiskSensor::setMaxValue( SensorParams *sp )
{
    Meter *meter;
    meter = sp->getMeter();
    const QString mntPt = sp->getParam( "MOUNTPOINT" );
 
    QString f;
    f = sp->getParam("FORMAT");
    if( f == "%fp" || f == "%up" )
    {
        meter->setMax( 100 );
    }
    else
    {
        meter->setMax( getTotalSpace( mntPt ) / 1024 );
    }
}
 
*/
