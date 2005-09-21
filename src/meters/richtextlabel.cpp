/***************************************************************************
 *   Copyright (C) 2003 by Wilfried Huss                                   *
 *   Wilfried.Huss@gmx.at                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <krun.h>
#include <kdebug.h>
#include "karambawidget.h"
#include "richtextlabel.h"
#include <QAbstractTextDocumentLayout>
#include <QPalette>
#include <QStringList>

RichTextLabel::RichTextLabel(KarambaWidget* k) :
        Meter(k, 0, 0, 100, 100),
        source(""),
        colorGrp(k->colorGroup()),
        m_type(Qt::AutoText)
{
    originalSize = QSize(0, 0);
    textDoc= new QTextDocument(this);
    textDoc->setPageSize(QSizeF(width(),height()));
    textDoc->setDefaultFont(font());
}

RichTextLabel::RichTextLabel(KarambaWidget* k, int x, int y, int w, int h) :
        Meter(k, x, y, w, h),
        source(""),
        colorGrp(k->colorGroup()),
        underlineLinks(false)
{
    kdDebug() << k_funcinfo << x << ", " << y << ", " << w << ", " << h << endl;
    originalSize = QSize(w, h);
    textDoc= new QTextDocument(this);
    textDoc->setPageSize(QSizeF(width(),height()));
    textDoc->setDefaultFont(font());
}

RichTextLabel::~RichTextLabel()
{

}

void RichTextLabel::setText(QString t,QString textformat)
{
    if(textformat.toLower()=="plaintext")
        setTextFormat(Qt::PlainText);
    else if(textformat.toLower()=="richtext")
        setTextFormat(Qt::RichText);
    else if(textformat.toLower()=="autotext")
    {
        setTextFormat(Qt::AutoText);
    }
    if(t.isEmpty())
        source="";
    if(textFormat()==Qt::PlainText)
    {
        textDoc->setPlainText(source);
    }
    else if(textFormat()==Qt::RichText)
    {
        textDoc->setHtml(source); 
    }
    else if(textFormat()==Qt::AutoText)
    {
        if(Qt::mightBeRichText(source))
           textDoc->setHtml(source);
        else textDoc->setPlainText(source);
    }
    textDoc->setPageSize(QSizeF(width(),height()));
}

void RichTextLabel::setValue(QString text)
{
    setText(text);
}

void RichTextLabel::setValue(int v)
{
    setText(QString::number(v));
}

void RichTextLabel::setFontString(QString f)
{
    setFont(QFont(f));
    textDoc->setDefaultFont(font());
}


void RichTextLabel::setFontSize(int size)
{
    QFont f=font();
    f.setPixelSize(size);
    setFont(f);
    textDoc->setDefaultFont(font());
}

int RichTextLabel::getFontSize() const
{
    return font().pixelSize();
}

void RichTextLabel::setFixedPitch(bool fp)
{
    QFont f=font();
    f.setFixedPitch(fp);
    setFont(f);
    textDoc->setDefaultFont(font());
}

bool RichTextLabel::getFixedPitch() const
{
    return font().fixedPitch();
}

void RichTextLabel::setTextProps(TextField* t)
{
    if(t)
    {
        setFontSize(t->getFontSize());
        setFont(t->getFont());
        colorGrp.setColor(QColorGroup::Text, t->getColor());
    }
}

void RichTextLabel::setWidth(int p_width)
{
    resize(p_width,height());
    // rearrange text
    textDoc->setPageSize(QSizeF(width(),height()));
//     if(originalSize.height() < 1)
//         setHeight(text->height());
}



void RichTextLabel::paintEvent(QPaintEvent* )
{
    QPainter p(this);
    QAbstractTextDocumentLayout::PaintContext pc;
    pc.palette.setCurrentColorGroup(QPalette::Active);
    pc.palette.setActive(colorGrp);
    textDoc->documentLayout()->draw(&p,pc);
}

/*
bool RichTextLabel::click(QMouseEvent* e)
{
    if (hidden)
    {
        return false;
    }
    QPoint point(e->x() - getX(), e->y() - getY());
    QString anchor = text->anchorAt(point);
    if (anchor[0] != '#')
    {
        if (e->button() == Qt::LeftButton)
        {
            KRun :: runCommand(anchor);
        }
        return false;
    }
    else
    {
        //call callback meterClicked
        return true;
    }
}*/

QString RichTextLabel::anchorAt(qreal x, qreal y)
{
    QPointF point(x , y);
    QString anchor = textDoc->documentLayout()->anchorAt(point);
    if (anchor[0] == '#')
    {
        return anchor.remove(0, 1);
    }
    else
    {
        // ASSERT: should never happen
        return "";
    }
}

bool RichTextLabel::insideActiveArea(qreal x, qreal y)
{
    QPointF point(x, y);
    return textDoc->documentLayout()->anchorAt(point) != ""; // && text -> inText(point);
}

void RichTextLabel::setColorGroup(const QColorGroup &colorg)
{
    colorGrp = colorg;
}

const QColorGroup & RichTextLabel::getColorGroup() const
{
    return colorGrp;
}

void RichTextLabel::update()
{
    QString format = QString(m_format);
    QRegExp rx("([\\s]%[.\\d\\w]+[\\s])");
    rx.indexIn(format);
    QStringList capList=rx.capturedTexts();
    foreach(QString cap, capList)
    {
        QString temp=cap;
        temp.remove('%');
        temp=temp.trimmed();
        QVariant replText=decodeDot(temp);
        format.replace(cap,replText.toString());
    }
    setValue(format);
}

#include "richtextlabel.moc"
