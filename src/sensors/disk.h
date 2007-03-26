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

#include <K3Process>

#include "sensor.h"

class DiskSensor :  public Sensor
{
    Q_OBJECT
public:
    DiskSensor(int msec);
    ~DiskSensor();
    void update();
    void setMaxValue(SensorParams *sp);

private:
    int getFreeSpace(const QString &mntPt) const;
    int getUsedSpace(const QString &mntPt) const;
    int getTotalSpace(const QString &mntPt) const;
    int getPercentUsed(const QString &mntPt) const;
    int getPercentFree(const QString &mntPt) const;

    K3ShellProcess ksp;
    QString sensorResult;

    QMap<QString, QString> mntMap;
    QStringList stringList;

    int init;

private slots:
    void receivedStdout(K3Process *, char *buffer, int);
    void processExited(K3Process *);

signals:
    void initComplete();

};
#endif // DISKSENSOR_H
