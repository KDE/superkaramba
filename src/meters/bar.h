/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson <karlsson.h@home.se>              *
 *   Copyright (C) 2003 Matt Broadstone <mbroadst@gmail.com>               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef BAR_H
#define BAR_H

#include "meter.h"

class QPainter;
class QTimer;

class Bar : public Meter
{
    Q_OBJECT
public:
    Bar(Karamba* k, int x, int y, int width, int height);
    ~Bar();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);

    virtual void setMax(int m);
    virtual void setMin(int m);

public slots:
    bool setImage(const QString &imagePath);
    QString getImage() const;

    void setValue(const QString&);
    void setValue(int);
    int  getValue() const;

    void setVertical(bool);
    int  getVertical() const;

    void recalculateValue();

    void valueChanged();

private:
    int m_barValue;
    int m_oldBarValue;
    int m_value;

    QTimer *m_timer;

    int m_pixmapWidth;
    int m_pixmapHeight;

    bool m_verticalBar;

    QString m_imagePath;
    QPixmap m_pixmap;

};

#endif // BAR_H
