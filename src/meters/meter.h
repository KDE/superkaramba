/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson <karlsson.h@home.se>              *
 *   Copyright (C) 2003 Matt Broadstone <mbroadst@gmail.com>               *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef METER_H
#define METER_H

#include <QObject>
#include <QMouseEvent>
#include <QGraphicsItem>

#include "../superkaramba_export.h"

class QPainter;
class Karamba;

class SUPERKARAMBA_EXPORT Meter : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    Meter(Karamba* k, int ix, int iy, int iw, int ih);
    Meter(Karamba* k);
    virtual ~Meter();

    virtual int getX() const;
    virtual void setX(int);

    virtual int getY() const;
    virtual void setY(int);

    virtual int getWidth() const;
    virtual void setWidth(int);

    virtual int getHeight() const;
    virtual void setHeight(int);

    virtual int getMin() const;
    virtual void setMin(int min);

    virtual int getMax() const;
    virtual void setMax(int max);

    virtual int getValue() const;
    virtual void setValue(int);

    virtual QString getStringValue() const;
    virtual void setValue(const QString&);

    virtual QColor getColor() const;
    virtual void setColor(QColor color);

    bool isEnabled() const;
    void setEnabled(bool enable);

    virtual void setSize(int x, int y, int width, int height);

    virtual void show();
    virtual void hide();

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget) = 0;

    virtual QRectF boundingRect() const;

protected:
    QRectF m_boundingBox;

    bool m_clickable;
    bool m_hidden;
    int m_minValue;
    int m_maxValue;

    QColor m_color;

    Karamba* m_karamba;
};

#endif // METER_H
