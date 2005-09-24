/***************************************************************************
 *   Copyright (C) 2003 by Ralph M. Churchill                              *
 *   mrchucho@yahoo.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "karambaapp.h"
#include "rsssensor.h"
#include <qdom.h>
#include <qregexp.h>
#include <kurl.h>
#include <kio/netaccess.h>

RssSensor::RssSensor( const QString &src, int interval, const QString &form, const QString &enc)
        : Sensor(interval),
        source(src),
        format(form),
        encoding(enc)

{
    // Format:
    //  %t = title (DEFAULT)
    //  %d = desc

    if( !encoding.isEmpty() )
    {
        codec = QTextCodec::codecForName( encoding.ascii() );
        if ( codec == 0)
            codec = QTextCodec::codecForLocale();
    }
    else
        codec = QTextCodec::codecForLocale();
}

RssSensor::~RssSensor()
{}

void RssSensor::addMeter(Meter* meter)
{
    disconnect(this, SIGNAL(rssValues(QVariant) ), meter, SLOT(storeData(QVariant)));
    connect(this, SIGNAL(rssValues(QVariant)), meter, SLOT(storeData(QVariant)));
}

void RssSensor::update()
{
    QDomDocument doc;
    QFile file;
    QString tmpFile;
    bool OK = false;

    if(KIO::NetAccess::download(KURL(source), tmpFile, karambaApp->parentWindow()))
    {
        file.setName(tmpFile);
        if ( file.open(IO_ReadOnly | IO_Translate) )
        {
            if ( doc.setContent( &file ) )
            {
                OK = true;
            }
            else
            {
                qDebug("Error on building DOM");
            }
        }
        else
        {
            qDebug("Error opening file");
        }
    }
    else
    {
        qDebug( "Error Downloading: %s", source.ascii());
    }

    if ( OK )
    {
        QDomElement docElem = doc.documentElement();
        QDomNode n = docElem.firstChild();
        if (!n.isNull())
        {
            QDomNodeList links = docElem.elementsByTagName( "link" );
            QDomNodeList displays;
            if ( format.contains( "%d", false ) > 0 )
            {
                displays = docElem.elementsByTagName( "description" );
            }
            else
            {
                displays = docElem.elementsByTagName( "title" );
            }

            QRegExp rx("^http://", false );
            QList<QVariant> list;
            for (uint i=1; i < displays.count(); ++i )
            {
                QString dispTxt = displays.item( i ).toElement().text();
                QString linkTxt = links.item( i ).toElement().text();
                if( (rx.search(dispTxt) == -1) && (rx.search(linkTxt) != -1) )
                {
                    QMap<QString, QVariant> e;
                    e["%text"] = dispTxt;
                    e["%link"] = linkTxt;
                    list << QVariant(e);
                }
                else
                {
                    qDebug("Skipping");
                }
            }
            emit rssValues(QVariant(list));
        }
        else
        {
            qDebug ("Document Node was null!!");
        }
    }
    // Cleanup
    file.close();
    KIO::NetAccess::removeTempFile( tmpFile );
}

#include "rsssensor.moc"
