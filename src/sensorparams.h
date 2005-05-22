
#ifndef SENSORPARAMS_H
#define SENSORPARAMS_H

#include "meter.h"
#include <qstring.h>
#include <qmap.h>
/**
 *
 * Hans Karlsson
 **/
class SensorParams : public QObject
{
public:
    SensorParams( Meter* );

    ~SensorParams();

    void addParam( QString name, QString value);
    QString getParam( QString ) const;

    Meter* getMeter() const;
private:
Meter *meter;
QMap<QString,QString> params;

};

#endif
