/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef BAR_H
#define BAR_H

#include "meter.h"
#include <qpixmap.h>
#include <qstring.h>
#include <qpainter.h>
#include <QTimerEvent>

class Bar : public Meter
{
Q_OBJECT
public:
    Bar(Karamba* k,int ix,int iy,int iw,int ih );
    ~Bar();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                    QWidget *widget);

    virtual void setMax( int m );
    virtual void setMin( int m );

public slots:
    bool setImage( QString imagePath );
    QString getImage() { return imagePath; };

    void setValue( int );
    int  getValue() { return barValue; };
    void setValue( QString );
    void recalculateValue() {setValue(barValue); };

    void setVertical( bool );
    int  getVertical() { return vertical; };

    void valueChanged();

private:
    int barValue;
    int oldBarValue;
    int value;

    QTimer *m_timer;

    int pixmapWidth;
    int pixmapHeight;

    bool vertical; // vertical bar?

    QString imagePath;
    QPixmap pixmap;

  protected:
    //void timerEvent(QTimerEvent *event);
};

#endif // BAR_H
