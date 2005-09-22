/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "meter.h"
#include "sensor.h"
#include "karambawidget.h"
#include <krootpixmap.h>
#include <kdebug.h>

Meter::Meter(KarambaWidget* k, int ix, int iy, int iw, int ih): 
        QWidget(k),
        boundingBox(ix, iy, iw, ih), leftButtonAction(""), middleButtonAction(""),
        rightButtonAction(""), clickable(true), hidden(0), m_sensor(0),
        m_karamba(k),autoUpdate(true)
{
    setGeometry(ix,iy,iw,ih);
    KRootPixmap* root= new KRootPixmap(this);
    root->start();
}

Meter::Meter(KarambaWidget* k):QWidget(k),
        boundingBox(0, 0, 0, 0), leftButtonAction(""), middleButtonAction(""),
        rightButtonAction(""), clickable(true), hidden(0), m_sensor(0),
        m_karamba(k)
{
    KRootPixmap* root= new KRootPixmap(this);
    root->start();
}

Meter::~Meter()
{}

void Meter::attachToSensor(Sensor* sensor)
{
    sensor->addMeter(this);    
    m_sensor = sensor;
}

void Meter::detachFromSensor()
{
    m_sensor->deleteMeter(this);
    m_sensor = 0;
}

/*bool Meter::click(QMouseEvent*)
{
    return false;
}*/
/*
void Meter::setSize(int ix, int iy, int iw, int ih)
{
    boundingBox.setRect(ix, iy, iw, ih);
    recalculateValue();
}

int Meter::getX() const
{
    return boundingBox.x();
}

int Meter::getY() const
{
    return boundingBox.y();
}

void Meter::setX(int newx)
{
    int temp = boundingBox.width();
    boundingBox.setX(newx);
    boundingBox.setWidth(temp);
}

void Meter::setY(int newy)
{
    int temp = boundingBox.height();
    boundingBox.setY(newy);
    boundingBox.setHeight(temp);
}

int Meter::getWidth() const
{
    return boundingBox.width();
}
int Meter::getHeight() const
{
    return boundingBox.height();
}

void Meter::setWidth(int width)
{
    boundingBox.setWidth(width);
    recalculateValue();
}

void Meter::setHeight(int height)
{
    boundingBox.setHeight(height);
    recalculateValue();
}

QRect Meter::getBoundingBox()
{
    return boundingBox;
}

void Meter::setEnabled(bool e)
{
    clickable = e;
}

bool Meter::isEnabled()
{
    return clickable;
}

bool Meter::insideActiveArea(int x, int y)
{
    return boundingBox.contains(x, y) && clickable;
}*/

void Meter::storeData(QVariant value)
{
    data=value;
    if(autoUpdate)
    {
        updateData();
    }
}
void Meter::updateData()
{
}

QVariant Meter::decodeDot( QString formatString )
{
    QStringList list=formatString.split('.');
    QVariant value=data;
    foreach(QString section,list)
    {
        if(value.toMap().find(section)==value.toMap().end()) return QVariant();
        else
        {
            value=value.toMap().value(section);
        }
    }
    kdDebug()<< 11<<formatString << value<< endl ;
    return value ;
}

#include "meter.moc"
