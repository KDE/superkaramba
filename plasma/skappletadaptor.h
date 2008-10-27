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

#include <plasma/applet.h>
#include <plasma/containment.h>
#include <plasma/dataengine.h>
//#include <plasma/widgets/widget.h>
//#include <plasma/widgets/label.h>
//#include <plasma/widgets/lineedit.h>

#include "karamba.h"
#include "sensors/plasmaengine.h"

#include <QObject>
#include <QHash>
#include <QVariant>
#include <QPointer>
#include <QPainter>
#include <QBrush>
#include <QColor>

/**
* The SkPainterAdaptor class implements an adaptor for
* QPainter objects.
*/
class SkPainterAdaptor : public QObject
{
        Q_OBJECT
        Q_ENUMS(BrushStyle)
    public:
        SkPainterAdaptor(QObject* parent, QPainter* painter) : QObject(parent), m_painter(painter) { setObjectName("PlasmaPainter"); }
        virtual ~SkPainterAdaptor() {}
        QPainter* painter() const { return m_painter; }
        enum BrushStyle {
            NoBrush = Qt::NoBrush,
            SolidBrush = Qt::SolidPattern,
            DenseBrush = Qt::Dense7Pattern,
            LinearGradientBrush = Qt::LinearGradientPattern,
            ConicalGradientBrush = Qt::ConicalGradientPattern,
            RadialGradientBrush = Qt::RadialGradientPattern
        };
    public Q_SLOTS:

        /// Save the painter.
        void save() { m_painter->save(); }
        /// Restore the painter.
        void restore() { m_painter->restore(); }

        /// Set the used foreground-color.
        void setColor(const QString& color) { m_painter->setBrush( getColor(m_painter->brush(),color) ); }
        /// Set the used foreground-BrushStyle.
        void setStyle(int brushStyle) { m_painter->setBrush( getBrushStyle(m_painter->brush(),brushStyle) ); }

        /// Set the used background-color.
        void setBackgroundColor(const QString& color) { m_painter->setBackground( getColor(m_painter->background(),color) ); }
        /// Set the used background-BrushStyle
        void setBackgroundStyle(int brushStyle) { m_painter->setBackground( getBrushStyle(m_painter->background(),brushStyle) ); }

        /// Set the used pen-color.
        void setPenColor(const QString& color) { m_painter->setPen(QColor(color)); }
        //void setPenWidth(double width) { QPen pen = m_painter->pen(); pen.setWidthF(width); pen.setStyle(Qt::SolidLine); m_painter->setPen(pen); }

        /// Set the used opacity.
        void setOpacity(double opacity) { m_painter->setOpacity(opacity); }

        /// Rotate by an defined angle.
        void rotate(double angle) { m_painter->rotate(angle); }
        /// Scale by x and y.
        void scale(double x, double y) { m_painter->scale(x,y); }
        /// Shear by x and y.
        void shear(double x, double y) { m_painter->shear(x,y); }
        /// Translate to x and y.
        void translate(double x, double y) { m_painter->translate(x,y); }

        /// Draw an ellipse.
        void drawEllipse(const QRectF& r) { m_painter->drawEllipse(r); }
        /// Draw a line.
        void drawLine(const QPointF& p1, const QPointF& p2) { m_painter->drawLine(p1, p2); }
        /// Draw a pie.
        void drawPie(const QRectF& r, int startAngle, int spanAngle) { m_painter->drawPie(r, startAngle, spanAngle); }
        /// Draw a point.
        void drawPoint(const QPointF& p) { m_painter->drawPoint(p); }
        /// Draw a rect.
        void drawRect(const QRectF& r) { m_painter->drawRect(r); }
        /// Draw some text.
        void drawText(const QPointF& position, const QString& text) { m_painter->drawText(position, text); }
        //void drawText(const QRectF& r, const QString& text) { m_painter->drawText(r, text); }

    private:
        QPainter* m_painter;
        QBrush getColor(QBrush brush, const QString& color) {
            QColor c(color);
            if( c.isValid() ) {
                brush.setColor(c);
                if( brush.style() == Qt::NoBrush )
                    brush.setStyle(Qt::SolidPattern);
            }
            return brush;
        }
        QBrush getBrushStyle(QBrush brush, int brushStyle) {
            brush.setStyle( (Qt::BrushStyle) brushStyle );
            return brush;
        }
};

