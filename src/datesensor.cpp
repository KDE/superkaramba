/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "datesensor.h"

#include <qapplication.h>
#include <QDesktopWidget>

DateSensor::DateSensor( int interval ) : Sensor( interval )
{
	hidden = true;
}
DateSensor::~DateSensor()
{
}

void DateSensor::update()
{
    QDateTime qdt  =  QDateTime::currentDateTime();
    QString format;
    SensorParams *sp;
    Meter *meter;

    foreach (QObject *it, objList)
    {
        sp = (SensorParams*)(it);
        meter = sp->getMeter();
        format = sp->getParam("FORMAT");

        if (format.length() == 0 )
        {
	   format = "hh:mm";
        }
        meter->setValue(qdt.toString(format));
    }
}

void DateSensor::slotCalendarDeleted()
{
	hidden = true;
	cal = 0L;
}

DatePicker::DatePicker(QWidget *parent)
    : Q3VBox( parent, 0, Qt::Window | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint )
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
                close();
}

void DateSensor::toggleCalendar(QMouseEvent *ev)
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


				if (c.y()+h > deskR.bottom())	c.setY(deskR.bottom()-h-1);
				if (c.x()+w > deskR.right())	c.setX(deskR.right()-w-1);
				cal->move(c);
				cal->show();

			}
			else
			{
				cal->close();
			}
		}
	}
}

void DateSensor::mousePressEvent(QMouseEvent *ev)
{
	switch (ev->button()) 
	{
		case Qt::LeftButton:
			toggleCalendar(ev);
			break;
		default:
			break;
	}
}


#include "datesensor.moc"
