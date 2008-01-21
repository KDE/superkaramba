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

#include <QPainter>
#include <QColor>

#include "meter.h"

class Graph : public Meter
{
    Q_OBJECT

public:
    Graph(Karamba* k, int ix, int iy, int iw, int ih, int nbrPoints);
    Graph();
    ~Graph();

    void setValue(int);
    int getValue() const;
    void setValue(const QString&);
    QColor getFillColor() const;
    void setFillColor(QColor);
    void setScrollDirection(const QString&);
    QString getScrollDirection() const;
    void setPlotDirection(const QString&);
    QString getPlotDirection() const;
    bool shouldFill() const;
    void shouldFill(bool b);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);
private:

    enum SCROLL_DIR {
        SCROLL_LEFT = -1,
        SCROLL_RIGHT = 1
    };

    enum PLOT_DIR {
        PLOT_DOWN = -1,
        PLOT_UP = 1
    };

    int nbrPoints;
    int lastValue;

    bool fill;
    SCROLL_DIR scrollDir;
    PLOT_DIR plotDir;
    QColor fillColor;
    QVector<int> m_values;
};

#endif // GRAPH_H
