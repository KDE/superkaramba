/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "noatunsensor.h"
#include <QByteArray>
#include <QTime>

NoatunSensor::NoatunSensor( int interval,  DCOPClient *c)
        : Sensor( interval )
{
    client = c;
    noatunID = "noatun";
}


NoatunSensor::~NoatunSensor()
{}


void NoatunSensor::update()
{
    QMap<QString, QVariant> map;

    QString title;
    int songLength = 0;
    int currentTime = 0;

    bool running = isRunning();

    if( running )
    {
        title = getTitle();

        if( title.isEmpty() )
            title = "Noatun";
        currentTime = getTime();
        if( currentTime == -1 )
            currentTime = 0;

        songLength = getLength();
        if( songLength == -1 )
            songLength = 0;

        map["%title"] = title;
        map["%id"] = noatunID;

        map["%length"] = QTime( 0,0,0 ).addMSecs( songLength ).toString( "h:mm:ss" );

        map["%time"] = QTime( 0,0,0 ).addMSecs( currentTime ).toString( "h:mm:ss" );
        map["%remain"] = QTime( 0,0,0 ).addMSecs( songLength ).addMSecs(-currentTime ).toString( "h:mm:ss" );

        emit noatunValues(QVariant(map));
    }
    else
    {
        emit noatunValues(QVariant());
    }
}

void NoatunSensor::addMeter(Meter* meter)
{
    connect(this, SIGNAL(noatunValues(QVariant)), meter, SLOT(update(QVariant)));
}

bool NoatunSensor::isRunning()
{
    QRegExp rx("(noatun)|(noatun-\\d+)");
    DCOPCStringList list = client->registeredApplications();
    DCOPCStringList::iterator it;
    it = list.begin();
    bool foundNoatun = false;
    noatunID = "noatun";
    while( foundNoatun == false && it != list.end() )
    {
        if( rx.search(  *it ) != -1 )
        {
            foundNoatun = true;
            noatunID = *it;
        }
        ++it;
    }
    return ( client->isApplicationRegistered ( noatunID ) );
}


QString NoatunSensor::getTitle()
{
 /*   DCOPCString data, replyData;
    QByteArray replyType;
    QString result;
    QDataStream arg(&data, IO_WriteOnly);
    arg << 5;
    if (!client->call( noatunID, DCOPCString("Noatun"), DCOPCString("title()"),
                       data, replyType, replyData))
    {
        result = "";
        qDebug("there was some error using DCOP.");
    }
    else
    {
        QDataStream reply(&replyData, IO_ReadOnly);
        if (replyType == "QString")
        {
            reply >> result;
            result.replace( QRegExp("_")," " );
            result.replace( QRegExp(".mp3$"),"" );

        }
        else
        {
            result = "";
            qDebug("title returned an unexpected type of reply!");
        }
    }
    return result;
*/
}


int NoatunSensor::getTime()
{
    int result;
    DCOPReply dcopResult = DCOPRef( noatunID, "Noatun").call("position()");
    if (!dcopResult.get(result))
    {
        qDebug("Something went wrong with the call!");
	result = 0;
    }

    return result;
}


int NoatunSensor::getLength()
{
    int result;
    DCOPReply dcopResult = DCOPRef( noatunID, "Noatun").call("length()");
    if (!dcopResult.get(result))
    {
        qDebug("Something went wrong with the call!");
        result = 0;
    }

    return result;
}


void NoatunSensor::setMaxValue( SensorParams *sp)
{
    Meter *meter;
    meter = sp->getMeter();
    QString f;
    f = sp->getParam("FORMAT");

    if ( f == "%full" )
        meter->setMax( 1 );

}
