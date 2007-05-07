/****************************************************************************
 * Copyright (c) 2005 Alexander Wiedenbruch <mail@wiedenbruch.de>
 * Copyright (c) 2007 Matt Broadstone <mbroadst@gmail.com>
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

#ifndef INPUT_H
#define INPUT_H

#include <QTextLayout>
#include <QTimer>

#include "meter.h"

class TextField;
class Karamba;
class Input : public Meter
{
    Q_OBJECT
public:
    Input(Karamba* k, int ix, int iy, int iw, int ih);
    Input();
    ~Input();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);

    void setValue(const QString &text);
    QString getStringValue() const;

    void setBGColor(QColor c);
    QColor getBGColor() const;
    void setColor(QColor c);
    QColor getColor() const;
    void setFontColor(QColor fontColor);
    QColor getFontColor() const;
    void setSelectionColor(QColor selectionColor);
    QColor getSelectionColor() const;
    void setSelectedTextColor(QColor selectedTextColor);
    QColor getSelectedTextColor() const;
    void setTextProps(TextField*);

    void hide();
    void show();

    void setSize(int ix, int iy, int iw, int ih);
    void setX(int ix);
    void setY(int iy);
    void setWidth(int iw);
    void setHeight(int ih);

    void setFont(const QString &f);
    QString getFont() const;
    void setFontSize(int size);
    int getFontSize() const;

    void setInputFocus();
    void clearInputFocus();
    void keyPress(QKeyEvent *event);
    void mouseEvent(QEvent *e);
    void mouseEventRelease(QGraphicsSceneMouseEvent *e);
    void mouseEventMove(QGraphicsSceneHoverEvent *e);

    int getTextWidth() const;

    void setSelection(int start, int length);
    void clearSelection();
    QTextLayout::FormatRange getSelection() const;

protected:
    void focusOutEvent(QFocusEvent *event);

private Q_SLOTS:
    void blinkCursor();

private:
    QFont m_font;

    QColor m_bgColor;
    QColor m_fgColor;
    QColor m_fontColor;
    QColor m_selectedTextColor;
    QColor m_selectionColor;

    QString m_text;

    QTextLayout m_textLayout;

    double m_hscroll;
    int m_cursorPos;
    bool m_cursorVisible;

    QTimer m_cursorTimer;

    bool m_mouseMoved;
    int m_selStart;
    int m_selLength;

    QVector<QTextLayout::FormatRange> m_selection;

    void layoutText();
};

#endif
