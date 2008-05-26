/****************************************************************************
*  imagelabel.cpp  -  ImageLabel meter
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

#include <qpixmap.h>
#include <qtimer.h>
#include <qtooltip.h>
#include <kpixmapeffect.h>
#include <kdebug.h>
#include <kimageeffect.h>
#include <ktempfile.h>
#include <kio/job.h>
#include "karambaapp.h"
#include "imagelabel.h"

// Effect
Effect::Effect(ImageLabel* img, int msec) :
  myImage(img)
{
  if (msec > 0)
  {
    // remove the effect after the given time
    //QTimer::singleShot (millisec, myImage, SLOT(slotEffectExpired()));
    //timer -> changeInterval(millisec);
    millisec = msec;
  }
  else
  {
    millisec = msec;
  }
}

Effect::~Effect()
{
}

void Effect::startTimer()
{
  if (millisec > 0)
  {
    QTimer::singleShot (millisec, myImage, SLOT(slotEffectExpired()));
    millisec = 0;
  }
}

// Intensity
Intensity::Intensity(ImageLabel* img, float r, int millisec) :
  Effect(img, millisec)
{
  ratio = r;
  ratio = (ratio > 1)  ? 1 : ratio;
  ratio = (ratio < -1) ? -1 : ratio;
}

KPixmap Intensity::apply(KPixmap pixmap)
{
  return KPixmapEffect::intensity(pixmap, ratio);
}

// ChannelIntensity
ChannelIntensity::ChannelIntensity(ImageLabel* img, float r, QString c,
                                   int millisec) :
  Effect(img, millisec)
{
  ratio = r;
  ratio = (ratio > 1)  ? 1 : ratio;
  ratio = (ratio < -1) ? -1 : ratio;

  channel = 0;
  if (c.find("red", 0 , false))
  {
    channel = 0;
  }
  else if (c.find("green", 0, false))
  {
    channel = 1;
  }
  else if (c.find("blue", 0, false))
  {
    channel = 2;
  }
}

KPixmap ChannelIntensity::apply(KPixmap pixmap)
{
  return KPixmapEffect::channelIntensity(pixmap, ratio,
    (KPixmapEffect::RGBComponent)channel);
}

// ToGray
ToGray::ToGray(ImageLabel* img, int millisec) : Effect(img, millisec)
{
}

KPixmap ToGray::apply(KPixmap pixmap)
{
  return KPixmapEffect::toGray(pixmap);
}

/***********************************************************************/

ImageLabel::ImageLabel(karamba* k, int ix,int iy,int iw,int ih) :
  Meter(k, ix,iy,iw,ih), zoomed(false), rollover(false)
{
   background = 0;
  cblend = 0;
  //scaleMat.reset();
  //rotMat.reset();
  scale_w = 1;
  scale_h = 1;
  rot_angle = 0;

  doScale = false;
  doRotate = false;

  imageEffect = 0;
}

ImageLabel::ImageLabel(karamba* k) :
  Meter(k), zoomed(false), rollover(false)
{
  cblend = 0;
  background = 0;
}

ImageLabel::~ImageLabel()
{
  if (imageEffect != 0)
  {
    delete imageEffect;
    imageEffect = 0;
  }
  if(!old_tip_rect.isNull())
  {
    QToolTip::remove(m_karamba, old_tip_rect);
  }
}

void ImageLabel::setValue(long v)
{
 setValue( QString::number( v ) );
}

void ImageLabel::show()
{
  Meter::show();
  setEnabled(true);
}

void ImageLabel::hide()
{
  Meter::hide();
  setEnabled(false);
}

void ImageLabel::rotate(int deg)
{
  doRotate = !(deg == 0);

  rot_angle = deg;

  applyTransformations();
}

void ImageLabel::scale(int w, int h)
{
  doScale = !(w == realpixmap.width() && h == realpixmap.height());

  scale_w = w;
  scale_h = h;

  applyTransformations();
}

