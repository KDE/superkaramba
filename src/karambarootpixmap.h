/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef KARAMBAROOTPIXMAP_H
#define KARAMBAROOTPIXMAP_H

#include <qwidget.h>
#include <krootpixmap.h>
#include <ksharedpixmap.h>

//#include "karamba.h"

/**
@author Hans Karlsson
*/
class KarambaRootPixmap : public KRootPixmap
{
public:
    //KarambaRootPixmap();
    KarambaRootPixmap( QWidget *);

    ~KarambaRootPixmap();


void  updateBackground ( KSharedPixmap * );

private:
QWidget *widget;


};

#endif
