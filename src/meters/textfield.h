/***************************************************************************
 *   Copyright (C) 2003 by Ralph M. Churchill                              *
 *   mrchucho@yahoo.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef TEXTFIELD_H
#define TEXTFIELD_H
#include <qstring.h>
#include <qcolor.h>
#include <qfont.h>

/**
 *
 * Ralph M. Churchill
 **/
class TextField
{
public:
    TextField();
    TextField( const TextField& );
    ~TextField();

    TextField& operator=(const TextField& );

    void setFontSize( int );
    void setColor(QColor clr);
    void setBGColor(QColor clr);
    void setFont( const QString& );
    void setAlignment( int );
    void setAlignment( const QString& );
    void setFixedPitch( bool );
    void setShadow( int );

    int getFontSize() const;
    QColor getColor() const;
    QColor getBGColor() const;
    QString getFont() const;
    int getAlignment() const;
    QString getAlignmentAsString() const;
    bool getFixedPitch() const;
    int getShadow() const;
    int getLineHeight() const;

protected:
    int alignment;
    QFont font;
    QColor color;
    QColor bgColor;
    int shadow;
    int lineHeight;

}
;
#endif // TEXTFIELD_H
