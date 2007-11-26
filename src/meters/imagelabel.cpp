/****************************************************************************
*  imagelabel.cpp  -  ImageLabel meter
*
*  Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>
*  Copyright (c) 2004 Petri Damst� <damu@iki.fi>
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

#include "imagelabel.h"
#include "karambaapp.h"

#include <QtGui/QPixmap>
#include <QtGui/QToolTip>
#include <QtGui/QCursor>
#include <QtGui/QBitmap>
#include <QtGui/QImage>

#include <qimageblitz.h>

#include <KIconEffect>
#include <KTemporaryFile>
#include <KDebug>
#include <kio/job.h>
#include <KRun>

// Effect
Effect::Effect(ImageLabel* img, int msec) :
        myImage(img)
{
    if (msec > 0) {
        // remove the effect after the given time
        //QTimer::singleShot (millisec, myImage, SLOT(slotEffectExpired()));
        //timer -> start(millisec);
        millisec = msec;
    } else {
        millisec = msec;
    }
}

Effect::~Effect()
{}

void Effect::startTimer()
{
    if (millisec > 0) {
        QTimer::singleShot(millisec, myImage, SLOT(slotEffectExpired()));
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

QPixmap Intensity::apply(QPixmap pixmap)
{
    QImage img = pixmap.toImage();
    return QPixmap::fromImage(Blitz::intensity(img, ratio));
}

// ChannelIntensity
ChannelIntensity::ChannelIntensity(ImageLabel* img, float r, const QString &c,
                                   int millisec) :
        Effect(img, millisec)
{
    ratio = r;
    ratio = (ratio > 1)  ? 1 : ratio;
    ratio = (ratio < -1) ? -1 : ratio;

    channel = 0;
    if (c.contains("red", Qt::CaseInsensitive)) {
        channel = 0;
    } else if (c.contains("green", Qt::CaseInsensitive)) {
        channel = 1;
    } else if (c.contains("blue", Qt::CaseInsensitive)) {
        channel = 2;
    }
}

QPixmap ChannelIntensity::apply(QPixmap pixmap)
{
    QImage img = pixmap.toImage();
    return QPixmap::fromImage(Blitz::channelIntensity(img, ratio,
                                           Blitz::RGBChannel(channel+2)));
}

// ToGray
ToGray::ToGray(ImageLabel* img, int millisec) : Effect(img, millisec)
{}

QPixmap ToGray::apply(QPixmap pixmap)
{
    QImage image = pixmap.toImage();
    KIconEffect::toGray(image, 1.0f); // maybe Blitz::grayscale() would be better
    return pixmap = QPixmap::fromImage(image);
}

// Alpha
ToAlpha::ToAlpha(ImageLabel *img, const QColor &alphaColor, int alpha, int millisec) : Effect(img, millisec), m_alphaColor(alphaColor), m_alpha(alpha)
{}

QPixmap ToAlpha::apply(QPixmap pixmap)
{
    QPixmap alpha(pixmap.width(), pixmap.height());
    alpha.fill(QColor(m_alpha, m_alpha, m_alpha));

    if (m_alphaColor.isValid()) {
        QBitmap mask = pixmap.createMaskFromColor(m_alphaColor, Qt::MaskOutColor);
        QPainter painter(&alpha);
        painter.drawPixmap(0, 0, mask);
    }

    pixmap.setAlphaChannel(alpha);
    return pixmap;
}

/***********************************************************************/

ImageLabel::ImageLabel(Karamba* k, int ix, int iy, int iw, int ih) :
        Meter(k, ix, iy, iw, ih), m_allowClick(false), zoomed(false), rollover(false), m_renderer(0), m_connected(false)
{
    pixmap = QPixmap(iw, ih);
    pixmap.fill(Qt::transparent);

    background = 0;
    //scaleMat.reset();
    //rotMat.reset();
    scale_w = 1;
    scale_h = 1;
    rot_angle = 0;

    doScale = false;
    doRotate = false;

    imageEffect = 0;
}

ImageLabel::ImageLabel(Karamba* k) :
        Meter(k), zoomed(false), rollover(false), m_renderer(0),
        m_connected(false)
{
    background = 0;
}

ImageLabel::~ImageLabel()
{
    if (imageEffect != 0) {
        delete imageEffect;
        imageEffect = 0;
    }
    if (!old_tip_rect.isNull()) {}

    delete m_renderer;
}

