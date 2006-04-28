/****************************************************************************
 * Copyright (c) 2005 Alexander Wiedenbruch <mail@wiedenbruch.de>
 *
 * This file is part of SuperKaramba.
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

#include "sklineedit.h"
#include "karamba.h"
#include "kdebug.h"

SKLineEdit::SKLineEdit(QWidget *w, Input *i) : QLineEdit(w), m_input(i)
{
  frameColor = Qt::gray;
  setBackgroundColor(Qt::white);
}

SKLineEdit::~SKLineEdit()
{
}

void SKLineEdit::drawFrame(QPainter *p)
{
  p->setPen(frameColor);
  p->drawRect(frameRect());
}

void SKLineEdit::drawContents(QPainter *p)
{
  QLineEdit::drawContents(p);
}

void SKLineEdit::setFrameColor(QColor c)
{
  frameColor = c;
  repaint();
}

void SKLineEdit::setBackgroundColor(QColor c)
{
  QLineEdit::setBackgroundColor(c);
  repaint();
}

QColor SKLineEdit::getFrameColor() const
{
  return frameColor;
}

void SKLineEdit::keyPressEvent(QKeyEvent* e)
{
  QLineEdit::keyPressEvent(e);

  if(!e->text().isEmpty())
  {
    karamba* k = static_cast<karamba*>(parent());
    k->keyPressed(e->text(), m_input);
  }
}

void SKLineEdit::keyReleaseEvent(QKeyEvent* e)
{
  QLineEdit::keyReleaseEvent(e);
}

Input* SKLineEdit::getInput()
{
  return m_input;
}
