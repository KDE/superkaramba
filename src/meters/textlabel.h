/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef TEXTLABEL_H
#define TEXTLABEL_H

#include <QColor>
#include <QFont>
#include <QStringList>
#include <QRectF>

#include "textfield.h"
#include "meter.h"

class QPainter;
class Karamba;
class TextLabel : public Meter
{
    Q_OBJECT
public:
    typedef enum ScrollType { ScrollNone, ScrollNormal,
                              ScrollBackAndForth, ScrollOnePass };

    TextLabel(Karamba *k, int x, int y, int w, int h);
    TextLabel(Karamba *k);
    ~TextLabel();

    void setTextProps(TextField*);
    void setValue(QString);
    void setValue(int);
    //virtual QString getStringValue() const { return value.join("\n"); };
    QString getStringValue() const
    {
        return value.join("\n");
    };
    void setFontSize(int);
    void setBGColor(QColor clr);
    void setFont(QString);
    void setAlignment(QString);
    void setFixedPitch(bool);
    void setShadow(int);

    void paint(QPainter *p, const QStyleOptionGraphicsItem *option,
               QWidget *widget);

    virtual void show();
    virtual void hide();
    int getFontSize() const;
    QColor getBGColor() const;
    QString getFont() const;
    QString getAlignment() const;
    bool getFixedPitch() const;
    int getShadow() const;
    void setScroll(ScrollType type, QPoint speed, int gap, int pause);
    void setScroll(QString type, QPoint speed, int gap, int pause);

    void attachClickArea(QString leftMouseButton, QString middleMouseButton,
                         QString rightMouseButton);

    bool mouseEvent(QGraphicsSceneMouseEvent *e);

    QRectF boundingRect();

    void allowClick(bool enable);
    bool clickable();

private:
    int alignment;
    int clip;
    QStringList value;
    QFont font;
    QColor bgColor;
    int lineHeight;
    QSize textSize;
    int shadow;
    TextField text;
    QPoint scrollSpeed;
    QPoint scrollPos;
    int scrollGap;
    int scrollPause;
    int pauseCounter;
    ScrollType scrollType;
    QPoint origPoint;

    bool m_clickable;

    int drawText(QPainter *p, int x, int y, int width, int height,
                 QString text);
    bool calculateScrollCoords(QRect meterRect, QRect &textRect,
                               QPoint &next, int x, int y);
    void calculateTextSize();
};

#endif // TEXTLABEL_H
