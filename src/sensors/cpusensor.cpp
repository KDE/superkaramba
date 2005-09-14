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

#include "cpusensor.h"
#include "cpusensor.moc"

CPUSensor::CPUSensor( QString cpu, int interval )
        :   Sensor(interval),
        userTicks(0),
        sysTicks(0),
        niceTicks(0),
        idleTicks(0)
{
    cpuNbr = cpu;
    QRegExp rx("^\\d+$");
    if( rx.search( cpu.lower() ) == -1)
    {
        cpuNbr = "";
    }
    cpuNbr = "cpu"+cpuNbr;
    getCPULoad();
}

CPUSensor::~CPUSensor()
{}

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
    if ( file.open(IO_ReadOnly | IO_Translate) )
    {
        QTextStream t( &file );        // use a text stream
        QRegExp rx( cpuNbr+"\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)");
        line = t.readLine();
        rx.search( line );
        while( (line = t.readLine()) !=0 && rx.cap(0) == "" )
        {
            rx.search( line );
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

    getTicks(uTicks, sTicks, nTicks, iTicks);

    const long totalTicks = ((uTicks - userTicks) +
                             (sTicks - sysTicks) +
                             (nTicks - niceTicks) +
                             (iTicks - idleTicks));

    int load  = (totalTicks == 0) ? 0 : (int) ( 100.0 * ( (uTicks+sTicks+nTicks) - (userTicks+sysTicks+niceTicks))/( totalTicks+0.001) + 0.5 );
    user = (totalTicks == 0) ? 0 : (int) ( 100.0 * ( uTicks - userTicks)/( totalTicks+0.001) + 0.5 );
    idle = (totalTicks == 0) ? 0 : (int) ( 100.0 * ( iTicks - idleTicks)/( totalTicks+0.001) + 0.5 );
    system = (totalTicks == 0) ? 0 : (int) ( 100.0 * ( sTicks - sysTicks)/( totalTicks+0.001) + 0.5 );
    nice = (totalTicks == 0) ? 0 : (int) ( 100.0 * ( nTicks - niceTicks)/( totalTicks+0.001) + 0.5 );

    userTicks = uTicks;
    sysTicks = sTicks;
    niceTicks = nTicks;
    idleTicks = iTicks;

    return load;
}

void CPUSensor::addMeter(Meter* meter)
{
    disconnect(0, 0, meter, SLOT(update(QVariant)));
    connect(this, SIGNAL(cpuValues(QVariant)), meter, SLOT(update(QVariant)));
}

void CPUSensor::update()
{
    QMap<QString, QVariant> map;

    int load = getCPULoad();

    map["%load"] = QVariant(load);
    map["%user"] = QVariant(user);
    map["%nice"] = QVariant(nice);
    map["%idle"] = QVariant(idle);
    map["%system"] = QVariant(system);
    map["%v"] = QVariant(load);

    emit cpuValues(QVariant(map));
}
/*
void CPUSensor::setMaxValue( SensorParams *sp )
{
    Meter *meter;
    meter = sp->getMeter();
    meter->setMax( 100 );
}
*/
