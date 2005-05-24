/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "sensor.h"
Sensor::Sensor(int iMsec)
{
    objList = new QObjectList();
    objList->setAutoDelete( true );
    msec = iMsec;
}

void Sensor::start()
{
    if (!timer.isActive())
    {
        connect (&timer,SIGNAL(timeout()),this,SLOT(update()));
        timer.start( (msec == 0)?1000:msec);
    }
}

Sensor::~Sensor()
{
objList->clear();
delete objList;
}

void Sensor::addMeter( SensorParams *sp )
{
  objList->append(sp);
}

SensorParams* Sensor::hasMeter( Meter *meter )
{
  QObjectListIt it( *objList );
  while ( it != 0 )
    {
      if (((SensorParams*) *it)->getMeter() == meter)
        {
          return (SensorParams*) *it;
        }
      ++it;
    }
  return NULL;
}

void Sensor::deleteMeter( Meter *meter )
{
  SensorParams* sp = hasMeter(meter);

  if (sp)
    objList->removeRef(sp);
}

void Sensor::setMaxValue( SensorParams* )
{
}

#include "sensor.moc"
