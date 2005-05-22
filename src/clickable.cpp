/***************************************************************************
 *   Copyright (C) 2003 by Ralph M. Churchill                              *
 *   mrchucho@yahoo.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "clickable.h"


Clickable::Clickable( int x, int y, int w, int h )
{
    boundingBox = QRect( x, y, w, h );
}

Clickable::~Clickable()
{}

/*
void Clickable::setOnClick( QString oc )
{
    onClick = oc;
}

void Clickable::setOnMiddleClick( QString oc )
{
    onMiddleClick = oc;
}
*/

QRect Clickable::getBoundingBox()
{
    return boundingBox;
}
