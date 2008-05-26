/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "graph.h"
#include <qstring.h>

Graph::Graph(karamba* k, int x, int y, int w, int h, int nbrPts):
  Meter(k, x, y, w, h), lastValue(0)
{

    nbrPoints = (nbrPts==0)? 50:nbrPts ;
    ptPtr = 0;
    values = new int[nbrPoints];
    for(int i = 0; i < nbrPoints; i++)
        values[i] = 0;
    minValue = 0;
    maxValue = 100;
}

Graph::~Graph()
{
    delete[] values;
}

void Graph::setValue( long v)
{
    if( v > maxValue)
    {
        // maxValue = v;
        v = maxValue;
    }
    if( v < minValue)
    {
        //minValue = v;
        v = minValue;
    }
    lastValue = v;
    values[ptPtr] = (int) (v / (maxValue + 0.0001) * getHeight());
    ptPtr = (ptPtr + 1) % nbrPoints;
}

void Graph::setValue( QString v )
{
  setValue((long)(v.toDouble() + 0.5));
}

void Graph::mUpdate(QPainter *p)
{
  if (hidden == 0)
  {
    double step = (getWidth() / (nbrPoints-1.001));
    double xPos = 0;
    double nextXPos = 0;
    p->setPen(color);
    for (int i = 0; i < nbrPoints - 1 ; i ++)
    {
      nextXPos = xPos + step;
      p->drawLine(getX() + (int)xPos, getY()+getHeight() -
                    (int) values[(ptPtr+i) % nbrPoints] ,
                  getX() + (int)nextXPos, getY()+getHeight() -
                    (int) values[(ptPtr + i +1) % nbrPoints] );
      xPos = nextXPos;
    }
  }
}

#include "graph.moc"
