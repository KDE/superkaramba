
#ifndef SENSORSENSOR_H
#define SENSORSENSOR_H

#include <qstring.h>
#include <qtextcodec.h>
#include <qmap.h>
#include <qstringlist.h>
#include <qregexp.h>
#include <kprocess.h>
#include <kprocio.h>
#include <kstaticdeleter.h>


#include "sensor.h"

/**
 *
 * Hans Karlsson
 **/
class SensorSensor : public Sensor
{
    Q_OBJECT
public:
    static SensorSensor* self();
    void addMeter(Meter*);
    static bool isSingleton() { return true; }

signals:
    void sensorValues(QVariant);

private:
    friend class KStaticDeleter<SensorSensor>;
    SensorSensor(int interval=2000);
    virtual ~SensorSensor();

    KShellProcess ksp;
    QString extraParams;
    static SensorSensor* m_self;

    QMap<QString,QVariant> sensorMap;
#ifdef __FreeBSD__

    QMap<QString,QString> sensorMapBSD;
#endif

    QString sensorResult;

private slots:
    void receivedStdout(KProcess *, char *buffer, int);
    void processExited(KProcess *);
    void update();


};

#endif
