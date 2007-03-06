/*
 * Copyright (c) 2007 Alexander Wiedenbruch <mail@wiedenbruch.de>
 *
 * This file is part of SuperKaramba.
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

#include <QGraphicsScene>
#include <QNetworkInterface>

#include <kdebug.h>
#include <kmenu.h>
#include <krun.h>
#include <kservice.h>
#include <klocale.h>

#include "karamba.h"
#include "karambaapp.h"
#include "lineparser.h"
#include "showdesktop.h"
#include "karambainterface.h"
#include "karambainterface.moc"

KarambaInterface::KarambaInterface(Karamba *k)
        : QObject(),
        m_karamba(k)
{
    setObjectName("karamba");

    Kross::Manager::self().addObject(this, "karamba");

    QString scriptFile = k->theme().path() + "/" + k->theme().pythonModule() + ".py";
    m_action = new Kross::Action(this, KUrl(scriptFile));
    m_action->trigger();
}

KarambaInterface::~KarambaInterface()
{
    delete m_action;
}

// Testing functions -----------------------

bool KarambaInterface::checkKaramba(const Karamba *k) const
{
    if (!k) {
        kWarning() << "Widget pointer was 0" << endl;
        return false;
    }

    if (!karambaApp->hasKaramba(k)) {
        kWarning() << "Widget " << (long)k << " not found" << endl;
        return false;
    }

    return true;
}

bool KarambaInterface::checkMeter(const Karamba *k, const Meter *m, const QString &type) const
{
    if (!m) {
        kWarning() << "Meter pointer was 0" << endl;
        return false;
    }

    if (!k->hasMeter(m)) {
        kWarning() << "Widget does not have meter " << (long)m << endl;
        return false;
    }

    if (!m->inherits(type.toAscii().data())) {
        kWarning() << "Meter is not of type " << type << endl;
        return false;
    }

    return true;
}

bool KarambaInterface::checkKarambaAndMeter(const Karamba *k, const Meter *m, const QString &type)
    const
{
    return checkKaramba(k) && checkMeter(k, m, type);
}

QVariantList KarambaInterface::getMeterMinMax(const Karamba *k, const Meter *m, const QString &type)
    const
{
    if (!checkKarambaAndMeter(k, m, type))
        return QVariantList();

    QVariantList ret;
    ret << m->getMax();
    ret << m->getMin();
    return ret;
}

QVariantList KarambaInterface::getMeterSize(const Karamba *k, const Meter *m, const QString &type)
    const
{
    if (!checkKarambaAndMeter(k, m, type))
        return QVariantList();

    QVariantList list;
    list << QVariant::fromValue(m->getWidth());
    list << QVariant::fromValue(m->getHeight());
    return list;
}

QVariantList KarambaInterface::getMeterPos(const Karamba *k, const Meter *m, const QString &type)
    const
{
    if (!checkKarambaAndMeter(k, m, type))
        return QVariantList();

    QVariantList ret;
    ret << QVariant::fromValue(m->getX());
    ret << QVariant::fromValue(m->getY());
    return ret;
}

QString KarambaInterface::getMeterSensor(const Karamba *k, const Meter *m, const QString &type)
    const
{
    if (!checkKarambaAndMeter(k, m, type))
        return QString::null;

    return k->getSensor(m);

}

int KarambaInterface::getMeterValue(const Karamba *k, const Meter *m, const QString &type) const
{
    if (!checkKarambaAndMeter(k, m, type))
        return 0;

    return m->getValue();
}

QObject* KarambaInterface::getThemeMeter(const Karamba *k, const QString &meter, const QString
        &type) const
{
    if (!checkKaramba(k))
        return 0;

    QGraphicsItem *item;
    QList<QGraphicsItem*> list = ((QGraphicsItemGroup*)k)->children();
    foreach(item, list) {
        Meter *m = (Meter*)item;
        if (m->objectName() == meter) {
            if (checkMeter(k, m, type));
            return m;
        }
    }
    return 0;
}

bool KarambaInterface::hideMeter(const Karamba *k, Meter *m, const QString &type) const
{
    if (!checkKarambaAndMeter(k, m, type))
        return false;

    m->hide();
    return true;
}

bool KarambaInterface::moveMeter(const Karamba *k, Meter *m, const QString &type, int x, int y)
    const
{
    if (!checkKarambaAndMeter(k, m, type))
        return false;

    k->moveMeter(m, x, y);
    return true;
}

bool KarambaInterface::resizeMeter(const Karamba *k, Meter *m, const QString &type, int width, int
        height) const
{
    if (!checkKarambaAndMeter(k, m, type))
        return false;

    m->setSize(m->getX(), m->getY(), width, height);
    return true;
}

bool KarambaInterface::setMeterMinMax(const Karamba *k, Meter *m, const QString &type, int min, int max)
    const
{
    if (!checkKarambaAndMeter(k, m, type))
        return false;

    m->setMin(min);
    m->setMax(max);
    return true;
}

bool KarambaInterface::setMeterSensor(Karamba *k, Meter *m, const QString &type,
        const QString &sensor) const
{
    if (!checkKarambaAndMeter(k, m, type))
        return false;

    k->setSensor(LineParser(sensor), m);
    return true;
}

QObject* KarambaInterface::setMeterValue(const Karamba *k, Meter *m, const QString &type, int value)
    const
{
    if (!checkKarambaAndMeter(k, m, type))
        return 0;

    m->setValue(value);
    return m;
}

bool KarambaInterface::showMeter(const Karamba *k, Meter *m, const QString &type) const
{
    if (!checkKarambaAndMeter(k, m, type))
        return false;

    m->show();
    return true;
}

bool KarambaInterface::setMeterColor(Karamba *k, Meter *m, QString type, int red, int green, int blue)
{
    if (!checkKarambaAndMeter(k, m, type))
        return false;

    m->setColor(QColor(red, green, blue));
    return true;
}

QVariantList KarambaInterface::getMeterColor(Karamba *k, Meter *m, QString type)
{
    if (!checkKarambaAndMeter(k, m, type))
        return QVariantList();

    QColor color = m->getColor();

    QVariantList ret;
    ret << color.red();
    ret << color.green();
    ret << color.blue();

    return ret;
}

QString KarambaInterface::getMeterStringValue(Karamba *k, Meter *m, QString type)
{
    if (!checkKarambaAndMeter(k, m, type))
        return QString::null;

    return m->getStringValue();
}

QObject* KarambaInterface::setMeterStringValue(Karamba *k, Meter *m, QString type, QString value)
{
    if (!checkKarambaAndMeter(k, m, type))
        return 0;

    m->setValue(value);

    return m;
}


// Calls to scripts ----------------------
void KarambaInterface::callInitWidget(Karamba *k)
{
    emit initWidget(k);
}

void KarambaInterface::callWidgetUpdated(Karamba *k)
{
    emit widgetUpdated(k);
}

void KarambaInterface::callWidgetClosed(Karamba *k)
{
    emit widgetClosed(k);
}

void KarambaInterface::callMenuOptionChanged(Karamba *k, QString key, bool value)
{
    emit menuOptionChanged(k, key, value);
}

void KarambaInterface::callMenuItemClicked(Karamba* k, KMenu* menu, QAction *id)
{
    emit menuItemClicked(k, menu, id);
}

void KarambaInterface::callActiveTaskChanged(Karamba *k, Task* t)
{
    emit activeTaskChanged(k, (long long)t);
}

void KarambaInterface::callTaskAdded(Karamba *k, Task *t)
{
    emit taskAdded(k, (long long)t);
}

void KarambaInterface::callTaskRemoved(Karamba *k, Task *t)
{
    emit taskRemoved(k, (long long)t);
}

void KarambaInterface::callStartupAdded(Karamba *k, Startup *t)
{
    emit startupAdded(k, (long long)t);
}

void KarambaInterface::callStartupRemoved(Karamba *k, Startup *t)
{
    emit startupRemoved(k, (long long)t);
}

void KarambaInterface::callCommandFinished(Karamba *k, int pid)
{
    emit commandFinished(k, pid);
}

void KarambaInterface::callCommandOutput(Karamba *k, int pid, char* buffer)
{
    emit commandOutput(k, pid, QString(buffer));
}

void KarambaInterface::callItemDropped(Karamba *k, QString text, int x, int y)
{
    emit itemDropped(k, text, x, y);
}

void KarambaInterface::callMeterClicked(Karamba *k, Meter *m, int button)
{
    emit meterClicked(k, m, button);
}

void KarambaInterface::callMeterClicked(Karamba *k, QString str, int button)
{
    emit meterClicked(k, str, button);
}

void KarambaInterface::callWidgetClicked(Karamba *k, int x, int y, int button)
{
    emit widgetClicked(k, x, y, button);
}

void KarambaInterface::callDesktopChanged(Karamba *k, int desktop)
{
    emit desktopChanged(k, desktop);
}

void KarambaInterface::callWidgetMouseMoved(Karamba *k, int x, int y, int button)
{
    emit widgetMouseMoved(k, x, y, button);
}

void KarambaInterface::callKeyPressed(Karamba *k, Meter *meter, QString key)
{
    emit keyPressed(k, meter, key);
}

// Calls from scripts --------------------

/** Bar/createBar
*
* SYNOPSIS
*   long createBar(widget, x, y, w, h, image)
* DESCRIPTION
*   This creates a bar at x, y with width and height w, h.
* ARGUMENTS
*   * long widget -- karamba
*   * long x -- x coordinate
*   * long y -- y coordinate
*   * long w -- width
*   * long h -- height
*   * string image -- Path to image
* RETURN VALUE
*   Pointer to new bar meter
*/
QObject* KarambaInterface::createBar(Karamba* k, int x, int y, int w, int h, const QString &path)
    const
{
    if (!checkKaramba(k))
        return NULL;

    Bar *tmp = new Bar(k, x, y, w, h);
    tmp->setImage(path);
    tmp->setValue(50);
    k->addToGroup(tmp);

    return tmp;
}

