/***************************************************************************
 *   Copyright (C) 2003 by Ralph M. Churchill <mrchucho@yahoo.com>         *
 *   Copyright (C) 2007 Matt Broadstone <mbroadst@gmail.com>               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CLICKMAP_H
#define CLICKMAP_H

#include "textfield.h"
#include "meter.h"

class ClickMap : public Meter
{
    Q_OBJECT
public:
    ClickMap(Karamba* k, int x, int y, int w, int h);
    ~ClickMap();

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0);

    void setValue(const QString&);
    void setValue(int);
    void setTextProps(TextField *);

private:
    TextField text;

    QStringList links;
    QStringList displays;

};

#endif
