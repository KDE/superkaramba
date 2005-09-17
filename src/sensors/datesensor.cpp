/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <QApplication>
#include <QDesktopWidget>

#include "datesensor.h"
#include "datesensor.moc"

DateSensor::DateSensor(int interval)
        :   Sensor( interval ),
        hidden(true)
{
}

DateSensor::~DateSensor()
{}

void DateSensor::update()
{
    QDateTime qdt  =  QDateTime::currentDateTime();
    data["d"]=QVariant(qdt.toString("d"));
    data["dd"]=QVariant(qdt.toString("dd"));
    data["ddd"]=QVariant(qdt.toString("ddd"));
    data["dddd"]=QVariant(qdt.toString("ddddd"));
    data["M"]=QVariant(qdt.toString("M"));
    data["MM"]=QVariant(qdt.toString("MM"));
    data["MMM"]=QVariant(qdt.toString("MMM"));
    data["MMMM"]=QVariant(qdt.toString("MMMM"));
    data["yy"]=QVariant(qdt.toString("yy"));
    data["yyyy"]=QVariant(qdt.toString("yyyy"));
    data["h"]=QVariant(qdt.toString("h"));
    data["hh"]=QVariant(qdt.toString("hh"));
    data["m"]=QVariant(qdt.toString("m"));
    data["mm"]=QVariant(qdt.toString("mm"));
    data["s"]=QVariant(qdt.toString("s"));
    data["ss"]=QVariant(qdt.toString("ss"));
    data["z"]=QVariant(qdt.toString("z"));
    data["zzz"]=QVariant(qdt.toString("zzz"));
    data["AP"]=QVariant(qdt.toString("AP"));
    data["ap"]=QVariant(qdt.toString("ap"));
    emit dateValue(QVariant(data));
}

void DateSensor::addMeter(Meter* meter)
{
    disconnect(0, 0, meter, SLOT(update(QVariant)));
    connect(this, SIGNAL(dateValue(QVariant)), meter, SLOT(update(QVariant)));
}

void DateSensor::slotCalendarDeleted()
{
    hidden = true;
    cal = 0L;
}

DatePicker::DatePicker(QWidget *parent)
        : Q3VBox(parent, 0, Qt::Window | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setFrameStyle( QFrame::PopupPanel | QFrame::Raised );
    //KWin::setOnAllDesktops( handle(), true );
    picker = new KDatePicker(this);
    picker->setCloseButton(true);

    /* name and icon for kicker's taskbar */
    //setCaption(i18n("Calendar"));
    //setIcon(SmallIcon("date"));
}

void DatePicker::keyReleaseEvent(QKeyEvent *e)
{
    Q3VBox::keyReleaseEvent(e);
    if (e->key() == Qt::Key_Escape)
    {
        close();
    }
}

/*void DateSensor::toggleCalendar(QMouseEvent *ev)
{
    foreach (QObject *it, objList)
    {
        SensorParams *sp = (SensorParams*)(it);
        Meter *meter = sp->getMeter();
        QString width = sp->getParam("CALWIDTH");
        QString height = sp->getParam("CALHEIGHT");
 
        QRect rect(meter->getX(),meter->getY(),width.toInt(), height.toInt());
        if (rect.contains( ev->x(), ev->y() ))
        {
            if (hidden)
            {
                hidden = false;
                cal = new DatePicker(0);
 
                connect(cal, SIGNAL(destroyed()), SLOT(slotCalendarDeleted()));
                QPoint c = (QPoint(ev->x(), ev->y()));
 
                int w = cal->sizeHint().width();
                int h = cal->sizeHint().height();
 
                // make calendar fully visible
                QRect deskR = QApplication::desktop()->screenGeometry(QApplication::desktop()->screenNumber(c));
 
                if (c.y()+h > deskR.bottom())
                {
                    c.setY(deskR.bottom()-h-1);
                }
                if (c.x()+w > deskR.right())
                {
                    c.setX(deskR.right()-w-1);
                }
 
                cal->move(c);
                cal->show();
 
            }
            else
            {
                cal->close();
            }
        }
    }
}*/

/*void DateSensor::mousePressEvent(QMouseEvent *ev)
{
    switch (ev->button()) 
    {
        case Qt::LeftButton:
            toggleCalendar(ev);
            break;
        default:
            break;
    }
}*/

