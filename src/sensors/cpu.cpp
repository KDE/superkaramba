/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "cpu.h"

#include <QTextStream>
#include <QFile>

#ifdef __FreeBSD__
#include <sys/time.h>
#include <sys/dkstat.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/resource.h>
#endif

#if defined(Q_OS_NETBSD)
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/sched.h>
#endif

CPUSensor::CPUSensor(const QString &cpu, int interval) :
        Sensor(interval), userTicks(0), sysTicks(0), niceTicks(0), idleTicks(0)
{
    cpuNbr = cpu;
    QRegExp rx("^\\d+$");
    if (rx.indexIn(cpu.toLower()) == -1)
        cpuNbr = "";
    cpuNbr = "cpu" + cpuNbr;
    getCPULoad();
}

CPUSensor::~CPUSensor()
{}

void CPUSensor::getTicks(long &u, long &s, long &n, long &i)
{
#ifdef __FreeBSD__
    static long cp_time[CPUSTATES];

    size_t size = sizeof(cp_time);

    /* get the cp_time array */
    if (sysctlbyname("kern.cp_time", &cp_time, &size, NULL, 0) != -1) {
        u = cp_time[CP_USER];
        s = cp_time[CP_SYS] + cp_time[CP_INTR];
        n = cp_time[CP_NICE];
        i = cp_time[CP_IDLE];
    }
#else
#if defined(Q_OS_NETBSD)
    static uint64_t cp_time[CPUSTATES];

    size_t size = sizeof(cp_time);

    /* get the cp_time array */
    if (sysctlbyname("kern.cp_time", &cp_time, &size, NULL, 0) != -1) {
        u = cp_time[CP_USER];
        s = cp_time[CP_SYS] + cp_time[CP_INTR];
        n = cp_time[CP_NICE];
        i = cp_time[CP_IDLE];
    }
#else
    QFile file("/proc/stat");
    QString line;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream t(&file);          // use a text stream
        QRegExp rx(cpuNbr + "\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)");
        line = t.readLine();
        rx.indexIn(line);

        while ((line = t.readLine()) != 0 && rx.cap(0).isEmpty()) {
            rx.indexIn(line);
        }
        //user
        u = rx.cap(1).toLong();
        //nice
        n = rx.cap(2).toLong();
        //system
        s = rx.cap(3).toLong();
        //idle
        i = rx.cap(4).toLong();
        file.close();
    }
#endif
#endif
    else {
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

    int load  = (totalTicks == 0) ? 0 : (int)(100.0 * ((uTicks + sTicks + nTicks) - (userTicks + sysTicks + niceTicks)) / (totalTicks + 0.001) + 0.5);
    user = (totalTicks == 0) ? 0 : (int)(100.0 * (uTicks - userTicks) / (totalTicks + 0.001) + 0.5);
    idle = (totalTicks == 0) ? 0 : (int)(100.0 * (iTicks - idleTicks) / (totalTicks + 0.001) + 0.5);
    system = (totalTicks == 0) ? 0 : (int)(100.0 * (sTicks - sysTicks) / (totalTicks + 0.001) + 0.5);
    nice = (totalTicks == 0) ? 0 : (int)(100.0 * (nTicks - niceTicks) / (totalTicks + 0.001) + 0.5);
    suload = (totalTicks == 0) ? 0 : (int)(100.0 * ((uTicks + sTicks) - (userTicks + sysTicks)) / (totalTicks+0.001) + 0.5);

    userTicks = uTicks;
    sysTicks = sTicks;
    niceTicks = nTicks;
    idleTicks = iTicks;

    return load;
}

void CPUSensor::update()
{
    SensorParams *sp;
    Meter *meter;
    QString format;
    int load = getCPULoad();

    QObject *it;
    foreach(it, *objList) {
        sp = qobject_cast<SensorParams*>(it);
        meter = sp->getMeter();
        format = sp->getParam("FORMAT");

        if (format.length() == 0) {
            format = QString::number(load);
            meter->setValue(format);
            continue;
        }

        int index = 0;

        index = format.indexOf("%load", 0, Qt::CaseInsensitive);
        if (index != -1)
            format.replace(index, 5, QString::number(load));

        index = format.indexOf("%v", 0, Qt::CaseInsensitive);
        if (index != -1)
            format.replace(index, 2, QString::number(load));

        index = format.indexOf("%user", 0, Qt::CaseInsensitive);
        if (index != -1)
            format.replace(index, 5, QString::number(user));

        index = format.indexOf("%nice", 0, Qt::CaseInsensitive);
        if (index != -1)
            format.replace(index, 5, QString::number(nice));

        index = format.indexOf("%system", 0, Qt::CaseInsensitive);
        if (index != -1)
            format.replace(index, 7, QString::number(system));

        index = format.indexOf("%idle", 0, Qt::CaseInsensitive);
        if (index != -1)
          format.replace(index, 5, QString::number(idle));

        index = format.indexOf("%suload", 0, Qt::CaseInsensitive);
        if (index != -1)
          format.replace(index, 7, QString::number(suload));

        meter->setValue(format);
    }
}

void CPUSensor::setMaxValue(SensorParams *sp)
{
    Meter *meter;
    meter = sp->getMeter();
    meter->setMax(100);
}

#include "cpu.moc"
