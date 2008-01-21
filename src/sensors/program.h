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

#include <K3Process>
#include <K3ProcIO>
#include <QTextCodec>

class Karamba;

class ProgramSensor :  public Sensor
{
    Q_OBJECT
public:
    explicit ProgramSensor(Karamba* k, const QString &programName, int msec = 1000, const QString &encoding = QString());
    ~ProgramSensor();
    void update();

private:
    QTextCodec *codec;
    K3ShellProcess ksp;
    QString programName;
    QString sensorResult;
    Karamba* m_karamba;

    void replaceLine(QString& format, const QString& line);
    void replaceArgs(QRegExp& regEx, QString& format, const QStringList& tokens);

public slots:
    void receivedStdout(K3Process *proc, char *buffer, int buflen);
    void processExited(K3Process *proc);
};

#endif // PROGRAMSENSOR_H
