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
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ****************************************************************************/

#ifndef INPUT_H
#define INPUT_H

#include <meter.h>
#include <sklineedit.h>

#include <qpainter.h>
#include <qcolor.h>
#include <qlineedit.h>
#include <qwidget.h>
#include <qstring.h>
#include <qfont.h>

#include "textfield.h"

class Input : public Meter
{
    Q_OBJECT
public:
    Input(KarambaWidget* k, int ix, int iy, int iw, int ih);
    Input();

    ~Input();

    void setValue(QString text);
    QString getStringValue() const;

    void setBGColor(QColor c);
    QColor getBGColor() const;
    void setSelectionColor(QColor selectionColor);
    QColor getSelectionColor() const;
    void setSelectedTextColor(QColor selectedTextColor);
    QColor getSelectedTextColor() const;
    void setTextProps(TextField*);

    void setMeterFont(QString f);
    QString getFont() const;
    void setFontSize(int size);
    int getFontSize() const;
    void setFGColor(QColor c);
    QColor getFGColor() const;
    

private:
    QLineEdit *edit;
protected:
    void paintEvent(QPaintEvent*);
};

#endif