/** Bar/deleteBar
*
* SYNOPSIS
*   long deleteBar(widget, bar)
* DESCRIPTION
*   This deletes a bar.
* ARGUMENTS
*   * long widget -- karamba
*   * long bar -- Pointer to bar
* RETURN VALUE
*   1 if successful
*/
bool KarambaInterface::deleteBar(Karamba *k, Bar *bar) const
{
    if (!checkKarambaAndMeter(k, bar, "Bar"))
        return false;

    return k->removeMeter(bar);
}

/** Bar/setBarMinMax
*
* SYNOPSIS
*   long setBarMinMax(widget, bar, min, max)
* DESCRIPTION
*   Returns current bar value.
* ARGUMENTS
*   * long widget -- karamba
*   * long bar -- pointer to bar
*   * long min -- min value
*   * long max -- max value
* RETURN VALUE
*   1 if successful
*/
bool KarambaInterface::setBarMinMax(const Karamba *k, Bar *bar, int min, int max) const
{
    return setMeterMinMax(k, bar, "Bar", min, max);
}

/** Bar/getBarMinMax
*
* SYNOPSIS
*   tuple getBarMinMax(widget, bar)
* DESCRIPTION
*   Returns possible min and max values of the bar.
* ARGUMENTS
*   * long widget -- karamba
*   * long bar -- pointer to bar
* RETURN VALUE
*   max & min
*/
QVariantList KarambaInterface::getBarMinMax(const Karamba *k, const Bar *bar) const
{
    return getMeterMinMax(k, bar, "Bar");
}

/** Bar/moveBar
*
* SYNOPSIS
*   long moveBar(widget, bar, x, y)
* DESCRIPTION
*   This will move the bar to new x and y coordinates.
* ARGUMENTS
*   * long widget -- karamba
*   * long bar -- pointer to bar
*   * long x -- x coordinate
*   * long y -- y coordinate
* RETURN VALUE
*   1 if successful
*/
bool KarambaInterface::moveBar(const Karamba *k, Bar *bar, int x, int y) const
{
    return moveMeter(k, bar, "Bar", x, y);
}

/** Bar/getBarPos
*
* SYNOPSIS
*   tuple getBarPos(widget, bar)
* DESCRIPTION
*   Given a reference to a bar object, this will return a tuple
*   containing the x and y coordinate of a bar object.
* ARGUMENTS
*   * long widget -- karamba
*   * long bar -- pointer to bar
* RETURN VALUE
*   x and y coordinates
*/
QVariantList KarambaInterface::getBarPos(const Karamba *k, const Bar *bar) const
{
    return getMeterPos(k, bar, "Bar");
}

/** Bar/setBarSensor
*
* SYNOPSIS
*   long setBarSensor(widget, bar, sensor)
* DESCRIPTION
*   Sets the sensor string of the bar.
* ARGUMENTS
*   * long widget -- karamba
*   * long bar -- pointer to bar
*   * string sensor -- new sensor as in theme files
* RETURN VALUE
*   1 if successful
*/
bool KarambaInterface::setBarSensor(Karamba *k, Bar *bar, const QString &sensor) const
{
    return setMeterSensor(k, bar, "Bar", sensor);
}

/** Bar/getBarSensor
*
* SYNOPSIS
*   string getBarSensor(widget, bar)
* DESCRIPTION
*   Gets the current sensor string
* ARGUMENTS
*   * long widget -- karamba
*   * long bar -- pointer to bar
* RETURN VALUE
*   sensor string
*/
QString KarambaInterface::getBarSensor(const Karamba *k, const Bar *bar) const
{
    return getMeterSensor(k, bar, "Bar");
}

