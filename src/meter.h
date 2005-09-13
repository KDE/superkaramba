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
#include <QMouseEvent>

#include <qfileinfo.h>

class KarambaWidget;
class Sensor;

class Meter : public QObject
{
Q_OBJECT
public:

  Meter(KarambaWidget* k, int ix,int iy,int iw,int ih);
  Meter(KarambaWidget* k);
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

  virtual void mUpdate(QPainter *)=0 ;

    // TODO Review this methods and change python ifaces //
  virtual void setMax(int max) {};
  virtual void setMin(int min) {};
  virtual int getMax() { return 0; };
  virtual int getMin() { return 0; };
  virtual void setValue(int) {};
  virtual int getValue() { return -1; };
  virtual void setValue(QString) {};
  virtual QString getStringValue() const { return QString::null; };
  virtual void recalculateValue() {};

  virtual void setColor(QColor clr) {};
  virtual QColor getColor() { return Qt::black; };
   ////////////////////////////////////////////////////////
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

    void acceptSensor(Sensor*);
    QString getFormat() { return m_format; }
    void setFormat(QString f) { m_format = f; }
    Sensor* getSensor() { return m_sensor; }
  /*
  void setOnClick( QString );
  void setOnMiddleClick( QString );
  */
public slots:
  void update(QVariant value);

protected: // Protected attributes
  QRect boundingBox;

  // Actions to execute when clicked on meter
  QString leftButtonAction;
  QString middleButtonAction;
  QString rightButtonAction;

  bool clickable;
  int hidden;

    QString m_format;
    Sensor* m_sensor;
  KarambaWidget* m_karamba;
};

#endif // METER_H
