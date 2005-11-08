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
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/socket.h>
#include <net/route.h>
#endif

#include <QTextStream>

#include "networksensor.h"
#include <kdebug.h>
static KStaticDeleter<NetworkSensor> networkSensorDeleter;
NetworkSensor* NetworkSensor::m_self = 0;

NetworkSensor* NetworkSensor::self()
{
    if (!m_self)
    {
        networkSensorDeleter.setObject(m_self, new NetworkSensor());
    }

    return m_self;
}

NetworkSensor::NetworkSensor( int interval ):Sensor( interval )
{

    netTimer.start();
    getInOutBytes();
    start();
    

}
NetworkSensor::~NetworkSensor()
{}

void NetworkSensor::start()
{
    if (!m_timer.isActive())
    {
        connect (&m_timer,SIGNAL(timeout()),this,SLOT(update()));
        m_timer.start( (m_msec == 0)?1000:m_msec);
    }
}

void NetworkSensor::getInOutBytes()
{
    qulonglong inB=0,outB=0;
    QFile file("/proc/net/dev");
    QString line;
    if ( file.open(QIODevice::ReadOnly | QIODevice::Text) )
    {
        QTextStream t( &file );        // use a text stream
        line = t.readLine();
        while(line !=0 && !line.contains(':'))
        {
            line = t.readLine();
        }
        const double delay = (double) netTimer.restart()/1000.0; // msec elapsed since last update
        while(line !=0 && line.contains(':'))
        {
            QString dev=line.left(line.indexOf(':')).simplified();
            QRegExp rx( "\\W+"+dev+":\\D*(\\d+)(?:\\D+\\d+){7}\\D+(\\d+)", false);
            rx.search(line);
            inB = rx.cap(1).toULong();
            outB = rx.cap(2).toULong();
            if(data.find(dev)==data.end())
            {
                QVariantMap map;
                map["inkb"] = 0;
                map["in"] = 0;
                map["outkb"] = 0;
                map["out"] = 0;
                map["intotal"]= inB;
                map["outtotal"]= outB;
                data[dev]=map;
            }
            else
            {
                QVariantMap map;
                receivedBytes=data[dev].toMap().value("intotal").toULongLong();
                transmittedBytes=data[dev].toMap().value("outtotal").toULongLong();
                map["inkb"] = ((inB-receivedBytes)/1000)/delay;
                map["in"] = (inB - receivedBytes)/delay;
                map["outkb"] = ((outB - transmittedBytes)/1000)/delay;
                map["out"] = (outB - transmittedBytes)/delay;
                map["intotal"]=inB;
                map["outtotal"]=outB;
                data[dev]=map;
            }
            line = t.readLine();
        }
        file.close();
    }
    
}


void NetworkSensor::addMeter(Meter* meter)
{
    disconnect(this, SIGNAL(networkValues(QVariant )), meter, SLOT(storeData(QVariant)));
    connect(this, SIGNAL(networkValues(QVariant)), meter, SLOT(storeData(QVariant)));
}

void NetworkSensor::update()
{
    getInOutBytes();
    emit networkValues(QVariant(data));
}

#include "networksensor.moc"
