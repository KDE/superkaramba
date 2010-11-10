/*
 * Copyright (C) 2007 Sebastian Sauer <mail@dipe.org>
 *
 * This file is part of SuperKaramba.
 *
 * SuperKaramba is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * SuperKaramba is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SuperKaramba; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "plasmaengine.h"
#include <kdebug.h>

#include <plasma/dataenginemanager.h>

#if 0
#include <QFile>
#include <QTextStream>
#endif

/// \internal helper function that translates plasma data into a QVariantMap.
QVariantMap dataToMap(Plasma::DataEngine::Data data)
{
    QVariantMap map;
    Plasma::DataEngine::DataIterator it(data);
    while( it.hasNext() ) {
        it.next();
        map.insert(it.key(), it.value());
    }
    return map;
}

/*
/// \internal helper function that translates a QVariantMap into plasma data.
Plasma::DataEngine::Data mapToData(QVariantMap map)
{
    Plasma::DataEngine::Data data;
    for(QVariantMap::Iterator it = map.begin(); it != map.end(); ++it)
        data.insert(it.key(), it.value());
    return data;
}
*/

/*****************************************************************************************
* PlasmaSensorConnector
*/

/// \internal d-pointer class.
class PlasmaSensorConnector::Private
{
    public:
        Meter* meter;
        QString source;
        QString format;
};

PlasmaSensorConnector::PlasmaSensorConnector(Meter *meter, const QString& source) : QObject(meter), d(new Private)
{
    //kDebug()<<"PlasmaSensorConnector Ctor"<<endl;
    setObjectName(source);
    d->meter = meter;
    d->source = source;
}

PlasmaSensorConnector::~PlasmaSensorConnector()
{
    //kDebug()<<"PlasmaSensorConnector Dtor"<<endl;
    delete d;
}

Meter* PlasmaSensorConnector::meter() const
{
    return d->meter;
}

QString PlasmaSensorConnector::source() const
{
    return d->source;
}

void PlasmaSensorConnector::setSource(const QString& source)
{
    d->source = source;
}

QString PlasmaSensorConnector::format() const
{
    return d->format;
}

void PlasmaSensorConnector::setFormat(const QString& format)
{
    d->format = format;
}

void PlasmaSensorConnector::dataUpdated(const QString& source, const Plasma::DataEngine::Data &data)
{
    //kDebug()<<"PlasmaSensorConnector::dataUpdated d->source="<<d->source<<" source="<<source<<endl;
    if( d->source.isEmpty() ) {
        emit sourceUpdated(source, dataToMap(data));
        return;
    }
    if( source != d->source ) {
        return;
    }
    QString v = d->format;
    Plasma::DataEngine::DataIterator it(data);
    while( it.hasNext() ) {
        it.next();
        QString s = QString("%%1").arg( it.key() );
        v.replace(s,it.value().toString());
    }
    d->meter->setValue(v);
}

/*****************************************************************************************
* PlasmaSensor
*/

/// \internal d-pointer class.
class PlasmaSensor::Private
{
    public:
        Plasma::DataEngine* engine;
        QString engineName;
        explicit Private() : engine(0) {}
};

PlasmaSensor::PlasmaSensor(int msec) : Sensor(msec), d(new Private)
{
    kDebug()<<"PlasmaSensor Ctor"<<endl;
}

PlasmaSensor::~PlasmaSensor()
{
    kDebug()<<"PlasmaSensor Dtor"<<endl;
    delete d;
}

Plasma::DataEngine* PlasmaSensor::engineImpl() const
{
    return d->engine;
}

void PlasmaSensor::setEngineImpl(Plasma::DataEngine* engine, const QString& engineName)
{
    d->engine = engine;
    d->engineName = engineName;
}

QString PlasmaSensor::engine()
{
    return d->engine ? d->engineName : QString();
}

