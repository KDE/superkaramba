/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "karambawidget.h"

#include "bar.h"
#include "bar.moc"

Bar::Bar(KarambaWidget* k, int x, int y, int w, int h)
    : Meter(k, x, y, w, h),
      value(0),
      barValue(0),
      vertical(false)
{
    m_minValue = 0;
    m_maxValue = 100;
}

Bar::~Bar()
{
}

bool Bar::setImage(QString fileName)
{
    QFileInfo fileInfo(fileName);
    bool res = false;

    if(m_karamba->theme().isThemeFile(fileName))
    {
        QByteArray ba = m_karamba->theme().readThemeFile(fileName);
        res = pixmap.loadFromData(ba);
    }
    else
    {
        res = pixmap.load(fileName);
    }

    pixmapWidth = pixmap.width();
    pixmapHeight = pixmap.height();

    if(getWidth()==0 || getHeight()==0)
    {
        setWidth(pixmapWidth);
        setHeight(pixmapHeight);
    }

    if(res)
    {
        imagePath = fileName;
    }

    return res;
}

void Bar::setValue(int v)
{
    if(v > m_maxValue)
    {
        // maxValue = v;
        v = m_maxValue;
    }

    if(v < m_minValue)
    {
        //minValue = v;
        v = m_minValue;
    }

    barValue = v;

    int diff = m_maxValue - m_minValue;
    if(diff != 0)
    {
        if(vertical)
        {
            value = int((v-m_minValue)*getHeight() / diff + 0.5);
        }
        else // horizontal
        {
            value = int((v-m_minValue)*getWidth() / diff + 0.5);
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
    m_maxValue = m;
    recalculateValue();
}

void Bar::setMin(int m)
{
    m_minValue = m;
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

void Bar::update(QVariant value)
{
    QVariantMap map = value.toMap();
    setValue(map[m_format].toInt());
}
