/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson <karlsson.h@home.se>              *
 *   Copyright (C) 2007 Matt Broadstone <mbroadst@gmail.com>               *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "textlabel.h"
#include "textlabel.moc"

#include <QPainter>
#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>
#include <QCursor>

#include <KRun>

TextLabel::TextLabel(Karamba *k, int x, int y, int w, int h)
        :   Meter(k, x, y, w, h),
        alignment(Qt::AlignLeft),
        clip(0),
        bgColor(0, 0, 0),
        lineHeight(0),
        shadow(0),
        scrollSpeed(0, 0),
        scrollPos(0, 0),
        scrollGap(0),
        scrollPause(0),
        pauseCounter(0),
        scrollType(ScrollNone),
        m_clickable(false),
        m_sizeGiven(true)
{
    origPoint = QPoint(x, y);

    calculateTextSize();
    if (h != 0 || w != 0)
        clip = 0;
    else
        clip = Qt::TextDontClip;

    if (h <= 0 || w <= 0) {
        setWidth(-1);
        setHeight(-1);
        m_sizeGiven = false;
    }
}

TextLabel::TextLabel(Karamba *k)
        :   Meter(k, 0, 0, 0, 0),
        alignment(Qt::AlignLeft),
        clip(0),
        bgColor(0, 0, 0),
        lineHeight(0),
        shadow(0),
        scrollSpeed(0, 0),
        scrollPos(0, 0),
        scrollGap(0),
        scrollPause(0),
        pauseCounter(0),
        scrollType(ScrollNone),
        m_clickable(false)
{}

TextLabel::~TextLabel()
{}

void TextLabel::show()
{
    Meter::show();
    setEnabled(true);
}

void TextLabel::hide()
{
    Meter::hide();
    setEnabled(false);
}

void TextLabel::setTextProps(TextField *field)
{
    if (field) {
        text = *field;
        //lineHeight = t->getLineHeight();
        shadow = field->getShadow();
        alignment = field->getAlignment();
        setFontSize(field->getFontSize());
        setFont(field->getFont());

        setColor(field->getColor());
        setBGColor(field->getBGColor());
    }

    calculateTextSize();
}

void TextLabel::calculateTextSize()
{
    int tmp;

    prepareGeometryChange();

    QFontMetrics fm(font);
    lineHeight = fm.height();
    textSize.setWidth(0);
    textSize.setHeight(lineHeight * value.count());
    QStringList::Iterator it = value.begin();

    while (it != value.end()) {
        tmp = fm.width(*it);
        if (tmp > textSize.width())
            textSize.setWidth(tmp);

        ++it;
    }

    if ((getWidth() <= 0) || !m_sizeGiven) {
        setWidth(textSize.width());
    }

    if ((getHeight() <= 0) || !m_sizeGiven) {
        setHeight(textSize.height());
    }

    if (!m_sizeGiven) {
        if (alignment == Qt::AlignLeft) {
            setX(origPoint.x());
        }
        else if (alignment == Qt::AlignRight) {
            setX(origPoint.x() - textSize.width());
        }
        else if (alignment == Qt::AlignHCenter) {
            setX(origPoint.x() - textSize.width() / 2);
        }
    }

    update();
}

void TextLabel::setValue(const QString &text)
{
    value = text.split('\n');

    calculateTextSize();
}

void TextLabel::setValue(int v)
{
    value = QStringList(QString::number(v));

    calculateTextSize();
}

void TextLabel::setBGColor(QColor clr)
{
    bgColor = clr;
}

QColor TextLabel::getBGColor() const
{
    return bgColor;
}

void TextLabel::setFont(const QString &f)
{
    font.setFamily(f);
    calculateTextSize();
}

QString TextLabel::getFont() const
{
    return font.family();
}

void TextLabel::setFontSize(int size)
{
    font.setPixelSize(size);
    calculateTextSize();
}

int TextLabel::getFontSize() const
{
    return font.pixelSize();
}