/** Bar/resizeBar
*
* SYNOPSIS
*   long resizeBar(widget, bar, w, h)
* DESCRIPTION
*   This will resize bar to new height and width.
* ARGUMENTS
*   * long widget -- karamba
*   * long bar -- pointer to bar
*   * long w -- new width
*   * long h -- new height
* RETURN VALUE
*   1 if successful
*/
bool KarambaInterface::resizeBar(const Karamba *k, Bar *bar, int width, int height) const
{
    return resizeMeter(k, bar, "Bar", width, height);
}

/** Bar/getBarSize
*
* SYNOPSIS
*   tuple getBarSize(widget, bar)
* DESCRIPTION
*   Given a reference to a bar object, this will return a tuple
*   containing the height and width of a bar object.
* ARGUMENTS
*   * long widget -- karamba
*   * long bar -- pointer to bar
* RETURN VALUE
*   width and height
*/
QVariantList KarambaInterface::getBarSize(const Karamba *k, const Bar *bar) const
{
    return getMeterSize(k, bar, "Bar");
}

/** Bar/setBarValue
*
* SYNOPSIS
*   long setBarValue(widget, bar, value)
* DESCRIPTION
*   Sets current bar value.
* ARGUMENTS
*   * long widget -- karamba
*   * long bar -- pointer to bar
*   * long value -- new value
* RETURN VALUE
*   1 if successful
*/
QObject* KarambaInterface::setBarValue(const Karamba *k, Bar *bar, int value) const
{
    return setMeterValue(k, bar, "Bar", value);
}

/** Bar/getBarValue
*
* SYNOPSIS
*   long getBarValue(widget, bar)
* DESCRIPTION
*   Returns current bar value.
* ARGUMENTS
*   * long widget -- karamba
*   * long bar -- pointer to bar
* RETURN VALUE
*   value
*/
int KarambaInterface::getBarValue(const Karamba *k, const Bar *bar) const
{
    return getMeterValue(k, bar, "Bar");
}

/** Bar/getThemeBar
*
* SYNOPSIS
*   long getThemeBar(widget, name)
* DESCRIPTION
*   You can reference a bar in your python code that was created in the
*   theme file. Basically, you just add a NAME= value to the BAR line in
*   the .theme file. Then if you want to use that object, instead of calling
*   createBar, you can call this function.
*
*   The name you pass to the function is the same one that you gave it for
*   the NAME= parameter in the .theme file.
* ARGUMENTS
*   * long widget -- karamba
*   * string name -- name of the bar to get
* RETURN VALUE
*   Pointer to bar
*/
QObject* KarambaInterface::getThemeBar(const Karamba *k, const QString &meter) const
{
    return getThemeMeter(k, meter, "Bar");
}

/** Bar/hideBar
*
* SYNOPSIS
*   long hideBar(widget, bar)
* DESCRIPTION
*   This hides an bar. In other words, during subsequent calls to
*   widgetUpdate(), this bar will not be drawn.
* ARGUMENTS
*   * long widget -- karamba
*   * long bar -- pointer to bar
* RETURN VALUE
*   1 if successful
*/
bool KarambaInterface::hideBar(const Karamba *k, Bar *bar) const
{
    return hideMeter(k, bar, "Bar");
}

/** Bar/showBar
*
* SYNOPSIS
*   long showBar(widget, bar)
* DESCRIPTION
*   This shows an bar. In other words, during subsequent calls to
*   widgetUpdate(), this bar will be drawn.
* ARGUMENTS
*   * long widget -- karamba
*   * long bar -- pointer to bar
* RETURN VALUE
*   1 if successful
*/
bool KarambaInterface::showBar(const Karamba *k, Bar *bar) const
{
    return showMeter(k, bar, "Bar");
}

/** Bar/setBarVertical
*
* SYNOPSIS
*   long setBarVertical(widget, bar)
* DESCRIPTION
*   Set bar vertical
* ARGUMENTS
*   * long widget -- karamba
*   * long bar -- pointer to bar
*   * long vertical -- 1 if vertical
* RETURN VALUE
*   1 if successful
*/
bool KarambaInterface::setBarVertical(const Karamba *k, Bar *bar, bool vert) const
{
    if (!checkKarambaAndMeter(k, bar, "Bar"))
        return false;

    bar->setVertical(vert);
    return true;
}

/** Bar/getBarVertical
*
* SYNOPSIS
*   string getBarVertical(widget, bar)
* DESCRIPTION
*   Check if bar is vertical bar
* ARGUMENTS
*   * long widget -- karamba
*   * long bar -- pointer to bar
* RETURN VALUE
*   1 if vertical
*/
bool KarambaInterface::getBarVertical(const Karamba *k, const Bar *bar) const
{
    if (!checkKarambaAndMeter(k, bar, "Bar"))
        return false;

    return bar->getVertical();
}

/** Bar/setBarImage
*
* SYNOPSIS
*   long setBarImage(widget, bar, image)
* DESCRIPTION
*   Use the image in path for the background as the bar
* ARGUMENTS
*   * long widget -- karamba
*   * long bar -- pointer to bar
*   * string image -- new image
* RETURN VALUE
*   1 if successful
*/
bool KarambaInterface::setBarImage(const Karamba *k, Bar *bar, const QString &image) const
{
    if (!checkKarambaAndMeter(k, bar, "Bar"))
        return false;

    return bar->setImage(image);
}

/** Bar/getBarImage
*
* SYNOPSIS
*   string getBarImage(widget, bar)
* DESCRIPTION
*   Get bar image
* ARGUMENTS
*   * long widget -- karamba
*   * long bar -- pointer to bar
* RETURN VALUE
*   path to bar image
*/
QString KarambaInterface::getBarImage(const Karamba *k, const Bar *bar) const
{
    if (!checkKarambaAndMeter(k, bar, "Bar"))
        return QString::null;

    return bar->getImage();
}




QObject* KarambaInterface::createGraph(Karamba* k, int x, int y, int w, int h, int points)
{
    if (!checkKaramba(k))
        return NULL;

    Graph *tmp = new Graph(k, x, y, w, h, points);
    k->addToGroup(tmp);

    return tmp;
}

bool KarambaInterface::deleteGraph(Karamba *k, Graph *graph)
{
    if (!checkKarambaAndMeter(k, graph, "Graph"))
        return false;

    return k->removeMeter(graph);
}

bool KarambaInterface::setGraphMinMax(Karamba *k, Graph *graph, int min, int max)
{
    return setMeterMinMax(k, graph, "Graph", min, max);
}

QVariantList KarambaInterface::getGraphMinMax(Karamba *k, Graph *graph)
{
    return getMeterMinMax(k, graph, "Graph");
}

bool KarambaInterface::moveGraph(Karamba *k, Graph *graph, int x, int y)
{
    return moveMeter(k, graph, "Graph", x, y);
}

QVariantList KarambaInterface::getGraphPos(Karamba *k, Graph *graph)
{
    return getMeterPos(k, graph, "Graph");
}

