/***************************************************************************
 *   Copyright (C) 2003 by Ralph M. Churchill                              *
 *   mrchucho@yahoo.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CLICKABLE_H
#define CLICKABLE_H

#include <qstring.h>
#include <qrect.h>
#include <qevent.h>


/**
 *
 * Ralph M. Churchill
 **/
class Clickable
{
public:
    Clickable(int x, int y, int w, int h );

    virtual ~Clickable();

    virtual void click( QMouseEvent* ) = 0;
    /*
    void setOnClick( QString );
    void setOnMiddleClick( QString );
    */

    virtual QRect getBoundingBox();

protected:
    QRect boundingBox;
    /*
    QString onClick;
    QString onMiddleClick;
    */
};

#endif
