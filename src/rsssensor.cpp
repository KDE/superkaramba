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
{
}

void RssSensor::update()
{
    QDomDocument doc;
    QFile file;
    QString tmpFile;
    bool OK = false;

#if defined(KDE_3_3)
    if(KIO::NetAccess::download(KURL(source), tmpFile, karambaApp->parentWindow()))
#else
    if(KIO::NetAccess::download(KURL(source), tmpFile))
#endif
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
    else {
        qDebug( "Error Downloading: %s", source.ascii());
    }

    if ( OK )
    {
        SensorParams *sp;
        Meter *meter;

        QObjectListIt it( *objList );
        while (it != 0)
        {
            sp = (SensorParams*)(*it);
            meter = sp->getMeter();

            // this is a hack to force the
            // clickmap to reset its data lists
            meter->setValue(0);

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
                for (uint i=1; i < displays.count(); ++i )
                {
                    QString dispTxt = displays.item( i ).toElement().text();
                    QString linkTxt = links.item( i ).toElement().text();
                    if( (rx.search(dispTxt) == -1) && (rx.search(linkTxt) != -1) )
                    {
                        meter->setValue( dispTxt );
                        meter->setValue( linkTxt );
                    }
                    else
                    {
                        qDebug("Skipping");
                    }
                }
            }
            else
            {
                qDebug ("Document Node was null!!");
            }

            ++it;
        }
    }
    // Cleanup
    file.close();
    KIO::NetAccess::removeTempFile( tmpFile );
}

#include "rsssensor.moc"
