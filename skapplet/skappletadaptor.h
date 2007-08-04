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

#include <plasma/widgets/widget.h>
//#include <plasma/widgets/label.h>
//#include <plasma/widgets/lineedit.h>

#include "skapplet.h"
#include "sensors/plasmaengine.h"
#include "skwidgetadaptor.h"

namespace Skip {

/**
* The AppletAdaptor class implements an adaptor for
* \a SuperKarambaApplet Plasma::Applet objects.
*/
class AppletAdaptor : public QObject
{
        Q_OBJECT
    public:
        AppletAdaptor(QObject* parent, SuperKarambaApplet *applet) : QObject(parent), m_applet(applet), m_widget(new Plasma::Widget(applet->karamba())) {
            setObjectName("PlasmaApplet");
        }
        virtual ~AppletAdaptor() {
            delete m_widget;
            qDeleteAll(m_engines.values());
        }

        void paintInterface(QPainter *painter, const QStyleOptionGraphicsItem *option, const QRect &rect) {
            Q_UNUSED(option);
            Painter p(this, painter);
            emit paint(&p, rect);
        }

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
        * @return a \a EngineAdaptor object if it was loaded
        *         else NULL is returned.
        */
        QObject* dataEngine(const QString& name) {
            if( m_engines.contains(name) )
                return m_engines[name];
            Plasma::DataEngine* engine = m_applet->dataEngine(name);
            if( (! engine) || (! engine->isValid()) )
                return 0;
            PlasmaSensor* sensor = new PlasmaSensor;
            sensor->setEngineImpl(engine, name);
            m_engines.insert(name, sensor);
            return sensor;
        }

        /**
        * Create and return a new Plasma::Widget instance.
        */
        QObject* widget(const QString& widgetName) {
            //Plasma::Widget* parent = dynamic_cast<SuperKarambaWidgetAdaptor*>(parentWidget);
            return WidgetFactory::createWidget(widgetName, this, m_widget);
            /*
            SuperKarambaWidgetAdaptor* parentWidgetAdaptor = dynamic_cast<SuperKarambaWidgetAdaptor*>(parentWidget);
            SuperKarambaWidgetAdaptor* widgetAdaptor = new SuperKarambaWidgetAdaptor(this, m_applet, parentWidgetAdaptor);
            if( ! typeName.isEmpty() ) widgetAdaptor->setType(typeName);
            return widgetAdaptor;
            */
        }

    Q_SIGNALS:

        /**
        * This signal is emitted if the configuration interface should be shown.
        */
        void showConfigurationInterface();

        /**
        * This signal is emitted if the configuration interface should be shown.
        */
        void paint(QObject* painter, const QRect &rect);

    private:
        SuperKarambaApplet *m_applet;
        Plasma::Widget* m_widget;
        QHash<QString, PlasmaSensor*> m_engines;
};

} // end of namespace SuperKarambaPlasmaApplet

#endif
