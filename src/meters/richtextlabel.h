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
#include <q3simplerichtext.h>
#include <qpainter.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qrect.h>
#include <qsize.h>
#include "karambawidget.h"

class QTextDocument;
class RichTextLabel : public Meter
{
    Q_OBJECT
public:
    RichTextLabel(KarambaWidget*);
    RichTextLabel(KarambaWidget* k, int x, int y, int w, int h);
    ~RichTextLabel();

    void setText(QString text, QString f="");
    void setValue(QString text);
    void setValue(int v);
    QString getStringValue()
    {
        return source;
    };

    void setFontString(QString font);
    QString getFont() const
    {
        return font().family();
    }
    void setFontSize(int);
    int getFontSize() const;
    void setFixedPitch(bool);
    bool getFixedPitch() const;
    void setTextProps( TextField* t );
    void setColorGroup(const QColorGroup &colorg);
    const QColorGroup &getColorGroup() const;
    void setWidth(int width);

    virtual bool insideActiveArea(qreal,qreal);

 //   virtual bool click(QMouseEvent*);
 //   virtual void mUpdate(QPainter*);

    QString anchorAt(qreal, qreal);
    inline Qt::TextFormat textFormat() { return m_type;}
    void setTextFormat(Qt::TextFormat f) 
    {
        m_type=f;
    }  

public slots:
    void update();

private:
    QTextDocument* textDoc;
    QString source;
    QColorGroup colorGrp;
    bool underlineLinks;
    QSize originalSize;
    Qt::TextFormat m_type;
protected:
    virtual void paintEvent(QPaintEvent* pe);
};

#endif
