/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "textfile.h"

#include <QTextCodec>
#include <QTextStream>
#include <QDomDocument>

TextFileSensor::TextFileSensor(const QString &fn, bool iRdf, int interval, const QString &encoding)
        : Sensor(interval)
{
    fileName = fn;
    rdf = iRdf;

    if (!encoding.isEmpty()) {
        codec = QTextCodec::codecForName(encoding.toAscii().constData());
        if (codec == 0)
            codec = QTextCodec::codecForLocale();
    } else
        codec = QTextCodec::codecForLocale();
}

TextFileSensor::~TextFileSensor()
{}

void TextFileSensor::update()
{
    QVector<QString> lines;
    QFile file(fileName);
    QString line;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (rdf) {
            QDomDocument doc;
            if (!doc.setContent(&file)) {
                file.close();
                return;
            }
            QDomElement docElem = doc.documentElement();
            QDomNode n = docElem.firstChild();
            if (!n.isNull()) {
                QDomNodeList titles = docElem.elementsByTagName("title");
                QDomNodeList links = docElem.elementsByTagName("link");

                int i;
                for (i = 0; i < titles.count(); ++i) {
                    QDomElement element = titles.item(i).toElement();
                    lines.push_back(element.text());

                    element = links.item(i).toElement();
                    lines.push_back(element.text());
                }
            }
        } else {
            QTextStream t(&file);
            while ((line = t.readLine()) != 0) {
                lines.push_back(line);
            }
        }
        file.close();
    }

    int lineNbr;
    SensorParams *sp;
    Meter *meter;

    int count = (int) lines.size();
    QObject *obj;
    foreach(obj, *objList) {
        sp = (SensorParams*)(obj);
        meter = sp->getMeter();
        lineNbr = (sp->getParam("LINE")).toInt();
        if (lineNbr >= 1  && lineNbr <= (int) count) {
            meter->setValue(lines[lineNbr-1]);
        }
        if (-lineNbr >= 1 && -lineNbr <= (int) count) {
            meter->setValue(lines[count+lineNbr]);
        }

        if (lineNbr == 0) {
            QString text;
            for (int i = 0; i < count; i++) {
                text += lines[i] + '\n';
            }
            meter->setValue(text);
        }
    }
}

#include "textfile.moc"
