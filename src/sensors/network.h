/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef NETWORKSENSOR_H
#define NETWORKSENSOR_H

#include "sensor.h"

#include <qdatetime.h>
#include <qfile.h>
#include <qregexp.h>
#ifdef __FreeBSD__
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <net/if_mib.h>
#endif

class NetworkSensor :  public Sensor
{
    Q_OBJECT
public:
    NetworkSensor( QString device, int interval );
    ~NetworkSensor();
    void update();


private:
    unsigned long receivedBytes;
    unsigned long transmittedBytes;
    QTime netTimer;
    QString device;
#ifdef __FreeBSD__
    int if_number;
    ifmibdata if_mib;
#endif
    void getInOutBytes (unsigned long &in,unsigned long &out) const;

};
#endif // NETWORKSENSOR_H

