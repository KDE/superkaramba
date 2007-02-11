/***************************************************************************
 *   Copyright (C) 2003 by Ralph M. Churchill                              *
 *   mrchucho@yahoo.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "clickmap.h"
#include <qregexp.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <krun.h>

ClickMap::ClickMap(Karamba* k, int x, int y, int w, int h )
    :Meter(k, x, y, w, h )
{
/*
    if( h != 0 || w != 0)
        clip = 0;
    else
        clip = Qt::DontClip;
*/

    if( h == 0 || w == 0)
    {
        setWidth(-1);
        setHeight(-1);
    }
}

ClickMap::~ClickMap()
{
}

void ClickMap::setTextProps( TextField *t )
{
    text = *t;
}

void ClickMap::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
  int index = (int)((e->pos().y() - getY()) / text.getLineHeight()) + 1;
  if (index >= 1 && index <= (int)displays.count()) {
    // qDebug( "You clicked item " + QString::number( index ) + ", " +
   //  displays[index - 1] + " " + links[index - 1] );
   KRun::runCommand("konqueror " + links[index - 1]);
  }

  return;
}

void ClickMap::paint(QPainter *p, const QStyleOptionGraphicsItem *option,
                QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    int i = 0; //text.getLineHeight();
    int row = 1;

    p->setFont(text.getFont());
    p->setOpacity(m_opacity);

    QStringList::Iterator it = displays.begin();
    while( it != displays.end() && (row <= getHeight() || getHeight() == -1 )   )
    {
        p->setPen( text.getColor() );
        // p->drawText(x,y+i,width,height,  Qt::AlignCenter | Qt::ExpandTabs, *it);
        p->drawText(getX(), getY() + i + text.getLineHeight(), *it);
        i += text.getLineHeight();
        it++;
        row++;
    }
}

void ClickMap::setValue( QString v )
{
  QRegExp rx("^http://", Qt::CaseInsensitive);
    if ( rx.indexIn( v ) == -1 )
    {
        displays.append( v );
    }
    else
    {
        links.append( v );
    }
}

void ClickMap::setValue( int v )
{
    if ( v == 0 )
    {
        links.clear();
        displays.clear();
    }
}

#include "clickmap.moc"
