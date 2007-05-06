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

Graph::Graph(Karamba* k, int x, int y, int w, int h, int nbrPts):
        Meter(k, x, y, w, h), lastValue(0)
{
    nbrPoints = (nbrPts == 0) ? 50 : nbrPts;

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

    m_values << v;

    if (m_values.count() > nbrPoints) {
        m_values.remove(0);
    }

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

        QPolygonF line;

        for (int i = 0; i < nbrPoints - m_values.count() - 1; i++) {
            line << QPoint(getWidth() + 2, getHeight());

            line.translate(((qreal) - getWidth() - 1) / nbrPoints, 0);
        }

        foreach (int v, m_values) {
            QPoint point(getWidth() - 1,
                    getHeight() - v * getHeight() / m_maxValue);

            line.translate(((qreal) - getWidth() - 1) / nbrPoints, 0);

            line << point;
        }

        p->drawPolyline(line);
    }
}

#include "graph.moc"
