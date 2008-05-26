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
#include "karamba.h"
#include "richtextlabel.h"

RichTextLabel::RichTextLabel(karamba* k) :
    Meter(k, 0, 0, 100, 100),
    text(0),
    source(""),
    colorGrp(k->colorGroup()),
    underlineLinks(false)
{
  originalSize = QSize(0, 0);
}

RichTextLabel::RichTextLabel(karamba* k, int x, int y, int w, int h) :
    Meter(k, x, y, w, h),
    text(0),
    source(""),
    colorGrp(k->colorGroup()),
    underlineLinks(false)
{
  kdDebug() << k_funcinfo << x << ", " << y << ", " << w << ", " << h << endl;
  originalSize = QSize(w, h);
}

RichTextLabel::~RichTextLabel()
{
  if (text != 0)
  {
    delete text;
    text = 0;
  }
}

void RichTextLabel::setText(QString t, bool linkUnderline)
{
  source = t;
  if (text != 0)
  {
    delete text;
    text = 0;
  }
  else
  {
    // set underlineLinks only when RichTextLabel is created, not
    // when text is changed.
    underlineLinks = linkUnderline;
  }

  text = new QSimpleRichText(t, font, m_karamba->theme().path(),
                             0,        // default QStyleSheet*
                             0,        // default QMimeSourceFactory
                             -1,       // no pageBreak
                             Qt::blue, // (has no effect) link Color
                             underlineLinks);

  // set the text to a reasonable size
  text->adjustSize();
  if(originalSize.width() < 1)
    setWidth(text->width());
  else
    text->setWidth(getWidth());
  if(originalSize.height() < 1)
    setHeight(text->height());
}

void RichTextLabel::setValue(QString text)
{
  setText(text);
}

void RichTextLabel::setValue(long v)
{
  setText(QString::number(v));
}

void RichTextLabel::setFont(QString f)
{
  font.setFamily(f);
  if(text != 0)
    text->setDefaultFont(font);
}

QString RichTextLabel::getFont() const
{
  return font.family();
}

void RichTextLabel::setFontSize(int size)
{
  font.setPixelSize(size);
  if(text != 0)
    text->setDefaultFont(font);
}

int RichTextLabel::getFontSize() const
{
  return font.pixelSize();
}

void RichTextLabel::setFixedPitch(bool fp)
{
  font.setFixedPitch(fp);
  if(text != 0)
    text->setDefaultFont(font);
}

bool RichTextLabel::getFixedPitch() const
{
  return font.fixedPitch();
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

void RichTextLabel::setWidth(int width)
{
  Meter::setWidth(width);
  // rearrange text
  text->setWidth(getWidth());
  if(originalSize.height() < 1)
    setHeight(text->height());
}

void RichTextLabel::mUpdate(QPainter* p)
{
  if (hidden || text == 0)
  {
    return;
  }

  int x = getX();
  int y = getY();
  int w = getWidth();
  int h = getHeight();
  QRect clipRect(x, y, w, h);
  text->draw(p, x, y, clipRect, colorGrp, 0 /* no background */);
}

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
}

QString RichTextLabel::anchorAt(int x, int y)
{
  QPoint point(x - getX(), y - getY());
  QString anchor = text->anchorAt(point);
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

bool RichTextLabel::insideActiveArea(int x, int y)
{
  QPoint point(x - getX(), y - getY());
  return text->anchorAt(point) != ""; // && text -> inText(point);
}

void RichTextLabel::setColorGroup(const QColorGroup &colorg)
{
  colorGrp = colorg;
}

const QColorGroup & RichTextLabel::getColorGroup() const
{
  return colorGrp;
}

#include "richtextlabel.moc"
