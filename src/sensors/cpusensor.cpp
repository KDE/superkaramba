/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifdef __FreeBSD__
#include <sys/time.h>
#include <sys/dkstat.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/resource.h>
#endif

#include <QTextStream>
#include <kdebug.h>


#include "cpusensor.h"
#include "cpusensor.moc"

static KStaticDeleter<CPUSensor> cpuSensorDeleter;
CPUSensor* CPUSensor::m_self = 0;

CPUSensor* CPUSensor::self()
{
    if (!m_self)
    {
        cpuSensorDeleter.setObject(m_self, new CPUSensor());
    }

    return m_self;
}


CPUSensor::CPUSensor(int interval )
        :   Sensor(interval)
{
#ifdef __GNUC__
#warning Inefficient propgramming, no freebsd support
#endif
/* actually, we are doing here the changing of cpu0 to cpu1 and so on and then getCPULoad(), but what we should have done actually is calling getCPULoad() once, which would fillup the data Map correctly.*/
    QFile file("/proc/stat");
    QString line;
    //creating all the fields into data
    if ( file.open(QIODevice::ReadOnly | QIODevice::Text) )
    {
        QTextStream t( &file );        // use a text stream
        while( (line = t.readLine()) !=0 )
        {
            QString cpu= line.left(line.indexOf(' '));
            if(!cpu.startsWith("cpu")) break;                
            QVariantMap map;
            map["load"] =  QVariant();
            map["user"] = QVariant();
            map["nice"] = QVariant();
            map["idle"] = QVariant();
            map["system"] = QVariant();
            data[cpu]=QVariant(map);
            QVariantMap map2;
            map2["userTicks"]=0;
            map2["sysTicks"] = 0;
            map2["niceTicks"] = 0;
            map2["idleTicks"] = 0;
            oldData[cpu]= map;
        }
    }
    start();
}

CPUSensor::~CPUSensor()
{}

void CPUSensor::start()
{
    if (!m_timer.isActive())
    {
        connect (&m_timer,SIGNAL(timeout()),this,SLOT(update()));
        m_timer.start( (m_msec == 0)?1000:m_msec);
    }
}


void CPUSensor::getTicks (long &u,long &s,long &n,long &i)
{
#ifdef __FreeBSD__
    static long cp_time[CPUSTATES];

    size_t size = sizeof(cp_time);

    /* get the cp_time array */
    if (sysctlbyname("kern.cp_time", &cp_time, &size, NULL, 0) != -1)
    {
        u = cp_time[CP_USER];
        s = cp_time[CP_SYS] + cp_time[CP_INTR];
        n = cp_time[CP_NICE];
        i = cp_time[CP_IDLE];
    }
#else
    QFile file("/proc/stat");
    QString line;
    if ( file.open(QIODevice::ReadOnly | QIODevice::Text) )
    {
        QTextStream t( &file );        // use a text stream
        QRegExp rx( cpuNbr+"\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)");
        while( (line = t.readLine()) !=0 && rx.cap(0) == "" )
        {
            rx.indexIn( line );
        }
        //user
        u = rx.cap(1).toLong();
        //system
        s = rx.cap(2).toLong();
        //nice
        n = rx.cap(3).toLong();
        //idle
        i = rx.cap(4).toLong();
        file.close();
    }
#endif
    else
    {
        u = 0;
        s = 0;
        n = 0;
        i = 0;
    }
}

int CPUSensor::getCPULoad()
{
    long uTicks, sTicks, nTicks, iTicks;
    int userTicks, sysTicks, niceTicks, idleTicks;
    userTicks=oldData[cpuNbr].toMap()["userTicks"].toInt();
    sysTicks=oldData[cpuNbr].toMap()["sysTicks"].toInt();
    niceTicks=oldData[cpuNbr].toMap()["niceTicks"].toInt();
    idleTicks=oldData[cpuNbr].toMap()["idleTicks"].toInt();
    getTicks(uTicks, sTicks, nTicks, iTicks);

    const long totalTicks = ((uTicks - userTicks) + (sTicks - sysTicks) 
            +(nTicks - niceTicks) +(iTicks - idleTicks));

    int load  = (totalTicks == 0) ? 0 : (int) ( 100.0 * ( (uTicks+sTicks+nTicks) - (userTicks+sysTicks+niceTicks))/( totalTicks+0.001) + 0.5 );
    user = (totalTicks == 0) ? 0 : (int) ( 100.0 * ( uTicks - userTicks)/( totalTicks+0.001) + 0.5 );
    idle = (totalTicks == 0) ? 0 : (int) ( 100.0 * ( iTicks - idleTicks)/( totalTicks+0.001) + 0.5 );
    system = (totalTicks == 0) ? 0 : (int) ( 100.0 * ( sTicks - sysTicks)/( totalTicks+0.001) + 0.5 );
    nice = (totalTicks == 0) ? 0 : (int) ( 100.0 * ( nTicks - niceTicks)/( totalTicks+0.001) + 0.5 );
    QVariantMap map;
    map["userTicks"]=(int)uTicks;
    map["sysTicks"] = (int)sTicks;
    map["niceTicks"] = (int)nTicks;
    map["idleTicks"] = (int)iTicks;
    oldData[cpuNbr]= map;
    return load;
}

void CPUSensor::addMeter(Meter* meter)
{
    disconnect(this, SIGNAL(cpuValues(QVariant)), meter, SLOT(storeData(QVariant)));
    connect(this, SIGNAL(cpuValues(QVariant)), meter, SLOT(storeData(QVariant)));
}

void CPUSensor::update()
{
    QList<QString> cpus=data.keys();
    foreach(QString cpu,cpus)
    {
        cpuNbr=cpu;
        int load=getCPULoad();
        QVariantMap map;
        map["load"]=QVariant(load);
        map["user"]=QVariant(user);
        map["nice"]=QVariant(nice);
        map["idle"]=QVariant(idle);
        map["system"]=QVariant(system);
        data[cpu]=map;
    }
    emit cpuValues(QVariant(data));
}
/*
void CPUSensor::setMaxValue( SensorParams *sp )
{
    Meter *meter;
    meter = sp->getMeter();
    meter->setMax( 100 );
}
*/