bool KarambaInterface::setGraphSensor(Karamba *k, Graph *graph, QString sensor)
{
    return setMeterSensor(k, graph, "Graph", sensor);
}

QString KarambaInterface::getGraphSensor(Karamba *k, Graph *graph)
{
    return getMeterSensor(k, graph, "Graph");
}

bool KarambaInterface::resizeGraph(Karamba *k, Graph *graph, int width, int height)
{
    return resizeMeter(k, graph, "Graph", width, height);
}

QVariantList KarambaInterface::getGraphSize(Karamba *k, Graph *graph)
{
    return getMeterSize(k, graph, "Graph");
}

QObject* KarambaInterface::setGraphValue(Karamba *k, Graph *graph, int value)
{
    return setMeterValue(k, graph, "Graph", value);
}

int KarambaInterface::getGraphValue(Karamba *k, Graph *graph)
{
    return getMeterValue(k, graph, "Graph");
}

QObject* KarambaInterface::getThemeGraph(Karamba *k, QString meter)
{
    return getThemeMeter(k, meter, "Graph");
}

bool KarambaInterface::hideGraph(Karamba *k, Graph *graph)
{
    return hideMeter(k, graph, "Graph");
}

bool KarambaInterface::showGraph(Karamba *k, Graph *graph)
{
    return showMeter(k, graph, "Graph");
}

bool KarambaInterface::setGraphColor(Karamba *k, Graph *graph, int red, int green, int blue)
{
    return setMeterColor(k, graph, "Graph", red, green, blue);
}

QVariantList KarambaInterface::getGraphColor(Karamba *k, Graph *graph)
{
    return getMeterColor(k, graph, "Graph");
}



QObject* KarambaInterface::createImage(Karamba* k, int x, int y, QString image)
{
    if (!checkKaramba(k))
        return NULL;

    ImageLabel *tmp = new ImageLabel(k, x, y, 0, 0);
    tmp->setValue(image);

    k->setSensor(LineParser(image), tmp);

    k->addToGroup(tmp);

    return tmp;
}

bool KarambaInterface::deleteImage(Karamba *k, ImageLabel *image)
{
    if (!checkKarambaAndMeter(k, image, "ImageLabel"))
        return false;

    return k->removeMeter(image);
}

bool KarambaInterface::setImageMinMax(Karamba *k, ImageLabel *image, int min, int max)
{
    return setMeterMinMax(k, image, "ImageLabel", min, max);
}

QVariantList KarambaInterface::getImageMinMax(Karamba *k, ImageLabel *image)
{
    return getMeterMinMax(k, image, "ImageLabel");
}

bool KarambaInterface::moveImage(Karamba *k, ImageLabel *image, int x, int y)
{
    return moveMeter(k, image, "ImageLabel", x, y);
}

QVariantList KarambaInterface::getImagePos(Karamba *k, ImageLabel *image)
{
    return getMeterPos(k, image, "ImageLabel");
}

bool KarambaInterface::setImageSensor(Karamba *k, ImageLabel *image, QString sensor)
{
    return setMeterSensor(k, image, "ImageLabel", sensor);
}

QString KarambaInterface::getImageSensor(Karamba *k, ImageLabel *image)
{
    return getMeterSensor(k, image, "ImageLabel");
}

bool KarambaInterface::resizeImage(Karamba *k, ImageLabel *image, int width, int height)
{
    return resizeMeter(k, image, "ImageLabel", width, height);
}

QVariantList KarambaInterface::getImageSize(Karamba *k, ImageLabel *image)
{
    return getMeterSize(k, image, "ImageLabel");
}

QObject* KarambaInterface::setImagePath(Karamba *k, ImageLabel *image, QString path)
{
    return setMeterStringValue(k, image, "ImageLabel", path);
}

QString KarambaInterface::getImagePath(Karamba *k, ImageLabel *image)
{
    return getMeterStringValue(k, image, "ImageLabel");
}

QObject* KarambaInterface::getThemeImage(Karamba *k, QString meter)
{
    return getThemeMeter(k, meter, "ImageLabel");
}

bool KarambaInterface::hideImage(Karamba *k, ImageLabel *image)
{
    return hideMeter(k, image, "ImageLabel");
}

bool KarambaInterface::showImage(Karamba *k, ImageLabel *image)
{
    return showMeter(k, image, "ImageLabel");
}

bool KarambaInterface::setImageColor(Karamba *k, ImageLabel *image, int red, int green, int blue)
{
    return setMeterColor(k, image, "ImageLabel", red, green, blue);
}

QVariantList KarambaInterface::getImageColor(Karamba *k, ImageLabel *image)
{
    return getMeterColor(k, image, "ImageLabel");
}

bool KarambaInterface::addImageTooltip(Karamba *k, ImageLabel *image, QString text)
{
    if (!checkKarambaAndMeter(k, image, "ImageLabel"))
        return false;

    image->setTooltip(text);
    return true;
}

bool KarambaInterface::changeImageChannelIntensity(Karamba *k, ImageLabel *image, double ratio, QString channel, int ms)
{
    if (!checkKarambaAndMeter(k, image, "ImageLabel"))
        return false;

    image->channelIntensity(ratio, channel, ms);
    return true;
}

bool KarambaInterface::changeImageIntensity(Karamba *k, ImageLabel *image, double ratio, int ms)
{
    if (!checkKarambaAndMeter(k, image, "ImageLabel"))
        return false;

    image->intensity(ratio, ms);
    return true;
}

bool KarambaInterface::changeImageToGray(Karamba *k, ImageLabel *image, int ms)
{
    if (!checkKarambaAndMeter(k, image, "ImageLabel"))
        return false;

    image->toGray(ms);
    return true;
}

QObject* KarambaInterface::createBackgroundImage(Karamba *k, int x, int y, QString imagePath)
{
    if (!checkKaramba(k))
        return 0;

    ImageLabel *tmp = new ImageLabel(k, x, y, 0, 0);
    tmp->setValue(imagePath);
    tmp->setBackground(true);
    k->setSensor(LineParser(imagePath), tmp);

    k->addToGroup(tmp);

    return tmp;
}

QObject* KarambaInterface::createTaskIcon(Karamba *k, int x, int y, int ctask)
{
    if (!checkKaramba(k))
        return NULL;

    QList<Task::TaskPtr> tasks = TaskManager::self()->tasks().values();
    Task::TaskPtr task;
    Task::TaskPtr currTask;
    foreach(task, tasks) {
        if ((long)task.data() == (long)ctask) {
            //task found
            currTask = task;
            break;
        }
    }

    if (currTask.isNull()) {
        return 0;
    }

    //retrieve the QPixmap that represents this image
    QPixmap iconPixmap = KWin::icon(currTask->window());

    ImageLabel *tmp = new ImageLabel(k, x, y, 0, 0);
    tmp->setValue(iconPixmap);

    k->addToGroup(tmp);
    return tmp;
}

