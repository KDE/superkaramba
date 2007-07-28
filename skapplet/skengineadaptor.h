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

#ifndef SKENGINEADAPTOR_H
#define SKENGINEADAPTOR_H

#include <QObject>
#include <QVariant>
#include <plasma/dataengine.h>

/**
* The SuperKarambaEngineAdaptor class implements an adaptor for
* Plasma::DataEngine objects.
*/
class SuperKarambaEngineAdaptor : public QObject
{
        Q_OBJECT
    public:
        explicit SuperKarambaEngineAdaptor(Plasma::DataEngine* engine) : QObject(), m_engine(engine) {
            connect(m_engine, SIGNAL(newSource(QString)), this, SIGNAL(sourceAdded(QString)));
            connect(m_engine, SIGNAL(sourceRemoved(QString)), this, SIGNAL(sourceRemoved(QString)));
        }
        virtual ~SuperKarambaEngineAdaptor() {}

    public Q_SLOTS:

        /**
        * Return a list of all the data sources available via this DataEngine.
        */
        QStringList sources() const { return m_engine->sources(); }

        /**
        * Gets the data associated with a data source.
        */
        QVariantMap query(const QString& source) const {
            QVariantMap map;
            Plasma::DataEngine::Data data = m_engine->query(source);
            Plasma::DataEngine::DataIterator it(data);
            while( it.hasNext() ) {
                it.next();
                map.insert(it.key(), it.value());
            }
            return map;
        }

    Q_SIGNALS:

        /**
         * Emitted when a new data source is created
         **/
        void sourceAdded(const QString& source);

        /**
         * Emitted when a data source is removed.
         **/
        void sourceRemoved(const QString& source);

    private:
        Plasma::DataEngine* m_engine;
};

#endif