void ImageLabel::smoothScale(int w, int h)
{
    doScale = !(w == realpixmap.width() && h == realpixmap.height());

    scale_w = w;
    scale_h = h;

    applyTransformations(true);

//  double widthFactor = ((double)w) / ((double)realpixmap.width());
//  double heightFactor = ((double)h) / ((double)realpixmap.height());

//  pixmap.convertFromImage(realpixmap.convertToImage().smoothScale(w, h));

//  setWidth(pixmap.width());
//  setHeight(pixmap.height());

}

void ImageLabel::removeImageTransformations()
{
    doScale = false;
    doRotate = false;

    scale_w = 1;
    scale_h = 1;
    rot_angle = 0;
    pixmap = realpixmap;
}

void ImageLabel::applyTransformations(bool useSmoothScale)
{
    pixmap = realpixmap;
    if (doRotate)
    {
        // KDE and QT seem to miss a high quality image rotation
        QWMatrix rotMat;
        rotMat.rotate(rot_angle);
        pixmap = pixmap.xForm(rotMat);
    }
    if (doScale)
    {
        if (m_karamba -> useSmoothTransforms() || useSmoothScale)
        {
            pixmap.convertFromImage(
              pixmap.convertToImage().smoothScale(scale_w, scale_h));
        }
        else
        {
            double widthFactor = ((double)scale_w) / ((double)pixmap.width());
            double heightFactor = ((double)scale_h) / ((double)pixmap.height());
            QWMatrix scaleMat;
            scaleMat.scale(widthFactor, heightFactor);
            pixmap = pixmap.xForm(scaleMat);
        }
    }
    if (imageEffect != 0)
    {
        pixmap = imageEffect -> apply(pixmap);
    }
    setWidth(pixmap.width());
    setHeight(pixmap.height());
}

void ImageLabel::slotCopyResult(KIO::Job* job)
{
  QString tempFile = ((KIO::FileCopyJob*)job)->destURL().path();
  if(job->error() == 0)
  {
    setValue(tempFile);
    imagePath = ((KIO::FileCopyJob*)job)->srcURL().path();
    emit pixmapLoaded();
  }
  else
  {
    qWarning("Error downloading (%s): %s", job->errorText().ascii(),
                                           tempFile.ascii());
  }
  KIO::NetAccess::removeTempFile(tempFile);
}

void ImageLabel::setValue(QString fn)
{
  // use the first line
  QStringList sList = QStringList::split( "\n", fn );
  QString fileName = *sList.begin();
  KURL url(fileName);
  QRegExp rx("^[a-zA-Z]{1,5}:/",false);
  bool protocol = (rx.search(fileName)!=-1)?true:false;
  QPixmap pm;

  if(protocol && url.isLocalFile() == false)
  {
    KTempFile tmpFile;
    KIO::FileCopyJob* copy = KIO::file_copy(fileName, tmpFile.name(), 0600,
                                            true, false, false);
    connect(copy, SIGNAL(result(KIO::Job*)),
            this, SLOT(slotCopyResult(KIO::Job*)));
    return;
  }
  else
  {
    if(m_karamba->theme().isThemeFile(fileName))
    {
      QByteArray ba = m_karamba->theme().readThemeFile(fileName);
      pm.loadFromData(ba);
    }
    else
    {
      pm.load(fileName);
    }
    imagePath = fileName;
  }
  setValue(pm);
}

//Matthew Kay: a new version of setValue to be used by createTaskIcon()
/**
 * set the internal pixmap of this image to the given QPixmap pix
 */
void ImageLabel::setValue(QPixmap& pix)
{
  realpixmap = KPixmap(pix);
  pixmap = realpixmap;
  setWidth(pixmap.width());
  setHeight(pixmap.height());

  pixmapWidth = pixmap.width();
  pixmapHeight = pixmap.height();
  rect_off = QRect(getX(),getY(),pixmapWidth,pixmapHeight);
}

