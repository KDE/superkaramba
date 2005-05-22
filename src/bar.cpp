/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "bar.h"
#include "karamba.h"

Bar::Bar(karamba* k, int x, int y, int w, int h) : Meter(k, x, y, w, h)
{
  value = 0;
  minValue = 0;
  maxValue = 100;
  barValue = 0;
  vertical = FALSE;
}

Bar::~Bar()
{
}

bool Bar::setImage(QString fileName)
{
  QFileInfo fileInfo(fileName);
  bool res = false;
  QString path;

  if(fileInfo.isRelative() && m_karamba->zipTheme())
  {
    QByteArray ba = m_karamba->readZipFile(fileName);
    res = pixmap.loadFromData(ba);
    path = fileName;
  }
  else
  {
    if(fileInfo.isRelative())
    {
      path = m_karamba->themePath + "/" + fileName;
    }
    else
    {
      path = fileName;
    }
    //qDebug( "bar: fileName=" + path );
    res = pixmap.load( path );
  }
  pixmapWidth = pixmap.width();
  pixmapHeight = pixmap.height();

  if(getWidth()==0 || getHeight()==0)
  {
    setWidth(pixmapWidth);
    setHeight(pixmapHeight);
  }
  if(res)
    imagePath = path;
  return res;
}

void Bar::setValue( int v )
{
  if(v > maxValue)
  {
    // maxValue = v;
    v = maxValue;
  }

  if(v < minValue)
  {
    //minValue = v;
    v = minValue;
  }

  barValue = v;

  int diff = maxValue - minValue;
  if(diff != 0)
  {
    if(vertical)
    {
      value = int((v-minValue)*getHeight() / diff + 0.5);
    }
    else // horizontal
    {
      value = int((v-minValue)*getWidth() / diff + 0.5);
    }
  }
  else
  {
    value = 0;
  }
}

void Bar::setValue(QString v)
{
  setValue((int)(v.toDouble() + 0.5));
}

void Bar::setMax(int m)
{
  Meter::setMax(m);
  recalculateValue();
}

void Bar::setMin(int m)
{
  Meter::setMin(m);
  recalculateValue();
}

void Bar::setVertical(bool b)
{
  vertical = b;
}

void Bar::mUpdate(QPainter *p)
{
  int x, y, width, height;
  x = getX();
  y = getY();
  width = getWidth();
  height = getHeight();
  //only draw image if not hidden
  if(hidden == 0)
  {
    if(vertical)
    {
      //  int v = int( (value-minValue)*height / (maxValue-minValue) + 0.5 );
      p->drawTiledPixmap(x, y+height-value, width, value, pixmap, 0,
                         pixmapHeight-value);
    }
    else // horizontal
    {
      //int v = int( (value-minValue)*width / (maxValue-minValue) + 0.5 );
      p->drawTiledPixmap(x, y, value, height, pixmap);
    }
  }
}
