/***************************************************************************
 *   Copyright (C) 2003 by Ralph M. Churchill                              *
 *   mrchucho@yahoo.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "rss.h"
#include "rss.moc"

#include <QDomDocument>
#include <QTextCodec>
#include <QFile>

#include <KUrl>
#include <kio/netaccess.h>

RssSensor::RssSensor(const QString &src, int interval, const QString &form, const QString &enc)
        : Sensor(interval),
        source(src),
        format(form),
        encoding(enc)

{
    // Format:
    //  %t = title (DEFAULT)
    //  %d = desc

    if (!encoding.isEmpty()) {
        codec = QTextCodec::codecForName(encoding.toAscii().constData());
        if (codec == 0)
            codec = QTextCodec::codecForLocale();
    } else
        codec = QTextCodec::codecForLocale();
}

RssSensor::~RssSensor()
{}

void RssSensor::update()
{
    QDomDocument doc;
    QFile file;
    QString tmpFile;
    bool OK = false;

    if (KIO::NetAccess::download(KUrl(source), tmpFile, 0)) {
        file.setFileName(tmpFile);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            if (doc.setContent(&file)) {
                OK = true;
            } else {
                qDebug("Error on building DOM");
            }
        } else {
            qDebug("Error opening file");
        }
    } else {
        qDebug("Error Downloading: %s", source.toAscii().constData());
    }

    if (OK) {
        SensorParams *sp;
        Meter *meter;

        QObject *obj;
        foreach(obj, *objList) {
            sp = (SensorParams*)(obj);
            meter = sp->getMeter();

            // this is a hack to force the
            // clickmap to reset its data lists
            meter->setValue(0);

            QDomElement docElem = doc.documentElement();
            QDomNode n = docElem.firstChild();
            if (!n.isNull()) {
                QDomNodeList links = docElem.elementsByTagName("link");
                QDomNodeList displays;
                if (format.contains("%d", Qt::CaseInsensitive) > 0) {
                    displays = docElem.elementsByTagName("description");
                } else {
                    displays = docElem.elementsByTagName("title");
                }

                QRegExp rx("^http://", Qt::CaseInsensitive);
                for (int i = 1; i < displays.count(); ++i) {
                    QString dispTxt = displays.item(i).toElement().text();
                    QString linkTxt = links.item(i).toElement().text();
                    if ((rx.indexIn(dispTxt) == -1) && (rx.indexIn(linkTxt) != -1)) {
                        meter->setValue(dispTxt);
                        meter->setValue(linkTxt);
                    } else {
                        qDebug("Skipping");
                    }
                }
            } else {
                qDebug("Document Node was null!!");
            }
        }
    }
    // Cleanup
    file.close();
    KIO::NetAccess::removeTempFile(tmpFile);
}

