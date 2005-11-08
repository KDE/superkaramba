/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "memsensor.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qstring.h>
#include <qregexp.h>

#ifdef __FreeBSD__
#include <sys/time.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/resource.h>
#include <unistd.h>
#include <kvm.h>
#include <sys/file.h>
#include <osreldate.h>


/* define pagetok in terms of pageshift */
#define pagetok(size) ((size) << pageshift)
#endif
#include <kdebug.h>

static KStaticDeleter<MemSensor> memSensorDeleter;
MemSensor* MemSensor::m_self = 0;

MemSensor* MemSensor::self()
{
    if (!m_self)
    {
        memSensorDeleter.setObject(m_self, new MemSensor());
    }

    return m_self;
}

MemSensor::MemSensor(int msec) : Sensor(msec)
{
#ifdef __FreeBSD__
    /* get the page size with "getpagesize" and calculate pageshift from it */
    int pagesize = getpagesize();
    pageshift = 0;
    while (pagesize > 1)
    {
        pageshift++;
        pagesize >>= 1;
    }

    /* we only need the amount of log(2)1024 for our conversion */
    pageshift -= 10;

# if __FreeBSD_version < 500018

    connect(&ksp, SIGNAL(receivedStdout(KProcess *, char *, int )),
            this,SLOT(receivedStdout(KProcess *, char *, int )));
    connect(&ksp, SIGNAL(processExited(KProcess *)),
            this,SLOT(processExited( KProcess * )));

    swapTotal = swapUsed = 0;

    MaxSet = false;

    readValues();
# else

    kd = kvm_open("/dev/null", "/dev/null", "/dev/null", O_RDONLY, "kvm_open");
# endif
#else

    readValues();
#endif
    start();
}

MemSensor::~MemSensor()
{}

#ifdef __FreeBSD__
void MemSensor::receivedStdout(KProcess *, char *buffer, int len )
{
    buffer[len] = 0;
    sensorResult += QString( QCString(buffer) );
}
#else
void MemSensor::receivedStdout(KProcess *, char *, int)
{}
#endif

void MemSensor::start()
{
    if (!m_timer.isActive())
    {
        connect (&m_timer,SIGNAL(timeout()),this,SLOT(update()));
        m_timer.start( (m_msec == 0)?1000:m_msec);
    }
}



void MemSensor::processExited(KProcess *)
{
#ifdef __FreeBSD__
    QStringList stringList = QStringList::split('\n',sensorResult);
    sensorResult = "";
    QStringList itemsList = QStringList::split(' ', stringList[1]);

    swapUsed = itemsList[2].toInt();
    swapTotal = itemsList[1].toInt();
#endif
}

int MemSensor::getMemTotal()
{
#ifdef __FreeBSD__
    static int mem = 0;
    size_t size = sizeof(mem);

    sysctlbyname("hw.physmem", &mem, &size, NULL, 0);
    return (mem / 1024);
#else

    QRegExp rx( "MemTotal:\\s*(\\d+)" );
    rx.indexIn( meminfo );
    return ( rx.cap(1).toInt() );
#endif
}

int MemSensor::getMemFree()
{
#ifdef __FreeBSD__
    static int mem = 0;
    size_t size = sizeof(mem);

    sysctlbyname("vm.stats.vm.v_free_count", &mem, &size, NULL, 0);
    return (pagetok(mem));
#else

    QRegExp rx( "MemFree:\\s*(\\d+)" );
    rx.indexIn( meminfo );
    return ( rx.cap(1).toInt() );
#endif
}


int MemSensor::getBuffers()
{
#ifdef __FreeBSD__
    static int mem = 0;
    size_t size = sizeof(mem);

    sysctlbyname("vfs.bufspace", &mem, &size, NULL, 0);
    return (mem / 1024);
#else

    QRegExp rx( "Buffers:\\s*(\\d+)" );
    rx.indexIn( meminfo );
    return ( rx.cap(1).toInt() );
#endif
}

int MemSensor::getCached()
{
#ifdef __FreeBSD__
    static int mem = 0;
    size_t size = sizeof(mem);

    sysctlbyname("vm.stats.vm.v_cache_count", &mem, &size, NULL, 0);
    return (pagetok(mem));
#else

    QRegExp rx1( "Cached:\\s*(\\d+)" );
    QRegExp rx2( "SwapCached:\\s*(\\d+)" );
    rx1.search( meminfo );
    rx2.search( meminfo );
    return ( rx1.cap(1).toInt() + rx2.cap(1).toInt() );
#endif
}


