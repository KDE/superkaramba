/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "program.h"
#include "karamba.h"


ProgramSensor::ProgramSensor(Karamba* k, const QString &progName, int interval, const QString &encoding)
        : Sensor(interval),
        m_karamba(k)
{
    if (!encoding.isEmpty()) {
        codec = QTextCodec::codecForName(encoding.toAscii().constData());
        if (codec == 0)
            codec = QTextCodec::codecForLocale();
    } else
        codec = QTextCodec::codecForLocale();


    programName = progName;
    //update();
    connect(&ksp, SIGNAL(receivedStdout(K3Process *, char *, int)),
            this, SLOT(receivedStdout(K3Process *, char *, int)));
    connect(&ksp, SIGNAL(processExited(K3Process *)),
            this, SLOT(processExited(K3Process *)));
}

ProgramSensor::~ProgramSensor()
{}

void ProgramSensor::receivedStdout(K3Process *, char *buffer, int len)
{
    buffer[len] = 0;
    sensorResult += codec->toUnicode(buffer);
}

void ProgramSensor::replaceLine(QString &format, const QString &line)
{
    const QStringList tokens = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    QRegExp dblDigit("%(\\d\\d)");
    replaceArgs(dblDigit, format, tokens);
    QRegExp digit("%(\\d)");
    replaceArgs(digit, format, tokens);
}

void ProgramSensor::replaceArgs(QRegExp& regEx, QString& format, const QStringList& tokens)
{
    int pos = 0;
    while (pos >= 0) {
        pos = regEx.indexIn(format, pos);
        if (pos > -1) {
            QString matched = regEx.cap(1);
            int tokenIndex = matched.toInt() - 1;
            QString replacement = "";
            if (tokenIndex < tokens.size()) {
                replacement = tokens.at(tokenIndex);
            }
            format.replace(QRegExp('%' + matched), replacement);
            pos += regEx.matchedLength();
        }
    }
}


void ProgramSensor::processExited(K3Process *)
{
    int lineNbr;
    SensorParams *sp;
    Meter *meter;
    QString value;
    QVector<QString> lines;
    QStringList stringList = sensorResult.split('\n');
    QStringList::ConstIterator end(stringList.constEnd());
    for (QStringList::ConstIterator it = stringList.constBegin(); it != end; ++it) {
        lines.push_back(*it);
    }

    int count = (int) lines.size();
    QObject *object;
    foreach(object, *objList) {
        sp = (SensorParams*)(object);
        meter = sp->getMeter();
        if (meter != 0) {
            lineNbr = (sp->getParam("LINE")).toInt();
            if (lineNbr >= 1  && lineNbr <= (int) count) {
                value = lines[lineNbr-1];
            } else if (-lineNbr >= 1 && -lineNbr <= (int) count) {
                value = lines[count+lineNbr];
            } else if (lineNbr != 0) {
                value.clear();
            } else {
                value = sensorResult;
            }

            QString format = sp->getParam("FORMAT");
            if (!format.isEmpty()) {
                QString returnValue;
                QStringList lineList = value.split('\n');
                QStringList::ConstIterator lineListEnd(lineList.constEnd());
                for (QStringList::ConstIterator line = lineList.constBegin(); line != lineListEnd; ++line) {
                    QString formatCopy = format;
                    replaceLine(formatCopy, *line);
                    returnValue += formatCopy;
                    if ( lineList.size() > 1) {
                      returnValue += '\n';
                    }
                }
                value = returnValue;
            }

            meter->setValue(value);
        }
    }

    sensorResult = "";
}

void ProgramSensor::update()
{
    QString prog = programName;
    m_karamba->replaceNamedValues(&prog);
    if ( prog.isEmpty() || prog.startsWith("%echo ")) {
        sensorResult += prog.mid(6);
        processExited(NULL);
    } else {
        ksp.clearArguments();
        ksp << prog;

        ksp.start(K3ProcIO::NotifyOnExit, K3ProcIO::Stdout);
    }
}

#include "program.moc"
