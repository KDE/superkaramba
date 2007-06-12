/***************************************************************************
 *   Copyright (C) 2003 by Wilfried Huss <Wilfried.Huss@gmx.at>            *
 *   Copyright (C) 2003 Matt Broadstone <mbroadst@gmail.com>               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "richtextlabel.h"
#include "richtextlabel.moc"

#include "karamba.h"
#include "textlabel.h"

#include <QApplication>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QAbstractTextDocumentLayout>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

#include <KRun>

RichTextLabel::RichTextLabel(Karamba* k)
        :   Meter(k, 0, 0, 0, 0),
        text(0),
        source(""),
        colorGrp(QApplication::palette()),
        underlineLinks(false)
{
    originalSize = QSize(0, 0);
}

RichTextLabel::RichTextLabel(Karamba* k, int x, int y, int w, int h)
        :   Meter(k, x, y, w, h),
        text(0),
        source(""),
        colorGrp(QApplication::palette()),
        underlineLinks(false)
{
    originalSize = QSize(w, h);
}

RichTextLabel::~RichTextLabel()
{
    if (text != 0) {
        delete text;
        text = 0;
    }
}

void RichTextLabel::setText(const QString &t, bool linkUnderline)
{
    source = t;
    if (text != 0) {
        delete text;
        text = 0;
    } else {
        // set underlineLinks only when RichTextLabel is created, not
        // when text is changed.
        underlineLinks = linkUnderline;
    }

    text = new QTextDocument();
    text->setHtml(t);
    text->setDefaultFont(font);
    text->setTextWidth(getWidth());

    QTextCharFormat format;
    format.setForeground(QBrush(colorGrp.color(QPalette::Text)));
    QTextCursor cursor(text);
    cursor.select(QTextCursor::Document);
    cursor.mergeCharFormat(format);

    if (getWidth() < 1) {
        text->adjustSize();
        Meter::setWidth((int)text->textWidth());
        Meter::setHeight((int)text->size().height());
    }
}

void RichTextLabel::setValue(const QString &text)
{
    setText(text);
}

void RichTextLabel::setValue(int v)
{
    setText(QString::number(v));
}

void RichTextLabel::setFont(const QString &f)
{
    font.setFamily(f);
    if (text != 0)
        text->setDefaultFont(font);
}

QString RichTextLabel::getFont() const
{
    return font.family();
}

void RichTextLabel::setFontSize(int size)
{
    font.setPixelSize(size);
    if (text != 0)
        text->setDefaultFont(font);
}

int RichTextLabel::getFontSize() const
{
    return font.pixelSize();
}

void RichTextLabel::setFixedPitch(bool fp)
{
    font.setFixedPitch(fp);
    if (text != 0)
        text->setDefaultFont(font);
}

bool RichTextLabel::getFixedPitch() const
{
    return font.fixedPitch();
}

void RichTextLabel::setTextProps(TextField* t)
{
    if (t) {
        setFontSize(t->getFontSize());
        setFont(t->getFont());
        colorGrp.setColor(QPalette::Text, t->getColor());

        QTextCharFormat format;
        format.setForeground(QBrush(colorGrp.color(QPalette::Text)));
        QTextCursor cursor(text);
        cursor.select(QTextCursor::Document);
        cursor.mergeCharFormat(format);
    }
}

void RichTextLabel::setWidth(int width)
{
    Meter::setWidth(width);
    text->setTextWidth(getWidth());
}

void RichTextLabel::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                          QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (m_hidden || text == 0)
        return;

    int w = getWidth();
    int h = getHeight();

    text->drawContents(painter, QRect(0, 0, w, h));
}

bool RichTextLabel::mouseEvent(QEvent *e)
{
    Qt::MouseButtons button;
    QPointF pos;
    if (QGraphicsSceneMouseEvent *event = dynamic_cast<QGraphicsSceneMouseEvent*>(e)) {
        button = event->button();
        pos = mapFromParent(event->pos());
    } else if (QGraphicsSceneWheelEvent *event = dynamic_cast<QGraphicsSceneWheelEvent*>(e)) {
        button = event->buttons();
        pos = mapFromParent(event->pos());
    }

    QString link = text->documentLayout()->anchorAt(pos);

    if (link[0] != '#') {
        if (button == Qt::LeftButton)
            KRun::runCommand(link,0L);

        return false;
    } else
        return true;
}

QString RichTextLabel::getAnchor(QPointF point)
{
    QPointF pos = point - boundingRect().topLeft();
    return text->documentLayout()->anchorAt(pos).remove(0, 1);
}

void RichTextLabel::setColorGroup(const QPalette &colorg)
{
    colorGrp = colorg;
}

const QPalette& RichTextLabel::getColorGroup() const
{
    return colorGrp;
}

