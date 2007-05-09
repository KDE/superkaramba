/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson <karlsson.h@home.se>              *
 *   Copyright (C) 2003 Matt Broadstone <mbroadst@gmail.com>               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "bar.h"
#include "bar.moc"

#include "karamba.h"

#include <QPainter>
#include <QTimer>


Bar::Bar(Karamba* k, int x, int y, int width, int height)
        : Meter(k, x, y, width, height)
{
    m_value = 0;
    m_minValue = 0;
    m_maxValue = 100;
    m_barValue = 0;
    m_verticalBar = false;
    m_oldBarValue = -1;

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(valueChanged()));
}

Bar::~Bar()
{
    delete m_timer;
}

bool Bar::setImage(const QString &fileName)
{
    QFileInfo fileInfo(fileName);
    bool res = false;

    if (m_karamba->theme().isThemeFile(fileName)) {
        QByteArray ba = m_karamba->theme().readThemeFile(fileName);
        res = m_pixmap.loadFromData(ba);
    } else
        res = m_pixmap.load(fileName);

    m_pixmapWidth = m_pixmap.width();
    m_pixmapHeight = m_pixmap.height();

    if (getWidth() == 0 || getHeight() == 0) {
        setWidth(m_pixmapWidth);
        setHeight(m_pixmapHeight);
    }

    if (res)
        m_imagePath = fileName;

    return res;
}

QString Bar::getImage() const
{
    return m_imagePath;
}

void Bar::setValue(int v)
{
    if (v > m_maxValue)
        v = m_maxValue;

    if (v < m_minValue)
        v = m_minValue;

    m_oldBarValue = m_barValue;
    m_barValue = v;

    m_timer->start(5);
}

void Bar::setValue(const QString &v)
{
    setValue((int)(v.toDouble() + 0.5));
}

int Bar::getValue() const
{
    return m_barValue;
}

void Bar::valueChanged()
{
    int diff = m_maxValue - m_minValue;
    int size = 0;

    if (diff != 0) {
        if (m_verticalBar)
            size = getHeight();
        else
            size = getWidth();

        if (m_oldBarValue < m_barValue) {
            if (m_value < int((m_barValue - m_minValue)*size / diff + 0.5)) {
                m_value++;
                m_oldBarValue++;
            } else
                m_timer->stop();
        } else if (m_oldBarValue > m_barValue) {
            if (m_value > int((m_barValue - m_minValue)*size / diff + 0.5)) {
                m_value--;
                m_oldBarValue--;
            } else
                m_timer->stop();
        } else {
            m_timer->stop();
            m_value = int((m_barValue - m_minValue) * size / diff + 0.5);
        }
    } else {
        m_value = 0;
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
    m_verticalBar = b;
}

int Bar::getVertical() const
{
    return m_verticalBar;
}

void Bar::recalculateValue()
{
    setValue(m_barValue);
}

void Bar::paint(QPainter *p, const QStyleOptionGraphicsItem *option,
                QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // only draw image if not hidden
    if (!m_hidden) {
        int width, height;
        width = getWidth();
        height = getHeight();

        if (m_verticalBar)
            p->drawTiledPixmap(0, 0 + height - m_value, width, m_value, m_pixmap, 0, m_pixmapHeight
                    - m_value);
        else // horizontal
            p->drawTiledPixmap(0, 0, m_value, height, m_pixmap);
    }
}
