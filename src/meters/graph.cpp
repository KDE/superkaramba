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

    fill = false;
    fillColor = m_color;
    scrollDir = SCROLL_RIGHT;
    plotDir = PLOT_UP;
    //prime m_values with a size of nbrPoints and filled with 0's
    m_values.resize(nbrPoints);
    m_values.fill(0);
}

Graph::~Graph()
{}

QColor Graph::getFillColor() const
{
    return fillColor;
}
void Graph::setFillColor(QColor c)
{
    fillColor = c;
    fill = true;
    update();
}

void Graph::shouldFill(bool b)
{
    if (fill != b) {
      fill = b;
      update();
    }
}

bool Graph::shouldFill() const
{
    return fill;
}

int Graph::getValue() const
{
    return lastValue;
}

void Graph::setValue(int v)
{
    //keep lastValue between the bounds
    if (v > m_maxValue) {
        lastValue = m_maxValue;
    } else if (v < m_minValue) {
        lastValue = m_minValue;
    } else {
        lastValue = v;
    }
    m_values.prepend(lastValue);
    if (m_values.count() > nbrPoints) {
        m_values.erase(m_values.end() - 1);
    }
    update();
}

void Graph::setValue(const QString &v)
{
    double d = v.toDouble();
    if (d > 0) {
        d += 0.5;
    } else if (d < 0) {
        d -= 0.5;
    }
    setValue((int)d);
}

void Graph::setScrollDirection(const QString& dir)
{
    if (!dir.isEmpty() && dir.toLower() == "left")
        scrollDir = SCROLL_LEFT;
    else
        scrollDir = SCROLL_RIGHT;
}

QString Graph::getScrollDirection() const
{
    if (scrollDir == SCROLL_LEFT)
        return "LEFT";
    return "RIGHT";
}

void Graph::setPlotDirection(const QString& dir)
{
    if (!dir.isEmpty() && dir.toLower() == "down")
        plotDir = PLOT_DOWN;
    else
        plotDir = PLOT_UP;
}
QString Graph::getPlotDirection() const
{
    if (plotDir == PLOT_DOWN)
        return "DOWN";
    return "UP";
}

/**
  Notes on graphing rewrite - jwm

  Regardless of scroll direction, data is kept in m_values in order of most recent value first trailing off
  to the oldest value getting dropped from the list when it is at an index of nbrPoints:
    V1, V2, V3, ... ,Vn  where n=nbrPoints
  It is handier to use zero indexes, so we'll use n = nbrPoints - 1 and the following value list definition:
    V0, V1, V2, ... , Vn where n = nbrPoint - 1

  Scroll: (NOTE: when talking about scroll right or scroll left, I'm not sure which is more intuitive...to talk
  about which way the old data moves off of the screen or which way a "virtual" scroll bar would move...in the
  end I don't really care.

  Paint:
  During the creation of the polygon if the graph scrolls "left" that means that the newest values appear to
  be coming in off of the left (or a "virtual" scroll bar is being moved to the left). The oldest data will
  move off to the right and eventually drop off of the graph. This matches the natural order of the underlying
  data as represented above going from V0 to Vn. For left scrolling, this order must be reversed. When one
  primes the vector to be the right size and filled with 0's, the logic is simpler and more consistent. Indexes
  into the value array and the position on the polygon vector become functions of each other at all times.
  This eliminates the need for the previous approach of tracking pixel positions less than the current count
  of data points to graph and it also eliminates the need for all of the translation calls that were happening.
  Original graph code translated the polygon points to shift them over one 'step' before appending the next
  value. The approach below should be considerably less compute intensive especially as the number of data
  points grows.

  Invariants with scroll direction:
    m_values
      V0, V1, V2, ... Vn
    horizontal step
      move right by (pixel width count / data point count - 1)
      simplisticly, if the graph is 100 wide and there are 5 data points, the step value is 25
    vertical step factor:
      This is the distance between each data point and is given by:
        (pixel height count / (# possible int data points))
      so if the graph is 20 tall, and has a min of -10 and a max of 10, the absolute *distance* is
      max - min, but the number of points that involves is max - min + 1
    n:
      n = nbrPoints - 1 (since n is zero based)

  SCROLL_LEFT:
    Polygon
      { P0(V0), P1(V1), P2(V2), ... Pn(Vn) }
        1. i goes from 0 to n, k also goes from o to n at the same time
        2. append P to the end of the polygon making it Pk(step * i, Vi) where k == i

  SCROLL_RIGHT:
    Polygon
     { P0(Vn), P1(Vn-1), P2(Vn-2), ... Pn(V0) }
        1. i goes from n to 0, k goes from 0 to n (same as before)
        2. append P to the end of the polygon making it Pk(step * k, Vi) where k = n - i (which is the same as i = n - k)

 */
void Graph::paint(QPainter *p, const QStyleOptionGraphicsItem *option,
                  QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (m_hidden)
        return;

    //might want to fudge the width and height by -1 here
    int width = getWidth();
    int height = getHeight();

    int top = 0;
    int bottom = height;

    if (plotDir == PLOT_DOWN) {
        bottom = 0;
        top = height;
    }

    int n = nbrPoints - 1;

    qreal h_step = ((qreal) width) / n;
    qreal v_step = ((qreal) height * plotDir) / (m_maxValue - m_minValue) * -1; //neg one shows up here to make subtracting from h (bottom) easier later, can just add the two and not worry about order

    QPolygonF polygon;

    for (int k = 0; k <= n;k++) {
        int i = k;
        if (scrollDir == SCROLL_RIGHT) {
            i = n - k;
        }
        qreal raw = m_values.at(i);
        qreal v;
        //some quick shortcuts for edge cases
        if (raw <= m_minValue)
            v = bottom;
        else if (raw >= m_maxValue)
            v = top;
        else
            v = ((raw - m_minValue) * v_step) + bottom; // since the coordinate system is one with 0 at the top and h at the bottom for the default case

        polygon << QPointF(h_step * k, v);
    }

    p->setRenderHint(QPainter::Antialiasing);
    p->setRenderHint(QPainter::SmoothPixmapTransform);
    if (fill) {
        //compute the 0 boundary for the baseline
        qreal baseline = ((-m_minValue) * v_step) + bottom;
        //add "edges" from the baseline to the first and last points of graph
        polygon.insert(0, QPointF(polygon.first().x(), baseline));
        polygon.insert(polygon.end(), QPointF(polygon.last().x(), baseline));

        p->save();
        QBrush tpBrush(fillColor);
        p->setPen(Qt::NoPen);
        p->setBrush(tpBrush);
        p->drawPolygon(polygon);
        p->restore();
        //erase our edges allowing the reuse of this exact polygon
        polygon.erase(polygon.begin());
        polygon.erase(polygon.end() - 1);
    }
    p->setPen(m_color);
    p->drawPolyline(polygon);
}

#include "graph.moc"
