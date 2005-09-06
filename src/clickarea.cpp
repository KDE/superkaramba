/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "clickarea.h"
#include "clickarea.moc"

ClickArea::ClickArea(KarambaWidget* k, int x, int y, int w, int h )
        : Meter(k, x, y, w, h )
{
    value = "";
    rect = QRect( x, y, w, h );
}

ClickArea::~ClickArea()
{
}

bool ClickArea::click( QMouseEvent *e )
{
    if( rect.contains( e->x(), e->y() ) )
    {
        //qDebug(QString::number(e->type()));
	//KShellProcess ksp;
        QString program;
        if( e->button() == Qt::LeftButton )
        {
            program = onClick;
        }
        program.replace( QRegExp("%v", false), value );

        if( !program.isEmpty())
        {
            //qDebug(program);
          KRun::runCommand(program);
	  //  ksp << program;
          //  ksp.start(KProcIO::DontCare, KProcIO::NoCommunication);
        }
    }
    return false;
}

void ClickArea::setOnClick( QString oc )
{
    onClick = oc;
}

void ClickArea::setOnMiddleClick( QString oc )
{
    onMiddleClick = oc;
}


QRect ClickArea::getRectangle()
{
    return rect;
}

void ClickArea::mUpdate( QPainter *p )
{

    p->drawRect(boundingBox);

}

void ClickArea::setValue( int v)
{

    setValue( QString::number( v ) );

}


void ClickArea::setValue( QString v )
{
    value = v;
}