void ImageLabel::mUpdate(QPainter* p, int backgroundUpdate)
{
  if (backgroundUpdate == 1)
  {
    //only draw image if not hidden
    if (hidden == 0)
    {
      if (cblend == 0)
        //draw the pixmap
        p->drawPixmap(getX(),getY(),pixmap);
      else
      {
        //Blend this image with a color

        QImage image = pixmap.convertToImage();

        QImage result = KImageEffect::blend(QColor(255,0,0), image, 0.5f);
        p->drawImage(getX(),getY(),result);

        //p->drawRect(boundingBox);
      }
    }
    // start Timer
    if (imageEffect != 0)
    {
      imageEffect -> startTimer();
    }
  }
}

void ImageLabel::mUpdate(QPainter* p)
{
  //only draw image if not hidden
  if (hidden == 0 && background == 0)
  {
    if (cblend == 0)
    {
      //draw the pixmap
      p->drawPixmap(getX(),getY(),pixmap);
    }
    else
    {
      //Blend this image with a color

      QImage image = pixmap.convertToImage();

      QImage result = KImageEffect::blend(QColor(255,0,0), image, 0.5f);
      p->drawImage(getX(),getY(),result);

      //p->drawRect(boundingBox);
    }
  }
  // start Timer
  if (imageEffect != 0)
  {
    imageEffect -> startTimer();
  }
}

bool ImageLabel::click(QMouseEvent* e)
{
    if (getBoundingBox().contains(e -> x(), e -> y()) && isEnabled())
    {
        QString program;
        if (e -> button() == Qt::LeftButton)
        {
            program = leftButtonAction;
        }
        else if (e -> button() == Qt::MidButton)
        {
            program = middleButtonAction;
        }
        else if (e -> button() == Qt::RightButton)
        {
            program = rightButtonAction;
        }

        if( !program.isEmpty() )
        {
            KRun::runCommand(program);
        }
        else
        {
          return true;
        }
    }
    return false;
}

void ImageLabel::parseImages(QString fn, QString fn_roll, int _xoff,
                             int _yoff, int _xon, int _yon)
{
  //fn = filename;
  //fn_roll = filename_roll;

  xoff = _xoff;
  yoff = _yoff;
  xon = _xon;
  yon = _yon;

  // use the first line
  QStringList sList = QStringList::split( "\n", fn );
  QString fileName = *sList.begin();
  QFileInfo fileInfo( fileName );
  QString path;

  QRegExp rx("^http://",false);
  bool fileOnNet = (rx.search(fileName)!=-1)?true:false;


  if( fileInfo.isRelative() && !fileOnNet )
  {
    path = m_karamba->theme().path() + "/" + fileName;
  }
  else
  {
    path = fileName;
  }

  if ( fileOnNet )
  {
    QString tmpFile;
#if defined(KDE_3_2)
    if(KIO::NetAccess::download(KURL(path), tmpFile, karambaApp->parentWindow()))
#else
    if(KIO::NetAccess::download(KURL(path), tmpFile))
#endif
    {
      pixmap_off = KPixmap(tmpFile);
      KIO::NetAccess::removeTempFile(tmpFile);
      qDebug( "Downloaded: %s to %s", path.ascii(), tmpFile.ascii() );
    }
    else
    {
      qDebug( "Error Downloading: %s", path.ascii());
    }
  }
  else
  {
    pixmap_off = KPixmap( path );
  }

  pixmapOffWidth = pixmap.width();
  pixmapOffHeight = pixmap.height();

  rect_off = QRect(xoff,yoff,pixmapWidth,pixmapHeight);
/////////////////////////////
  if (fn_roll.isEmpty())
    return;

  rollover=true;
  sList = QStringList::split( "\n", fn_roll );
  fileName = *sList.begin();
  fileInfo = QFileInfo( fileName );

  fileOnNet = (rx.search(fileName)!=-1)?true:false;


  if( fileInfo.isRelative() && !fileOnNet )
  {
    path = m_karamba->theme().path() + "/" + fileName;
  }
  else
  {
    path = fileName;
  }

  if ( fileOnNet )
  {
    QString tmpFile;
#if defined(KDE_3_2)
    if(KIO::NetAccess::download(KURL(path), tmpFile, karambaApp->parentWindow()))
#else
    if(KIO::NetAccess::download(KURL(path), tmpFile))
#endif
    {
      pixmap_on = KPixmap(tmpFile);
      KIO::NetAccess::removeTempFile(tmpFile);
      qDebug( "Downloaded: %s to %s", path.ascii(), tmpFile.ascii());
    }
    else
    {
      qDebug( "Error Downloading: %s", path.ascii());
    }
  }
  else
  {
    pixmap_on = KPixmap( path );
  }
  pixmapOnWidth = pixmap_on.width();
  pixmapOnHeight = pixmap_on.height();

  rect_on = QRect(xon,yon,pixmapOnWidth,pixmapOnHeight);
}

