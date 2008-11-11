/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "network.h"

#include <QTextStream>
#include <QNetworkInterface>

#include <KDebug>

#ifdef __FreeBSD__
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/socket.h>
#include <net/route.h>
#endif

NetworkSensor::NetworkSensor(const QString &dev, int interval): Sensor(interval)
{
    device = dev.toLower();

#ifdef __FreeBSD__
    /* Determine number of interfaces */
    u_int   n    = 0;
    size_t  nlen = 0;
    nlen = sizeof(n);
    sysctlbyname("net.link.generic.system.ifcount", &n, &nlen, NULL, 0);

    size_t  if_miblen = 0;
    if_miblen = sizeof(if_mib);
    static  int name[] = {
                             CTL_NET,
                             PF_LINK,
                             NETLINK_GENERIC,
                             IFMIB_IFDATA,
                             0,
                             IFDATA_GENERAL
                         };

    /*
       If the device was defined by the theme, find the right devicenumber.
       If not, use the device that holds the default route.
     */

    if_number = -1;
    int if_gw = -1;

    for (int i = 1; i <= n; ++i) {
        name[4] = i;
        /* Get data for iface-number i */
        sysctl(name, 6, (void*)&if_mib, (size_t*)&if_miblen, (void*)NULL, (size_t)0);

        /* We found the right interface? */
        if (QString(if_mib.ifmd_name) == device) {
            if_number = i;
            break;
        }

        /* Does the interface hold the default route? */
        if (if_mib.ifmd_flags & RTF_GATEWAY)
            if_gw = i;
    }

    if ((if_number == -1) && (if_gw != -1))
        if_number = if_gw;
#else
    if (device.isEmpty()) {
        device = DEFAULT_DEVICE;
    }
#endif
    interfaceList = device.split("|", QString::SkipEmptyParts);
    refreshDevice();
    getIPAddress();

    getInOutBytes(receivedBytes, transmittedBytes);
    netTimer.start();

}

void NetworkSensor::refreshDevice() {
    int rank = interfaceList.count();
    //TODO: what about BSD?
    QFile file ("/proc/net/dev");

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream t(&file);       // use a text stream
        t.readLine();               // reads: "Inter-|   Receive..."
        t.readLine();                  // reads: " face |bytes    "
        QString line = t.readLine();   // finally reads something we care about

        QStringList::ConstIterator listEnd(interfaceList.constEnd());
        while ((rank != 0) && (line != 0)) {
            int i = 0;
            for (QStringList::ConstIterator dev = interfaceList.constBegin();
                (dev != listEnd) && (i < rank);
                ++dev, ++i)
                {
                    if (line.contains(*dev)) {
                        device = (*dev);
                        //deviceLine = line;
                        rank = i;
                    }
                }
                line = t.readLine();
        }
        file.close();
    }

    if (rank >= interfaceList.count()) {
        device = NO_DEVICE;
        //deviceLine = "";
    }
}

NetworkSensor::~NetworkSensor()
{
}

void NetworkSensor::getInOutBytes(unsigned long &in, unsigned long &out) const
{
#ifdef __FreeBSD__
    if (if_number != -1) {
        size_t  if_miblen = 0;
        if_miblen = sizeof(if_mib);
        int name[] = { CTL_NET,
                       PF_LINK,
                       NETLINK_GENERIC,
                       IFMIB_IFDATA,
                       if_number,
                       IFDATA_GENERAL };

        sysctl(name, 6, (void*)&if_mib, (size_t*)&if_miblen, (void*)NULL, (size_t)0);

        in = if_mib.ifmd_data.ifi_ibytes;
        out = if_mib.ifmd_data.ifi_obytes;
    } else {
        in = 0;
        out = 0;
    }
#else
    QFile file("/proc/net/dev");
    QString line;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream t(&file);          // use a text stream
        line = t.readLine();

        while (line != 0 && !line.contains(device)) {
            line = t.readLine();
        }

        if (line.contains(device)) {
            QRegExp rx("\\W*" + device + ":\\D*(\\d+)(?:\\D+\\d+){7}\\D+(\\d+)", Qt::CaseInsensitive);
            rx.indexIn(line);
            in = rx.cap(1).toULong();
            out = rx.cap(2).toULong();
        } else {
            kDebug() << "Network sensor: can not find " << device ;
            in = 0;
            out = 0;
        }
        file.close();
    }
#endif
}

void NetworkSensor::getIPAddress()
{
    const QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();

    foreach (const QNetworkInterface &interface, interfaces) {
        if (device == interface.name()) {
            QList<QNetworkAddressEntry> entries = interface.addressEntries();
            if (entries.count() > 0) {
                ipAddress = entries[0].ip().toString();
            }
        }
    }
#if 0
    struct ifaddrs *ifa = NULL, *ifp = NULL;

    if (getifaddrs (&ifp) < 0) {
        kDebug() << "getifaddrs failed" ;
        ipAddress = NO_IP;
        return;
    }

    for (ifa = ifp; ifa; ifa = ifa->ifa_next) {
        char ip[200];
        socklen_t salen;

        if (ifa->ifa_addr->sa_family == AF_INET) {
            salen = sizeof (struct sockaddr_in);
        } else if (ifa->ifa_addr->sa_family == AF_INET6) {
            //continue since this doesn't support IPv6 at this point
            continue;
            //salen = sizeof (struct sockaddr_in6);
        } else {
            continue;
        }

        if (getnameinfo(ifa->ifa_addr, salen, ip, sizeof (ip), NULL, 0, NI_NUMERICHOST) < 0) {
            kDebug() << "getnameinfo < 0" ;
            continue;
        }

        if (device == QString::fromLatin1(ifa->ifa_name)) {
            ipAddress = QString::fromLatin1(ip);
            freeifaddrs(ifp);
            return;
        }
    }

    freeifaddrs(ifp);
    ipAddress = NO_IP;
#endif
}

void NetworkSensor::update()
{
    SensorParams *sp;
    Meter *meter;
    QString format;
    int decimals;

    unsigned long inB, outB;
    const double delay = (double) netTimer.elapsed(); // msec elapsed since last update
    refreshDevice();
    getIPAddress();
    getInOutBytes(inB, outB);
    netTimer.restart();

    QObject *object;
    foreach(object, *objList) {
        sp = (SensorParams*)(object);
        meter = sp->getMeter();
        format = sp->getParam("FORMAT");
        decimals = (sp->getParam("DECIMALS")).toInt();
        if (format.length() == 0) {
            format = "%in";
        }

        format.replace(QRegExp("%inkb", Qt::CaseInsensitive),
                       QString::number(((inB - receivedBytes)*8) / delay, 'f', decimals));

        format.replace(QRegExp("%in", Qt::CaseInsensitive),
                       QString::number((inB - receivedBytes) / delay, 'f', decimals));

        format.replace(QRegExp("%outkb", Qt::CaseInsensitive),
                       QString::number(((outB - transmittedBytes)*8) / delay, 'f', decimals));

        format.replace(QRegExp("%out", Qt::CaseInsensitive),
                       QString::number((outB - transmittedBytes) / delay, 'f', decimals));

        format.replace(QRegExp("%dev", Qt::CaseInsensitive), device);

        format.replace(QRegExp("%ip", Qt::CaseInsensitive), ipAddress);

        meter->setValue(format);
    }

    receivedBytes = inB;
    transmittedBytes = outB;
}

#include "network.moc"