int MemSensor::getSwapTotal()
{
#ifdef __FreeBSD__
# if __FreeBSD_version < 500018
    return(swapTotal);
# else

    int n = -1;
    int pagesize = getpagesize();
    int retavail = 0;

    if (kd != NULL)
        n = kvm_getswapinfo(kd, &swapinfo, 1, 0);

    if (n < 0 || swapinfo.ksw_total == 0)
        return(0);

    retavail = swapinfo.ksw_total * pagesize / 1024;

    return(retavail);
# endif
#else

    QRegExp rx( "SwapTotal:\\s*(\\d+)" );
    rx.indexIn( meminfo );
    return ( rx.cap(1).toInt() );
#endif
}

int MemSensor::getSwapFree()
{
#ifdef __FreeBSD__
# if __FreeBSD_version < 500018
    return(swapTotal - swapUsed);
# else

    int n = -1;
    int pagesize = getpagesize();
    int retfree = 0;

    if (kd != NULL)
        n = kvm_getswapinfo(kd, &swapinfo, 1, 0);
    if (n < 0 || swapinfo.ksw_total == 0)
        return(0);

    retfree = (swapinfo.ksw_total - swapinfo.ksw_used) * pagesize / 1024;

    return(retfree);
# endif
#else

    QRegExp rx( "SwapFree:\\s*(\\d+)" );
    rx.indexIn( meminfo );
    return ( rx.cap(1).toInt() );
#endif
}

void MemSensor::readValues()
{
#ifdef __FreeBSD__
# if __FreeBSD_version < 500018
    ksp.clearArguments();
    ksp << "swapinfo";
    ksp.start( KProcess::NotifyOnExit,KProcIO::Stdout);
# endif
#else

    QFile file("/proc/meminfo");
    QString line;
    meminfo="";
    if ( file.open(QIODevice::ReadOnly | QIODevice::Text) )
    {
        QTextStream t( &file );        // use a text stream
        QString line;
        while((line= t.readLine())!=0)
        {
            meminfo += line;
            meminfo +='\n';
        }
        file.close();
    }
#endif
}

void MemSensor::addMeter(Meter* meter)
{
    disconnect(this, SIGNAL(memValues(QVariant) ), meter, SLOT(storeData(QVariant)));
    connect(this, SIGNAL(memValues(QVariant)), meter, SLOT(storeData(QVariant)));
}

void MemSensor::update()
{
    readValues();
#if (defined(__FreeBSD__) && __FreeBSD_version < 500018)

    bool set
        = false;
#endif

    int totalMem = getMemTotal();
    int usedMem = totalMem - getMemFree();
    int usedMemNoBuffers =  usedMem - getBuffers() - getCached();
    int totalSwap = getSwapTotal();
    int usedSwap = totalSwap - getSwapFree();

#if (defined(__FreeBSD__) && __FreeBSD_version < 500018)

    if ( (!MaxSet) && (totalSwap > 0) )
    {
        setMaxValue(sp);
        bool set
            = true;
    }
#endif

    data["fmb"] = (int)(( totalMem - usedMemNoBuffers)/1024.0+0.5);
    data["fm"] = (int)( ( totalMem - usedMem  )/1024.0+0.5);

    data["umb"] = (int)((usedMemNoBuffers)/1024.0+0.5);
    data["um"] = (int)((usedMem)/1024.0+0.5 );

    data["tm"] = (int)( (totalMem)/1024.0+0.5);

    data["fs"] = (int)((totalSwap - usedSwap)/1024.0+0.5);
    data["us"] = (int)(usedSwap/1024.0+0.5);
    data["ts"] = (int)(totalSwap/1024.0+0.5);

    emit memValues(QVariant(data));

#if (defined(__FreeBSD__) && __FreeBSD_version < 500018)

    if (set
       )
        MaxSet = true;
#endif

}
/*
void MemSensor::setMaxValue( SensorParams *sp )
{
    Meter *meter;
    meter = sp->getMeter();
    QString f;
    f = sp->getParam("FORMAT");
 
    if (f.length() == 0 )
    {
        f = "%um";
    }
    if( f=="%fm" || f== "%um" || f=="%fmb" || f=="%umb" )
        meter->setMax( getMemTotal() / 1024 );
    if( f=="%fs" || f== "%us" )
        meter->setMax( getSwapTotal() / 1024 );
}
*/
#include "memsensor.moc"
