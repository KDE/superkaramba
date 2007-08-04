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
#include <QBrush>
#include <QColor>
#include <QPen>
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

    public Q_SLOTS:
        void save() { m_painter->save(); }
        void restore() { m_painter->restore(); }

        void setColor(const QString& color) { m_painter->setBrush( getColor(m_painter->brush(),color) ); }
        void setBackgroundColor(const QString& color) { m_painter->setBackground( getColor(m_painter->background(),color) ); }
        void setPenColor(const QString& color) { m_painter->setPen(QColor(color)); }
        //void setPenWidth(double width) { QPen pen = m_painter->pen(); pen.setWidthF(width); pen.setStyle(Qt::SolidLine); m_painter->setPen(pen); }

        void rotate(double angle) { m_painter->rotate(angle); }
        void scale(double x, double y) { m_painter->scale(x,y); }
        void shear(double x, double y) { m_painter->shear(x,y); }
        void translate(double x, double y) { m_painter->translate(x,y); }

        void drawEllipse(const QRectF& r) { m_painter->drawEllipse(r); }
        void drawLine(const QPointF& p1, const QPointF& p2) { m_painter->drawLine(p1, p2); }
        void drawPie(const QRectF& r, int startAngle, int spanAngle) { m_painter->drawPie(r, startAngle, spanAngle); }
        void drawPoint(const QPointF& p) { m_painter->drawPoint(p); }
        void drawRect(const QRectF& r) { m_painter->drawRect(r); }
        void drawText(const QPointF& position, const QString& text) { m_painter->drawText(position, text); }
        //void drawText(const QRectF& r, const QString& text) { m_painter->drawText(r, text); }

    private:
        QPainter* m_painter;

        QBrush getColor(const QBrush& _brush, const QString& color) {
            QBrush brush = _brush;
            QColor c(color);
            if( c.isValid() ) {
                //brush.setColor(c);
                //if( brush.style() == Qt::NoBrush ) brush.setStyle(Qt::SolidPattern);
                brush = QBrush(c);
            }
            return brush;
        }
};

#if 0
class WidgetAdaptor : public QObject {
        Q_OBJECT
    public:
        WidgetAdaptor(QObject* imp, bool owner) : QObject(imp), m_impl(imp), m_owner(owner) {}
        virtual ~WidgetAdaptor() { if(m_owner) delete m_impl; }
        QObject* impl() const { return m_impl; }
    public Q_SLOTS:
        virtual void updated(const QString& source, Plasma::DataEngine::Data data) {
            kDebug() << "WidgetAdaptor name=" << m_impl->objectName() << " class=" << m_impl->metaObject()->className() ;
            QMetaObject::invokeMethod(m_impl, "updated", Q_ARG(QString, source), Q_ARG(Plasma::DataEngine::Data, data));
        }
    private:
        QPointer<QObject> m_impl;
        bool m_owner;
};
#endif

class SvgAdaptor : public Plasma::Widget {
        Q_OBJECT
    public:
        SvgAdaptor(QGraphicsItem* parent) : Plasma::Widget(parent), m_svg(0) {}
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
            if( ! m_svg || ! painter ) {
                kDebug() << "SvgAdaptor::updated No SVG or invalid painter.";
                return;
            }
            m_svg->paint(painter->painter(), point, elementID);
        }
        void updated(const QString& source, Plasma::DataEngine::Data) {
            kDebug() << "SvgAdaptor::updated source=" << source ;
        }
    private:
        Plasma::Svg* m_svg;
};

class WidgetFactory {
    public:
        static QObject* createWidget(const QString& _widgetName, Plasma::Widget* parent) {
            QObject* widget = 0;
            const QString widgetName = _widgetName.toLower();
            if( widgetName == "label" )
                widget = new Plasma::Label(parent);
            if( widgetName == "lineedit" )
                widget = new Plasma::LineEdit(parent);
            if( widgetName == "pushbutton" || widgetName == "button" )
                widget = new Plasma::PushButton(parent);
            if( widgetName == "icon" )
                widget = new Plasma::Icon(parent);
            if( widgetName == "svg" )
                widget = new SvgAdaptor(parent);
            /*
            if( parent )
                if( Plasma::Widget* w = dynamic_cast<Plasma::Widget*>(widget) )
                    parent->addChild(w);
            */
            return widget;
        }
};

} // end of namespace SuperKarambaPlasmaApplet

#endif
