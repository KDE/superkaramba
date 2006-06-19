/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include <QtAlgorithms>
#include "sensor.h"

Sensor::Sensor(int p_msec)
{
    m_msec = p_msec; //initialization
}
// void Sensor::start()
// {
//     if (!m_timer.isActive())
//     {
//         connect (&m_timer,SIGNAL(timeout()),this,SLOT(update()));
//         m_timer.start( (m_msec == 0)?1000:m_msec);
//     }
// }

Sensor::~Sensor()
{}

//SensorParams* Sensor::hasMeter( Meter *meter )
//{
//  foreach (QObject *it, objList)
//  {
//      if (((SensorParams*) it)->getMeter() == meter)
//        {
//          return ((SensorParams*) it);
//        }
//  }
//  return NULL;
//}

void Sensor::addMeter(Meter* meter)
{
    disconnect(this, SIGNAL(valueChanged(QVariant) ), meter, SLOT(storeData(QVariant)));
     connect(this, SIGNAL(valueChanged(QVariant)), meter, SLOT(storeData(QVariant)));
}

void Sensor::deleteMeter( Meter *meter )
{
    disconnect(this, SIGNAL(valueChanged(QVariant )), meter, SLOT(storeData(QVariant)));
}

/*void Sensor::setMaxValue( SensorParams* )
{
}
*/
#include "sensor.moc"
