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

#include <QPixmap>
#include <QToolTip>

#include <KImageEffect>
#include <KPixmapEffect>
#include <KTemporaryFile>
#include <kio/job.h>
#include <KRun>

#include "karambaapp.h"
#include "imagelabel.h"

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
    return KPixmapEffect::intensity(pixmap, ratio);
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
    return KPixmapEffect::channelIntensity(pixmap, ratio,
                                           (KPixmapEffect::RGBComponent)channel);
}

// ToGray
ToGray::ToGray(ImageLabel* img, int millisec) : Effect(img, millisec)
{}

QPixmap ToGray::apply(QPixmap pixmap)
{
    return KPixmapEffect::toGray(pixmap);
}

/***********************************************************************/

ImageLabel::ImageLabel(Karamba* k, int ix, int iy, int iw, int ih) :
        Meter(k, ix, iy, iw, ih), m_allowClick(false), zoomed(false), rollover(false)
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

ImageLabel::ImageLabel(Karamba* k) :
        Meter(k), zoomed(false), rollover(false)
{
    cblend = 0;
    background = 0;
}

ImageLabel::~ImageLabel()
{
    if (imageEffect != 0) {
        delete imageEffect;
        imageEffect = 0;
    }
    if (!old_tip_rect.isNull()) {}
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
        // KDE and QT seem to miss a high quality image rotation
        QMatrix rotMat;
        rotMat.rotate(rot_angle);
        pixmap = pixmap.transformed(rotMat);
    }
    if (doScale) {
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
    if (imageEffect != 0) {
        pixmap = imageEffect -> apply(pixmap);
    }
    setWidth(pixmap.width());
    setHeight(pixmap.height());

    update();
}

void ImageLabel::slotCopyResult(KIO::Job* job)
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
                                                true, false, false);
        connect(copy, SIGNAL(result(KIO::Job*)),
                this, SLOT(slotCopyResult(KIO::Job*)));
        return;
    } else {
        if (m_karamba->theme().isThemeFile(fileName)) {
            QByteArray ba = m_karamba->theme().readThemeFile(fileName);
            pm.loadFromData(ba);
        } else {
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
//  kDebug() << "image update: " << imagePath  << endl;

    Q_UNUSED(option);
    Q_UNUSED(widget);

        //only draw image if not hidden
    if (!m_hidden)
    {
        painter->setOpacity(m_opacity);

        if (cblend == 0) {
            //draw the pixmap
            painter->drawPixmap(0, 0, pixmap);
        } else {
            //Blend this image with a color
            QImage image = pixmap.toImage();

            QImage result = KImageEffect::blend(QColor(255, 0, 0), image, 0.5f);
            painter->drawImage(0, 0, result);
        }
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
            KRun::runCommand(program);
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
            qDebug("Downloaded: %s to %s", path.toAscii().constData(), tmpFile.toAscii().constData());
        } else {
            qDebug("Error Downloading: %s", path.toAscii().constData());
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

void ImageLabel::rolloverImage(QMouseEvent *e)
{
    if (!rollover)
        return;

    prepareGeometryChange();

    if (zoomed) {
        if (!rect_off.contains(e->pos())) {
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
        if (rect_off.contains(e->pos())) {
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
    toolTipText = txt;
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

void ImageLabel::slotEffectExpired()
{
    removeEffects();
    m_karamba->update();
}

void ImageLabel::attachClickArea(QString leftMouseButton,
                                 QString  middleMouseButton,
                                 QString rightMouseButton)
{
    m_leftMouseButtonAction = leftMouseButton;
    m_middleMouseButtonAction = middleMouseButton;
    m_rightMouseButtonAction = rightMouseButton;

    setAcceptedMouseButtons(Qt::LeftButton | Qt::MidButton | Qt::RightButton);
}

bool ImageLabel::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip)
        QToolTip::showText(QPoint(getX(), getY()), toolTipText);

    return true;
}

#include "imagelabel.moc"
