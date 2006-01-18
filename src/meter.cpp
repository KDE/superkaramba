/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "meter.h"

Meter::Meter(karamba* k, int ix, int iy, int iw, int ih):
  boundingBox(ix, iy, iw, ih), leftButtonAction(""), middleButtonAction(""),
  rightButtonAction(""), clickable(true), hidden(0), minValue(0), maxValue(0),
  color(0,0,0), m_karamba(k)
{
}

Meter::Meter(karamba* k):
  boundingBox(0, 0, 0, 0), leftButtonAction(""), middleButtonAction(""),
  rightButtonAction(""), clickable(true), hidden(0), minValue(0), maxValue(0),
  color(0,0,0), m_karamba(k)
{
}

Meter::~Meter()
{
}

bool Meter::click(QMouseEvent*)
{
  return false;
}

void Meter::setSize(int ix, int iy, int iw, int ih)
{
  boundingBox.setRect(ix, iy, iw, ih);
  recalculateValue();
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
  int temp = boundingBox.width();
  boundingBox.setX(newx);
  boundingBox.setWidth(temp);
}

void Meter::setY(int newy)
{
  int temp = boundingBox.height();
  boundingBox.setY(newy);
  boundingBox.setHeight(temp);
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
  boundingBox.setWidth(width);
  recalculateValue();
}

void Meter::setHeight(int height)
{
  boundingBox.setHeight(height);
  recalculateValue();
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

#include "meter.moc"
