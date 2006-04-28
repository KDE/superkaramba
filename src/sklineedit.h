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

#ifndef SKLINEEDIT_H
#define SKLINEEDIT_H

#include <qlineedit.h>
#include <qwidget.h>
#include <qevent.h>
#include <qpainter.h>
#include <qcolor.h>

class Input;

class SKLineEdit : public QLineEdit
{
  public:
    SKLineEdit(QWidget *w, Input *i);
    ~SKLineEdit();

    void drawFrame(QPainter *p);
    void drawContents(QPainter *p);

    void setFrameColor(QColor c);
    QColor getFrameColor() const;

    void setBackgroundColor(QColor c);
    
    Input* getInput();

  protected:
    virtual void keyReleaseEvent(QKeyEvent* e);
    virtual void keyPressEvent(QKeyEvent* e);

  private:
    QColor frameColor;
    Input* m_input;
};

#endif
