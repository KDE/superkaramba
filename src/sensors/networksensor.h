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
#include <kstaticdeleter.h>

#include <QDateTime>
#include <QFile>
#include <QRegExp>
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
    void addMeter(Meter*);
    static NetworkSensor* self();
    static bool isSingleton() { return true; }
    void start();

signals:
    void networkValues(QVariant);

private:
    friend class KStaticDeleter<NetworkSensor>;
    NetworkSensor(int interval=2000 );
    virtual ~NetworkSensor();    
    qulonglong receivedBytes;
    qulonglong transmittedBytes;
    QMap<QString,QVariant> data;
    QTime netTimer;
    QString device;
    static NetworkSensor* m_self;
    void getInOutBytes();
public slots:
    void update();
    

};
#endif // NETWORKSENSOR_H

