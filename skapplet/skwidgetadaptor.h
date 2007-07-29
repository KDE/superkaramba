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

#ifndef SKWIDGETADAPTOR_H
#define SKWIDGETADAPTOR_H

#include <QObject>
#include <QMetaObject>
#include <QVariant>
#include <plasma/dataengine.h>
#include <plasma/widgets/widget.h>
#include <plasma/widgets/label.h>
#include <plasma/widgets/lineedit.h>

#include "skapplet.h"
#include "../src/karamba.h"

/**
* The SuperKarambaWidgetAdaptor class implements an adaptor for
* Plasma::DataEngine objects.
*/
class SuperKarambaWidgetAdaptor : public QObject
{
        Q_OBJECT
    public:

        SuperKarambaWidgetAdaptor(QObject* parent, SuperKarambaApplet* applet, SuperKarambaWidgetAdaptor* parentWidgetAdaptor = 0)
            : QObject(parent), m_applet(applet), m_parentWidgetAdaptor(parentWidgetAdaptor), m_type(NoType)
        {
            Q_ASSERT(m_applet);
            m_parentWidget = m_parentWidgetAdaptor ? m_parentWidgetAdaptor->widget() : 0;
            if( ! m_parentWidget )
                m_parentWidget = new Plasma::Widget( m_applet->karamba() );
        }

        virtual ~SuperKarambaWidgetAdaptor() {}

        Plasma::Widget* widget() const {
            switch( m_type ) {
                case NoType:
                    break;
                case Label:
                    return m_label;
                case LineEdit:
                    //return m_lineedit;
                    break;
            }
            return 0;
        }

    public Q_SLOTS:

        void setType(const QString& typeName) {
            if( typeName == "Label" ) {
                m_type = Label;
                m_label = new Plasma::Label(m_parentWidget);
            }
            else if( typeName == "LineEdit" ) {
                m_type = LineEdit;
                m_lineedit = new Plasma::LineEdit(m_parentWidget);
            }
            else
                m_type = NoType;
        }

        void updated(const QString& source, Plasma::DataEngine::Data data) {
            switch( m_type ) {
                case NoType:
                    break;
                case Label:
                    //m_label->setText();
                    break;
                case LineEdit:
                    QMetaObject::invokeMethod(m_lineedit, "updated", Q_ARG(QString, source), Q_ARG(Plasma::DataEngine::Data, data));
                    break;
            }
        }

    private:
        SuperKarambaApplet* m_applet;
        Plasma::Widget* m_parentWidget;
        SuperKarambaWidgetAdaptor* m_parentWidgetAdaptor;

        enum Type { NoType, Label, LineEdit };
        Type m_type;
        union {
            Plasma::Label* m_label;
            Plasma::LineEdit* m_lineedit;
        };
};

#endif
