/***************************************************************************
 *   Copyright (C) 2003 by Ralph M. Churchill                              *
 *   mrchucho@yahoo.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CLICKMAP_H
#define CLICKMAP_H

#include <meter.h>
#include "clickable.h"
#include "textfield.h"


/**
 *
 * Ralph M. Churchill
 **/
class ClickMap : public Meter
{
Q_OBJECT
public:
    ClickMap(karamba* k, int x, int y, int w, int h);
    ~ClickMap();

    virtual bool click( QMouseEvent* );
    void mUpdate( QPainter* );
    void setValue( QString );
    void setValue( long );
    void setTextProps( TextField * );

private:
    TextField text;

    QStringList links;
    QStringList displays;
};

#endif
