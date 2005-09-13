/***************************************************************************
*   Copyright (C) 2003 by Hans Karlsson                                   *
*   karlsson.h@home.se                                                      *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#include "xmmssensor.h"

#ifdef HAVE_XMMS
#include <xmmsctrl.h>
#endif // HAVE_XMMS

#include <QByteArray>

XMMSSensor::XMMSSensor( int interval, const QString &encoding )
    : Sensor( interval )
{
     if( !encoding.isEmpty() )
    {
        codec = QTextCodec::codecForName( encoding.ascii() );
        if ( codec == 0)
            codec = QTextCodec::codecForLocale();
    }
    else
        codec = QTextCodec::codecForLocale();

}

XMMSSensor::~XMMSSensor()
{
}

void XMMSSensor::addMeter(Meter* meter)
{
    connect(this, SIGNAL(xmmsValues(QVariant)), meter, SLOT(update(QVariant)));
}

void XMMSSensor::update()
{
#ifdef HAVE_XMMS
    int pos;
    QString title;
    int songLength = 0;
    int currentTime = 0;
    bool isPlaying = false;
    bool isRunning = xmms_remote_is_running(0);

    if( isRunning )
    {
        isPlaying = xmms_remote_is_playing(0);
        pos = xmms_remote_get_playlist_pos(0);
        qDebug("unicode start");
        title = codec->toUnicode( QByteArray( xmms_remote_get_playlist_title( 0, pos ) )  );
        qDebug("unicode end");
        if( title.isEmpty() )
            title = "XMMS";

        qDebug("Title: %s", title.ascii());
        songLength = xmms_remote_get_playlist_time( 0, pos );
        currentTime = xmms_remote_get_output_time( 0 );
    }
#endif // HAVE_XMMS


#ifdef HAVE_XMMS
    QMap<QString, QVariant> map;
    if( isRunning )
    {
        map["%title"] = title;
        map["%length"] = QTime( 0,0,0 ).addMSecs( songLength ).toString( "h:mm:ss" );
        map["%time"] = QTime( 0,0,0 ).addMSecs( currentTime ).toString( "h:mm:ss" );

        if( isPlaying  )
        {
            map["%remain"] = QTime( 0,0,0 ).addMSecs( songLength ).addMSecs(-currentTime ).toString( "h:mm:ss" );
        }
        else
        {
            map["%remain"] = QTime( 0,0,0 ).toString("h:mm:ss" );
        }
        emit xmmsValues(QVariant(map));
    }
    else
#endif // HAVE_XMMS
    {
        emit xmmsValues(QVariant());
    }
}
/*
void XMMSSensor::setMaxValue( SensorParams *sp)
{
    Meter *meter;
    meter = sp->getMeter();
    QString f;
    f = sp->getParam("FORMAT");

    if ( f == "%full" )
        meter->setMax( 1 );

}
*/
#include "xmmssensor.moc"