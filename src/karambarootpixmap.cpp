/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "karambarootpixmap.h"
#include "karambawidget.h"

KarambaRootPixmap::KarambaRootPixmap(QWidget *w) :
        KRootPixmap( w)
{
    widget = w;
}


KarambaRootPixmap::~KarambaRootPixmap()
{}

void  KarambaRootPixmap::updateBackground (KSharedPixmap *kpm)
{
   QPalette pal=widget->palette();
   pal.setBrush(widget->backgroundRole(), QBrush(*kpm));
   widget->setPalette(pal);
}

