/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "sensorparams.h"


SensorParams::SensorParams(Meter* m)
{
  meter = m;
}

SensorParams::~SensorParams()
{
}

void SensorParams::addParam( const QString &name, const QString &value){
    params[name] = value;
}

QString SensorParams::getParam( const QString &name ) const
{
    return params[name];
}

Meter* SensorParams::getMeter() const
{
   return meter;
}
