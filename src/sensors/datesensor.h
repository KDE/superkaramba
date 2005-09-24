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
#include <kstaticdeleter.h>

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
public slots:
    void update();
public:
    void addMeter(Meter*);
    static DateSensor* self();
    static bool isSingleton() { return true; }


signals:
    void dateValue(QVariant);

protected slots:
    void slotCalendarDeleted();

private:
    friend class KStaticDeleter<DateSensor>;
    DateSensor( int interval=2000 );
    virtual ~DateSensor();
    bool hidden;
    DatePicker* cal;
    QVariantMap data;
    static DateSensor* m_self;

};

#endif // SENSOR_H