int KarambaInterface::getImageHeight(Karamba *k, ImageLabel *image)
{
    if (!checkKarambaAndMeter(k, image, "ImageLabel"))
        return -1;

    return image->getHeight();
}

int KarambaInterface::getImageWidth(Karamba *k, ImageLabel *image)
{
    if (!checkKarambaAndMeter(k, image, "ImageLabel"))
        return -1;

    return image->getWidth();
}

bool KarambaInterface::removeImageEffects(Karamba *k, ImageLabel *image)
{
    if (!checkKarambaAndMeter(k, image, "ImageLabel"))
        return false;

    image->removeEffects();
    return true;
}

bool KarambaInterface::removeImageTransformations(Karamba *k, ImageLabel *image)
{
    if (!checkKarambaAndMeter(k, image, "ImageLabel"))
        return false;

    image->removeImageTransformations();
    return true;
}

bool KarambaInterface::resizeImageSmooth(Karamba *k, ImageLabel *image, int width, int height)
{
    if (!checkKarambaAndMeter(k, image, "ImageLabel"))
        return false;

    k->scaleImageLabel(image, width, height);
    return true;
}

bool KarambaInterface::rotateImage(Karamba *k, ImageLabel *image, int deg)
{
    if (!checkKarambaAndMeter(k, image, "ImageLabel"))
        return false;

    image->rotate(deg);
    return true;
}




bool KarambaInterface::addMenuConfigOption(Karamba *k, QString key, QString name)
{
    if (!checkKaramba(k))
        return false;

    k->addMenuConfigOption(key, name);
    return true;
}

QString KarambaInterface::readConfigEntry(Karamba *k, QString key)
{
    if (!checkKaramba(k))
        return QString::null;

    return k->getConfig()->group("theme").readEntry(key, QString());
}

bool KarambaInterface::readMenuConfigOption(Karamba *k, QString key)
{
    if (!checkKaramba(k))
        return false;

    return k->readMenuConfigOption(key);
}

bool KarambaInterface::setMenuConfigOption(Karamba *k, QString key, bool value)
{
    if (!checkKaramba(k))
        return false;

    return k->setMenuConfigOption(key, value);
}

bool KarambaInterface::writeConfigEntry(Karamba *k, QString key, QString value)
{
    if (!checkKaramba(k))
        return false;

    k->getConfig()->group("theme").writeEntry(key, value);

    return true;
}






QObject* KarambaInterface::createInputBox(Karamba* k, int x, int y, int w, int h, QString text)
{
    if (!checkKaramba(k))
        return NULL;

    Input *tmp = new Input(k, x, y, w, h);
    tmp->setValue(text);
    tmp->setTextProps(k->getDefaultTextProps());

    k->addToGroup(tmp);

    return tmp;
}

bool KarambaInterface::deleteInputBox(Karamba *k, Input *input)
{
    if (!checkKarambaAndMeter(k, input, "Input"))
        return false;

    return k->removeMeter(input);
}

bool KarambaInterface::moveInputBox(Karamba *k, Input *input, int x, int y)
{
    return moveMeter(k, input, "Input", x, y);
}

QVariantList KarambaInterface::getInputBoxPos(Karamba *k, Input *input)
{
    return getMeterPos(k, input, "Input");
}

bool KarambaInterface::resizeInputBox(Karamba *k, Input *input, int width, int height)
{
    return resizeMeter(k, input, "Input", width, height);
}

QVariantList KarambaInterface::getInputBoxSize(Karamba *k, Input *input)
{
    return getMeterSize(k, input, "Input");
}

QObject* KarambaInterface::changeInputBoxValue(Karamba *k, Input *input, QString text)
{
    return setMeterStringValue(k, input, "Input", text);
}

QString KarambaInterface::getInputBoxValue(Karamba *k, Input *input)
{
    return getMeterStringValue(k, input, "Input");
}

QObject* KarambaInterface::getThemeInputBox(Karamba *k, QString meter)
{
    return getThemeMeter(k, meter, "Input");
}

bool KarambaInterface::hideInputBox(Karamba *k, Input *input)
{
    return hideMeter(k, input, "Input");
}

bool KarambaInterface::showInputBox(Karamba *k, Input *input)
{
    return showMeter(k, input, "Input");
}

bool KarambaInterface::changeInputBoxFont(Karamba *k, Input *input, QString font)
{
    if (!checkKarambaAndMeter(k, input, "Input"))
        return false;

    input->setFont(font);
    return true;
}

QString KarambaInterface::getInputBoxFont(Karamba *k, Input *input)
{
    if (!checkKarambaAndMeter(k, input, "Input"))
        return QString::null;

    return input->getFont();
}

bool KarambaInterface::changeInputBoxFontColor(Karamba *k, Input *input, int red, int green, int blue)
{
    if (!checkKarambaAndMeter(k, input, "Input"))
        return false;

    input->setFontColor(QColor(red, green, blue));
    return true;
}

QVariantList KarambaInterface::getInputBoxFontColor(Karamba *k, Input *input)
{
    if (!checkKarambaAndMeter(k, input, "Input"))
        return QVariantList();

    QVariantList ret;

    QColor color = input->getFontColor();
    ret << color.red();
    ret << color.green();
    ret << color.blue();

    return ret;
}

bool KarambaInterface::changeInputBoxSelectionColor(Karamba *k, Input *input, int red, int green, int blue)
{
    if (!checkKarambaAndMeter(k, input, "Input"))
        return false;

    input->setSelectionColor(QColor(red, green, blue));
    return true;
}

QVariantList KarambaInterface::getInputBoxSelectionColor(Karamba *k, Input *input)
{
    if (!checkKarambaAndMeter(k, input, "Input"))
        return QVariantList();

    QVariantList ret;

    QColor color = input->getSelectionColor();
    ret << color.red();
    ret << color.green();
    ret << color.blue();

    return ret;
}

bool KarambaInterface::changeInputBoxBackgroundColor(Karamba *k, Input *input, int red, int green, int blue)
{
    if (!checkKarambaAndMeter(k, input, "Input"))
        return false;

    input->setBGColor(QColor(red, green, blue));
    return true;
}

QVariantList KarambaInterface::getInputBoxBackgroundColor(Karamba *k, Input *input)
{
    if (!checkKarambaAndMeter(k, input, "Input"))
        return QVariantList();

    QVariantList ret;

    QColor color = input->getBGColor();
    ret << color.red();
    ret << color.green();
    ret << color.blue();

    return ret;
}

bool KarambaInterface::changeInputBoxFrameColor(Karamba *k, Input *input, int red, int green, int blue)
{
    if (!checkKarambaAndMeter(k, input, "Input"))
        return false;

    input->setColor(QColor(red, green, blue));
    return true;
}

