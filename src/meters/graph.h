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
    Graph(KarambaWidget* k, int ix, int iy, int iw, int ih, int nbrPoints);
    Graph();
    ~Graph();

    void setValue( int );
    int getValue()
    {
        return values.last();
    };
    void setValue( QString );

    virtual void setMax(int max)
    {
        m_maxValue = max;
    };
    virtual void setMin(int min)
    {
        m_minValue = min;
    };
    virtual int getMax()
    {
        return m_minValue;
    };
    virtual int getMin()
    {
        return m_maxValue;
    };

    virtual void setColor(QColor clr)
    {
        m_color = clr;
    };
    virtual QColor getColor()
    {
        return m_color;
    };

    void mUpdate( QPainter * );

public slots:
    void update();

private:
    QList<int> values;

    int m_minValue;
    int m_maxValue;

    QColor m_color;
protected:
    virtual void paintEvent(QPaintEvent*);
};

#endif // GRAPH_H
