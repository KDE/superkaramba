/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <QString>

#include "graph.h"
#include "graph.moc"
#include <kdebug.h>
#include "karamba_python.h"
Graph::Graph(KarambaWidget* k, int x, int y, int w, int h, int nbrPts)
        :   Meter(k, x, y, w, h)
{
    nbrPts = (nbrPts==0)? 50:nbrPts ;
    for(int i = 0; i < nbrPts; i++)
    {
        values.append(0);
    }

    m_minValue = 0;
    m_maxValue = 100;
}

Graph::~Graph()
{}

void Graph::setValue(double v)
{
    kdDebug() << v << endl;
    v=qMin(v,m_maxValue);
    v=qMax(v,m_minValue);
    values.removeFirst();
    values.append( qRound(v / m_maxValue * (height()-1)));
    update();
}

void Graph::setValue(QString v)
{
    setValue(v.toDouble());
}

void Graph::updateData()
{
    setValue(decodeDot(m_format.remove('%')).toInt());
//     QVariantMap map = values.toMap();
//     setValue(map[m_format].toInt());
}

void Graph::paintEvent( QPaintEvent *)
{
    QPainter p(this);
    double step = (width() / (float)(values.size()-1));
    double xPos = 0;
    double nextXPos = 0;
    p.setPen(m_color);
    /*may be we can use here drawLines() instead of drawLine()? */
    for (int i = 0; i < values.size()- 1 ; i ++)
    {
        nextXPos=xPos+step;
        p.drawLine( (int)xPos, (height() -(int) values.at(i))-1 ,
                     (int)nextXPos, (height() - (int) values.at(i+1))-1);
        xPos = nextXPos;
    }
    if ((*pythonIface) && (*pythonIface)->isExtensionLoaded())
            (*pythonIface)->widgetUpdated(m_karamba);
}
