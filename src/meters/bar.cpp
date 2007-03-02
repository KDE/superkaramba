/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson <karlsson.h@home.se>              *
 *   Copyright (C) 2003 Matt Broadstone <mbroadst@gmail.com>               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <QPainter>
#include <QTimer>

#include <kdebug.h>

#include "karamba.h"
#include "bar.h"
#include "bar.moc"

Bar::Bar(Karamba* k, int x, int y, int w, int h)
    :   Meter(k, x, y, w, h)
{
    value = 0;
    minValue = 0;
    maxValue = 100;
    barValue = 0;
    vertical = false;
    oldBarValue = -1;

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(valueChanged()));
}

Bar::~Bar()
{
    delete m_timer;
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
        res = pixmap.load(fileName);

    pixmapWidth = pixmap.width();
    pixmapHeight = pixmap.height();

    if(getWidth()==0 || getHeight()==0)
    {
        setWidth(pixmapWidth);
        setHeight(pixmapHeight);
    }

    if(res)
        imagePath = fileName;

    return res;
}

void Bar::setValue( int v )
{
    if(v > maxValue)
        v = maxValue;

    if(v < minValue)
        v = minValue;

    oldBarValue = barValue;
    barValue = v;

    m_timer->start(5);
}

void Bar::setValue(QString v)
{
    setValue((int)(v.toDouble() + 0.5));
}

//void Bar::timerEvent(QTimerEvent *event)
void Bar::valueChanged()
{
    int diff = maxValue - minValue;
    int size = 0;

    if (diff != 0)
    {
        if(vertical)
            size = getHeight();
        else
            size = getWidth();

        if (oldBarValue < barValue)
        {
            if (value < int((barValue-minValue)*size / diff + 0.5))
            {
                value++;
                oldBarValue++;
            }
            else
                m_timer->stop();
        }
        else if (oldBarValue > barValue)
        {
            if (value > int((barValue-minValue)*size / diff + 0.5))
            {
                value--;
                oldBarValue--;
            }
            else
                m_timer->stop();
        }
        else
        {
            m_timer->stop();
            value = int((barValue-minValue)*size / diff + 0.5);
        }
    }
    else
    {
        value = 0;
        m_timer->stop();
    }

    update();
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

void Bar::paint(QPainter *p, const QStyleOptionGraphicsItem *option,
                QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    int width, height;
    width = getWidth();
    height = getHeight();

    //only draw image if not hidden
    if(hidden == 0)
    {
        p->setOpacity(m_opacity);
        if(vertical)
            p->drawTiledPixmap(0, 0+height-value, width, value, pixmap, 0, pixmapHeight-value);
        else // horizontal
            p->drawTiledPixmap(0, 0, value, height, pixmap);
    }
}
