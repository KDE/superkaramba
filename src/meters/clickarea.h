/*
 * Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>
 * Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>
 * Copyright (c) 2005 Ryan Nickell <p0z3r@earthlink.net>
 *
 * This file is part of SuperKaramba.
 *
 *  SuperKaramba is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  SuperKaramba is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SuperKaramba; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 ****************************************************************************/
#ifndef CLICKAREA_H
#define CLICKAREA_H

#include <qstring.h>
#include <qrect.h>
#include <qevent.h>
#include <qregexp.h>
#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>
#include <QRect>
#include <QPainter>

#include <kprocess.h>
#include <kprocio.h>
#include <krun.h>

#include <meters/meter.h>

/**
 *
 * Hans Karlsson
 **/
class ClickArea : public Meter
{
    Q_OBJECT
public:
    ClickArea(Karamba* k, bool preview, int x, int y, int w, int h);

    ~ClickArea();

    void setOnClick(QString);
    void setServiceOnClick(QString , QString, QString);
    void setOnMiddleClick(QString);

    bool mouseEvent(QGraphicsSceneMouseEvent *e);

    QRect getRectangle();

    void mUpdate(QPainter*);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0);

    void setValue(int);
    void setValue(const QString&);

    bool m_preview;

    QString onClick;
    QString svc_onClick;
    QString svc_name;
    QString svc_icon;
    QString onMiddleClick;
    QString value;
};

#endif
