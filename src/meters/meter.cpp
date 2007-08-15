/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson <karlsson.h@home.se>              *
 *   Copyright (C) 2003 Matt Broadstone <mbroadst@gmail.com>               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "meter.h"
#include "meter.moc"

#include "karamba.h"

#include <QMouseEvent>
#include <QTimer>
#include <QPainter>

static int zLevel = 0;

Meter::Meter(Karamba* k, int ix, int iy, int iw, int ih)
        :   QGraphicsItem(k, k->getScene()),
        m_clickable(true),
        m_hidden(false),
        m_minValue(0),
        m_maxValue(0),
        m_color(0, 0, 0),
        m_karamba(k)
{
    m_boundingBox = QRectF(0.0, 0.0, static_cast<double>(iw), static_cast<double>(ih));

    QPointF ps = QPointF(ix, iy);
    setPos(ps.x(), ps.y());

    setZValue(zLevel++);
}

Meter::Meter(Karamba* k)
        :   QGraphicsItem(k),
        m_boundingBox(0, 0, 0, 0),
        m_clickable(true),
        m_hidden(0),
        m_minValue(0),
        m_maxValue(0),
        m_color(0, 0, 0),
        m_karamba(k)
{}

Meter::~Meter()
{
    zLevel--;
}

int Meter::getX() const
{
    return static_cast<int>(x());
}

void Meter::setX(int newx)
{
    prepareGeometryChange();

    setPos(newx, y());

    update();
}

int Meter::getY() const
{
    return static_cast<int>(y());
}

void Meter::setY(int newy)
{
    prepareGeometryChange();

    setPos(x(), newy);

    update();
}

int Meter::getWidth() const
{
    return static_cast<int>(m_boundingBox.width());
}

void Meter::setWidth(int width)
{
    prepareGeometryChange();

    m_boundingBox.setWidth(width);

    update();
}

int Meter::getHeight() const
{
    return static_cast<int>(m_boundingBox.height());
}

void Meter::setHeight(int height)
{
    prepareGeometryChange();

    m_boundingBox.setHeight(height);

    update();
}

int Meter::getMin() const
{
    return m_minValue;
}

void Meter::setMin(int min)
{
    m_minValue = min;
}

int Meter::getMax() const
{
    return m_maxValue;
}

void Meter::setMax(int max)
{
    m_maxValue = max;
}

int Meter::getValue() const
{
    return -1;
}

void Meter::setValue(int)
{
}

QString Meter::getStringValue() const
{
    return QString();
}

void Meter::setValue(const QString&)
{
}

QColor Meter::getColor() const
{
    return m_color;
}

void Meter::setColor(QColor color)
{
    m_color = color;
}

void Meter::setSize(int x, int y, int width, int height)
{
    prepareGeometryChange();

    m_boundingBox.setRect(0, 0, width, height);
    setPos(x, y);

    update();
}

bool Meter::isEnabled() const
{
    return m_clickable;
}

void Meter::setEnabled(bool e)
{
    m_clickable = e;
}

void Meter::show()
{
    m_hidden = false;

    update();
}

void Meter::hide()
{
    m_hidden = true;

    update();
}

QRectF Meter::boundingRect() const
{
    return m_boundingBox;
}

