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
#include <QSizeF>
#include <QRectF>
#include <QVariant>
#include <QPainter>
#include <kdebug.h>
#include <kicon.h>
#include <plasma/dataengine.h>
#include <plasma/svg.h>
#include <plasma/widgets/widget.h>
#include <plasma/widgets/label.h>
#include <plasma/widgets/lineedit.h>
#include <plasma/widgets/pushbutton.h>
#include <plasma/widgets/icon.h>

#include "skapplet.h"
#include "../src/karamba.h"

namespace Skip {

class Painter : public QObject {
        Q_OBJECT
    public:
        Painter(QObject* parent, QPainter* painter) : QObject(parent), m_painter(painter) {}
        virtual ~Painter() {}
        QPainter* painter() const { return m_painter; }
    private:
        QPainter* m_painter;
};

class WidgetAdaptor : public QObject {
        Q_OBJECT
    public:
        WidgetAdaptor(QObject* parent, QObject* implementation, bool owner) : QObject(parent), m_impl(implementation), m_owner(owner) {}
        virtual ~WidgetAdaptor() { if(m_owner) delete m_impl; }
        QObject* impl() const { return m_impl; }
    public Q_SLOTS:
        virtual void updated(const QString& source, Plasma::DataEngine::Data data) {
            kDebug() << "WidgetAdaptor name=" << m_impl->objectName() << " class=" << m_impl->metaObject()->className() << endl;
            QMetaObject::invokeMethod(m_impl, "updated", Q_ARG(QString, source), Q_ARG(Plasma::DataEngine::Data, data));
        }
    private:
        QPointer<QObject> m_impl;
        bool m_owner;
};

/*
class LabelAdaptor : public WidgetAdaptor {
        Q_OBJECT
    public:
        LabelAdaptor(QObject* parent, Plasma::Label* label, bool owner) : WidgetAdaptor(parent, label, owner), m_label(label) {}
        virtual ~LabelAdaptor() {}
    public Q_SLOTS:
        QString text() const { return m_label->text(); }
        void setText(const QString& text) { m_label->setText(text); }
    private:
        Plasma::Label* m_label;
};
*/

class LineEditAdaptor : public WidgetAdaptor {
        Q_OBJECT
    public:
        LineEditAdaptor(QObject* parent, Plasma::LineEdit* edit, bool owner) : WidgetAdaptor(parent, edit, owner), m_edit(edit) {
            connect(edit, SIGNAL(editingFinished()), this, SIGNAL(editingFinished()));
            connect(edit, SIGNAL(textChanged(QString)), this, SIGNAL(textChanged(QString)));
        }
        virtual ~LineEditAdaptor() {}
    public Q_SLOTS:
        void setDefaultText(const QString &text) { m_edit->setDefaultText(text); }
        const QString toHtml() { return m_edit->toHtml(); }
        const QString toPlainText() { return m_edit->toPlainText(); }
    Q_SIGNALS:
        void editingFinished();
        void textChanged(const QString& text);
    private:
        Plasma::LineEdit* m_edit;
};

class ButtonAdaptor : public WidgetAdaptor {
        Q_OBJECT
    public:
        ButtonAdaptor(QObject* parent, Plasma::PushButton* button, bool owner) : WidgetAdaptor(parent, button, owner), m_button(button) {
            connect(button, SIGNAL(clicked()), this, SIGNAL(clicked()));
        }
        virtual ~ButtonAdaptor() {}
    public Q_SLOTS:
        void setText(const QString &text) { m_button->setText(text); }
        void setIcon(const QString& iconname)  { m_button->setIcon(iconname); }
    Q_SIGNALS:
        void clicked();
    private:
        Plasma::PushButton* m_button;
};

class IconAdaptor : public WidgetAdaptor {
        Q_OBJECT
    public:
        IconAdaptor(QObject* parent, Plasma::Icon* icon, bool owner) : WidgetAdaptor(parent, icon, owner), m_icon(icon) {
            connect(icon, SIGNAL(pressed(bool)), this, SIGNAL(pressed(bool)));
            connect(icon, SIGNAL(clicked()), this, SIGNAL(clicked()));
        }
        virtual ~IconAdaptor() {}
    public Q_SLOTS:
        void setIcon(const QString& iconname) { m_icon->setIcon( KIcon(iconname) ); }
        void setIconSize(double width, double height) { m_icon->setIconSize( QSizeF(width,height) ); }
    Q_SIGNALS:
        void pressed(bool down);
        void clicked();
    private:
        Plasma::Icon* m_icon;
};

class SvgAdaptor : public WidgetAdaptor {
        Q_OBJECT
    public:
        SvgAdaptor(QObject* parent) : WidgetAdaptor(parent, this, false), m_svg(0) {}
        virtual ~SvgAdaptor() { delete m_svg; }
    public Q_SLOTS:
        void setImageFile(const QString& imagefile) {
            delete m_svg;
            m_svg = new Plasma::Svg(imagefile);
        }
        void setSize(double width, double height) {
            if( ! m_svg ) return;
            m_svg->resize( QSizeF(width,height) );
        }
        void setContentType(const QString& contentType) {
            if( ! m_svg ) return;
            const QString s = contentType.toLower();
            Plasma::Svg::ContentType ct;
            if( s == "singleimage" )
                ct = Plasma::Svg::SingleImage;
            else if( s == "imageset" )
                ct = Plasma::Svg::ImageSet;
            else
                return;
            m_svg->setContentType(ct);
        }
        void paint(Painter* painter, const QPointF& point, const QString& elementID = QString()) {
            if( ! m_svg || ! painter ) return;
            m_svg->paint(painter->painter(), point, elementID);
        }
        void updated(const QString& source, Plasma::DataEngine::Data) {
            kDebug() << "SvgAdaptor::updated source=" << source << endl;
        }
    private:
        Plasma::Svg* m_svg;
};

class WidgetFactory {
    public:
        static WidgetAdaptor* createWidget(const QString& _widgetName, QObject* parent, Plasma::Widget* parentWidget) {
            const QString widgetName = _widgetName.toLower();
            //if( widgetName == "Label" )
            //    return new LabelAdaptor(parent, new Plasma::Label(parentWidget), true);
            if( widgetName == "lineedit" )
                return new LineEditAdaptor(parent, new Plasma::LineEdit(parentWidget), true);
            if( widgetName == "pushbutton" || widgetName == "button" )
                return new ButtonAdaptor(parent, new Plasma::PushButton(parentWidget), true);
            if( widgetName == "icon" )
                return new IconAdaptor(parent, new Plasma::Icon(parentWidget), true);
            if( widgetName == "svg" )
                return new SvgAdaptor(parent);
            return 0;
        }
};

} // end of namespace SuperKarambaPlasmaApplet

#endif
