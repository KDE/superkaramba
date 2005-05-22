/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef CPUSENSOR_H
#define CPUSENSOR_H
#include "sensor.h"

#include <qfile.h>
#include <qregexp.h>

class CPUSensor :  public Sensor
{
    Q_OBJECT
public:
    CPUSensor( QString cpuNbr, int interval );
    ~CPUSensor();
    void update();
    void setMaxValue( SensorParams *sp );

    int getCPULoad();

private:
    long userTicks;
    long sysTicks;
    long niceTicks;
    long idleTicks;

    int user;
    int system;
    int nice;
    int idle;

    void getTicks (long &u,long &s,long &n,long &i);
    QString cpuNbr;

};

#endif // CPUSENSOR_H