void TextLabel::setAlignment(const QString &align)
{
    QString a = align.toUpper();
    if (a == "LEFT" || a.isEmpty())
        alignment = Qt::AlignLeft;
    if (a == "RIGHT")
        alignment = Qt::AlignRight;
    if (a == "CENTER")
        alignment = Qt::AlignHCenter;

    calculateTextSize();
}

QString TextLabel::getAlignment() const
{
    if (alignment == Qt::AlignHCenter)
        return "CENTER";
    else if (alignment == Qt::AlignRight)
        return "RIGHT";
    else
        return "LEFT";
}

void TextLabel::setFixedPitch(bool fp)
{
    font.setFixedPitch(fp);
}

bool TextLabel::getFixedPitch() const
{
    return font.fixedPitch();
}

void TextLabel::setShadow(int s)
{
    shadow = s;
}

int TextLabel::getShadow() const
{
    return shadow;
}

void TextLabel::setScroll(const QString &a, const QPoint &speed, int gap, int pause)
{
    ScrollType t = TextLabel::ScrollNone;
    QString scroll = a.toUpper();

    if (scroll == "NONE")
        scroll = TextLabel::ScrollNone;
    else if (a == "NORMAL")
        scroll = TextLabel::ScrollNormal;
    else if (a == "BACKANDFORTH")
        scroll = TextLabel::ScrollBackAndForth;
    else if (a == "ONEPASS")
        scroll = TextLabel::ScrollOnePass;

    setScroll(t, speed, gap, pause);
}

void TextLabel::setScroll(ScrollType type, QPoint speed, int gap, int pause)
{
    scrollType = type;
    scrollSpeed = speed;

    switch (scrollType) {
    case ScrollNormal:
    case ScrollOnePass: {
        int x = 0, y = 0;

        if (speed.x() > 0)
            x = -1 * textSize.width();
        else if (speed.x() < 0)
            x = getWidth() - 1;

        if (speed.y() > 0)
            x = -1 * textSize.height();
        else if (speed.y() < 0)
            x = getHeight() - 1;

        scrollPos = QPoint(x, y);
        break;
    }

    case ScrollNone:
    case ScrollBackAndForth:
    default:
        scrollPos = QPoint(0, 0);
        break;
    }

    scrollGap = gap;
    scrollPause = pause;
    pauseCounter = 1;
}

int TextLabel::drawText(QPainter *p, int x, int y, int width, int height,
                        const QString &text)
{
    if (shadow != 0) {
        p->setPen(getBGColor());
        p->drawText(x + shadow, y + shadow, width, height,
                    alignment | clip | Qt::TextExpandTabs, text);
    }

    p->setPen(getColor());
    p->drawText(x, y, width, height, alignment | clip |
                Qt::TextExpandTabs, text);

    return 0;
}

bool TextLabel::calculateScrollCoords(const QRect &meterRect, QRect &textRect,
                                      QPoint &next, int x, int y)
{
    if (scrollType == ScrollBackAndForth &&
            (scrollSpeed.x() != 0 && textSize.width() < getWidth() ||
             scrollSpeed.y() != 0 && textSize.height() < getHeight()))
        return true;

    x += scrollPos.x();
    y += scrollPos.y();

    if (pauseCounter < 1) {
        scrollPos += scrollSpeed;

        // -1 | 0 | 1
        QPoint direction(scrollSpeed.x() / abs((scrollSpeed.x() == 0) ?
                                               1 : scrollSpeed.x()),
                         scrollSpeed.y() / abs((scrollSpeed.y() == 0) ?
                                               1 : scrollSpeed.y()));
        next = QPoint(-1 * direction.x() * (scrollGap + textSize.width()),
                      -1 * direction.y() * (scrollGap + textSize.height()));
        textRect.setCoords(x, y, x + textSize.width(), y + textSize.height());

        if (scrollType == ScrollBackAndForth) {
            if (direction.x() < 0 && textRect.right() <= meterRect.right() ||
                    direction.x() > 0 && textRect.left() >= meterRect.left()) {
                scrollSpeed.setX(scrollSpeed.x() * -1);
                pauseCounter = scrollPause;
            }
            if (direction.y() < 0 && textRect.bottom() <= meterRect.bottom() ||
                    direction.y() > 0 && textRect.top() >= meterRect.top()) {
                scrollSpeed.setY(scrollSpeed.y() * -1);
                pauseCounter = scrollPause;
            }
        } else if (!textRect.intersects(meterRect)) {
            if (scrollType == ScrollNormal)
                scrollPos += next;
            else if (scrollType == ScrollOnePass)
                return false;
        }
    } else
        --pauseCounter;

    return true;
}

