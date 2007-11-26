/****************************************************************************
*  imagelabel.h  -  ImageLabel meter
*
*  Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>
*  Copyright (c) 2004 Petri Damst� <damu@iki.fi>
*
*  This file is part of SuperKaramba.
*
*  SuperKaramba is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  SuperKaramba is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with SuperKaramba; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
****************************************************************************/

#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QPainter>
#include <QGraphicsSceneMouseEvent>

#include <kio/netaccess.h>
#include <kio/copyjob.h>
#include <KSvgRenderer>

#include "karamba.h"
#include "meter.h"

class ImageLabel;

// Abstract Effects Baseclass
class Effect : public QObject
{

    Q_OBJECT

public:
    Effect(ImageLabel*, int millisec);

    virtual ~Effect();

    virtual QPixmap apply(QPixmap pixmap) = 0;

    void startTimer();

protected:
    ImageLabel* myImage;

    int millisec;
};

// Intensity
class Intensity : public Effect
{
public:
    Intensity(ImageLabel*, float r, int millisec);

    QPixmap apply(QPixmap pixmap);

private:
    float ratio;
};


// ChannelIntensity
class ChannelIntensity : public Effect
{
public:
    ChannelIntensity(ImageLabel*, float r, const QString &c, int millisec);

    QPixmap apply(QPixmap pixmap);

private:
    float ratio;
    int channel;
};

// ToGray
class ToGray : public Effect
{
public:
    ToGray(ImageLabel*, int millisec);

    QPixmap apply(QPixmap pixmap);
};

// ToAlpha
class ToAlpha : public Effect
{
public:
    ToAlpha(ImageLabel*, const QColor &alphaColor, int alpha, int millisec);

    QPixmap apply(QPixmap pixmap);

private:
    QColor m_alphaColor;
    int m_alpha;
};

class ImageLabel : public Meter
{

    Q_OBJECT

public:
    ImageLabel(Karamba* k, int ix, int iy, int iw, int ih);
    ImageLabel(Karamba* k);
    ~ImageLabel();
    void setValue(const QString &imagePath);

    void setValue(int);
    void setValue(QPixmap&);
    QString getStringValue() const
    {
        return imagePath;
    }
    void scale(int, int);
    void smoothScale(int, int);

    void rotate(int);
    void removeImageTransformations();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);

    void rolloverImage(QGraphicsSceneHoverEvent *e);
    void parseImages(const QString &fn, const QString &fn_roll, int, int, int, int);
    virtual void show();
    virtual void hide();

    void setTooltip(QString txt);
    int background;
    // Pixmap Effects
    void removeEffects();
    void intensity(float ratio, int millisec);
    void channelIntensity(float ratio, QString channel, int millisec);
    void toGray(int millisec);
    void toAlpha(const QColor &alphaColor, int alpha, int millisec);
    void setBackground(int b);

    void attachClickArea(QString leftMouseButton, QString middleMouseButton,
                         QString rightMouseButton);

    bool mouseEvent(QEvent *e);

    void allowClick(bool);
    bool clickable();
    void setPixel(const QPoint &point, const QColor &pixel);

    bool enableAnimation(bool enable);
    bool animationEnabled() const;

    bool drawElement(const QString &element);
    QString elementDrawn() const;

private slots:

    // gets called if a timed effects needs to bee removed
    void slotEffectExpired();
    void slotCopyResult(KJob* job);
    void repaintSvg();

signals:
    void pixmapLoaded();

private:
    void applyTransformations(bool useSmoothScale = false);
    int pixmapWidth;
    int pixmapHeight;
    int pixmapOffWidth;
    int pixmapOffHeight;
    int pixmapOnWidth;
    int pixmapOnHeight;

    bool m_allowClick;

    // true if Image has been scaled
    bool doScale;
    // true if Image has been rotated
    bool doRotate;

    // Contains the current Effect or is 0 if no Effect is applied
    Effect* imageEffect;

    // Scale Matrix
    //QMatrix scaleMat;
    int scale_w;
    int scale_h;
    // Rotation Matrix
    //QMatrix rotMat;
    int rot_angle;

    QPixmap pixmap;
    QPixmap realpixmap;

    QRect rect_off, rect_on;
    QRect old_tip_rect;

    bool zoomed;
    //QString fn, fn_roll;
    bool rollover;
    QPixmap pixmap_off;
    QPixmap pixmap_on;
    int xoff, xon;
    int yoff, yon;
    QString imagePath;

    QString m_leftMouseButtonAction;
    QString m_middleMouseButtonAction;
    QString m_rightMouseButtonAction;

    KSvgRenderer *m_renderer;
    bool m_connected;

    QString m_element;
};

#endif // IMAGELABEL_H
