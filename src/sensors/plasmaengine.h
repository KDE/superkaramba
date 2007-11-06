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

#ifndef PLASMAENGINE_H
#define PLASMAENGINE_H

#include <QObject>

#include "superkaramba_export.h"
#include "sensor.h"
#include "meters/meter.h"

#include <plasma/dataengine.h>

/**
* This is a helper class that connects a Plasma::DataEngine together
* with a SuperKaramba Meter widget.
*/
class PlasmaSensorConnector : public QObject
{
        Q_OBJECT
    public:
        explicit PlasmaSensorConnector(Meter *meter, const QString& source);
        virtual ~PlasmaSensorConnector();
        Meter* meter() const;

    public Q_SLOTS:

        /**
        * Returns the name of the data source.
        */
        QString source() const;

        /**
        * Set the name of the data source.
        */
        void setSource(const QString& source);

        /**
        * Return the format the data should used for displaying.
        */
        QString format() const;

        /**
        * Set the format the data should use for displaying.
        * 
        */
        void setFormat(const QString& format);

    Q_SIGNALS:

        /**
         * Emitted when a data source got updated.
         *
         * This signal will only be emitted if no or an emoty source was defined.
         *
         * \param source The name of the data source.
         * \param data The map of the data that got changed.
         **/
        void sourceUpdated(const QString& source, QVariantMap data);

    private Q_SLOTS:

        /**
        * Plasma calls this if data changed.
        */
        void dataUpdated(const QString& source, const Plasma::DataEngine::Data &data);

    private:
        /// \internal d-pointer class.
        class Private;
        /// \internal d-pointer instance.
        Private* const d;
};

/**
* The PlasmaSensor class implements a SuperKaramba sensor that
* provides access to Plasma::DataEngine objects.
*/
class SUPERKARAMBA_EXPORT PlasmaSensor :  public Sensor
{
        Q_OBJECT
    public:
        PlasmaSensor(int interval = -1);
        virtual ~PlasmaSensor();
        Plasma::DataEngine* engineImpl() const;
        void setEngineImpl(Plasma::DataEngine* engine, const QString& engineName);

    public Q_SLOTS:

        /**
        * Return the name of the engine.
        */
        QString engine();

        /**
        * Set the engine that should be used. This could be e.g. "time" for
        * the Plasma TimeEngine. This method does use the Plasma::DataEngineManager
        * to fetch a Plasma::DataEngine .
        */
        virtual void setEngine(const QString& name);

        /**
        * Returns true if an engine was defined and if the engine is valid
        * else false got returned.
        */
        bool isValid() const;

        /**
        * Return a list of all the data sources available via this DataEngine.
        */
        QStringList sources() const;

        /**
        * Get a property.
        */
        QVariant property(const QByteArray& name) const;

        /**
        * Set a property.
        */
        void setProperty(const QByteArray& name, const QVariant& value);

        /**
        * Gets the data associated with a data source.
        */
        QVariantMap query(const QString& source);

        /**
        * Connect with a source. Each time the source is updated
        * the sourceUpdated() signal will be emitted.
        *
        * \return a PlasmaSensorConnector instance if the visualization object is
        * an instance of a Meter else NULL will be returned.
        */
        virtual QObject* connectSource(const QString& source, QObject* visualization = 0);

        /**
        * Disconnect from a source.
        */
        virtual void disconnectSource(const QString& source, QObject* visualization = 0);

    Q_SIGNALS:

        /**
         * Emitted when a new data source is created
         * \param source The name of the data source.
         **/
        void sourceAdded(const QString& source);

        /**
         * Emitted when a data source is removed.
         * \param source The name of the data source.
         **/
        void sourceRemoved(const QString& source);

        /**
         * Emitted when a data source got updated.
         * \param source The name of the data source.
         * \param data The map of the data that got changed.
         **/
        void sourceUpdated(const QString& source, QVariantMap data);

    protected Q_SLOTS:

        /**
        * Request to update the sensor. This will be called by the timer each sensor
        * may have and which got emitted each once per defined interval.
        */
        virtual void update();

        /**
        * Plasma calls this if data changed.
        */
        virtual void dataUpdated(const QString& source, Plasma::DataEngine::Data data);

    private:
        /// \internal d-pointer class.
        class Private;
        /// \internal d-pointer instance.
        Private* const d;
};

#endif // PLASMAENGINE_H
