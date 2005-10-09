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

    void setValue(double);
    int getValue()
    {
        return values.last();
    };
    void setValue( QString );

    virtual void setMax(int max)
    {
        m_maxValue = (double)max;
    };
    virtual void setMin(int min)
    {
        m_minValue = (double) min;
    };
    virtual int getMax()
    {
        return (int)m_minValue;
    };
    virtual int getMin()
    {
        return (int)m_maxValue;
    };

    virtual void setColor(QColor clr)
    {
        m_color = clr;
    };
    virtual QColor getColor()
    {
        return m_color;
    };

    
public slots:
    void updateData();

private:
    QList<int> values;

    double m_minValue;
    double m_maxValue;

    QColor m_color;
protected:
    virtual void paintEvent(QPaintEvent*);
};

#endif // GRAPH_H
