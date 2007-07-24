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