QVariantList KarambaInterface::getInputBoxFrameColor(Karamba *k, Input *input)
{
    if (!checkKarambaAndMeter(k, input, "Input"))
        return QVariantList();

    QVariantList ret;

    QColor color = input->getColor();
    ret << color.red();
    ret << color.green();
    ret << color.blue();

    return ret;
}

bool KarambaInterface::changeInputBoxSelectedTextColor(Karamba *k, Input *input, int red, int green, int blue)
{
    if (!checkKarambaAndMeter(k, input, "Input"))
        return false;

    input->setSelectedTextColor(QColor(red, green, blue));
    return true;
}

QVariantList KarambaInterface::getInputBoxSelectedTextColor(Karamba *k, Input *input)
{
    if (!checkKarambaAndMeter(k, input, "Input"))
        return QVariantList();

    QVariantList ret;

    QColor color = input->getSelectedTextColor();
    ret << color.red();
    ret << color.green();
    ret << color.blue();

    return ret;
}

bool KarambaInterface::changeInputBoxFontSize(Karamba *k, Input *input, int size)
{
    if (!checkKarambaAndMeter(k, input, "Input"))
        return false;

    input->setFontSize(size);
    return true;
}

int KarambaInterface::getInputBoxFontSize(Karamba *k, Input *input)
{
    if (!checkKarambaAndMeter(k, input, "Input"))
        return 0;

    return input->getFontSize();
}

bool KarambaInterface::setInputFocus(Karamba *k, Input *input)
{
    if (!checkKarambaAndMeter(k, input, "Input"))
        return false;

    input->setInputFocus();
    return true;
}

bool KarambaInterface::clearInputFocus(Karamba *k, Input *input)
{
    if (!checkKarambaAndMeter(k, input, "Input"))
        return false;

    input->clearInputFocus();
    return true;
}

QObject* KarambaInterface::getInputFocus(Karamba *k)
{
    if (!checkKaramba(k))
        return 0;

    QGraphicsItem *focusItem = k->getScene()->focusItem();
    return dynamic_cast<Input*>(focusItem);
}



bool KarambaInterface::menuExists(Karamba* k, KMenu* menu)
{
    return k->popupMenuExisting(menu);
}


QObject* KarambaInterface::addMenuItem(Karamba *k, KMenu *menu, QString text, QString icon)
{
    if (!checkKaramba(k))
        return 0;

    QAction *action = 0;
    if (menuExists(k, menu)) {
        action = k->addMenuItem(menu, text, icon);
    }

    return action;
}

QObject* KarambaInterface::addMenuSeparator(Karamba *k, KMenu *menu)
{
    if (!checkKaramba(k))
        return 0;

    QAction *sep = 0;
    if (menuExists(k, menu)) {
        sep = menu->addSeparator();
    }

    return sep;
}

QObject* KarambaInterface::createMenu(Karamba *k)
{
    if (!checkKaramba(k))
        return 0;

    return k->addPopupMenu();
}

bool KarambaInterface::deleteMenu(Karamba *k, KMenu *menu)
{
    if (!checkKaramba(k))
        return false;

    if (menuExists(k, menu)) {
        k->deletePopupMenu(menu);
        return true;
    }

    return false;
}


bool KarambaInterface::popupMenu(Karamba *k, KMenu *menu, int x, int y)
{
    if (!checkKaramba(k))
        return false;

    if (menuExists(k, menu)) {
        k->popupMenu(menu, QPoint(x, y));
        return true;
    }

    return false;
}

bool KarambaInterface::removeMenuItem(Karamba *k, KMenu *menu, QAction *action)
{
    if (!checkKaramba(k))
        return false;

    if (menuExists(k, menu)) {
        k->deleteMenuItem(action);
        return true;
    }

    return false;
}






bool KarambaInterface::acceptDrops(Karamba *k) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    k->setAcceptDrops(true);
    return true;
}

bool KarambaInterface::attachClickArea(const Karamba *k, Meter *m,
    const QString &leftButton, const QString &middleButton, const QString &rightButton) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    if (ImageLabel* image = dynamic_cast<ImageLabel*>(m)) {
        image->attachClickArea(leftButton, middleButton, rightButton);
        image->allowClick(true);
    } else if (TextLabel* text = dynamic_cast<TextLabel*>(m)) {
        text->attachClickArea(leftButton, middleButton, rightButton);
        text->allowClick(true);
    } else {
        return false;
    }

    return true;
}

bool KarambaInterface::callTheme(const Karamba *k, const QString &theme, const QString &info) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    return k->sendDataToTheme(theme, info);
}

bool KarambaInterface::changeInterval(Karamba *k, int interval) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    k->changeInterval(interval);
    return true;
}

int KarambaInterface::execute(const QString &command) const
{
    return KRun::runCommand(command);
}

QObject* KarambaInterface::createClickArea(Karamba *k, int x, int y, int width, int height,
        const QString &onClick) const
{
    if (!checkKaramba(k)) {
        return 0;
    }

    ClickArea *tmp = new ClickArea(k, false, x, y, width, height);
    tmp->setOnClick(onClick);

    k->addToGroup(tmp);

    return tmp;
}

bool KarambaInterface::removeClickArea(Karamba *k, ClickArea *area) const
{
    if (!checkKarambaAndMeter(k, area, "ClickArea")) {
        return false;
    }

    k->removeMeter(area);
    return true;
}

QObject* KarambaInterface::createServiceClickArea(Karamba *k, int x, int y, int width, int height,
        const QString &name, const QString &exec, const QString &icon) const
{
    if (!checkKaramba(k)) {
        return 0;
    }

    ClickArea *tmp = new ClickArea(k, false, x, y, width, height);
    tmp->setServiceOnClick(name, exec, icon);

    k->addToGroup(tmp);

    return tmp;
}

int KarambaInterface::executeInteractive(Karamba *k, const QStringList &command)
{
    if (!checkKaramba(k)) {
        return 0;
    }

    k->currProcess = new KProcess;
    *(k->currProcess) << command;

    connect(k->currProcess,
                          SIGNAL(processExited(KProcess *)),
                          k,
                          SLOT(processExited(KProcess *)));

    connect(k->currProcess,
                          SIGNAL(receivedStdout(KProcess *, char *, int)),
                          k,
                          SLOT(receivedStdout(KProcess *, char *, int)));

    k->currProcess->start(KProcess::NotifyOnExit, KProcess::Stdout);

    return k->currProcess->pid();
}

QString KarambaInterface::getIP(const Karamba *k, QString interface) const
{
    if (!checkKaramba(k)) {
        return QString::null;
    }

    QNetworkInterface iface = QNetworkInterface::interfaceFromName(interface);
    return iface.addressEntries()[0].ip().toString();
}

int KarambaInterface::getNumberOfDesktops(const Karamba *k) const
{
    if (!checkKaramba(k)) {
        return 0;
    }

    return k->getNumberOfDesktops();
}