void ImageLabel::setBackground(int b)
{
  background = b;
}

void ImageLabel::rolloverImage(QMouseEvent *e)
{
  if (!rollover)
    return;

  if (zoomed)
  {
    if (!rect_off.contains(e->pos()))
    {
      // rollover the image to the zoomed image
      //setValue(fn_roll);
      setX(xoff);
      setY(yoff);
      pixmap = pixmap_off;
      pixmapWidth = pixmapOffWidth;
      pixmapHeight = pixmapOffHeight;
      zoomed = false;
      m_karamba->step();
    }
  }
  else
  {
    if (rect_off.contains(e->pos()))
    {
      // rollover the image to the zoomed image
      //setValue(fn_roll);
      setX(xon);
      setY(yon);
      pixmap = pixmap_on;
      pixmapWidth = pixmapOnWidth;
      pixmapHeight = pixmapOnHeight;
      zoomed = true;
      m_karamba->step();
    }
  }
}

void ImageLabel::setTooltip(QString txt)
{
  QRect rect(getX(),getY(),pixmapWidth,pixmapHeight);
  QToolTip::add(m_karamba, rect, txt);
  old_tip_rect = QRect(rect.topLeft(), rect.bottomRight());
}


void ImageLabel::removeEffects()
{
  if (imageEffect != 0)
  {
    delete imageEffect;
    imageEffect = 0;
  }
  applyTransformations();
}

void ImageLabel::intensity(float ratio, int millisec)
{
  if (imageEffect != 0)
  {
    delete imageEffect;
    imageEffect = 0;
  }
  //KPixmapEffect::intensity(pixmap, ratio);
  imageEffect = new Intensity(this, ratio, millisec);
  applyTransformations();
}

void ImageLabel::channelIntensity(float ratio, QString channel, int millisec)
{
  if (imageEffect != 0)
  {
    delete imageEffect;
    imageEffect = 0;
  }
  //KPixmapEffect::channelIntensity(pixmap, ratio, rgbChannel);
  imageEffect = new ChannelIntensity(this, ratio, channel, millisec);
  applyTransformations();
}

void ImageLabel::toGray(int millisec)
{
  if (imageEffect != 0)
  {
    delete imageEffect;
    imageEffect = 0;
  }
  //KPixmapEffect::toGray(pixmap);
  imageEffect = new ToGray(this, millisec);
  applyTransformations();
}

void ImageLabel::slotEffectExpired()
{
  removeEffects();
  m_karamba -> externalStep();
}

void ImageLabel::attachClickArea(QString leftMouseButton,
                                 QString  middleMouseButton,
                                 QString rightMouseButton)
{
    leftButtonAction = leftMouseButton;
    middleButtonAction = middleMouseButton;
    rightButtonAction = rightMouseButton;
}

#include "imagelabel.moc"
