/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson <karlsson.h@home.se>              *
 *   Copyright (C) 2003 Matt Broadstone <mbroadst@gmail.com>               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <QMouseEvent>
#include <QTimer>
#include <QPainter>

#include <kdebug.h>

#include "../karamba.h"
#include "meter.h"
#include "meter.moc"

Meter::Meter(Karamba* k, int ix, int iy, int iw, int ih)
        :   QGraphicsItem(k, k->getScene()),
        m_clickable(true),
        m_hidden(false),
        m_minValue(0),
        m_maxValue(0),
        m_color(0, 0, 0),
        m_karamba(k),
        m_opacity(0)
{
    m_boundingBox = QRect(0, 0, iw, ih);

    QPointF ps = QPointF(ix, iy);
    setPos(ps.x(), ps.y());

    m_opacityTimer = new QTimer(this);
    connect(m_opacityTimer, SIGNAL(timeout()), this, SLOT(fadeIn()));
    m_opacityTimer->start(10);
}

Meter::Meter(Karamba* k)
        :   QGraphicsItem(k),
        m_boundingBox(0, 0, 0, 0),
        m_clickable(true),
        m_hidden(0),
        m_minValue(0),
        m_maxValue(0),
        m_color(0, 0, 0),
        m_karamba(k),
        m_opacity(1)
{}

Meter::~Meter()
{
    delete m_opacityTimer;
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
    return m_boundingBox.width();
}

void Meter::setWidth(int width)
{
    prepareGeometryChange();

    m_boundingBox.setWidth(width);

    update();
}

int Meter::getHeight() const
{
    return m_boundingBox.height();
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
    return QString::null;
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

double Meter::getOpacity() const
{
    return m_opacity;
}

void Meter::setOpacity(double value)
{
    m_opacity = value;
    update();
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
    return QRectF(m_boundingBox);
}

void Meter::fadeIn()
{
    setOpacity(m_opacity + 0.05);   // m_opacity is set in function

    if (m_opacity > 1.05) {
        m_opacityTimer->stop();
        delete m_opacityTimer;
        m_opacityTimer = (QTimer*)0;
    }
}

/*
void Meter::fadeOut(bool del)
{
  setOpacity(m_opacity - 0.05);   // m_opacity is set in function

  if(m_opacity < 0.0)
  {
    m_opacityTimer->stop();
    if(del)
    {
     // delete this;
    }
    else
    {
      //delete m_opacityTimer;
      //m_opacityTimer = (QTimer*)0;
    }
  }
}

void Meter::deleteMeter()
{
  m_opacityTimer = new QTimer(this);
  connect(m_opacityTimer, SIGNAL(timeout()), this, SLOT(fadeOut()));
  m_opacityTimer->start(10);
}
*/
/*
bool Meter::click(QMouseEvent*)
{
    return false;
}
*/

