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

#ifndef SKAPPLET_H
#define SKAPPLET_H

#include <QImage>
#include <QPaintDevice>
#include <QLabel>
#include <QPixmap>
#include <QTimer>
#include <QPaintEvent>
#include <QPainter>
#include <QTimer>
#include <QX11Info>
#include <QWidget>
#include <QGraphicsItem>
#include <QColor>

#include <plasma/applet.h>
#include <plasma/dataengine.h>
//#include "ui_clockConfig.h"

namespace Plasma
{
    class Svg;
}

class SuperKarambaApplet : public Plasma::Applet
{
        Q_OBJECT
    public:
        SuperKarambaApplet(QObject *parent, const QStringList &args);
        virtual ~SuperKarambaApplet();

        virtual void paintInterface(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
        //void setPath(const QString&);
        //QRectF boundingRect() const;
        virtual void constraintsUpdated();

    public slots:
        //void updated(const QString &name, const Plasma::DataEngine::Data &data);
        virtual void showConfigurationInterface();

    private slots:
        void configAccepted();
        void slotTimeout();

    private:
        KUrl m_themePath;
        QGraphicsItemGroup* m_themeItem;
        QTimer* m_timer;

        void loadKaramba();

        //bool m_showTimeString;
        //bool m_showSecondHand;
        //QRectF m_bounds;
        //int m_pixelSize;
        //QString m_timezone;
        //Plasma::Svg* m_theme;
        //QTime m_time;
        //KDialog *m_dialog; //should we move this into another class?
        //QTime m_lastTimeSeen;
        /// Designer Config file
        //Ui::clockConfig ui;
};

K_EXPORT_PLASMA_APPLET(skapplet, SuperKarambaApplet)

#endif
