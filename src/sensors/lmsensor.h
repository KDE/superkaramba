
#ifndef LMSENSOR_H
#define LMSENSOR_H

#include <K3Process>
#include <K3ProcIO>

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
    K3ShellProcess ksp;
    QString extraParams;

    QMap<QString, QString> sensorMap;
#ifdef __FreeBSD__
    QMap<QString, QString> sensorMapBSD;
#endif
    QString sensorResult;

private slots:
    void receivedStdout(K3Process *, char *buffer, int);
    void processExited(K3Process *);

};

#endif // LMSENSOR_H
