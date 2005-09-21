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

class Bar : public Meter
{
    Q_OBJECT
    Q_PROPERTY(QString imagePath READ getImage WRITE setImage)
    Q_PROPERTY(int barValue READ getValue WRITE setValue)
    Q_PROPERTY(int max READ getMax WRITE setMax)
    Q_PROPERTY(int min READ getMin WRITE setMin)
    Q_PROPERTY(bool vertical READ getVertical WRITE setVertical)
public:
    Bar(KarambaWidget* k,int ix,int iy,int iw,int ih );
    ~Bar();

    void mUpdate( QPainter * );

    virtual void setMax(int max);
    virtual void setMin(int min);
    virtual int getMax()
    {
        return m_minValue;
    };
    virtual int getMin()
    {
        return m_maxValue;
    };

public slots:
    bool setImage( QString imagePath );
    QString getImage()
    {
        return imagePath;
    };

    void setValue( int );
    int  getValue()
    {
        return barValue;
    };
    void setValue( QString );
    void recalculateValue()
    {
        setValue(barValue);
    };

    void setVertical( bool );
    int  getVertical()
    {
        return vertical;
    };

    void update();

private:
    int barValue;
    int value;

    int pixmapWidth;
    int pixmapHeight;

    bool vertical; // vertical bar?

    QString imagePath;
    QPixmap pixmap;

    int m_minValue;
    int m_maxValue;
protected:
    void paintEvent(QPaintEvent* event);
        
}
;
#endif // BAR_H
