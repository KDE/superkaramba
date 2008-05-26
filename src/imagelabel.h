/****************************************************************************
*  imagelabel.h  -  ImageLabel meter
*
*  Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>
*  Copyright (c) 2004 Petri Damstén <damu@iki.fi>
*
*  This file is part of SuperKaramba.
*
*  SuperKaramba is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  SuperKaramba is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with SuperKaramba; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
****************************************************************************/

#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include "meter.h"
#include <kpixmap.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qstring.h>
#include <qstringlist.h>
#include <kurl.h>
#include <kio/netaccess.h>
#include <qregexp.h>
#include <qtimer.h>
#include "karamba.h"

class ImageLabel;
class KIO::CopyJob;

// Abstract Effects Baseclass
class Effect : public QObject
{

Q_OBJECT

public:
  Effect(ImageLabel*, int millisec);

  virtual ~Effect();

  virtual KPixmap apply(KPixmap pixmap) = 0;

  void startTimer();

protected:
  ImageLabel* myImage;

  int millisec;
};

// Intensity
class Intensity : public Effect
{
public:
  Intensity(ImageLabel*, float r, int millisec);

  KPixmap apply(KPixmap pixmap);

private:
  float ratio;
};


// ChannelIntensity
class ChannelIntensity : public Effect
{
public:
  ChannelIntensity(ImageLabel*, float r, QString c, int millisec);

  KPixmap apply(KPixmap pixmap);

private:
  float ratio;
  int channel;
};

// ToGray
class ToGray : public Effect
{
public:
  ToGray(ImageLabel*, int millisec);

  KPixmap apply(KPixmap pixmap);
};

class ImageLabel : public Meter
{

Q_OBJECT

public:
  ImageLabel(karamba* k, int ix,int iy,int iw,int ih );
  ImageLabel(karamba* k);
  ~ImageLabel();
  void setValue( QString imagePath );

  void setValue( long );
  void setValue( QPixmap& );
  QString getStringValue() { return imagePath; };
  void scale( int, int );
  void smoothScale( int, int );

  void rotate(int);
  void removeImageTransformations();
  void mUpdate( QPainter * );
  void mUpdate( QPainter *, int );
  
  void rolloverImage(QMouseEvent *e);
  void parseImages( QString fn, QString fn_roll, int, int, int, int );
  virtual void show();
  virtual void hide();

  void setTooltip(QString txt);
  int cblend;
  int background;
  // Pixmap Effects
  void removeEffects();
  void intensity(float ratio, int millisec);
  void channelIntensity(float ratio, QString channel, int millisec);
  void toGray(int millisec);
  void setBackground(int b);

  void attachClickArea(QString leftMouseButton, QString middleMouseButton,
                       QString rightMouseButton);

  virtual bool click(QMouseEvent*);

private slots:

  // gets called if a timed effects needs to bee removed
  void slotEffectExpired();
  void slotCopyResult(KIO::Job* job);

signals:
  void pixmapLoaded();

private:
  void applyTransformations(bool useSmoothScale = false);
  int pixmapWidth;
  int pixmapHeight;
  int pixmapOffWidth;
  int pixmapOffHeight;
  int pixmapOnWidth;
  int pixmapOnHeight;

  // true if Image has been scaled
  bool doScale;
  // true if Image has been rotated
  bool doRotate;

  // Contains the current Effect or is 0 if no Effect is applied
  Effect* imageEffect;

  // Scale Matrix
  //QWMatrix scaleMat;
  int scale_w;
  int scale_h;
  // Rotation Matrix
  //QWMatrix rotMat;
  int rot_angle;

  KPixmap pixmap;
  KPixmap realpixmap;

  QRect rect_off, rect_on;
  QRect old_tip_rect;

  bool zoomed;
  //QString fn, fn_roll;
  bool rollover;
  KPixmap pixmap_off;
  KPixmap pixmap_on;
  int xoff,xon;
  int yoff,yon;
  QString imagePath;
};

#endif // IMAGELABEL_H
