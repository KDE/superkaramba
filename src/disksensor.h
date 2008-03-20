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
class DiskSensor :  public Sensor
{
Q_OBJECT
public:
  DiskSensor(int msec );
  ~DiskSensor();
  void update();
  void setMaxValue( SensorParams *sp );

private:
  long getFreeSpace(QString mntPt) const;
  long getUsedSpace(QString mntPt) const;
  long getTotalSpace(QString mntPt) const;
  int getPercentUsed(QString mntPt) const;
  int getPercentFree(QString mntPt) const;

  KShellProcess ksp;
  QString sensorResult;

  QMap<QString,QString> mntMap;
  QStringList stringList;

  int init;

private slots:
  void receivedStdout(KProcess *, char *buffer, int);
  void processExited(KProcess *);

signals:
  void initComplete();

};
#endif // DISKSENSOR_H
