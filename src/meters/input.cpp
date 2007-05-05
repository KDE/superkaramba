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

#include "input.h"
#include "input.moc"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>

#include "textfield.h"

Input::Input(Karamba* k, int x, int y, int w, int h)
        :   Meter(k, x, y, w, h),
        m_hscroll(0),
        m_cursorPos(0),
        m_cursorVisible(true)
{
    setFlags(QGraphicsItem::ItemIsFocusable);

    connect(&m_cursorTimer, SIGNAL(timeout()), (QObject*)this, SLOT(blinkCursor()));
    m_cursorTimer.start(1000);
}

Input::~Input()
{}

/*
  Some code in this method is copied from QLineEdit.
  Copyright (C) 1992-2006 Trolltech ASA. All rights reserved.
*/
void Input::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                  QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setOpacity(m_opacity);

    painter->setPen(m_fgColor);
    QBrush oldBrush = painter->brush();
    painter->setBrush(m_bgColor);
    painter->drawRect(boundingRect());
    painter->setBrush(oldBrush);

    QTextLine line = m_textLayout.lineAt(0);

    int widthUsed = qRound(line.naturalTextWidth()) + 1 + 4;

    QFontMetrics fm(m_font);
    QRectF innerRect(boundingRect().x() + 2, boundingRect().y(),
                     boundingRect().width() - 4, fm.height());
    painter->setClipRect(innerRect);

    QPointF topLeft = innerRect.topLeft();

    double curPos = line.cursorToX(m_cursorPos);
    if (4 + widthUsed <= innerRect.width()) {
        m_hscroll = 0;
    } else if (curPos - m_hscroll >= innerRect.width()) {
        m_hscroll = curPos - innerRect.width() + 1;
    } else if (curPos - m_hscroll < 0) {
        m_hscroll = curPos;
    } else if (widthUsed - m_hscroll < innerRect.width()) {
        m_hscroll = widthUsed - innerRect.width() + 1;
    }
    topLeft.rx() -= m_hscroll;
    topLeft.ry() += (boundingRect().height() - line.height()) / 2;

    painter->setPen(m_fontColor);
    m_textLayout.draw(painter, topLeft, QVector<QTextLayout::FormatRange>(),
                      innerRect);

    if (hasFocus() && m_cursorVisible)
        m_textLayout.drawCursor(painter, topLeft, m_cursorPos);
}

void Input::mouseEvent(QEvent *e)
{
    QPointF pos;
    if (QGraphicsSceneMouseEvent *event = dynamic_cast<QGraphicsSceneMouseEvent*>(e)) {
        pos = event->pos();
    } else if (QGraphicsSceneWheelEvent *event = dynamic_cast<QGraphicsSceneWheelEvent*>(e)) {
        pos = event->pos();
    }

    QTextLine line = m_textLayout.lineAt(0);

    QPoint mappedPos = mapFromParent(pos).toPoint();
    m_cursorPos = line.xToCursor(mappedPos.x() - 5 + m_hscroll);
    m_cursorVisible = true;

    update();
}

void Input::mouseDropEvent(QGraphicsSceneDragDropEvent *event)
{
    Q_UNUSED(event);
    //kDebug() << "Input::mouseDropEvent()" << endl;
}

void Input::focusOutEvent(QFocusEvent *event)
{
    Q_UNUSED(event);

    m_cursorTimer.stop();
    update();
}

void Input::blinkCursor()
{
    m_cursorVisible = !m_cursorVisible;
    update();
}

void Input::keyPress(QKeyEvent *event)
{
    bool append = true;

    switch (event->key()) {
    case Qt::Key_Backspace:
        if (m_cursorPos > 0) {
            m_text.remove(m_cursorPos - 1, 1);
            m_cursorPos--;
        }
        append = false;
        break;

    case Qt::Key_Delete:
        m_text.remove(m_cursorPos, 1);
        append = false;
        break;

    case Qt::Key_Left:
        m_cursorPos--;
        append = false;
        break;

    case Qt::Key_Right:
        m_cursorPos++;
        append = false;
        break;

    case Qt::Key_Home:
        m_cursorPos = 0;
        append = false;
        break;

    case Qt::Key_End:
        m_cursorPos = m_text.length();
        append = false;
        break;

    case Qt::Key_Enter:
    case Qt::Key_Return:
        append = false;
        break;
    }

    if (append) {
        m_text.insert(m_cursorPos, event->text());
        m_cursorPos += event->text().length();
    }

    if (m_cursorPos < 0)
        m_cursorPos = 0;
    if (m_cursorPos > m_text.length())
        m_cursorPos = m_text.length();

    m_cursorVisible = true;

    layoutText();
}


void Input::setValue(const QString &text)
{
    m_text = text;

    layoutText();
}

QString Input::getStringValue() const
{
    return m_text;
}

void Input::setBGColor(QColor c)
{
    m_bgColor = c;
    update();
}

void Input::setColor(QColor c)
{
    m_fgColor = c;
    update();
}

QColor Input::getBGColor() const
{
    return m_bgColor;
}

QColor Input::getColor() const
{
    return m_fgColor;
}

void Input::hide()
{
    Meter::hide();
}

void Input::show()
{
    Meter::show();
}

void Input::setSize(int ix, int iy, int iw, int ih)
{
    Meter::setSize(ix, iy, iw, ih);
}

void Input::setX(int ix)
{
    Meter::setX(ix);
}

void Input::setY(int iy)
{
    Meter::setY(iy);
}

void Input::setWidth(int iw)
{
    Meter::setWidth(iw);
}

void Input::setHeight(int ih)
{
    Meter::setHeight(ih);
}

void Input::setFont(const QString &f)
{
    m_font.setFamily(f);

    layoutText();
}

QString Input::getFont() const
{
    return m_font.family();
}

void Input::setFontColor(QColor fontColor)
{
    m_fontColor = fontColor;
    update();
}

QColor Input::getFontColor() const
{
    return m_fontColor;
}

void Input::setSelectionColor(QColor selectionColor)
{
    m_selectionColor = selectionColor;
    update();
}

QColor Input::getSelectionColor() const
{
    return m_selectionColor;
}

void Input::setSelectedTextColor(QColor selectedTextColor)
{
    m_selectedTextColor = selectedTextColor;
}

QColor Input::getSelectedTextColor() const
{
    return m_selectedTextColor;
}

void Input::setFontSize(int size)
{
    m_font.setPixelSize(size);

    layoutText();
}

int Input::getFontSize() const
{
    return m_font.pixelSize();
}

void Input::setTextProps(TextField* t)
{
    if (t) {
        setFontSize(t->getFontSize());
        setFont(t->getFont());
        setColor(t->getColor());
        setBGColor(t->getBGColor());
    }

    layoutText();
}

void Input::setInputFocus()
{
    setFocus();
    update();
}

void Input::clearInputFocus()
{
    clearFocus();
    update();
}

void Input::layoutText()
{
    m_textLayout.setText(m_text);
    m_textLayout.setFont(m_font);

    m_textLayout.beginLayout();
    QTextLine line = m_textLayout.createLine();
    line.setPosition(QPointF(0, 0));
    m_textLayout.endLayout();

    update();
}
