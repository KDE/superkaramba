/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef DISKSENSOR_H
#define DISKSENSOR_H
#include "sensor.h"
#include <qmap.h>
#include <qstring.h>
#include <qtextcodec.h>
#include <qregexp.h>
#include <qstringlist.h>
#include <kprocess.h>
#include <kstaticdeleter.h>

class DiskSensor :  public Sensor
{
    Q_OBJECT
public slots:
    void update();
public:
    void setMaxValue( SensorParams *sp );
    void addMeter(Meter *);
    static DiskSensor* self();
    static bool isSingleton() { return true; }

signals:
    void diskValues(QVariant);

private:
    friend class KStaticDeleter<DiskSensor>;
    DiskSensor(int msec=2000 );
    virtual ~DiskSensor();
    int getFreeSpace(QString mntPt) const;
    int getUsedSpace(QString mntPt) const;
    int getTotalSpace(QString mntPt) const;
    int getPercentUsed(QString mntPt) const;
    int getPercentFree(QString mntPt) const;

    KShellProcess ksp;
    QString sensorResult;

    QMap<QString,QString> mntMap;
    QMap<QString,QVariant> data;
    QStringList stringList;
    static DiskSensor* m_self;

    int init;

private slots:
    void receivedStdout(KProcess *, char *buffer, int);
    void processExited(KProcess *);

signals:
    void initComplete();

};
#endif // DISKSENSOR_H
