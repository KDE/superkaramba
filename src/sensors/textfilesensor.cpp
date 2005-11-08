/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include <QVector>
#include "qdom.h"

#include "textfilesensor.h"
#include "textfilesensor.moc"

TextFileSensor::TextFileSensor( const QString &fn, bool iRdf, int interval, const QString &encoding )
        : Sensor( interval )
{
    fileName = fn;
    rdf = iRdf;

    if( !encoding.isEmpty() )
    {
        codec = QTextCodec::codecForName( encoding.ascii() );
        if ( codec == 0)
            codec = QTextCodec::codecForLocale();
    }
    else
        codec = QTextCodec::codecForLocale();
}

TextFileSensor::~TextFileSensor()
{}

void TextFileSensor::addMeter(Meter* meter)
{
    disconnect(this, SIGNAL(textfileValues(QVariant) ), meter, SLOT(storeData(QVariant )));
    connect(this, SIGNAL(textfileValues(QVariant)), meter, SLOT(storeData(QVariant )));
}

void TextFileSensor::update()
{
    QStringList lines;
    QFile file(fileName);
    QString line;
    if ( file.open(QIODevice::ReadOnly | IO_Translate) )
    {
        if (rdf)
        {
            QDomDocument doc;
            if ( !doc.setContent( &file ) )
            {
                file.close();
                return;
            }
            QDomElement docElem = doc.documentElement();
            QDomNode n = docElem.firstChild();
            if (!n.isNull())
            {
                QDomNodeList titles = docElem.elementsByTagName( "title" );
                QDomNodeList links = docElem.elementsByTagName( "link" );

                int i;
                for ( i = 0; i < titles.count(); ++i )
                {
                    QDomElement element = titles.item( i ).toElement();
                    lines << element.text();

                    element = links.item( i ).toElement();
                    lines << element.text();
                }
            }
        }
        else
        {
            QTextStream t( &file );        // use a text stream
            while( (line = t.readLine()) !=0 )
            {
                lines.push_back(line);
            }
        }
        file.close();
    }

    emit textfileValues(QVariant(lines));
}