void PlasmaSensor::setEngine(const QString& name)
{
    //kDebug()<<"PlasmaSensor::setEngine name="<<name<<endl;
    if( d->engine ) {
        disconnect(d->engine, SIGNAL(newSource(QString)), this, SIGNAL(sourceAdded(QString)));
        disconnect(d->engine, SIGNAL(sourceRemoved(QString)), this, SIGNAL(sourceRemoved(QString)));
        Plasma::DataEngineManager::self()->unloadEngine(d->engineName);
    }
    d->engineName.clear();
    d->engine = Plasma::DataEngineManager::self()->engine(name);
    if( ! d->engine || ! d->engine->isValid() ) {
        d->engine = Plasma::DataEngineManager::self()->loadEngine(name);
        if( ! d->engine || ! d->engine->isValid() ) {
            kWarning()<<"PlasmaSensor::setEngine: No such engine: "<<name<<endl;
            return;
        }
    }
    d->engineName = name;
    connect(d->engine, SIGNAL(newSource(QString)), this, SIGNAL(sourceAdded(QString)));
    connect(d->engine, SIGNAL(sourceRemoved(QString)), this, SIGNAL(sourceRemoved(QString)));
    //d->engine->setProperty("reportSeconds", true);
}

bool PlasmaSensor::isValid() const
{
    return d->engine && d->engine->isValid();
}

QStringList PlasmaSensor::sources() const
{
    return d->engine ? d->engine->sources() : QStringList();
}

QVariant PlasmaSensor::property(const QByteArray& name) const
{
    return d->engine ? d->engine->property(name) : QVariant();
}

void PlasmaSensor::setProperty(const QByteArray& name, const QVariant& value)
{
    if( d->engine )
        d->engine->setProperty(name, value);
}

QVariantMap PlasmaSensor::query(const QString& source)
{
    //kDebug()<<"PlasmaSensor::query"<<endl;
    return d->engine ? dataToMap(d->engine->query(source)) : QVariantMap();
}

QObject* PlasmaSensor::connectSource(const QString& source, QObject* visualization)
{
    //kDebug()<<"PlasmaSensor::connectSource source="<<source<<endl;
    if( ! d->engine ) {
        kWarning()<<"PlasmaSensor::connectSource: No engine"<<endl;
        return 0;
    }
    if( Meter* m = dynamic_cast<Meter*>(visualization) ) {
        PlasmaSensorConnector* c = new PlasmaSensorConnector(m, source);
        d->engine->connectSource(source, c);
        kDebug()<<"PlasmaSensor::connectSource meter, engine isValid="<<d->engine->isValid();
        return c;
    }
    d->engine->connectSource(source, visualization ? visualization : this);
    return 0;
}

void PlasmaSensor::disconnectSource(const QString& source, QObject* visualization)
{
    //kDebug()<<"PlasmaSensor::disconnectSource"<<endl;
    if( Meter* m = dynamic_cast<Meter*>(visualization) ) {
        foreach(PlasmaSensorConnector* c, m->findChildren<PlasmaSensorConnector*>(source))
            if( c->meter() == m )
                delete c;
    }
    else if( d->engine ) {
        d->engine->disconnectSource(source, visualization ? visualization : this);
    }
    else
        kWarning()<<"PlasmaSensor::disconnectSource: No engine"<<endl;
}

void PlasmaSensor::update()
{
    kDebug()<<"PlasmaSensor::update"<<endl;
    /*TODO
    foreach(QObject *it, *objList) {
        SensorParams *sp = qobject_cast<SensorParams*>(it);
        Meter *meter = sp->getMeter();
        const QString format = sp->getParam("FORMAT");
        //if (format.length() == 0) format = "%um";
        //format.replace(QRegExp("%fmb", Qt::CaseInsensitive),QString::number((int)((totalMem - usedMemNoBuffers) / 1024.0 + 0.5)));
        //meter->setValue(format);
    }
    */
}

void PlasmaSensor::dataUpdated(const QString& source, Plasma::DataEngine::Data data)
{
    //kDebug()<<"PlasmaSensor::dataUpdated source="<<source<<endl;
    emit sourceUpdated(source, dataToMap(data));
}

#include "plasmaengine.moc"
