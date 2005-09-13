/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef DATESENSOR_H
#define DATESENSOR_H
#include "sensor.h"
#include "sensorparams.h"
#include <qdatetime.h>
#include <kdatepicker.h>
#include <q3vbox.h>

class DatePicker : public Q3VBox
{
public:
	DatePicker(QWidget*);
private:
	KDatePicker *picker;
	void keyReleaseEvent(QKeyEvent *e);
};

class DateSensor :  public Sensor
{
Q_OBJECT
public:
  DateSensor( int interval );
  ~DateSensor();

  void update();
  void addMeter(Meter*);

signals:
  void dateValue(QVariant);

protected slots:
  void slotCalendarDeleted();

private:
  bool hidden;
  DatePicker* cal;

};

#endif // SENSOR_H