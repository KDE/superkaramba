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

#ifndef SKAPPLETADAPTOR_H
#define SKAPPLETADAPTOR_H

#include <QObject>
#include <QHash>
#include <QVariant>
#include <plasma/applet.h>
#include <plasma/dataengine.h>

#include "skapplet.h"
#include "skengineadaptor.h"

/**
* The SuperKarambaAppletAdaptor class implements an adaptor for
* \a SuperKarambaApplet Plasma::Applet objects.
*/
class SuperKarambaAppletAdaptor : public QObject
{
        Q_OBJECT
    public:
        SuperKarambaAppletAdaptor(QObject* parent, SuperKarambaApplet *applet) : QObject(parent), m_applet(applet) {
            setObjectName("PlasmaApplet");
        }
        virtual ~SuperKarambaAppletAdaptor() { qDeleteAll(m_engines.values()); }

    public Q_SLOTS:

        /**
        * Return the name of the applet as defined in the plasma-skapplet-default.desktop file.
        */
        QString name() const { return m_applet->name(); }

        /**
        * Return the category of the applet as defined in the plasma-skapplet-default.desktop file.
        */
        QString category() const { return m_applet->category(); }

        /**
        * Return the bouding rectangle of the applet.
        */
        QRectF boundingRect() { return m_applet->boundingRect(); }

        /**
        * Loads and returns the given DataEngine.
        *
        * Tries to load the data engine given by @p name.  Each engine is
        * only loaded once, and that instance is re-used on all subsequent
        * requests.
        *
        * @param name Name of the data engine to load
        * @return a \a SuperKarambaEngineAdaptor object if it was loaded
        *         else NULL is returned.
        */
        QObject* dataEngine(const QString& name) {
            if( m_engines.contains(name) )
                return m_engines[name];
            Plasma::DataEngine* engine = m_applet->dataEngine(name);
            if( (! engine) || (! engine->isValid()) )
                return 0;
            SuperKarambaEngineAdaptor* engineadaptor = new SuperKarambaEngineAdaptor(engine);
            m_engines.insert(name, engineadaptor);
            return engineadaptor;
        }

    Q_SIGNALS:

        /**
        * This signal is emitted if the configuration interface should be shown.
        */
        void showConfigurationInterface();

    private:
        SuperKarambaApplet *m_applet;
        QHash<QString, SuperKarambaEngineAdaptor*> m_engines;
};

#endif