void ImageLabel::setValue(int v)
{
    setValue(QString::number(v));
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

//  pixmap.convertFromImage(realpixmap.toImage().smoothScale(w, h));

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
    prepareGeometryChange();

    pixmap = realpixmap;
    if (doRotate) {
        QMatrix rotMat;
        rotMat.rotate(rot_angle);
        pixmap = pixmap.transformed(rotMat, Qt::SmoothTransformation);
    }

    if (doScale) {
        if (m_renderer) {
            QPixmap pm(scale_w, scale_h);
            pm.fill(Qt::transparent);
            QPainter painter(&pm);
            if (m_element.isEmpty()) {
                m_renderer->render(&painter);
            } else {
                m_renderer->render(&painter, m_element);
            }
            pixmap = pm;
        } else {
            if (useSmoothScale) {
                pixmap = QPixmap::fromImage(pixmap.toImage().scaled(scale_w, scale_h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
            } else {
                double widthFactor = ((double)scale_w) / ((double)pixmap.width());
                double heightFactor = ((double)scale_h) / ((double)pixmap.height());
                QMatrix scaleMat;
                scaleMat.scale(widthFactor, heightFactor);
                pixmap = pixmap.transformed(scaleMat);
            }
        }
    }
    if (imageEffect != 0) {
        pixmap = imageEffect -> apply(pixmap);
    }
    setWidth(pixmap.width());
    setHeight(pixmap.height());

    update();
}

void ImageLabel::slotCopyResult(KJob* job)
{
    QString tempFile = ((KIO::FileCopyJob*)job)->destUrl().path();
    if (job->error() == 0) {
        setValue(tempFile);
        imagePath = ((KIO::FileCopyJob*)job)->srcUrl().path();
        emit pixmapLoaded();
    } else {
        qWarning("Error downloading (%s): %s", job->errorText().toAscii().constData(),
                 tempFile.toAscii().constData());
    }
    KIO::NetAccess::removeTempFile(tempFile);
}

void ImageLabel::setValue(const QString &fn)
{
    // use the first line
    QStringList sList = fn.split("\n");
    QString fileName = *sList.begin();
    KUrl url(fileName);
    QRegExp rx("^[a-zA-Z]{1,5}:/", Qt::CaseInsensitive);
    bool protocol = (rx.indexIn(fileName) != -1) ? true : false;
    QPixmap pm;

    if (protocol && url.isLocalFile() == false) {
        KTemporaryFile tmpFile;
        tmpFile.setAutoRemove(false);
        tmpFile.open();
        KIO::FileCopyJob* copy = KIO::file_copy(fileName, tmpFile.fileName(), 0600,
                                                KIO::Overwrite);
        connect(copy, SIGNAL(result(KJob*)),
                this, SLOT(slotCopyResult(KJob*)));
        return;
    } else {
        if (m_karamba->theme().isThemeFile(fileName)) {
            QByteArray ba = m_karamba->theme().readThemeFile(fileName);
            if (fileName.endsWith("svg", Qt::CaseInsensitive) || fileName.endsWith("svgz", Qt::CaseInsensitive)) {
                m_renderer = new KSvgRenderer(ba);
            } else {
                pm.loadFromData(ba);
            }
        } else {
            if (fileName.endsWith("svg", Qt::CaseInsensitive) || fileName.endsWith("svgz", Qt::CaseInsensitive)) {
                m_renderer = new KSvgRenderer(fileName);
           } else {
                pm.load(fileName);
            }
        }

        if (m_renderer) {
            QPixmap tmpPm(m_renderer->defaultSize());
            tmpPm.fill(Qt::transparent);
            QPainter painter(&tmpPm);
            if (m_element.isEmpty()) {
                m_renderer->render(&painter);
            } else {
                m_renderer->render(&painter, m_element);
            }
            pm = tmpPm;

            if (m_renderer->animated()) {
                m_connected = connect(m_renderer, SIGNAL(repaintNeeded()), this, SLOT(repaintSvg()));
            }
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
    prepareGeometryChange();

    realpixmap = QPixmap(pix);
    pixmap = realpixmap;
    setWidth(pixmap.width());
    setHeight(pixmap.height());

    pixmapWidth = pixmap.width();
    pixmapHeight = pixmap.height();
    rect_off = QRect(getX(), getY(), pixmapWidth, pixmapHeight);

    update();
}

void ImageLabel::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget)
{
//  kDebug() << "image update: " << imagePath  ;

    Q_UNUSED(option);
    Q_UNUSED(widget);

    //only draw image if not hidden
    if (!m_hidden) {
        //draw the pixmap
        painter->drawPixmap(0, 0, pixmap);
    }

    // start Timer
    if (imageEffect != 0)
    {
        imageEffect -> startTimer();
    }
}

bool ImageLabel::mouseEvent(QEvent *e)
{
    if (isEnabled()) {
        Qt::MouseButtons button;
        if (QGraphicsSceneMouseEvent *event = dynamic_cast<QGraphicsSceneMouseEvent*>(e)) {
            button = event->button();
        } else if (QGraphicsSceneWheelEvent *event = dynamic_cast<QGraphicsSceneWheelEvent*>(e)) {
            button = event->buttons();
        }

        QString program;
        if (button == Qt::LeftButton) {
            program = m_leftMouseButtonAction;
        } else if (button == Qt::MidButton) {
            program = m_middleMouseButtonAction;
        } else if (button == Qt::RightButton) {
            program = m_rightMouseButtonAction;
        }

        if (!program.isEmpty()) {
            KRun::runCommand(program,0L);
        } else {
            return true;
        }
    }
    return false;
}

void ImageLabel::allowClick(bool enable)
{
    m_allowClick = enable;
}

bool ImageLabel::clickable()
{
    return m_allowClick;
}

void ImageLabel::parseImages(const QString &fn, const QString &fn_roll, int _xoff,
                             int _yoff, int _xon, int _yon)
{
    //fn = filename;
    //fn_roll = filename_roll;

    xoff = _xoff;
    yoff = _yoff;
    xon = _xon;
    yon = _yon;

    // use the first line
    QStringList sList = fn.split("\n");
    QString fileName = *sList.begin();
    QFileInfo fileInfo(fileName);
    QString path;

    QRegExp rx("^http://", Qt::CaseInsensitive);
    bool fileOnNet = (rx.indexIn(fileName) != -1) ? true : false;


    if (fileInfo.isRelative() && !fileOnNet) {
        path = m_karamba->theme().path() + '/' + fileName;
    } else {
        path = fileName;
    }

    if (fileOnNet) {
        QString tmpFile;
        if (KIO::NetAccess::download(KUrl(path), tmpFile, 0)) {
            pixmap_off = QPixmap(tmpFile);
            KIO::NetAccess::removeTempFile(tmpFile);
            kDebug() << "Downloaded: " << path << " to " << tmpFile ;
        } else {
            kDebug() << "Error Downloading: " << path ;
        }
    } else {
        pixmap_off = QPixmap(path);
    }

    pixmapOffWidth = pixmap.width();
    pixmapOffHeight = pixmap.height();

    rect_off = QRect(xoff, yoff, pixmapWidth, pixmapHeight);
/////////////////////////////
    if (fn_roll.isEmpty())
        return;

    rollover = true;
    sList = fn_roll.split("\n");
    fileName = *sList.begin();
    fileInfo = QFileInfo(fileName);

    fileOnNet = (rx.indexIn(fileName) != -1) ? true : false;


    if (fileInfo.isRelative() && !fileOnNet) {
        path = m_karamba->theme().path() + '/' + fileName;
    } else {
        path = fileName;
    }

    if (fileOnNet) {
        QString tmpFile;
        if (KIO::NetAccess::download(KUrl(path), tmpFile, 0)) {
            pixmap_on = QPixmap(tmpFile);
            KIO::NetAccess::removeTempFile(tmpFile);
            qDebug("Downloaded: %s to %s", path.toAscii().constData(), tmpFile.toAscii().constData());
        } else {
            qDebug("Error Downloading: %s", path.toAscii().constData());
        }
    } else {
        pixmap_on = QPixmap(path);
    }
    pixmapOnWidth = pixmap_on.width();
    pixmapOnHeight = pixmap_on.height();

    rect_on = QRect(xon, yon, pixmapOnWidth, pixmapOnHeight);
}

void ImageLabel::setBackground(int b)
{
    background = b;
}

void ImageLabel::rolloverImage(QGraphicsSceneHoverEvent *e)
{
    if (!rollover)
        return;

    prepareGeometryChange();

    if (zoomed) {
        if (!rect_off.contains(e->pos().toPoint())) {
            // rollover the image to the zoomed image
            //setValue(fn_roll);
            setX(xoff);
            setY(yoff);
            pixmap = pixmap_off;
            pixmapWidth = pixmapOffWidth;
            pixmapHeight = pixmapOffHeight;
            zoomed = false;
        }
    } else {
        if (rect_off.contains(e->pos().toPoint())) {
            // rollover the image to the zoomed image
            //setValue(fn_roll);
            setX(xon);
            setY(yon);
            pixmap = pixmap_on;
            pixmapWidth = pixmapOnWidth;
            pixmapHeight = pixmapOnHeight;
            zoomed = true;
        }
    }

    update();
}

void ImageLabel::setTooltip(QString txt)
{
    setToolTip(txt);
}


void ImageLabel::removeEffects()
{
    if (imageEffect != 0) {
        delete imageEffect;
        imageEffect = 0;
    }
    applyTransformations();
}

void ImageLabel::intensity(float ratio, int millisec)
{
    if (imageEffect != 0) {
        delete imageEffect;
        imageEffect = 0;
    }
    //QPixmapEffect::intensity(pixmap, ratio);
    imageEffect = new Intensity(this, ratio, millisec);
    applyTransformations();
}

void ImageLabel::channelIntensity(float ratio, QString channel, int millisec)
{
    if (imageEffect != 0) {
        delete imageEffect;
        imageEffect = 0;
    }
    //QPixmapEffect::channelIntensity(pixmap, ratio, rgbChannel);
    imageEffect = new ChannelIntensity(this, ratio, channel, millisec);
    applyTransformations();
}

void ImageLabel::toGray(int millisec)
{
    if (imageEffect != 0) {
        delete imageEffect;
        imageEffect = 0;
    }
    //QPixmapEffect::toGray(pixmap);
    imageEffect = new ToGray(this, millisec);
    applyTransformations();
}

void ImageLabel::toAlpha(const QColor &alphaColor, int alpha, int millisec)
{
    if (imageEffect != 0) {
        delete imageEffect;
        imageEffect = 0;
    }

    imageEffect = new ToAlpha(this, alphaColor, alpha, millisec);
    applyTransformations();
}

void ImageLabel::slotEffectExpired()
{
    removeEffects();
    m_karamba->update();
}

void ImageLabel::attachClickArea(QString leftMouseButton,
                                 QString middleMouseButton,
                                 QString rightMouseButton)
{
    m_leftMouseButtonAction = leftMouseButton;
    m_middleMouseButtonAction = middleMouseButton;
    m_rightMouseButtonAction = rightMouseButton;

    setAcceptedMouseButtons(Qt::LeftButton | Qt::MidButton | Qt::RightButton);
    setCursor(QCursor(Qt::PointingHandCursor));
}

void ImageLabel::setPixel(const QPoint &point, const QColor &pixel)
{
    QPainter painter(&pixmap);
    painter.setPen(pixel);
    painter.drawPoint(point);
}

void ImageLabel::repaintSvg()
{
    QPainter painter(&pixmap);
    pixmap.fill(Qt::transparent);

    if (m_element.isEmpty()) {
        m_renderer->render(&painter);
    } else {
        m_renderer->render(&painter, m_element);
    }

    update();
}

bool ImageLabel::enableAnimation(bool enable)
{
    if (!m_renderer || !m_renderer->animated()) {
        return false;
    }

    if (enable && !m_connected) {
            m_connected = connect(m_renderer, SIGNAL(repaintNeeded()), this, SLOT(repaintSvg()));
    } else if (!enable && m_connected) {
            m_connected = !disconnect(m_renderer, SIGNAL(repaintNeeded()), this, SLOT(repaintSvg()));
    }

    return true;
}

bool ImageLabel::animationEnabled() const
{
    return m_connected;
}

bool ImageLabel::drawElement(const QString &element)
{
    if (element.isEmpty()) {
        m_element.clear();

        repaintSvg();

        return true;
    }

    if (m_renderer && m_renderer->elementExists(element)) {
        m_element = element;

        repaintSvg();

        return true;
    }

    return false;
}

QString ImageLabel::elementDrawn() const
{
    return m_element;
}

#include "imagelabel.moc"
