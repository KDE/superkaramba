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

#include "input.h"
#include "kdebug.h"
#include "karamba_python.h"
Input::Input(KarambaWidget* k, int x, int y, int w, int h):
        Meter(k, x, y, w, h)
{
    edit = new QLineEdit(this);
    edit->setGeometry(0,0,w,h);
    edit->show();
}

Input::~Input()
{}

void Input::paintEvent(QPaintEvent*)
{
    edit->update();
}

void Input::setValue(QString text)
{
    edit->setText(text);
}

QString Input::getStringValue() const
{
    return edit->text();
}

void Input::setBGColor(QColor c)
{
    QPalette pal=edit->palette();
    pal.setColor(QPalette::Base,c);
    edit->setPalette(pal);
}

void Input::setFGColor(QColor c)
{
    QPalette pal=edit->palette();
    pal.setColor(QPalette::Text,c);
    edit->setPalette(pal);
}

QColor Input::getBGColor() const
{
    return edit->palette().color(QPalette::Base);
}

QColor Input::getFGColor() const
{
    return edit->palette().color(QPalette::Text);
}


void Input::setMeterFont(QString fs)
{
    QFont f=edit->font();
    f.setFamily(fs);
    edit->setFont(f);
    
}

QString Input::getFont() const
{
    return edit->font().family();
}


void Input::setSelectionColor(QColor selectionColor)
{
    QPalette palette = edit->palette();
    palette.setColor(QPalette::Highlight, selectionColor);
    edit->setPalette(palette);
}

QColor Input::getSelectionColor() const
{
    return edit->palette().color(QPalette::Highlight);
}

void Input::setSelectedTextColor(QColor selectedTextColor)
{
    QPalette palette = edit->palette();
    palette.setColor(QPalette::HighlightedText, selectedTextColor);
    edit->setPalette(palette);
}

QColor Input::getSelectedTextColor() const
{
    return edit->palette().color(QPalette::HighlightedText);
}

void Input::setFontSize(int size)
{
    QFont f=edit->font();
    f.setPixelSize(size);
    edit->setFont(f);
}

int Input::getFontSize() const
{
    return edit->font().pixelSize();
}

void Input::setTextProps(TextField* t)
{
    if(t)
    {
        setFontSize(t->getFontSize());
        setMeterFont(t->getFont());
        setColor(t->getColor());
        setBGColor(t->getBGColor());
    }
}

#include "input.moc"
