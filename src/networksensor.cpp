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

#include "networksensor.h"

NetworkSensor::NetworkSensor( QString dev, int interval ):Sensor( interval )
{
   device = dev.lower();

#ifdef __FreeBSD__
   /* Determine number of interfaces */
   u_int   n    = 0;
   size_t  nlen = 0;
   nlen = sizeof(n);
   sysctlbyname("net.link.generic.system.ifcount", &n, &nlen, NULL, 0);

   size_t  if_miblen = 0;
   if_miblen = sizeof(if_mib);
   static  int name[] = { CTL_NET,
                          PF_LINK,
                          NETLINK_GENERIC,
                          IFMIB_IFDATA,
                          0,
                          IFDATA_GENERAL };

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
      if ( if_mib.ifmd_flags & RTF_GATEWAY )
         if_gw = i;
   }

   if ((if_number == -1) && (if_gw != -1))
      if_number = if_gw;
#else
   if( device.isEmpty() )
    device = "eth0";
#endif
     getInOutBytes(receivedBytes,transmittedBytes);
    netTimer.start();

}
NetworkSensor::~NetworkSensor()
{
}
void NetworkSensor::getInOutBytes ( unsigned long &in,unsigned long &out) const
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
   }
   else {
      in = 0;
      out = 0;
   }
#else
    QFile file("/proc/net/dev");
    QString line;
    if ( file.open(IO_ReadOnly | IO_Translate) )
    {
        QTextStream t( &file );        // use a text stream
        line = t.readLine();
        while(line !=0 && !line.contains(device))
        {
            line = t.readLine();
        }
        if ( line.contains( device ) )
        {
            QRegExp rx( "\\W+"+device+":\\D*(\\d+)(?:\\D+\\d+){7}\\D+(\\d+)", false);
            rx.search(line);
            in = rx.cap(1).toULong();
            out = rx.cap(2).toULong();
       }
        else
        {
            qDebug("Network sensor: can not find %s", device.ascii());
            in = 0;
            out = 0;
        }
    file.close();
    }
#endif
}

void NetworkSensor::update()
{
    SensorParams *sp;
    Meter *meter;
    QObjectListIt it( *objList );
    QString format;
    int decimals;

    unsigned long inB, outB;
    const double delay = (double) netTimer.elapsed(); // msec elapsed since last update
    getInOutBytes( inB, outB );
    netTimer.restart();

    while( it != 0 )
    {
        sp = (SensorParams*)(*it);
        meter = sp->getMeter();
        format = sp->getParam( "FORMAT" );
        decimals = ( sp->getParam( "DECIMALS" ) ).toInt();
        if (format.length() == 0 )
        {
            format = "%in";
        }

        format.replace( QRegExp("%inkb", false), QString::number( ((inB - receivedBytes)*8)/delay, 'f', decimals ) );
        format.replace( QRegExp("%in", false), QString::number( (inB - receivedBytes)/delay, 'f', decimals ) );

        format.replace( QRegExp("%outkb", false), QString::number( ((outB - transmittedBytes)*8)/delay, 'f', decimals ) );
        format.replace( QRegExp("%out", false), QString::number( (outB - transmittedBytes)/delay, 'f', decimals ) );

        meter->setValue( format );
        ++it;
    }
    receivedBytes = inB;
    transmittedBytes = outB;
}

#include "networksensor.moc"
