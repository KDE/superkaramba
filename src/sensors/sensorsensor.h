
#ifndef SENSORSENSOR_H
#define SENSORSENSOR_H

#include <qstring.h>
#include <qtextcodec.h>
#include <qmap.h>
#include <qstringlist.h>
#include <qregexp.h>
#include <kprocess.h>
#include <kprocio.h>


#include "sensor.h"

/**
 *
 * Hans Karlsson
 **/
class SensorSensor : public Sensor
{
    Q_OBJECT
public:
    SensorSensor(int interval, char tempUnit);

    ~SensorSensor();

    void update();


private:
    KShellProcess ksp;
    QString extraParams;

    QMap<QString,QString> sensorMap;
#ifdef __FreeBSD__
    QMap<QString,QString> sensorMapBSD;
#endif
    QString sensorResult;

private slots:
    void receivedStdout(KProcess *, char *buffer, int);
    void processExited(KProcess *);



};

#endif
