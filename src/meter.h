/***************************************************************************
 *   Copyright (C) 2003 by Hans Karlsson                                   *
 *   karlsson.h@home.se                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef METER_H
#define METER_H

#include <qpixmap.h>
#include <qpainter.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qobject.h>

#include <qfileinfo.h>

class karamba;

class Meter : public QObject
{
Q_OBJECT
public:

  Meter(karamba* k, int ix,int iy,int iw,int ih);
  Meter(karamba* k);
  virtual ~Meter();
  virtual int getX();
  virtual int getY();
  virtual int getWidth();
  virtual int getHeight();
  virtual void setX(int);
  virtual void setY(int);
  virtual void setWidth(int);
  virtual void setHeight(int);

  virtual void setSize(int ix, int iy, int iw, int ih);

  virtual void setMax(long max) { maxValue = max; };
  virtual void setMin(long min) { minValue = min; };
  virtual long getMax() { return minValue; };
  virtual long getMin() { return maxValue; };

  void setThemePath( QString );

  virtual void mUpdate(QPainter *)=0 ;

  virtual void setValue(long) {};
  virtual long getValue() { return -1; };
  virtual void setValue(QString) {};
  virtual QString getStringValue() const { return QString::null; };
  virtual void recalculateValue() {};

  virtual void setColor(QColor clr) { color = clr; };
  virtual QColor getColor() { return color; };

  virtual void show() { hidden = 0; };
  virtual void hide() { hidden = 1; };

  QRect getBoundingBox();

  // true when given coordinate point is inside the meters
  // active reagion and meter is enabled
  virtual bool insideActiveArea(int, int);

  // returns true when callback meterClicked should be called.
  virtual bool click( QMouseEvent* );

  void setEnabled(bool);
  bool isEnabled();

  /*
  void setOnClick( QString );
  void setOnMiddleClick( QString );
  */

protected: // Protected attributes
  QString themePath;

  QRect boundingBox;

  // Actions to execute when clicked on meter
  QString leftButtonAction;
  QString middleButtonAction;
  QString rightButtonAction;

  bool clickable;
  int hidden;
  long minValue;
  long maxValue;

  QColor color;
  karamba* m_karamba;
};

#endif // METER_H
