
#ifndef CLICKAREA_H
#define CLICKAREA_H

#include <qstring.h>
#include <qrect.h>
#include <qevent.h>
#include <qregexp.h>

#include <kprocess.h>
#include <kprocio.h>
#include <krun.h>

#include <meter.h>
#include "clickable.h"
/**
 *
 * Hans Karlsson
 **/
class ClickArea : public Meter
{
    Q_OBJECT
public:
    ClickArea(karamba* k, int x, int y, int w, int h );

    ~ClickArea();

    virtual bool click( QMouseEvent* );
    void setOnClick( QString );
    void setOnMiddleClick( QString );


    QRect getRectangle();

    void mUpdate( QPainter* );
    void setValue( int );
    void setValue( QString );

    QRect rect;
    QString onClick;
    QString onMiddleClick;
    QString value;
};

#endif