QString KarambaInterface::getPrettyThemeName(const Karamba *k) const
{
    if (!checkKaramba(k)) {
        return QString::null;
    }

    return k->prettyName();
}

QStringList KarambaInterface::getServiceGroups(const Karamba *k, QString path) const
{
    if (!checkKaramba(k)) {
        return QStringList();
    }

    // Use QHash/QMap for this?
    return QStringList();
}

QString KarambaInterface::getThemePath(const Karamba *k) const
{
    if (!k) {
         k = m_karamba;
    }

    if (!checkKaramba(k)) {
        return QString::null;
    }

    return k->theme().path();
}

double KarambaInterface::getUpdateTime(const Karamba *k) const
{
    if (!checkKaramba(k)) {
        return 0;
    }

    return k->getUpdateTime();
}

bool KarambaInterface::setUpdateTime(Karamba *k, double updateTime) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    k->setUpdateTime(updateTime);
    return true;
}

bool KarambaInterface::hide(Karamba *k) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    k->hide();
    return true;
}

bool KarambaInterface::show(Karamba *k) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    k->show();
    return true;
}

QString KarambaInterface::language(const Karamba *k) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    return k->theme().locale()->language();
}

bool KarambaInterface::managementPopup(const Karamba *k) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    k->popupGlobalMenu();
    return true;
}

Karamba* KarambaInterface::openNamedTheme(const QString &themePath, const QString &themeName,
        bool subTheme) const
{
    Karamba *newTheme;
    QFileInfo file(themePath);

    if (file.exists()) {
        QString prettyName(themeName);
        KarambaApplication* app = (KarambaApplication*)qApp;
        if (!app->themeExists(prettyName)) {
            newTheme = new Karamba(KUrl(themePath));
            newTheme->show();
        }
    }
    return newTheme;
}

Karamba* KarambaInterface::openTheme(const QString &themePath) const
{
    Karamba *newTheme;
    QFileInfo file(themePath);

    if (file.exists()) {
        newTheme = new Karamba(KUrl(themePath));
        newTheme->show();
    }

    return newTheme;
}

QString KarambaInterface::readThemeFile(const Karamba *k, const QString &file) const
{
    if (!checkKaramba(k)) {
        return QString::null;
    }

    return k->theme().readThemeFile(file);
}

bool KarambaInterface::reloadTheme(Karamba *k) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    k->reloadConfig();
    return true;
}

bool KarambaInterface::run(const QString &appName, const QString &command, const QString &icon,
        const QStringList &arguments)
{
    KService service(appName, command, icon);

    KRun::run(service, arguments, 0);

    return true;
}

QVariant KarambaInterface::getIncommingData(const Karamba *k) const
{
    if (!checkKaramba(k)) {
        return QVariant();
    }

    return k->retrieveReceivedData();
}


// Is the theme path or the pretty name required?
bool KarambaInterface::setIncommingData(const Karamba *k, const QString &themePath, QVariant data)
    const
{
    if (checkKaramba(k)) {
        return false;
    }

    return k->sendData();
}

bool KarambaInterface::toggleShowDesktop(const Karamba *k) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    ShowDesktop::self()->toggle();
    return true;
}

bool KarambaInterface::translateAll(const Karamba *k, int x, int y) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    QList<QGraphicsItem*> items = ((QGraphicsItemGroup*)k)->children();

    foreach(QGraphicsItem *item, items) {
        Meter *meter = dynamic_cast<Meter*>(item);
        meter->setSize(meter->getX() + x,
                        meter->getY() + y,
                        meter->getWidth(),
                        meter->getHeight());
    }

    return true;
}

QString KarambaInterface::userLanguage(const Karamba *k) const
{
    if (!checkKaramba(k)) {
        return QString::null;
    }

    return KGlobal::locale()->language();
}

QStringList KarambaInterface::userLanguages(const Karamba *k) const
{
    if (!checkKaramba(k)) {
        return QStringList();
    }

    return KGlobal::locale()->languageList();
}

bool KarambaInterface::wantRightButton(Karamba *k, bool enable) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    k->setWantRightButton(enable);
    return true;
}







QObject* KarambaInterface::createRichText(Karamba* k, QString text, bool underline)
{
    if (!checkKaramba(k))
        return NULL;

    RichTextLabel *tmp = new RichTextLabel(k);
    tmp->setText(text, underline);
    tmp->setTextProps(k->getDefaultTextProps());

    k->addToGroup(tmp);

    return tmp;
}

bool KarambaInterface::deleteRichText(Karamba *k, RichTextLabel *label)
{
    if (!checkKarambaAndMeter(k, label, "RichTextLabel"))
        return false;

    return k->removeMeter(label);
}

bool KarambaInterface::moveRichText(Karamba *k, RichTextLabel *label, int x, int y)
{
    return moveMeter(k, label, "RichTextLabel", x, y);
}

QVariantList KarambaInterface::getRichTextPos(Karamba *k, RichTextLabel *label)
{
    return getMeterPos(k, label, "RichTextLabel");
}

bool KarambaInterface::resizeRichText(Karamba *k, RichTextLabel *label, int width, int height)
{
    return resizeMeter(k, label, "RichTextLabel", width, height);
}

bool KarambaInterface::setRichTextWidth(Karamba *k, RichTextLabel *label, int width)
{
    if (!checkKarambaAndMeter(k, label, "RichTextLabel"))
        return false;

    label->setWidth(width);
    return true;
}

QVariantList KarambaInterface::getRichTextSize(Karamba *k, RichTextLabel *label)
{
    return getMeterSize(k, label, "RichTextLabel");
}

QObject* KarambaInterface::changeRichText(Karamba *k, RichTextLabel *label, QString text)
{
    return setMeterStringValue(k, label, "RichTextLabel", text);
}

QString KarambaInterface::getRichTextValue(Karamba *k, RichTextLabel *label)
{
    return getMeterStringValue(k, label, "RichTextLabel");
}

QObject* KarambaInterface::getThemeRichText(Karamba *k, QString meter)
{
    return getThemeMeter(k, meter, "RichTextLabel");
}

bool KarambaInterface::hideRichText(Karamba *k, RichTextLabel *label)
{
    return hideMeter(k, label, "RichTextLabel");
}

bool KarambaInterface::showRichText(Karamba *k, RichTextLabel *label)
{
    return showMeter(k, label, "RichTextLabel");
}

bool KarambaInterface::changeRichTextFont(Karamba *k, RichTextLabel *label, QString font)
{
    if (!checkKarambaAndMeter(k, label, "RichTextLabel"))
        return false;

    label->setFont(font);
    return true;
}

QString KarambaInterface::getRichTextFont(Karamba *k, RichTextLabel *label)
{
    if (!checkKarambaAndMeter(k, label, "RichTextLabel"))
        return QString::null;

    return label->getFont();
}

