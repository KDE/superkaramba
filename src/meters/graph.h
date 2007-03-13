/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef GRAPH_H
#define GRAPH_H

#include "meter.h"
#include <qpainter.h>
#include <qcolor.h>

class Graph : public Meter
{
    Q_OBJECT

public:
    Graph(Karamba* k, int ix, int iy, int iw, int ih, int nbrPoints);
    Graph();
    ~Graph();

    void setValue(int);
    int getValue() const
    {
        return lastValue;
    }
    void setValue(const QString&);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);

private:
    int nbrPoints;
    int lastValue;

    QPolygonF m_points;
};

#endif // GRAPH_H