void TextLabel::paint(QPainter *p, const QStyleOptionGraphicsItem *option,
                      QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (!m_hidden) {
        int i = 0; // lineHeight;
        int row = 1;
        int width = Meter::getWidth();
        int height = Meter::getHeight();
        QRect meterRect(0, 0, width, height);
        QRect textRect;
        QPoint next;

        p->setFont(font);
        if (scrollType != ScrollNone) {
            p->setClipRect(0, 0, width, height);
            if (!calculateScrollCoords(meterRect, textRect, next, 0, 0)) {
                p->setClipping(false);
                return;
            }
            width = textSize.width();
            height = textSize.height();
        }

        QStringList::Iterator it = value.begin();
        while (it != value.end() && (row <= height || height == -1)) {
            drawText(p, 0, 0 + i, width, height, *it);

            // Draw more instances of text if scroll type is normal scroll
            if (scrollType == ScrollNormal) {
                textRect.adjust(next.x(), next.y(), next.x(), next.y());
                while (textRect.intersects(meterRect)) {
                    drawText(p, textRect.x(), textRect.y() + i, width, height, *it);
                    textRect.adjust(next.x(), next.y(), next.x(), next.y());
                }
            }

            i += lineHeight;
            it++;
            row++;
        }

        if (scrollType != ScrollNone)
            p->setClipping(false);
    }
}

bool TextLabel::mouseEvent(QEvent* e)
{
    if (isEnabled()) {
        Qt::MouseButtons button;
        if (QGraphicsSceneMouseEvent *event = dynamic_cast<QGraphicsSceneMouseEvent*>(e)) {
            button = event->button();
        } else if (QGraphicsSceneWheelEvent *event = dynamic_cast<QGraphicsSceneWheelEvent*>(e)) {
            button = event->buttons();
        }

        QString program;
        if (button == Qt::LeftButton)
            program = m_leftMouseButtonAction;
        else if (button == Qt::MidButton)
            program = m_middleMouseButtonAction;
        else if (button == Qt::RightButton)
            program = m_rightMouseButtonAction;

        if (!program.isEmpty())
            KRun::runCommand(program,0L);
        else
            return true;
    }

    return false;
}

void TextLabel::allowClick(bool enable)
{
    m_clickable = enable;
}

bool TextLabel::clickable()
{
    return m_clickable;
}

void TextLabel::attachClickArea(const QString &leftMouseButton,
                                const QString &middleMouseButton,
                                const QString &rightMouseButton)
{
    m_leftMouseButtonAction = leftMouseButton;
    m_middleMouseButtonAction = middleMouseButton;
    m_rightMouseButtonAction = rightMouseButton;

    QGraphicsItem::setCursor(QCursor(Qt::PointingHandCursor));
}

QRectF TextLabel::boundingRect() const
{
    return Meter::boundingRect();
}

int TextLabel::getTextWidth() const
{
    QFontMetrics fm(font);
    return fm.width(value[0]);
}

int TextLabel::getX() const
{
    return origPoint.x();
}

int TextLabel::getY() const
{
    return origPoint.y();
}

int TextLabel::getWidth() const
{
    if (!m_sizeGiven) {
        return -1;
    } else {
        return Meter::getWidth();
    }
}

int TextLabel::getHeight() const
{
    if (!m_sizeGiven) {
        return -1;
    } else {
        return Meter::getHeight();
    }
}

void TextLabel::setSize(int x, int y, int width, int height)
{
    origPoint = QPoint(x, y);

    if (height <= 0 || width <= 0) {
        m_sizeGiven = false;
    } else {
        m_sizeGiven = true;
    }

    Meter::setSize(x, y, width, height);

    calculateTextSize();
}

