/***************************************************************************
 *   Copyright (C) 2003 by Wilfried Huss                                   *
 *   Wilfried.Huss@gmx.at                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef RICHTEXTLABEL_H
#define RICHTEXTLABEL_H

#include "meter.h"
#include <qstring.h>
#include <qpainter.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qrect.h>
#include <qsize.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <QPalette>  //KDE4 QT Support
#include <QTextDocument>
#include "karamba.h"

class RichTextLabel : public Meter
{
    Q_OBJECT
  public:
    RichTextLabel(Karamba*);
    RichTextLabel(Karamba* k, int x, int y, int w, int h);
    ~RichTextLabel();

    void setText(QString text, bool linkUnderline = false);
    void setValue(QString text);
    void setValue(int v);
    QString getStringValue() { return source; };

    void setFont(QString font);
    QString getFont() const;
    void setFontSize(int);
    int getFontSize() const;
    void setFixedPitch(bool);
    bool getFixedPitch() const;
    void setTextProps( TextField* t );
    void setColorGroup(const QPalette &colorg);
    const QPalette& getColorGroup() const;
    void setWidth(int width);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                    QWidget *widget = 0);

    bool mouseEvent(QGraphicsSceneMouseEvent *event);
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
