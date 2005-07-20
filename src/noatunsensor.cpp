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
    QString format;
    SensorParams *sp;
    Meter *meter;
    QObjectListIt it( *objList );

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
    }


    while (it != 0)
    {
        sp = (SensorParams*)(*it);
        meter = sp->getMeter();

        if( running )
        {

            format = sp->getParam("FORMAT");
            if (format.length() == 0 )
            {
                format = "%title %time / %length";
            }

            if( format.lower() == "%ms" )
            {
                meter->setMax( songLength );
                meter->setValue( currentTime );
            }
            else
                if ( format.lower() == "%full" )
                {
                    meter->setValue( 1 );
                }
                else

                {
                   format.replace( QRegExp("%title", false), title );
		   format.replace( QRegExp("%id", false), noatunID );

                    format.replace( QRegExp("%length", false), QTime( 0,0,0 ).
                                    addMSecs( songLength )
                                    .toString( "h:mm:ss" ) );

                    format.replace( QRegExp("%time", false), QTime( 0,0,0 ).
                                    addMSecs( currentTime )
                                    .toString( "h:mm:ss" ) );
                    format.replace( QRegExp("%remain", false), QTime( 0,0,0 ).
                                    addMSecs( songLength )
                                    .addMSecs(-currentTime )
                                    .toString( "h:mm:ss" ) );

                    meter->setValue(format);
                }
        }
        else

        {
            meter->setValue("");
        }
        ++it;


    }

}

bool NoatunSensor::isRunning()
{
    QRegExp rx("(noatun)|(noatun-\\d+)");
    QCStringList list = client->registeredApplications();
    QValueList<QCString>::iterator it;
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
    QByteArray data, replyData;
    QCString replyType;
    QString result;
    QDataStream arg(data, IO_WriteOnly);
    arg << 5;
    if (!client->call( noatunID, "Noatun", "title()",
                       data, replyType, replyData))
    {
        result = "";
        qDebug("there was some error using DCOP.");
    }
    else
    {
        QDataStream reply(replyData, IO_ReadOnly);
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
}


int NoatunSensor::getTime()
{
    QByteArray data, replyData;
    QCString replyType;
    int result;
    QDataStream arg(data, IO_WriteOnly);
    arg << 5;
    if (!client->call( noatunID, "Noatun", "position()",
                       data, replyType, replyData))
    {
        result = 0;
        qDebug("there was some error using DCOP.");
    }
    else
    {
        QDataStream reply(replyData, IO_ReadOnly);
        if (replyType == "int")
        {
            reply >> result;
        }
        else
        {
            result = 0;
            qDebug("title returned an unexpected type of reply!");
        }
    }
    return result;
}


int NoatunSensor::getLength()
{
    QByteArray data, replyData;
    QCString replyType;
    int result;
    QDataStream arg(data, IO_WriteOnly);
    arg << 5;
    if (!client->call( noatunID, "Noatun", "length()",
                       data, replyType, replyData))
    {
        result = 0;
        qDebug("there was some error using DCOP.");
    }
    else
    {
        QDataStream reply(replyData, IO_ReadOnly);
        if (replyType == "int")
        {
            reply >> result;
        }
        else
        {
            result = 0;
            qDebug("title returned an unexpected type of reply!");
        }
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