/**
* The SkAppletAdaptor class implements an adaptor for
* Plasma::Applet objects.
*/
class SkAppletAdaptor : public QObject
{
        Q_OBJECT
    public:
        SkAppletAdaptor(Karamba *karamba, Plasma::Applet *applet)
            : QObject(karamba)
            , m_karamba(karamba)
            , m_applet(applet)
            //, m_widget(new Plasma::Widget(applet->karamba()))
            , m_painterenabled(false) {
            setObjectName("PlasmaApplet");
        }
        virtual ~SkAppletAdaptor() {
            //delete m_widget;
            qDeleteAll(m_engines.values());
        }
        void paintInterface(QPainter *painter, const QStyleOptionGraphicsItem *option, const QRect &rect) {
            Q_UNUSED(option);
            if( m_painterenabled ) {
                SkPainterAdaptor p(this, painter);
                emit paint(&p, rect);
            }
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

        QRectF geometry() const { return m_applet->geometry(); }
        void setGeometry(const QRectF& geom) { m_applet->setGeometry(geom); }
        void resize(const QSizeF &size) { m_applet->resize(size);}//setMinimumSize(size);}//resize(size); }

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
        QObject* dataEngine(const QString& name)
        {
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

#if 0
        /**
        * Create and return a new Plasma::Widget instance.
        */
        QObject* widget(const QString& widgetName)
        {
            //Plasma::Widget* parent = dynamic_cast<SuperKarambaWidgetAdaptor*>(parentWidget);
            return WidgetFactory::createWidget(widgetName, m_widget);
            /*
            SuperKarambaWidgetAdaptor* parentWidgetAdaptor = dynamic_cast<SuperKarambaWidgetAdaptor*>(parentWidget);
            SuperKarambaWidgetAdaptor* widgetAdaptor = new SuperKarambaWidgetAdaptor(this, m_applet, parentWidgetAdaptor);
            if( ! typeName.isEmpty() ) widgetAdaptor->setType(typeName);
            return widgetAdaptor;
            */
        }
#endif

        /**
        * Return true if the painter is enabled.
        *
        * If the painter is enabled this applet will emit \a paint() signals
        * each time (re-)painting is requested. Please note, that while this
        * may eat more performance, it allows you to use the \a Skip::Painter
        * class to draw stuff yourself within scripts.
        */
        bool isPainterEnabled() { return m_painterenabled; }

        /**
        * Enable or disable the painter.
        */
        void setPainterEnabled(bool enabled) { m_painterenabled = enabled; }

    Q_SIGNALS:

        /**
        * This signal is emitted if the configuration interface should be shown.
        */
        void showConfigurationInterface();

        /**
        * This signal is emitted if the painter is enabled and if (re-)painting
        * should be done.
        *
        * See also the \a isPainterEnabled() and the \a setPainterEnabled() methods.
        */
        void paint(QObject* painter, const QRect &rect);

    protected:
        Karamba *m_karamba;
        Plasma::Applet *m_applet;
        //QPointer<Plasma::Widget> m_widget;
        bool m_painterenabled;
        QHash<QString, PlasmaSensor*> m_engines;
};

/**
* The SkContainmentAdaptor class extends the SkAppletAdaptor class
* to implements an adaptor for Plasma::Containment objects.
*/
class SkContainmentAdaptor : public SkAppletAdaptor
{
        Q_OBJECT
    public:
        SkContainmentAdaptor(Karamba *karamba, Plasma::Containment *containment)
            : SkAppletAdaptor(karamba, containment) {
            connect(containment, SIGNAL(appletRemoved(Plasma::Applet*)), this, SLOT(appletRemoved(Plasma::Applet*)));
//containment->setOpacity(0.5);
        }
        virtual ~SkContainmentAdaptor() { qDeleteAll(m_applets.values()); }
        Plasma::Containment* containment() const { return static_cast<Plasma::Containment*>(m_applet); }
    public Q_SLOTS:

        /**
        * Return the number of applets within this containment.
        */
        int appletCount() { return containment()->applets().count(); }

        /**
        * Return the applet with the defined index or NULL if there is no
        * such applet within this containment.
        */
        QObject* applet(int index) {
            if( index >= 0 && index < containment()->applets().count() )
                if( Plasma::Applet *applet = containment()->applets()[index] ) {
                    if( m_applets.contains(applet->id()) )
                        return m_applets[applet->id()];
                    SkAppletAdaptor *a = new SkAppletAdaptor(m_karamba, applet);
                    m_applets.insert(applet->id(), a);
                    return a;
                }
            return 0;
        }

        /**
        * Adds an applet to this Containment. The chosen \p name is the plugin name for the
        * applet, as given by KPluginInfo::pluginName() while the \p args are the optional
        * arguments passed to the applet and \p geometry is where to place the applet, or
        * to auto-place it if an invalid position is provided.
        */
        QObject* addApplet(const QString& appletname, const QVariantList& args = QVariantList(), const QRectF& geometry = QRectF(-1, -1, -1, -1)) {
            if( Plasma::Applet *applet = containment()->addApplet(appletname, args, geometry) ) {
                SkAppletAdaptor *a = new SkAppletAdaptor(m_karamba, applet);
                Q_ASSERT( ! m_applets.contains(applet->id()) );
                m_applets.insert(applet->id(), a);
                applet->raise();
                return a;
            }
            return 0;
        }

    private Q_SLOTS:
        void appletRemoved(Plasma::Applet* applet) {
            if( m_applets.contains(applet->id()) )
                delete m_applets.take(applet->id());
        }
    private:
        QMap<uint, SkAppletAdaptor*> m_applets;
};

#endif
