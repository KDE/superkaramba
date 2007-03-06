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

#include <kdebug.h>

Graph::Graph(Karamba* k, int x, int y, int w, int h, int nbrPts):
        Meter(k, x, y, w, h), lastValue(0)
{
    nbrPoints = (nbrPts == 0) ? 50 : nbrPts;

    for (int i = 0; i < nbrPoints; i++) {
        m_points << QPoint(w + 2, h);

        m_points.translate(((qreal) - w - 1) / nbrPoints, 0);
    }

    m_minValue = 0;
    m_maxValue = 100;
}

Graph::~Graph()
{}

void Graph::setValue(int v)
{
    if (v > m_maxValue) {
        // m_maxValue = v;
        v = m_maxValue;
    }
    if (v < m_minValue) {
        //m_minValue = v;
        v = m_minValue;
    }
    lastValue = v;

    QPoint newPoint(getWidth() - 1,
                    getHeight() - v * getHeight() / m_maxValue);

    m_points.translate(((qreal) - getWidth() - 1) / nbrPoints, 0);

    m_points << newPoint;

    if (m_points.count() > nbrPoints)
        m_points.remove(0);

    update();
}

void Graph::setValue(const QString &v)
{
    setValue((int)(v.toDouble() + 0.5));
}

void Graph::paint(QPainter *p, const QStyleOptionGraphicsItem *option,
                  QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (!m_hidden) {
        p->setPen(m_color);
        p->setOpacity(m_opacity);

        p->drawPolyline(m_points);
    }
}

#include "graph.moc"
