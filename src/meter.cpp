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
#include "karamba_python.h"

Meter::Meter(KarambaWidget* k, int ix, int iy, int iw, int ih): 
        QWidget(k),
        boundingBox(ix, iy, iw, ih), leftButtonAction(""), middleButtonAction(""),
        rightButtonAction(""), clickable(true), hidden(0), m_sensor(0),
        m_karamba(k),autoUpdate(true)
{
    setGeometry(ix,iy,iw,ih);
    KRootPixmap* root= new KRootPixmap(this);
    root->start();
    pythonIface= &(m_karamba->pythonIface);
    
}

Meter::Meter(KarambaWidget* k):QWidget(k),
        boundingBox(0, 0, 0, 0), leftButtonAction(""), middleButtonAction(""),
        rightButtonAction(""), clickable(true), hidden(0), m_sensor(0),
        m_karamba(k)
{
    KRootPixmap* root= new KRootPixmap(this);
    root->start();
    pythonIface = &(m_karamba->pythonIface);
    
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
    kDebug() <<value.toMap().keys() << endl;
    foreach(QString section,list)
    {
        if(value.toMap().find(section)==value.toMap().end()) return QVariant();
        else
        {
            value=value.toMap().value(section);
        }
    }
    return value ;
}

void Meter::mousePressEvent( QMouseEvent * e )
{
    int button = 0;

    if( e->button() == Qt::LeftButton )
        button = 1;
    else if( e->button() == Qt::MidButton )
        button = 2;
    else if( e->button() == Qt::RightButton )
        button = 3;
    if ((*pythonIface) && (*pythonIface)->isExtensionLoaded())
    {
        (*pythonIface)->widgetClicked(m_karamba,e->x(),e->y(),button);
        (*pythonIface)->meterClicked(m_karamba, this, button);
    }
    kDebug() << "mousePressEvent"<< button<< endl;
}

void Meter::mouseReleaseEvent( QMouseEvent * me )
{
    
}

void Meter::mouseMoveEvent( QMouseEvent * e )
{
    if ((*pythonIface) && (*pythonIface)->isExtensionLoaded())
    {
        int button = 0;

        if( e->state() == Qt::LeftButton)
            button = 1;
        else if( e->state() == Qt::MidButton )
            button = 2;
        else if( e->state() == Qt::RightButton )
            button = 3;
        (*pythonIface)->widgetMouseMoved(m_karamba, e->x(), e->y(), button);
    }
    
}

void Meter::mouseDoubleClickEvent( QMouseEvent * e )
{
    int button = 0;

    if( e->button() == Qt::LeftButton )
        button = 1;
    else if( e->button() == Qt::MidButton )
        button = 2;
    else if( e->button() == Qt::RightButton )
        button = 3;
    if ((*pythonIface) && (*pythonIface)->isExtensionLoaded() )
    {
        (*pythonIface)->widgetClicked(m_karamba,e->x(),e->y(),button);
        (*pythonIface)->meterClicked(m_karamba, this, button);
    }
}

void Meter::keyPressEvent( QKeyEvent * ke )
{
    if ((*pythonIface) && (*pythonIface)->isExtensionLoaded())
        (*pythonIface)->keyPressed(m_karamba,this, ke->text());
}

void Meter::keyReleaseEvent( QKeyEvent * ke )
{
}

void Meter::wheelEvent( QWheelEvent * e )
{
    if ((*pythonIface) && (*pythonIface)->isExtensionLoaded())
    {
        int button = 0;

        if( e->delta() > 0 )
            button = 4;
        else
            button = 5;

        (*pythonIface)->widgetClicked(m_karamba, e->x(), e->y(), button);
    }
}



#include "meter.moc"
