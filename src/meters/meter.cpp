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
        boundingBox(ix, iy, iw, ih),
        leftButtonAction(""),
        middleButtonAction(""),
        rightButtonAction(""),
        clickable(true),
        hidden(0),
        minValue(0),
        maxValue(0),
        color(0,0,0),
        m_karamba(k),
        m_opacity(0.0)
{
    m_opacityTimer = new QTimer(this);
    connect(m_opacityTimer, SIGNAL(timeout()), this, SLOT(fadeIn()));
    m_opacityTimer->start(10);
}

Meter::Meter(Karamba* k)
    :   QGraphicsItem(k),
        boundingBox(0, 0, 0, 0),
        leftButtonAction(""),
        middleButtonAction(""),
        rightButtonAction(""),
        clickable(true),
        hidden(0),
        minValue(0),
        maxValue(0),
        color(0,0,0),
        m_karamba(k),
        m_opacity(1)
{
}

Meter::~Meter()
{
    delete m_opacityTimer;
}

void Meter::fadeIn()
{
    setOpacity(m_opacity + 0.05);   // m_opacity is set in function

    if (m_opacity > 1.05)
    {
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

bool Meter::click(QMouseEvent*)
{
    return false;
}

void Meter::setSize(int ix, int iy, int iw, int ih)
{
    prepareGeometryChange();

    boundingBox.setRect(ix, iy, iw, ih);
    recalculateValue();

    update();
}

void Meter::setThemePath( QString path )
{
    themePath = path;
}

int Meter::getX()
{
    return boundingBox.x();
}

int Meter::getY()
{
    return boundingBox.y();
}

void Meter::setX(int newx)
{
    prepareGeometryChange();

    int temp = boundingBox.width();
    boundingBox.setX(newx);
    boundingBox.setWidth(temp);

    update();
}

void Meter::setY(int newy)
{
    prepareGeometryChange();

    int temp = boundingBox.height();
    boundingBox.setY(newy);
    boundingBox.setHeight(temp);

    update();
}

int Meter::getWidth()
{
    return boundingBox.width();
}

int Meter::getHeight()
{
    return boundingBox.height();
}

void Meter::setWidth(int width)
{
    prepareGeometryChange();

    boundingBox.setWidth(width);
    recalculateValue();

    update();
}

void Meter::setHeight(int height)
{
    prepareGeometryChange();

    boundingBox.setHeight(height);
    recalculateValue();

    update();
}

QRect Meter::getBoundingBox()
{
    return boundingBox;
}

void Meter::setEnabled(bool e)
{
    clickable = e;
}

bool Meter::isEnabled()
{
    return clickable;
}

bool Meter::insideActiveArea(int x, int y)
{
    return boundingBox.contains(x, y) && clickable;
}

void Meter::setOpacity(double value)
{
    m_opacity = value;
    update();
}

