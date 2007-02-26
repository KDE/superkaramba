/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson <karlsson.h@home.se>              *
 *   Copyright (C) 2003 Matt Broadstone <mbroadst@gmail.com>               *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef METER_H
#define METER_H

#include <QObject>
#include <QMouseEvent>
#include <QGraphicsItem>

class QPainter;
class Karamba;
class Meter : public QObject,
              public QGraphicsItem
{
Q_OBJECT
public:
    Meter(Karamba* k, int ix,int iy,int iw,int ih);
    Meter(Karamba* k);

    virtual ~Meter();
    virtual int getX();
    virtual int getY();
    virtual int getWidth();
    virtual int getHeight();
    virtual void setX(int);
    virtual void setY(int);
    virtual void setWidth(int);
    virtual void setHeight(int);

    virtual void setSize(int ix, int iy, int iw, int ih);

    virtual void setMax(int max) { maxValue = max; };
    virtual void setMin(int min) { minValue = min; };
    virtual int getMax() { return minValue; };
    virtual int getMin() { return maxValue; };

    void setThemePath( QString );

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                    QWidget *widget) = 0;

    virtual QRectF boundingRect() const { return QRectF(boundingBox); };

    virtual void setValue(int) {};
    virtual int getValue() { return -1; };
    virtual void setValue(QString) {};
    virtual QString getStringValue() const { return QString::null; };
    virtual void recalculateValue() {};

    virtual void setColor(QColor clr) { color = clr; };
    virtual QColor getColor() { return color; };

    virtual void show() { hidden = 0; };
    virtual void hide() { hidden = 1; };

    void setOpacity(double value);

    QRect getBoundingBox();

    // true when given coordinate point is inside the meters
    // active reagion and meter is enabled
    virtual bool insideActiveArea(int, int);

    // returns true when callback meterClicked should be called.
    virtual bool click( QMouseEvent* );

    void setEnabled(bool);
    bool isEnabled();

    //void deleteMeter();

    /*
    void setOnClick( QString );
    void setOnMiddleClick( QString );
    */

protected: // Protected attributes
    QString themePath;

    QRect boundingBox;

    // Actions to execute when clicked on meter
    QString leftButtonAction;
    QString middleButtonAction;
    QString rightButtonAction;

    bool clickable;
    int hidden;
    int minValue;
    int maxValue;

    QColor color;
    Karamba* m_karamba;

    QTimer *m_opacityTimer;

    double m_opacity;

private Q_SLOTS:
    void fadeIn();
    //void fadeOut(bool del = true);

};

#endif // METER_H
