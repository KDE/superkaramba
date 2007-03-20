/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                    *
 *                                                                         *
 *   Copyright (C) 2004,2005 Luke Kenneth Casson Leighton <lkcl@lkcl.net>  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "clickarea.h"

#include <kservice.h>
#include <kservicegroup.h>

#include <QMouseEvent>
#include <kdebug.h>


ClickArea::ClickArea(Karamba* k, bool preview, int x, int y, int w, int h)
        : Meter(k, x, y, w, h), m_preview(preview)
{
    value = "";
    setAcceptedMouseButtons(Qt::LeftButton);
}

ClickArea::~ClickArea()
{}

bool ClickArea::mouseEvent(QGraphicsSceneMouseEvent *e)
{
    //KShellProcess ksp;
    Q_UNUSED(e);

    if (!svc_name.isEmpty()) {
        KService sv(svc_name, svc_onClick, svc_icon);
        KUrl::List l;
        KRun::run(sv, l, 0);
        return false;
    } else {
        QString program;
        program = onClick;
        program.replace(QRegExp("%v", Qt::CaseInsensitive), value);

        if (!program.isEmpty()) {
            //qDebug(program);
            KRun::runCommand(program);
        }
    }

    return false;
}

void ClickArea::setOnClick(QString oc)
{
    onClick = oc;
}

void ClickArea::setServiceOnClick(QString name , QString exec, QString icon)
{
    svc_name = name;
    svc_onClick = exec;
    svc_icon = icon;
}

void ClickArea::setOnMiddleClick(QString oc)
{
    onMiddleClick = oc;
}


QRect ClickArea::getRectangle()
{
    return boundingRect().toRect();
}

void ClickArea::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                      QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (m_preview)
        painter->drawRect(boundingRect());
}


void ClickArea::setValue(int v)
{
    setValue(QString::number(v));
}


void ClickArea::setValue(const QString &v)
{
    value = v;
}

#include "clickarea.moc"
