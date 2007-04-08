/***************************************************************************
 *   Copyright (C) 2003 by Wilfried Huss <Wilfried.Huss@gmx.at>            *
 *   Copyright (C) 2003 Matt Broadstone <mbroadst@gmail.com>               *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef RICHTEXTLABEL_H
#define RICHTEXTLABEL_H

#include <QFont>
#include <QPalette>

#include "meter.h"

class QTextDocument;
class Karamba;
class TextField;

class RichTextLabel : public Meter
{
    Q_OBJECT
public:
    RichTextLabel(Karamba*);
    RichTextLabel(Karamba* k, int x, int y, int w, int h);
    ~RichTextLabel();

    void setText(const QString &text, bool linkUnderline = false);
    void setValue(const QString &text);
    void setValue(int v);
    QString getStringValue() const
    {
        return source;
    }

    void setFont(const QString &font);
    QString getFont() const;
    void setFontSize(int);
    int getFontSize() const;
    void setFixedPitch(bool);
    bool getFixedPitch() const;
    void setTextProps(TextField* t);
    void setColorGroup(const QPalette &colorg);
    const QPalette& getColorGroup() const;
    void setWidth(int width);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0);

    bool mouseEvent(QEvent *event);
    QString getAnchor(QPointF point);

private:
    QTextDocument* text;

    QString source;
    QFont font;
    QPalette colorGrp;
    bool underlineLinks;
    QSize originalSize;

};

#endif
