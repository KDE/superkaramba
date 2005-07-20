/***************************************************************************
 *   Copyright (C) 2003 by Ralph M. Churchill                              *
 *   mrchucho@yahoo.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "textfield.h"
#include <qfontmetrics.h>
#include <kdebug.h>

TextField::TextField( )
{
  setFontSize(12);
  setColor(QColor(192, 192, 192));
  setBGColor(QColor(0, 0, 0));
  setFont("Helvetica");
  setAlignment(Qt::AlignLeft);
  setFixedPitch(false);
  setShadow(0);
}

TextField::~TextField()
{
}

TextField::TextField( const TextField& def )
{
    setFontSize( def.getFontSize() );

    setColor(def.getColor());
    setBGColor(def.getBGColor());

    setFont( def.getFont() );
    setAlignment( def.getAlignment() );
    setFixedPitch( def.getFixedPitch() );
    setShadow( def.getShadow() );
}

TextField& TextField::operator=(const TextField& rhs)
{
    if( this == &rhs)
        return *this;

    setFontSize( rhs.getFontSize() );

    setColor(rhs.getColor());
    setBGColor(rhs.getBGColor());

    setFont( rhs.getFont() );
    setAlignment( rhs.getAlignment() );
    setFixedPitch( rhs.getFixedPitch() );
    setShadow( rhs.getShadow() );

    return *this;
}

void TextField::setColor(QColor clr)
{
  color = clr;
}

QColor TextField::getColor() const
{
    return color;
}

void TextField::setBGColor(QColor clr)
{
    bgColor = clr;
}

QColor TextField::getBGColor() const
{
    return bgColor;
}


void TextField::setFont(const QString &f)
{
    font.setFamily(f);
    lineHeight = QFontMetrics(font).height();
}


QString TextField::getFont() const
{
    return font.family();
}

void TextField::setFontSize(int size)
{
    font.setPointSize(size);
    lineHeight = QFontMetrics(font).height();
}

int TextField::getFontSize() const
{
    return font.pointSize();
}

void TextField::setAlignment( const QString &align )
{
    QString a = align.upper();
    if( a == "LEFT" || a.isEmpty() )
        alignment = Qt::AlignLeft;
    if( a == "RIGHT" )
        alignment = Qt::AlignRight;
    if( a == "CENTER" )
        alignment = Qt::AlignHCenter;
}

void TextField::setAlignment( int af )
{
    alignment = af;
}

int TextField::getAlignment() const
{
    return alignment;
}

QString TextField::getAlignmentAsString() const
{
    if( alignment == Qt::AlignHCenter )
        return "CENTER";
    else if( alignment == Qt::AlignRight )
        return "RIGHT";
    else
        return "LEFT";
}

void TextField::setFixedPitch( bool fp)
{
    font.setFixedPitch( fp );
}

bool TextField::getFixedPitch() const
{
    return font.fixedPitch();
}

void TextField::setShadow ( int s )
{
    shadow = s;
}

int TextField::getShadow() const
{
    return shadow;
}

int TextField::getLineHeight() const
{
    return lineHeight;
}
