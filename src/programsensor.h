/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PROGRAMSENSOR_H
#define PROGRAMSENSOR_H
#include "sensor.h"
#include <kprocess.h>
#include <kprocio.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qvaluevector.h>
#include <qtextcodec.h>
class ProgramSensor :  public Sensor
{
    Q_OBJECT
public:
    ProgramSensor(const QString &programName, int msec=1000, QString encoding="" );
    ~ProgramSensor();
    void update();

private:
    QTextCodec *codec;
    KShellProcess ksp;
    QString programName;
    QString sensorResult;

public slots:
    void receivedStdout(KProcess *proc, char *buffer, int buflen);
    void processExited(KProcess *proc);
};

#endif // PROGRAMSENSOR_H