bool KarambaInterface::changeRichTextSize(Karamba *k, RichTextLabel *label, int size)
{
    if (!checkKarambaAndMeter(k, label, "RichTextLabel"))
        return false;

    label->setFontSize(size);
    return true;
}

int KarambaInterface::getRichTextFontSize(Karamba *k, RichTextLabel *label)
{
    if (!checkKarambaAndMeter(k, label, "RichTextLabel"))
        return 0;

    return label->getFontSize();
}

bool KarambaInterface::setRichTextSensor(Karamba *k, RichTextLabel *label, QString sensor)
{
    return setMeterSensor(k, label, "RichTextLabel", sensor);
}

QString KarambaInterface::getRichTextSensor(Karamba *k, RichTextLabel *label)
{
    return getMeterSensor(k, label, "RichTextLabel");
}



QObject* KarambaInterface::createText(Karamba* k, int x, int y, int width, int height, QString text)
{
    if (!checkKaramba(k))
        return NULL;

    TextLabel *tmp = new TextLabel(k, x, y, width, height);
    tmp->setValue(text);
    tmp->setTextProps(k->getDefaultTextProps());

    k->addToGroup(tmp);

    return tmp;
}

bool KarambaInterface::deleteText(Karamba *k, TextLabel *text)
{
    if (!checkKarambaAndMeter(k, text, "TextLabel"))
        return false;

    return k->removeMeter(text);
}

bool KarambaInterface::moveText(Karamba *k, TextLabel *text, int x, int y)
{
    return moveMeter(k, text, "TextLabel", x, y);
}

QVariantList KarambaInterface::getTextPos(Karamba *k, TextLabel *text)
{
    return getMeterPos(k, text, "TextLabel");
}

bool KarambaInterface::setTextSensor(Karamba *k, TextLabel *text, QString sensor)
{
    return setMeterSensor(k, text, "TextLabel", sensor);
}

QString KarambaInterface::getTextSensor(Karamba *k, TextLabel *text)
{
    return getMeterSensor(k, text, "TextLabel");
}

bool KarambaInterface::resizeText(Karamba *k, TextLabel *text, int width, int height)
{
    return resizeMeter(k, text, "TextLabel", width, height);
}

QVariantList KarambaInterface::getTextSize(Karamba *k, TextLabel *text)
{
    return getMeterSize(k, text, "TextLabel");
}

QObject* KarambaInterface::getThemeText(Karamba *k, QString meter)
{
    return getThemeMeter(k, meter, "TextLabel");
}

bool KarambaInterface::hideText(Karamba *k, TextLabel *text)
{
    return hideMeter(k, text, "TextLabel");
}

bool KarambaInterface::showText(Karamba *k, TextLabel *text)
{
    return showMeter(k, text, "TextLabel");
}

QObject* KarambaInterface::changeText(Karamba *k, TextLabel *label, QString text)
{
    return setMeterStringValue(k, label, "TextLabel", text);
}

QString KarambaInterface::getTextValue(Karamba *k, TextLabel *label)
{
    return getMeterStringValue(k, label, "TextLabel");
}

bool KarambaInterface::changeTextShadow(Karamba *k, TextLabel *label, int shadow)
{
    if (!checkKarambaAndMeter(k, label, "TextLabel"))
        return false;

    label->setShadow(shadow);
    return true;
}

int KarambaInterface::getTextShadow(Karamba *k, TextLabel *text)
{
    if (!checkKarambaAndMeter(k, text, "TextLabel"))
        return -1;

    return text->getShadow();
}

bool KarambaInterface::changeTextFont(Karamba *k, TextLabel *text, QString font)
{
    if (!checkKarambaAndMeter(k, text, "TextLabel"))
        return false;

    text->setFont(font);
    return true;
}

QString KarambaInterface::getTextFont(Karamba *k, TextLabel *text)
{
    if (!checkKarambaAndMeter(k, text, "TextLabel"))
        return false;

    return text->getFont();
}

bool KarambaInterface::changeTextColor(Karamba *k, TextLabel *text, int red, int green, int blue)
{
    return setMeterColor(k, text, "TextLabel", red, green, blue);
}

QVariantList KarambaInterface::getTextColor(Karamba *k, TextLabel *text)
{
    return getMeterColor(k, text, "TextLabel");
}

bool KarambaInterface::changeTextSize(Karamba *k, TextLabel *text, int size)
{
    if (!checkKarambaAndMeter(k, text, "TextLabel"))
        return false;

    text->setFontSize(size);
    return true;
}

int KarambaInterface::getTextFontSize(Karamba *k, TextLabel *text)
{
    if (!checkKarambaAndMeter(k, text, "TextLabel"))
        return -1;

    return text->getFontSize();
}

QString KarambaInterface::getTextAlign(Karamba *k, TextLabel *text)
{
    if (!checkKarambaAndMeter(k, text, "TextLabel"))
        return false;

    return text->getAlignment();
}

bool KarambaInterface::setTextAlign(Karamba *k, TextLabel *text, QString alignment)
{
    if (!checkKarambaAndMeter(k, text, "TextLabel"))
        return false;

    text->setAlignment(alignment);
    return true;
}

bool KarambaInterface::setTextScroll(Karamba *k, TextLabel *text, QString type, int x, int y, int gap, int pause)
{
    if (!checkKarambaAndMeter(k, text, "TextLabel"))
        return false;

    text->setScroll(type, QPoint(x, y), gap, pause);
    return true;
}



// This is kept for compatibility only
bool KarambaInterface::createWidgetMask(Karamba *k, QString mask)
{
    Q_UNUSED(k);
    Q_UNUSED(mask);

    return true;
}

QVariantList KarambaInterface::getWidgetPosition(Karamba *k)
{
    if (!checkKaramba(k))
        return QVariantList();

    QPoint pos = k->getPosition();
    QVariantList ret;
    ret << pos.x();
    ret << pos.y();

    return ret;
}

bool KarambaInterface::moveWidget(Karamba *k, int x, int y)
{
    if (!checkKaramba(k))
        return false;

    k->moveToPos(QPoint(x, y));
    return true;
}

bool KarambaInterface::redrawWidget(Karamba *k)
{
    if (!checkKaramba(k))
        return false;

    foreach(QGraphicsItem *item, ((QGraphicsItemGroup*)k)->children())
    item->update();

    return true;
}

// This is kept for compatibility only
bool KarambaInterface::redrawWidgetBackground(Karamba *k)
{
    Q_UNUSED(k);

    return true;
}

bool KarambaInterface::resizeWidget(Karamba *k, int width, int height)
{
    if (!checkKaramba(k))
        return false;

    k->resizeTo(width, height);
    return true;
}

// This is kept for compatibility only
bool KarambaInterface::toggleWidgetRedraw(Karamba *k, bool enable)
{
    Q_UNUSED(k);
    Q_UNUSED(enable);

    return true;
}

