/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef METER_H
#define METER_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qwidget.h>
#include <QMouseEvent>

#include <qfileinfo.h>
#include <kglobal.h>

class KarambaWidget;
class Sensor;

class Meter : public QWidget
{
    Q_OBJECT
     /* So that for meter.x=50 we can use meter->setProperty(x,50) instead of
             several if,else,switch statements, After change to QWidget, many property
             will be vanished from here */
            
//     Q_PROPERTY(int x READ getX WRITE setX)
//     Q_PROPERTY(int y READ getY WRITE setY)
//     Q_PROPERTY(int width READ getWidth WRITE setWidth)
//     Q_PROPERTY(int height READ getHeight WRITE setHeight)

public:

    Meter(KarambaWidget* k, int ix,int iy,int iw,int ih);
    Meter(KarambaWidget* k);
    virtual ~Meter();
//     virtual int getX() const;
//     virtual int getY() const;
//     virtual int getWidth() const;
//     virtual int getHeight() const;
//     virtual void setX(int);
//     virtual void setY(int);
//     virtual void setWidth(int);
//     virtual void setHeight(int);

//     virtual void setSize(int ix, int iy, int iw, int ih);

//     virtual void mUpdate(QPainter *)=0 ;

    // TODO Review this methods and change python ifaces //
    virtual void setMax(int max)
    {}
    virtual void setMin(int min)
    {}
    virtual int getMax()
    {
        return 0;
    }
    virtual int getMin()
    {
        return 0;
    }
    virtual void setValue(int)
    {}
    
    virtual int getValue()
    {
        return -1;
    }
    virtual void setValue(QString)
    {}
    
    virtual QString getStringValue() const
    {
        return QString::null;
    }
    virtual void recalculateValue()
    {}
    

    virtual void setColor(QColor clr)
    {}
    
    virtual QColor getColor()
    {
        return Qt::black;
    }
    ////////////////////////////////////////////////////////
//     virtual void show()
//     {
//         hidden = 0;
//     }
//     virtual void hide()
//     {
//         hidden = 1;
//     }

    QRect getBoundingBox();

    // true when given coordinate point is inside the meters
    // active reagion and meter is enabled
//    virtual bool insideActiveArea(int, int);

    // returns true when callback meterClicked should be called.
//    virtual bool click( QMouseEvent* );

    void setEnabled(bool);
    bool isEnabled();

    void attachToSensor(Sensor*);
    void detachFromSensor();
    QString getFormat()
    {
        return m_format;
    }
    void setFormat(QString f)
    {
        m_format = f;
    }
    Sensor* getSensor()
    {
        return m_sensor;
    }
    virtual void setMeterFont(QString f)
    {
        QFont ft=font();
        ft.setFamily(f);
        setFont(ft);
    }
    
    /*
    void setOnClick( QString );
    void setOnMiddleClick( QString );
    */
public slots:
    virtual void updateData();
    void storeData(QVariant value);

protected: // Protected attributes
    QRect boundingBox;

    // Actions to execute when clicked on meter
    QString leftButtonAction;
    QString middleButtonAction;
    QString rightButtonAction;

    bool clickable;
    int hidden;
    bool autoUpdate; // the boolean to track , if signal of update will be ignored or honoured
    QVariant data; //data from sensor storage
    QString m_format;
    Sensor* m_sensor;
    KarambaWidget* m_karamba;
    QVariant decodeDot(QString formatString);
};

#endif // METER_H
