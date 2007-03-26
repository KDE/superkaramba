/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef MEMSENSOR_H
#define MEMSENSOR_H

#include <K3Process>

#ifdef __FreeBSD__
#include <k3procio.h>
#include <kvm.h>
#include <osreldate.h>
#endif

#include "sensor.h"

class MemSensor :  public Sensor
{
    Q_OBJECT
public:

    MemSensor(int interval);
    ~MemSensor();

    int getMemTotal();
    int getMemFree();
    int getBuffers();
    int getCached();

    int getSwapTotal();
    int getSwapFree();

    void update();
    void setMaxValue(SensorParams *sp);
    QString getMemLine();

private:
    QString meminfo;
    void readValues();
#if defined __FreeBSD__ || defined(Q_OS_NETBSD)
    int pageshift;           /* log base 2 of the pagesize */
    QString sensorResult;
    int swapTotal;
    int swapUsed;
# if (defined(__FreeBSD__) && __FreeBSD_version < 500018)
    K3ShellProcess ksp;
    bool MaxSet;

# elif defined __FreeBSD__
    kvm_t *kd;
    kvm_swap swapinfo;
# endif
#endif

private slots:
    void receivedStdout(K3Process *, char *buffer, int);
    void processExited(K3Process *);

};

#endif // MEMSENSOR_H
