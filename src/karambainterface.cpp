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
#include <QTimer>
#include <QDomDocument>
#include <QFileInfo>

#include <kdebug.h>
#include <kmenu.h>
#include <krun.h>
#include <kservice.h>
#include <klocale.h>

#include <kross/core/manager.h>
#include <kross/core/action.h>
//#include <kross/core/actioncollection.h>

#include "karamba.h"
#include "karambaapp.h"
#include "lineparser.h"
#include "showdesktop.h"
#include "karambainterface.h"
#include "karambainterface.moc"

/// \internal d-pointer class.
class KarambaInterface::Private
{
    public:
        /**
        * This is the in the constructor passed \a Karamba instance
        * and used e.g. at the \a getThemePath method to be able
        * to fetch the theme-path even outside of the functions if
        * there is no access to the Karamba-pointer.
        */
        Karamba *karamba;

        /**
        * The \a Kross::Action instance provides access to the
        * scripting backends.
        */
        Kross::Action *action;

        Private(Karamba *k) : karamba(k), action(0) {}
};

KarambaInterface::KarambaInterface(Karamba *k)
    : QObject()
    , d(new Private(k))
{
    setObjectName("karamba");

    // Publish this QObject. It will be available with e.g.
    //     import Karamba
    //     print dir(Karamba)
    Kross::Manager::self().addObject(this, "karamba");

    // Get the python theme file
    QString scriptFile = k->theme().path() + "/" + k->theme().pythonModule() + ".py";
    if( ! QFileInfo(scriptFile).exists() ) {
        kWarning() << "Python theme script file not found: " << scriptFile << endl;
    }
    else {
        kDebug() << "Python theme script file: " << scriptFile << endl;

        QFileInfo fi(scriptFile);
        d->action = new Kross::Action(this, scriptFile, fi.dir());
        d->action->setInterpreter("python");
        d->action->setCode( QString(
                "import karamba\n"

                "import sys\n"
                "sys.path.insert(0,karamba.getThemePath())\n"
                "sys.path.insert(0,'')\n"

                "execfile(\"%1\", globals(), locals())\n"

                "try: karamba.connect('initWidget(QObject*)',initWidget)\n"
                "except NameError: pass\n"
                "try: karamba.connect('widgetUpdated(QObject*)',widgetUpdated)\n"
                "except NameError: pass\n"
                "try: karamba.connect('widgetClosed(QObject*)',widgetClosed)\n"
                "except NameError: pass\n"
                "try: karamba.connect('menuItemClicked(QObject*, QObject*, QObject*)',menuItemClicked)\n"
                "except NameError: pass\n"
                "try: karamba.connect('menuOptionChanged(QObject*, QString, bool)',menuOptionChanged)\n"
                "except NameError: pass\n"
                "try: karamba.connect('activeTaskChanged(QObject*, long long)',activeTaskChanged)\n"
                "except NameError: pass\n"
                "try: karamba.connect('taskAdded(QObject*, long long)',taskAdded)\n"
                "except NameError: pass\n"
                "try: karamba.connect('taskRemoved(QObject*, long long)', taskRemoved)\n"
                "except NameError: pass\n"
                "try: karamba.connect('startupAdded(QObject*, long long)',startupAdded)\n"
                "except NameError: pass\n"
                "try: karamba.connect('startupRemoved(QObject*, long long)',startupRemoved)\n"
                "except NameError: pass\n"
                "try: karamba.connect('commandFinished(QObject*, int)',commandFinished)\n"
                "except NameError: pass\n"
                "try: karamba.connect('itemDropped(QObject*, QString, int, int)',itemDropped)\n"
                "except NameError: pass\n"
                "try: karamba.connect('meterClicked(QObject*, QObject*, int)',meterClicked)\n"
                "except NameError: pass\n"
                "try: karamba.connect('widgetClicked(QObject*, int, int, int)',widgetClicked)\n"
                "except NameError: pass\n"
                "try: karamba.connect('desktopChanged(QObject*, int)',desktopChanged)\n"
                "except NameError: pass\n"
                "try: karamba.connect('widgetMouseMoved(QObject*, int, int, int)',widgetMouseMoved)\n"
                "except NameError: pass\n"
                "try: karamba.connect('keyPressed(QObject*, QObject*, QString)',keyPressed)\n"
                "except NameError: pass\n"
            ).arg(scriptFile)
        );

        // Finally let's execute the actual python theme script file.
        d->action->trigger();
        //QTimer::singleShot(0, d->action, SLOT(trigger()));
    }
}

KarambaInterface::~KarambaInterface()
{
    delete d;
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
    if (!checkKarambaAndMeter(k, m, type)) {
        return QVariantList();
    }

    QVariantList ret;
    ret << m->getMax();
    ret << m->getMin();
    return ret;
}

QVariantList KarambaInterface::getMeterSize(const Karamba *k, const Meter *m, const QString &type)
    const
{
    if (!checkKarambaAndMeter(k, m, type)) {
        return QVariantList();
    }

    QVariantList list;
    list << QVariant::fromValue(m->getWidth());
    list << QVariant::fromValue(m->getHeight());
    return list;
}

QVariantList KarambaInterface::getMeterPos(const Karamba *k, const Meter *m, const QString &type)
    const
{
    if (!checkKarambaAndMeter(k, m, type)) {
        return QVariantList();
    }

    QVariantList ret;
    ret << QVariant::fromValue(m->getX());
    ret << QVariant::fromValue(m->getY());
    return ret;
}

QString KarambaInterface::getMeterSensor(const Karamba *k, const Meter *m, const QString &type)
    const
{
    if (!checkKarambaAndMeter(k, m, type)) {
        return QString::null;
    }

    return k->getSensor(m);

}

int KarambaInterface::getMeterValue(const Karamba *k, const Meter *m, const QString &type) const
{
    if (!checkKarambaAndMeter(k, m, type)) {
        return 0;
    }

    return m->getValue();
}

QObject* KarambaInterface::getThemeMeter(const Karamba *k, const QString &meter, const QString
        &type) const
{
    if (!checkKaramba(k)) {
        return 0;
    }

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
    if (!checkKarambaAndMeter(k, m, type)) {
        return false;
    }

    m->hide();

    return true;
}

bool KarambaInterface::moveMeter(const Karamba *k, Meter *m, const QString &type, int x, int y)
    const
{
    if (!checkKarambaAndMeter(k, m, type)) {
        return false;
    }

    k->moveMeter(m, x, y);

    return true;
}

bool KarambaInterface::resizeMeter(const Karamba *k, Meter *m, const QString &type, int width, int
        height) const
{
    if (!checkKarambaAndMeter(k, m, type)) {
        return false;
    }

    m->setSize(m->getX(), m->getY(), width, height);

    return true;
}

bool KarambaInterface::setMeterMinMax(const Karamba *k, Meter *m, const QString &type, int min, int max)
    const
{
    if (!checkKarambaAndMeter(k, m, type)) {
        return false;
    }

    m->setMin(min);
    m->setMax(max);

    return true;
}

bool KarambaInterface::setMeterSensor(Karamba *k, Meter *m, const QString &type,
        const QString &sensor) const
{
    if (!checkKarambaAndMeter(k, m, type)) {
        return false;
    }

    k->setSensor(LineParser(sensor), m);

    return true;
}

QObject* KarambaInterface::setMeterValue(const Karamba *k, Meter *m, const QString &type, int value)
    const
{
    if (!checkKarambaAndMeter(k, m, type)) {
        return 0;
    }

    m->setValue(value);

    return m;
}

bool KarambaInterface::showMeter(const Karamba *k, Meter *m, const QString &type) const
{
    if (!checkKarambaAndMeter(k, m, type)) {
        return false;
    }

    m->show();

    return true;
}

bool KarambaInterface::setMeterColor(const Karamba *k, Meter *m, const QString &type, int red, int
        green, int blue) const
{
    if (!checkKarambaAndMeter(k, m, type)) {
        return false;
    }

    m->setColor(QColor(red, green, blue));

    return true;
}

QVariantList KarambaInterface::getMeterColor(const Karamba *k, const Meter *m, const QString &type)
    const
{
    if (!checkKarambaAndMeter(k, m, type)) {
        return QVariantList();
    }

    QColor color = m->getColor();

    QVariantList ret;
    ret << color.red();
    ret << color.green();
    ret << color.blue();

    return ret;
}

QString KarambaInterface::getMeterStringValue(const Karamba *k, const Meter *m, const QString &type)
    const
{
    if (!checkKarambaAndMeter(k, m, type)) {
        return QString::null;
    }

    return m->getStringValue();
}

QObject* KarambaInterface::setMeterStringValue(const Karamba *k, Meter *m, const QString &type,
        const QString &value) const
{
    if (!checkKarambaAndMeter(k, m, type)) {
        return 0;
    }

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
*   reference createBar(widget, x, y, w, h, image)
* DESCRIPTION
*   This creates a bar at x, y with width and height w, h.
* ARGUMENTS
*   * reference to widget -- karamba
*   * integer x -- x coordinate
*   * integer y -- y coordinate
*   * integer w -- width
*   * integer h -- height
*   * string image -- Path to image
* RETURN VALUE
*   Reference to new bar meter
*/
QObject* KarambaInterface::createBar(Karamba* k, int x, int y, int w, int h, const QString &path)
    const
{
    if (!checkKaramba(k)) {
        return NULL;
    }

    Bar *tmp = new Bar(k, x, y, w, h);
    tmp->setImage(path);
    tmp->setValue(50);
    k->addToGroup(tmp);

    return tmp;
}

/** Bar/deleteBar
*
* SYNOPSIS
*   boolean deleteBar(widget, bar)
* DESCRIPTION
*   This deletes the bar.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to bar -- bar
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::deleteBar(Karamba *k, Bar *bar) const
{
    if (!checkKarambaAndMeter(k, bar, "Bar")) {
        return false;
    }

    return k->removeMeter(bar);
}

/** Bar/setBarMinMax
*
* SYNOPSIS
*   boolean setBarMinMax(widget, bar, min, max)
* DESCRIPTION
*   Returns current bar value.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to bar -- bar
*   * integer min -- min value
*   * integer max -- max value
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::setBarMinMax(const Karamba *k, Bar *bar, int min, int max) const
{
    return setMeterMinMax(k, bar, "Bar", min, max);
}

/** Bar/getBarMinMax
*
* SYNOPSIS
*   array getBarMinMax(widget, bar)
* DESCRIPTION
*   Returns possible min and max values of the bar.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference bar -- bar
* RETURN VALUE
*   array with max & min values
*/
QVariantList KarambaInterface::getBarMinMax(const Karamba *k, const Bar *bar) const
{
    return getMeterMinMax(k, bar, "Bar");
}

/** Bar/moveBar
*
* SYNOPSIS
*   boolean moveBar(widget, bar, x, y)
* DESCRIPTION
*   This will move the bar to new x and y coordinates.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to bar -- bar
*   * integer x -- x coordinate
*   * integer y -- y coordinate
* RETURN VALUE
*   boolean (true if successful)
*/
bool KarambaInterface::moveBar(const Karamba *k, Bar *bar, int x, int y) const
{
    return moveMeter(k, bar, "Bar", x, y);
}

/** Bar/getBarPos
*
* SYNOPSIS
*   array getBarPos(widget, bar)
* DESCRIPTION
*   Given a reference to a bar object, this will return a array
*   containing the x and y coordinate of a bar object.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to bar -- bar
* RETURN VALUE
*   array with x and y coordinates
*/
QVariantList KarambaInterface::getBarPos(const Karamba *k, const Bar *bar) const
{
    return getMeterPos(k, bar, "Bar");
}

/** Bar/setBarSensor
*
* SYNOPSIS
*   boolean setBarSensor(widget, bar, sensor)
* DESCRIPTION
*   Sets the sensor string of the bar.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to bar -- bar
*   * string sensor -- new sensor string as in theme files
* RETURN VALUE
*   true if successful
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
*   Gets the current sensor string.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to bar -- bar
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
*  boolean resizeBar(widget, bar, w, h)
* DESCRIPTION
*   This will resize bar to new height and width.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to bar -- bar
*   * integer w -- new width
*   * integer h -- new height
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::resizeBar(const Karamba *k, Bar *bar, int width, int height) const
{
    return resizeMeter(k, bar, "Bar", width, height);
}

/** Bar/getBarSize
*
* SYNOPSIS
*   array getBarSize(widget, bar)
* DESCRIPTION
*   Given a reference to a bar object, this will return a array
*   containing the height and width of the bar object.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to bar -- bar
* RETURN VALUE
*   array with width and height
*/
QVariantList KarambaInterface::getBarSize(const Karamba *k, const Bar *bar) const
{
    return getMeterSize(k, bar, "Bar");
}

/** Bar/setBarValue
*
* SYNOPSIS
*   reference setBarValue(widget, bar, value)
* DESCRIPTION
*   Sets current bar value.
* ARGUMENTS
*   * long widget -- karamba
*   * long bar -- pointer to bar
*   * long value -- new value
* RETURN VALUE
*   reference to bar
*/
QObject* KarambaInterface::setBarValue(const Karamba *k, Bar *bar, int value) const
{
    return setMeterValue(k, bar, "Bar", value);
}

/** Bar/getBarValue
*
* SYNOPSIS
*   integer getBarValue(widget, bar)
* DESCRIPTION
*   Returns current bar value.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to bar -- bar
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
*   reference getThemeBar(widget, name)
* DESCRIPTION
*   You can reference a bar in your python code that was created in the
*   theme file. Basically, you just add a NAME= value to the BAR line in
*   the .theme file. Then if you want to use that object, instead of calling
*   createBar, you can call this function.
*
*   The name you pass to the function is the same one that you gave it for
*   the NAME= parameter in the .theme file.
* ARGUMENTS
*   * reference to widget -- karamba
*   * string name -- name of the bar in the theme file
* RETURN VALUE
*   reference to bar
*/
QObject* KarambaInterface::getThemeBar(const Karamba *k, const QString &meter) const
{
    return getThemeMeter(k, meter, "Bar");
}

/** Bar/hideBar
*
* SYNOPSIS
*   boolean hideBar(widget, bar)
* DESCRIPTION
*   This hides an bar. In other words, during subsequent calls to
*   widgetUpdate(), this bar will not be drawn.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to bar -- bar
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::hideBar(const Karamba *k, Bar *bar) const
{
    return hideMeter(k, bar, "Bar");
}

/** Bar/showBar
*
* SYNOPSIS
*   boolean showBar(widget, bar)
* DESCRIPTION
*   This shows an bar. In other words, during subsequent calls to
*   widgetUpdate(), this bar will be drawn.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to bar -- bar
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::showBar(const Karamba *k, Bar *bar) const
{
    return showMeter(k, bar, "Bar");
}

/** Bar/setBarVertical
*
* SYNOPSIS
*   boolean setBarVertical(widget, bar)
* DESCRIPTION
*   Set bar vertical
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to bar -- bar
*   * boolean vertical -- true to draw the bar vertically
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::setBarVertical(const Karamba *k, Bar *bar, bool vert) const
{
    if (!checkKarambaAndMeter(k, bar, "Bar")) {
        return false;
    }

    bar->setVertical(vert);
    return true;
}

/** Bar/getBarVertical
*
* SYNOPSIS
*   boolean getBarVertical(widget, bar)
* DESCRIPTION
*   Check if bar is a vertical bar.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to bar -- bar
* RETURN VALUE
*   true if vertical
*/
bool KarambaInterface::getBarVertical(const Karamba *k, const Bar *bar) const
{
    if (!checkKarambaAndMeter(k, bar, "Bar")) {
        return false;
    }

    return bar->getVertical();
}

/** Bar/setBarImage
*
* SYNOPSIS
*   boolean setBarImage(widget, bar, image)
* DESCRIPTION
*   Use the image in path for the background as the bar
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to bar -- bar
*   * string image -- path to the new image
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::setBarImage(const Karamba *k, Bar *bar, const QString &image) const
{
    if (!checkKarambaAndMeter(k, bar, "Bar")) {
        return false;
    }

    return bar->setImage(image);
}

/** Bar/getBarImage
*
* SYNOPSIS
*   string getBarImage(widget, bar)
* DESCRIPTION
*   Get the path to the bar image.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to bar -- bar
* RETURN VALUE
*   path to bar image
*/
QString KarambaInterface::getBarImage(const Karamba *k, const Bar *bar) const
{
    if (!checkKarambaAndMeter(k, bar, "Bar")) {
        return QString::null;
    }

    return bar->getImage();
}




/** Graph/createGraph
*
* SYNOPSIS
*   reference createGraph(widget, x, y, width, height, points)
* DESCRIPTION
*   This creates a graph at x, y with width and height.
* ARGUMENTS
*   * reference to widget -- karamba
*   * integer x -- x coordinate
*   * integer y -- y coordinate
*   * integer w -- width
*   * integer h -- height
*   * integer points -- Number of points in graph
* RETURN VALUE
*   reference to new graph meter
*/
QObject* KarambaInterface::createGraph(Karamba* k, int x, int y, int w, int h, int points) const
{
    if (!checkKaramba(k)) {
        return NULL;
    }

    Graph *tmp = new Graph(k, x, y, w, h, points);
    k->addToGroup(tmp);

    return tmp;
}

/** Graph/deleteGraph
*
* SYNOPSIS
*   boolean deleteGraph(widget, graph)
* DESCRIPTION
*   This deletes graph.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to graph -- graph
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::deleteGraph(Karamba *k, Graph *graph) const
{
    if (!checkKarambaAndMeter(k, graph, "Graph")) {
        return false;
    }

    return k->removeMeter(graph);
}

/** Graph/setGraphMinMax
*
* SYNOPSIS
*   boolean setGraphMinMax(widget, graph, min, max)
* DESCRIPTION
*   Sets the graphs min and max values.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to graph -- graph
*   * integer min -- min value
*   * integer max -- max value
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::setGraphMinMax(const Karamba *k, Graph *graph, int min, int max) const
{
    return setMeterMinMax(k, graph, "Graph", min, max);
}

/** Graph/getGraphMinMax
*
* SYNOPSIS
*   array getGraphMinMax(widget, graph)
* DESCRIPTION
*   Returns current graph minimal and maximal values.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to graph -- graph
* RETURN VALUE
*   max and min values
*/
QVariantList KarambaInterface::getGraphMinMax(const Karamba *k, const Graph *graph) const
{
    return getMeterMinMax(k, graph, "Graph");
}

/** Graph/moveGraph
*
* SYNOPSIS
*   boolean moveGraph(widget, graph, x, y)
* DESCRIPTION
*   This will move the graph to the new x and y coordinates.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to graph -- graph
*   * integer x -- x coordinate
*   * integer y -- y coordinate
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::moveGraph(const Karamba *k, Graph *graph, int x, int y) const
{
    return moveMeter(k, graph, "Graph", x, y);
}

/** Graph/getGraphPos
*
* SYNOPSIS
*   array getGraphPos(widget, graph)
* DESCRIPTION
*   Given a reference to a graph object, this will return an array
*   containing the x and y coordinates of the graph object.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to graph -- graph
* RETURN VALUE
*   x and y coordinates
*/
QVariantList KarambaInterface::getGraphPos(const Karamba *k, const Graph *graph) const
{
    return getMeterPos(k, graph, "Graph");
}

/** Graph/setGraphSensor
*
* SYNOPSIS
*   boolean setGraphSensor(widget, graph, sensor)
* DESCRIPTION
*   Sets current sensor string of the graph.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to graph -- graph
*   * string sensor -- new sensor string as in theme files
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::setGraphSensor(Karamba *k, Graph *graph, const QString &sensor) const
{
    return setMeterSensor(k, graph, "Graph", sensor);
}

/** Graph/getGraphSensor
*
* SYNOPSIS
*   string getGraphSensor(widget, graph)
* DESCRIPTION
*   Gets the current sensor string of the graph.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to graph -- graph
* RETURN VALUE
*   sensor string
*/
QString KarambaInterface::getGraphSensor(const Karamba *k, const Graph *graph) const
{
    return getMeterSensor(k, graph, "Graph");
}

/** Graph/resizeGraph
*
* SYNOPSIS
*   boolean resizeGraph(widget, graph, width, height)
* DESCRIPTION
*   This will resize graph to new height and width.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to graph -- graph
*   * integer  width -- new width
*   * integer height -- new height
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::resizeGraph(const Karamba *k, Graph *graph, int width, int height) const
{
    return resizeMeter(k, graph, "Graph", width, height);
}

/** Graph/getGraphSize
*
* SYNOPSIS
*   array getGraphSize(widget, graph)
* DESCRIPTION
*   Given a reference to a graph object, this will return an array
*   containing the height and width of a graph object.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to graph -- graph
* RETURN VALUE
*   width and height of the graph
*/
QVariantList KarambaInterface::getGraphSize(const Karamba *k, const Graph *graph) const
{
    return getMeterSize(k, graph, "Graph");
}

/** Graph/setGraphValue
*
* SYNOPSIS
*   reference setGraphValue(widget, graph, value)
* DESCRIPTION
*   Sets current graph value.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to graph -- graph
*   * integer value -- new value
* RETURN VALUE
*   reference to graph
*/
QObject* KarambaInterface::setGraphValue(const Karamba *k, Graph *graph, int value) const
{
    return setMeterValue(k, graph, "Graph", value);
}

/** Graph/getGraphValue
*
* SYNOPSIS
*   integer getGraphValue(widget, graph)
* DESCRIPTION
*   Returns current graph value.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to graph -- graph
* RETURN VALUE
*   value
*/
int KarambaInterface::getGraphValue(const Karamba *k, const Graph *graph) const
{
    return getMeterValue(k, graph, "Graph");
}

/** Graph/getThemeGraph
*
* SYNOPSIS
*   reference getThemeGraph(widget, name)
* DESCRIPTION
*   You can reference graph in your python code that was created in the
*   theme file. Basically, you just add a NAME= value to the GRAPH line in
*   the .theme file. Then if you want to use that object, instead of calling
*   createGraph, you can call this function.
*
*   The name you pass to the function is the same one that you gave it for
*   the NAME= parameter in the .theme file.
* ARGUMENTS
*   * reference to widget -- karamba
*   * string name -- name of the graph in the theme file
* RETURN VALUE
*   reference to graph
*/
QObject* KarambaInterface::getThemeGraph(const Karamba *k, const QString &meter) const
{
    return getThemeMeter(k, meter, "Graph");
}

/** Graph/hideGraph
*
* SYNOPSIS
*   boolean hideGraph(widget, graph)
* DESCRIPTION
*   This hides a graph. In other words, during subsequent calls to
*   widgetUpdate(), this graph will not be drawn.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to graph -- graph
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::hideGraph(const Karamba *k, Graph *graph) const
{
    return hideMeter(k, graph, "Graph");
}

/** Graph/showGraph
*
* SYNOPSIS
*   boolean showGraph(widget, graph)
* DESCRIPTION
*   This shows a graph. In other words, during subsequent calls to
*   widgetUpdate(), this graph will be drawn.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to graph -- graph
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::showGraph(const Karamba *k, Graph *graph) const
{
    return showMeter(k, graph, "Graph");
}

/** Graph/setGraphColor
*
* SYNOPSIS
*   boolean setGraphColor(widget, graph, red, green, blue)
* DESCRIPTION
*   Sets current graph color.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to graph -- pointer to graph
*   * integer red -- red component of color
*   * integer green -- green component of color
*   * integer blue -- blue component of color
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::setGraphColor(const Karamba *k, Graph *graph, int red, int green, int blue)
    const
{
    return setMeterColor(k, graph, "Graph", red, green, blue);
}

/** Graph/getGraphColor
*
* SYNOPSIS
*   array getGraphColor(widget, graph)
* DESCRIPTION
*   Gets the current graph color components.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to graph -- graph
* RETURN VALUE
*   (red, green, blue)
*/
QVariantList KarambaInterface::getGraphColor(const Karamba *k, const Graph *graph) const
{
    return getMeterColor(k, graph, "Graph");
}





/** Image/createImage
*
* SYNOPSIS
*   reference createImage(widget, x, y, image)
* DESCRIPTION
*   This creates an image on your widget at x, y. The filename should be
*   given as the path parameter. In theory the image could be local or could
*   be a url. It works just like adding an image in your theme file. You
*   will need to save the return value to be able to call other functions on
*   your image, such as moveImage()
* ARGUMENTS
*   * reference to widget -- karamba
*   * integer x -- x coordinate
*   * integer y -- y coordinate
*   * string image -- image for the imagelabel
* RETURN VALUE
*   reference to new image meter
*/
QObject* KarambaInterface::createImage(Karamba* k, int x, int y, const QString &image) const
{
    if (!checkKaramba(k)) {
        return NULL;
    }

    ImageLabel *tmp = new ImageLabel(k, x, y, 0, 0);
    tmp->setValue(image);

    k->setSensor(LineParser(image), tmp);

    k->addToGroup(tmp);

    return tmp;
}

/** Image/deleteImage
*
* SYNOPSIS
*   boolean deleteImage(widget, image)
* DESCRIPTION
*   This removes the image from the widhet. Please do not call functions on "image"
*   after calling deleteImage, as it does not exist anymore and that could
*   cause crashes in some cases.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to image -- image
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::deleteImage(Karamba *k, ImageLabel *image) const
{
    if (!checkKarambaAndMeter(k, image, "ImageLabel")) {
        return false;
    }

    return k->removeMeter(image);
}

/** Image/moveImage
*
* SYNOPSIS
*   boolean moveImage(widget, image, x, y)
* DESCRIPTION
*   This moves an image to a new x, y relative to your widget. In other
*   words, (0,0) is the top corner of your widget, not the screen. The
*   imageToMove parameter is a reference to the image to move that you saved
*   as the return value from createImage()
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to image -- image
*   * integer x -- x coordinate
*   * integer y -- y coordinate
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::moveImage(Karamba *k, ImageLabel *image, int x, int y) const
{
    return moveMeter(k, image, "ImageLabel", x, y);
}

/** Image/getImagePos
*
* SYNOPSIS
*   array getImagePos(widget, image)
* DESCRIPTION
*   Given a reference to a image object, this will return a tuple
*   containing the x and y coordinate of a image object.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to image -- image
* RETURN VALUE
*   x and y coordinates of the image
*/
QVariantList KarambaInterface::getImagePos(const Karamba *k, const ImageLabel *image) const
{
    return getMeterPos(k, image, "ImageLabel");
}

/** Image/setImageSensor
*
* SYNOPSIS
*   long setImageSensor(widget, image, sensor)
* DESCRIPTION
*   Sets the current sensor string.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to image -- image
*   * string sensor -- new sensor string as in theme files
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::setImageSensor(Karamba *k, ImageLabel *image,
        const QString &sensor) const
{
    return setMeterSensor(k, image, "ImageLabel", sensor);
}

/** Image/getImageSensor
*
* SYNOPSIS
*   string getImageSensor(widget, image)
* DESCRIPTION
*   Gets the current sensor string.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to image -- image
* RETURN VALUE
*   sensor string
*/
QString KarambaInterface::getImageSensor(const Karamba *k, const ImageLabel *image) const
{
    return getMeterSensor(k, image, "ImageLabel");
}

/** Image/resizeImage
*
* SYNOPSIS
*   boolean resizeImage(widget, image, w, h)
* DESCRIPTION
*   This resizes your image to width, height. The image parameter is
*   a reference to an image that you saved as the return value from
*   createImage()
* ARGUMENTS
*   * reference widget -- karamba
*   * reference image -- image
*   * integer w -- width
*   * integer h -- height
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::resizeImage(const Karamba *k, ImageLabel *image, int width, int height) const
{
    return resizeMeter(k, image, "ImageLabel", width, height);
}

/** Image/getImageSize
*
* SYNOPSIS
*   array getImageSize(widget, image)
* DESCRIPTION
*   Given a reference to a image object, this will return an array
*   containing the height and width of a image object.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to image -- image
* RETURN VALUE
*   height and width of the image
*/
QVariantList KarambaInterface::getImageSize(const Karamba *k, const ImageLabel *image) const
{
    return getMeterSize(k, image, "ImageLabel");
}

/** Image/setImagePath
*
* SYNOPSIS
*   reference setImagePath(widget, image, path)
* DESCRIPTION
*   This will change image of a image widget.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to image -- image
*   * string path -- new path of the image
* RETURN VALUE
*   reference to the image
*/
QObject* KarambaInterface::setImagePath(const Karamba *k, ImageLabel *image, const QString &path)
    const
{
    return setMeterStringValue(k, image, "ImageLabel", path);
}

/** Image/getImagePath
*
* SYNOPSIS
*   string getImagePath(widget, image)
* DESCRIPTION
*   Returns the current image path.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to image -- image
* RETURN VALUE
*   path
*/
QString KarambaInterface::getImagePath(const Karamba *k, const ImageLabel *image) const
{
    return getMeterStringValue(k, image, "ImageLabel");
}

/** Image/getThemeImage
*
* SYNOPSIS
*   long getThemeImage(widget, name)
* DESCRIPTION
*   You can reference an image in your python code that was created in the
*   .theme file. Basically, you just add a NAME= value to the IMAGE line in
*   the .theme file. Then if you want to use that object, instead of calling
*   createImage, you can call this function.
*
*   The name you pass to the function is the same one that you gave it for
*   the NAME= parameter in the .theme file.
* ARGUMENTS
*   * reference to widget -- karamba
*   * string name -- name of the image in the theme files
* RETURN VALUE
*   reference to image
*/
QObject* KarambaInterface::getThemeImage(const Karamba *k, const QString &meter) const
{
    return getThemeMeter(k, meter, "ImageLabel");
}

/** Image/hideImage
*
* SYNOPSIS
*   boolean hideImage(widget, image)
* DESCRIPTION
*   This hides an image. In other words, during subsequent calls to
*   widgetUpdate(), this image will not be drawn.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to image -- image
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::hideImage(const Karamba *k, ImageLabel *image) const
{
    return hideMeter(k, image, "ImageLabel");
}

/** Image/showImage
*
* SYNOPSIS
*   boolean showImage(widget, image)
* DESCRIPTION
*   This shows a previously hidden image. It does not actually refresh the
*   image on screen. That is what redrawWidget() does.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to image -- image
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::showImage(const Karamba *k, ImageLabel *image) const
{
    return showMeter(k, image, "ImageLabel");
}

/** Image/addImageTooltip
*
* SYNOPSIS
*   boolean addImageTooltip(widget, image, text)
* DESCRIPTION
*   This creats a tooltip for image with text.
*
*   Note:
*   * If you move the image, the tooltip does not move! It stays! Do not
*     create a tooltip if the image is off-screen because you will not be
*     able to ever see it.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to image -- image
*   * string text -- tooltip text
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::addImageTooltip(const Karamba *k, ImageLabel *image, const QString &text)
    const
{
    if (!checkKarambaAndMeter(k, image, "ImageLabel")) {
        return false;
    }

    image->setTooltip(text);

    return true;
}

/** Image/changeImageChannelIntensity
*
* SYNOPSIS
*   boolean changeImageChannelIntensity(widget, image, ratio, channel, millisec)
* DESCRIPTION
*   Changes the "intensity" of the image color channel, which is similar to
*   it's brightness.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to image -- image
*   * decimal ratio -- -1.0 to 1.0 (dark to bright)
*   * string channel -- color channel (red|green|blue)
*   * integer millisec -- milliseconds before the image is restored (optional)
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::changeImageChannelIntensity(const Karamba *k, ImageLabel *image, double
        ratio, const QString &channel, int ms) const
{
    if (!checkKarambaAndMeter(k, image, "ImageLabel")) {
        return false;
    }

    image->channelIntensity(ratio, channel, ms);

    return true;
}

/** Image/changeImageIntensity
*
* SYNOPSIS
*   boolean changeImageIntensity(widget, image, ratio, millisec)
* DESCRIPTION
*   Changes the "intensity" of the image, which is similar to it's
*   brightness. ratio is a floating point number from -1.0 to 1.0 that
*   determines how much to brighten or darken the image. Millisec specifies
*   how long in milliseconds before the image is restored to it's original
*   form. This is useful for "mouse over" type animations. Using 0 for
*   millisec disables this feature and leaves the image permanently
*   affected.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to image -- image
*   * decimal ratio -- -1.0 to 1.0 (dark to bright)
*   * integer millisec -- milliseconds before the image is restored (optional)
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::changeImageIntensity(const Karamba *k, ImageLabel *image, double ratio, int
        ms) const
{
    if (!checkKarambaAndMeter(k, image, "ImageLabel")) {
        return false;
    }

    image->intensity(ratio, ms);

    return true;
}

/** Image/changeImageToGray
*
* SYNOPSIS
*   boolean changeImageToGray(widget, image, millisec)
* DESCRIPTION
*   Turns the given image into a grayscale image. Millisec specifies how
*   long in milliseconds before the image is restored to it's original form.
*   This is useful for "mouse over" type animations. Using 0 for millisec
*   disables this feature and leaves the image permanently affected.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to image -- image
*   * integer millisec -- milliseconds before the image is restored (optional)
* RETURN VALUE
*   1 if successful
*/
bool KarambaInterface::changeImageToGray(const Karamba *k, ImageLabel *image, int ms) const
{
    if (!checkKarambaAndMeter(k, image, "ImageLabel")) {
        return false;
    }

    image->toGray(ms);

    return true;
}

/** Image/createBackgroundImage
*
* SYNOPSIS
*   reference createBackgroundImage(widget, x, y, w, h, image)
* DESCRIPTION
*   This creates an background image on your widget at x, y. The filename
*   should be given as the path parameter. In theory the image could be
*   local or could be a url. It works just like adding an image in your
*   theme file. You will need to save the return value to be able to call
*   other functions on your image, such as moveImage()
* ARGUMENTS
*   * reference to widget -- karamba
*   * integer x -- x coordinate
*   * integer y -- y coordinate
*   * string image -- path to the image for the background
* RETURN VALUE
*   reference to new image meter
*/
QObject* KarambaInterface::createBackgroundImage(Karamba *k, int x, int y, const QString &imagePath)
    const
{
    if (!checkKaramba(k)) {
        return 0;
    }

    ImageLabel *tmp = new ImageLabel(k, x, y, 0, 0);
    tmp->setValue(imagePath);
    tmp->setBackground(true);
    k->setSensor(LineParser(imagePath), tmp);

    k->addToGroup(tmp);

    return tmp;
}

/** Image/createTaskIcon
*
* SYNOPSIS
*   reference createTaskIcon(widget, x, y, ctask)
* DESCRIPTION
*   This creates a task image at x, y.
* ARGUMENTS
*   * reference to widget -- karamba
*   * integer x -- x coordinate
*   * integer y -- y coordinate
*   * integer task -- task
* RETURN VALUE
*   reference to new image meter
*/
QObject* KarambaInterface::createTaskIcon(Karamba *k, int x, int y, int ctask) const
{
    if (!checkKaramba(k)) {
        return NULL;
    }

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

/** Image/getImageHeight
*
* SYNOPSIS
*   integer getImageHeight(widget, image)
* DESCRIPTION
*   This returns the height of an image. This is useful if you have rotated
*   an image and its size changed, so you do not know how big it is anymore.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to image -- image
* RETURN VALUE
*   height of the image
*/
int KarambaInterface::getImageHeight(const Karamba *k, const ImageLabel *image) const
{
    if (!checkKarambaAndMeter(k, image, "ImageLabel")) {
        return -1;
    }

    return image->getHeight();
}

/** Image/getImageWidth
*
* SYNOPSIS
*   integer getImageWidth(widget, image)
* DESCRIPTION
*   This returns the width of an image. This is useful if you have rotated
*   an image and its size changed, so you do not know how big it is anymore.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to image -- image
* RETURN VALUE
*   width of the image
*/
int KarambaInterface::getImageWidth(const Karamba *k, const ImageLabel *image) const
{
    if (!checkKarambaAndMeter(k, image, "ImageLabel")) {
        return -1;
    }

    return image->getWidth();
}

/** Image/removeImageEffects
*
* SYNOPSIS
*   boolean removeImageEffects(widget, image)
* DESCRIPTION
*   If you have called image effect commands on your image (ex:
*   changeImageIntensity), you can call this to restore your image to it's
*   original form.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to image -- image
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::removeImageEffects(const Karamba *k, ImageLabel *image) const
{
    if (!checkKarambaAndMeter(k, image, "ImageLabel")) {
        return false;
    }

    image->removeEffects();

    return true;
}

/** Image/removeImageTransformations
*
* SYNOPSIS
*   boolean removeImageTransformations(widget, image)
* DESCRIPTION
*   If you have rotated or resized your image, you can call this function to restore
*   your image to its original form.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to image -- image
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::removeImageTransformations(const Karamba *k, ImageLabel *image) const
{
    if (!checkKarambaAndMeter(k, image, "ImageLabel")) {
        return false;
    }

    image->removeImageTransformations();

    return true;
}

/** Image/resizeImageSmooth
*
* SYNOPSIS
*   boolean resizeImageSmooth(widget, image, w, h)
* DESCRIPTION
*    DEPRECATED: resizeImage now allows the user to pick whether to use fast
*    or smooth resizing from the SuperKaramba menu - This resizes your image
*    to width, height. The imageToResize parameter is a reference to an
*    image that you saved as the return value from createImage()
*
*    In version 0.50 and later this function works exactly as resizeImage().
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to image -- image
*   * integer w -- width
*   * integer h -- height
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::resizeImageSmooth(Karamba *k, ImageLabel *image, int width, int height)
    const
{
    if (!checkKarambaAndMeter(k, image, "ImageLabel")) {
        return false;
    }

    k->scaleImageLabel(image, width, height);

    return true;
}

/** Image/rotateImage
*
* SYNOPSIS
*   long rotateImage(widget, image, deg)
* DESCRIPTION
*   This rotates your image to by the specified amount of degrees. The
*   The image parameter is a reference to an image that you saved as the
*   return value from createImage()
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to image -- image
*   * integer deg -- degrees to rotate
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::rotateImage(const Karamba *k, ImageLabel *image, int deg) const
{
    if (!checkKarambaAndMeter(k, image, "ImageLabel")) {
        return false;
    }

    image->rotate(deg);

    return true;
}




/** Config/addMenuConfigOption
*
* SYNOPSIS
*   boolean addMenuConfigOption(widget, key, name)
* DESCRIPTION
*   SuperKaramba supports a simplistic configuration pop-up menu. This menu
*   appears when you right-click on a widget and choose Configure Theme.
*   Basically, it allows you to have check-able entrys in the menu to allow
*   the user to enable or disable features in your theme.
*
*   Before you use any configuration menu stuff, you NEED to add a new
*   callback to your script:
*
*   def menuOptionChanged(widget, key, value):
*
*   This will get called whenever a config menu option is changed. Now you
*   can add items to the config menu:
*
*   addMenuConfigOption(widget, String key, String name)
*
*   Key is the name of a key value where the value will be saved
*   automatically into the widget's config file. Name is the actual text that
*   will show up in the config menu.
*
*   For example, I could allow the user to enable or disable a clock showing
*   up in my theme:
*
*   karamba.addMenuConfigOption(widget, "showclock", "Display a clock")
* ARGUMENTS
*   * reference to widget -- karamba
*   * string key -- key for menu item
*   * string name -- name of the graph to get
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::addMenuConfigOption(Karamba *k, const QString &key, const QString &name)
    const
{
    if (!checkKaramba(k)) {
        return false;
    }

    k->addMenuConfigOption(key, name);

    return true;
}

/** Config/readConfigEntry
*
* SYNOPSIS
*   string readConfigEntry(widget, key, value)
* DESCRIPTION
*   This function reads an entry from the config file with the given key.
* ARGUMENTS
*   * reference to widget -- karamba
*   * string key -- key for config item
* RETURN VALUE
*   config value for key
*/
QString KarambaInterface::readConfigEntry(const Karamba *k, const QString &key) const
{
    if (!checkKaramba(k)) {
        return QString::null;
    }

    return k->getConfig()->group("theme").readEntry(key, QString());
}

/** Config/readMenuConfigOption
*
* SYNOPSIS
*   boolean readMenuConfigOption(widget, key)
* DESCRIPTION
*   This returns whether or not the given option is checked in the theme's
*   Configure Theme menu.
*
*   See addMenuConfigOption for a more detailed explanation.
* ARGUMENTS
*   * reference to widget -- karamba
*   * string key -- key for menu item
* RETURN VALUE
*   false is returned if it is not checked and true is returned if it is.
*/
bool KarambaInterface::readMenuConfigOption(const Karamba *k, const QString &key) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    return k->readMenuConfigOption(key);
}

/** Config/setMenuConfigOption
*
* SYNOPSIS
*   boolean setMenuConfigOption(widget, key, value)
* DESCRIPTION
*   This sets whether or not the given option is checked in the theme's
*   Configure Theme menu. Value should be false if key should not be checked and
*   true if key should be checked.
*
*   See addMenuConfigOption for a more detailed explanation.
* ARGUMENTS
*   * referencce to widget -- karamba
*   * string key -- key for menu item
*   * boolean value -- true for checked
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::setMenuConfigOption(Karamba *k, const QString &key, bool value) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    return k->setMenuConfigOption(key, value);
}

/** Config/writeConfigEntry
*
* SYNOPSIS
*   boolean writeConfigEntry(widget, key, value)
* DESCRIPTION
*   SuperKaramba automatically supports configuration files for each theme.
*   These files will be saved in /your/home/dir/.superkaramba/ and will be
*   named themenamerc where themename is the name of the theme.
*
*   This function writes an entry into the config file with the given key and
*   value.
*
*   For example, to save my favorite color, I would do
*   karamba.writeConfigEntry(widget, "FavColor", "Red")
* ARGUMENTS
*   * reference to widget -- karamba
*   * string key -- key for config item
*   * string value -- config value
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::writeConfigEntry(const Karamba *k, const QString &key, const QString &value)
    const
{
    if (!checkKaramba(k)) {
        return false;
    }

    k->getConfig()->group("theme").writeEntry(key, value);

    return true;
}






/** InputBox/createInputBox
*
* SYNOPSIS
*   reference createInputBox(widget, x, y, w, h, text)
* DESCRIPTION
*   This creates a new Input Box at x, y with width and height w, h. You need to save
*   the return value of this function to call other functions on your Input Box
*   field, such as changeInputBox().
*   The karamba widget is automatically set active, to allow user interactions.
* ARGUMENTS
*   * reference to widget -- karamba
*   * integer x -- x coordinate
*   * integer y -- y coordinate
*   * integer w -- width
*   * integer h -- height
*   * string text -- text for the Input Box
* RETURN VALUE
*   reference to new Input Box
*/
QObject* KarambaInterface::createInputBox(Karamba* k, int x, int y, int w, int h, const QString
        &text) const
{
    if (!checkKaramba(k)) {
        return NULL;
    }

    Input *tmp = new Input(k, x, y, w, h);
    tmp->setValue(text);
    tmp->setTextProps(k->getDefaultTextProps());

    k->addToGroup(tmp);

    return tmp;
}

/** InputBox/deleteInputBox
*
* SYNOPSIS
*   boolean deleteInputBox(widget, inputBox)
* DESCRIPTION
*   This removes the Input Box object from the widget. Please do not call functions of
*   the Input Box after calling deleteInputBox, as it does not exist anymore and that
*   could cause crashes in some cases.
*   The karamba widget ist automatically set passive, when no more Input Boxes are on
*   the karamba widget.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference widget -- InputBox
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::deleteInputBox(Karamba *k, Input *input) const
{
    if (!checkKarambaAndMeter(k, input, "Input")) {
        return false;
    }

    return k->removeMeter(input);
}

/** InputBox/moveInputBox
*
* SYNOPSIS
*   long moveInputBox(widget, inputBox, x, y)
* DESCRIPTION
*   This moves a Input Box object to a new x, y relative to your widget. In other
*   words, (0,0) is the top corner of your widget, not the screen.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to inputBox -- Input Box
*   * integer x -- x coordinate
*   * integer y -- y coordinate
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::moveInputBox(Karamba *k, Input *input, int x, int y) const
{
    return moveMeter(k, input, "Input", x, y);
}

/** InputBox/getInputBoxPos
*
* SYNOPSIS
*   array getInputBoxPos(widget, inputBox)
* DESCRIPTION
*   Given a reference to a Input Box object, this will return an array
*   containing the x and y coordinate of an Input Box.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to inputBox -- Input Box
* RETURN VALUE
*   x and y coordinates of the inputBox
*/
QVariantList KarambaInterface::getInputBoxPos(const Karamba *k, const Input *input) const
{
    return getMeterPos(k, input, "Input");
}

/** InputBox/resizeInputBox
*
* SYNOPSIS
*   boolean resizeInputBox(widget, inputBox, w, h)
* DESCRIPTION
*   This will resize Input Box to the new height and width.
* ARGUMENTS
*   * reference to widget -- karamba
*   * referenc to inputBox -- Input Box
*   * integer w -- new width
*   * integer h -- new height
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::resizeInputBox(const Karamba *k, Input *input, int width, int height) const
{
    return resizeMeter(k, input, "Input", width, height);
}

/** InputBox/getInputBoxSize
*
* SYNOPSIS
*   array getInputBoxSize(widget, inputBox)
* DESCRIPTION
*   Given a reference to a Input Box object, this will return an array
*   containing the height and width of a Input Box object.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to inputBox -- Input Box
* RETURN VALUE
*   width and hight of the input box
*/
QVariantList KarambaInterface::getInputBoxSize(const Karamba *k, const Input *input) const
{
    return getMeterSize(k, input, "Input");
}

/** InputBox/changeInputBox
*
* SYNOPSIS
*   reference changeInputBox(widget, inputBox, value)
* DESCRIPTION
*   This function will change the contents of a input box widget.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to inputBox -- Input Box
*   * string value -- new text of the input box
* RETURN VALUE
*   reference to input box
*/
QObject* KarambaInterface::changeInputBox(const Karamba *k, Input *input, const QString &text) const
{
    return setMeterStringValue(k, input, "Input", text);
}

/** InputBox/getInputBoxValue
*
* SYNOPSIS
*   string getInputBoxValue(widget, inputBox)
* DESCRIPTION
*   Returns current Input Box text.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to inputBox -- Input Box
* RETURN VALUE
*   input box text
*/
QString KarambaInterface::getInputBoxValue(const Karamba *k, const Input *input) const
{
    return getMeterStringValue(k, input, "Input");
}

/** InputBox/getThemeInputBox
*
* SYNOPSIS
*   reference getThemeInputBox(widget, name)
* DESCRIPTION
*   You can reference text in your python code that was created in the
*   theme file. Basically, you just add a NAME= value to the INPUT line in
*   the .theme file. Then if you want to use that object, instead of calling
*   createInputBox, you can call this function.
*
*   The name you pass to the function is the same one that you gave it for
*   the NAME= parameter in the .theme file.
* ARGUMENTS
*   * reference to widget -- karamba
*   * string name -- name of the Input Box in the theme file
* RETURN VALUE
*   reference to Input Box
*/
QObject* KarambaInterface::getThemeInputBox(const Karamba *k, const QString &meter) const
{
    return getThemeMeter(k, meter, "Input");
}

/** InputBox/hideInputBox
*
* SYNOPSIS
*   boolean hideInputBox(widget, inputBox)
* DESCRIPTION
*   Hides a Input Box that is visible.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to inputBox -- Input Box
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::hideInputBox(const Karamba *k, Input *input) const
{
    return hideMeter(k, input, "Input");
}

/** InputBox/showInputBox
*
* SYNOPSIS
*   boolean showInputBox(widget, inputBox)
* DESCRIPTION
*   Shows Input Box that has been hidden with hideInputBox()
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to inputBox -- Input Box
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::showInputBox(const Karamba *k, Input *input) const
{
    return showMeter(k, input, "Input");
}

/** InputBox/changeInputBoxFont
*
* SYNOPSIS
*   boolean changeInputBoxFont(widget, inputBox, font)
* DESCRIPTION
*   This will change the font of a Input Box widget. InputBox is the reference to the
*   Input Box object to change. Font is a string with the name of the font to use.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to inputBox -- inputBox
*   * string font -- font name
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::changeInputBoxFont(const Karamba *k, Input *input, const QString &font) const
{
    if (!checkKarambaAndMeter(k, input, "Input")) {
        return false;
    }

    input->setFont(font);

    return true;
}

/** InputBox/getInputBoxFont
*
* SYNOPSIS
*   string getInputBoxFont(widget, inputBox)
* DESCRIPTION
*   Gets the current Input Box font name
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to inputBox -- Input Box
* RETURN VALUE
*   font name
*/
QString KarambaInterface::getInputBoxFont(const Karamba *k, const Input *input) const
{
    if (!checkKarambaAndMeter(k, input, "Input")) {
        return QString::null;
    }

    return input->getFont();
}

/** InputBox/changeInputBoxFontColor
*
* SYNOPSIS
*   boolean changeInputBoxFontColor(widget, inputBox, r, g, b)
* DESCRIPTION
*   This will change the color of a text of a Input Box widget.
*   InputBox is the reference to the text object to change
*   r, g, b are ints from 0 to 255 that represent red, green, and blue.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to inputBox -- Input Box
*   * integer red -- red component of color
*   * integer green -- green component of color
*   * integer blue -- blue component of color
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::changeInputBoxFontColor(const Karamba *k, Input *input, int red, int green,
        int blue) const
{
    if (!checkKarambaAndMeter(k, input, "Input")) {
        return false;
    }

    input->setFontColor(QColor(red, green, blue));
    return true;
}

/** InputBox/getInputBoxFontColor
*
* SYNOPSIS
*   array getInputBoxFontColor(widget, inputBox)
* DESCRIPTION
*   Gets the current text color of a Input Box
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to inputBox -- Input Box
* RETURN VALUE
*   (red, green, blue)
*/
QVariantList KarambaInterface::getInputBoxFontColor(const Karamba *k, const Input *input) const
{
    if (!checkKarambaAndMeter(k, input, "Input")) {
        return QVariantList();
    }

    QVariantList ret;

    QColor color = input->getFontColor();
    ret << color.red();
    ret << color.green();
    ret << color.blue();

    return ret;
}

/** InputBox/changeInputBoxSelectionColor
*
* SYNOPSIS
*   boolean changeInputBoxSelectionColor(widget, inputBox, r, g, b)
* DESCRIPTION
*   This will change the color of the selection of a Input Box widget.
*   InputBox is the reference to the text object to change
*   r, g, b are ints from 0 to 255 that represent red, green, and blue.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to inputBox -- Input Box
*   * integer red -- red component of color
*   * integer green -- green component of color
*   * integer blue -- blue component of color
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::changeInputBoxSelectionColor(const Karamba *k, Input *input, int red,
        int green, int blue) const
{
    if (!checkKarambaAndMeter(k, input, "Input")) {
        return false;
    }

    input->setSelectionColor(QColor(red, green, blue));
    return true;
}

/** InputBox/getInputBoxSelectionColor
*
* SYNOPSIS
*   array getInputBoxSelectionColor(widget, inputBox)
* DESCRIPTION
*   Gets the current selection color of a Input Box
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to inputBox -- Input Box
* RETURN VALUE
*   (red, green, blue)
*/
QVariantList KarambaInterface::getInputBoxSelectionColor(const Karamba *k, const Input *input) const
{
    if (!checkKarambaAndMeter(k, input, "Input")) {
        return QVariantList();
    }

    QVariantList ret;

    QColor color = input->getSelectionColor();
    ret << color.red();
    ret << color.green();
    ret << color.blue();

    return ret;
}

/** InputBox/changeInputBoxBackgroundColor
*
* SYNOPSIS
*   reference changeInputBoxBackgroundColor(widget, inputBox, r, g, b)
* DESCRIPTION
*   This will change the background color of a Input Box widget.
*   InputBox is the reference to the text object to change
*   r, g, b are ints from 0 to 255 that represent red, green, and blue.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to inputBox -- Input Box
*   * integer red -- red component of color
*   * integer green -- green component of color
*   * integer blue -- blue component of color
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::changeInputBoxBackgroundColor(const Karamba *k, Input *input, int red, int
        green, int blue) const
{
    if (!checkKarambaAndMeter(k, input, "Input")) {
        return false;
    }

    input->setBGColor(QColor(red, green, blue));

    return true;
}

/** InputBox/getInputBoxBackgroundColor
*
* SYNOPSIS
*   array getInputBoxBackgroundColor(widget, inputBox)
* DESCRIPTION
*   Gets the current background color of a Input Box
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to inputBox -- Input Box
* RETURN VALUE
*   (red, green, blue)
*/
QVariantList KarambaInterface::getInputBoxBackgroundColor(const Karamba *k, const Input *input)
    const
{
    if (!checkKarambaAndMeter(k, input, "Input")) {
        return QVariantList();
    }

    QVariantList ret;

    QColor color = input->getBGColor();
    ret << color.red();
    ret << color.green();
    ret << color.blue();

    return ret;
}

/** InputBox/changeInputBoxFrameColor
*
* SYNOPSIS
*   boolean changeInputBoxFrameColor(widget, inputBox, r, g, b)
* DESCRIPTION
*   This will change the frame color of a Input Box widget.
*   InputBox is the reference to the text object to change
*   r, g, b are ints from 0 to 255 that represent red, green, and blue.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to inputBox -- Input Box
*   * integer red -- red component of color
*   * integer green -- green component of color
*   * integer blue -- blue component of color
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::changeInputBoxFrameColor(const Karamba *k, Input *input, int red, int green,
        int blue) const
{
    if (!checkKarambaAndMeter(k, input, "Input"))
        return false;

    input->setColor(QColor(red, green, blue));
    return true;
}

/** InputBox/getInputBoxFrameColor
*
* SYNOPSIS
*   array getInputBoxFrameColor(widget, inputBox)
* DESCRIPTION
*   Gets the current frame color of a Input Box
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to inputBox -- Input Box
* RETURN VALUE
*   (red, green, blue)
*/
QVariantList KarambaInterface::getInputBoxFrameColor(const Karamba *k, const Input *input) const
{
    if (!checkKarambaAndMeter(k, input, "Input")) {
        return QVariantList();
    }

    QVariantList ret;

    QColor color = input->getColor();
    ret << color.red();
    ret << color.green();
    ret << color.blue();

    return ret;
}

/** InputBox/changeInputBoxSelectedTextColor
*
* SYNOPSIS
*   boolean changeInputBoxSelectedTextColor(widget, inputBox, r, g, b)
* DESCRIPTION
*   This will change the selected text color of a Input Box widget.
*   InputBox is the reference to the text object to change
*   r, g, b are ints from 0 to 255 that represent red, green, and blue.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to inputBox -- Input Box
*   * integer red -- red component of color
*   * integer green -- green component of color
*   * integer blue -- blue component of color
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::changeInputBoxSelectedTextColor(const Karamba *k, Input *input, int red, int
        green, int blue) const
{
    if (!checkKarambaAndMeter(k, input, "Input")) {
        return false;
    }

    input->setSelectedTextColor(QColor(red, green, blue));

    return true;
}

/** InputBox/getInputBoxSelectedTextColor
*
* SYNOPSIS
*   array getInputBoxSelectedTextColor(widget, inputBox)
* DESCRIPTION
*   Gets the current selected text color of a Input Box.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to inputBox -- pointer to Input Box
* RETURN VALUE
*   (red, green, blue)
*/
QVariantList KarambaInterface::getInputBoxSelectedTextColor(const Karamba *k, const Input *input)
    const
{
    if (!checkKarambaAndMeter(k, input, "Input")) {
        return QVariantList();
    }

    QVariantList ret;

    QColor color = input->getSelectedTextColor();
    ret << color.red();
    ret << color.green();
    ret << color.blue();

    return ret;
}

/** InputBox/changeInputBoxFontSize
*
* SYNOPSIS
*   boolean changeInputBoxFontSize(widget, text, size)
* DESCRIPTION
*   This will change the font size of a Input Box widget.
*   InputBox is the reference to the text object to change.
*   Size is the new font point size.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to inputBox -- Input Box
*   * integer size -- new font size for text
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::changeInputBoxFontSize(const Karamba *k, Input *input, int size) const
{
    if (!checkKarambaAndMeter(k, input, "Input")) {
        return false;
    }

    input->setFontSize(size);
    return true;
}

/** InputBox/getInputBoxFontSize
*
* SYNOPSIS
*   integer getInputBoxFontSize(widget, inputBox)
* DESCRIPTION
*   Gets the current text font size.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to inputBox -- Input Box
* RETURN VALUE
*   text font size
*/
int KarambaInterface::getInputBoxFontSize(const Karamba *k, const Input *input) const
{
    if (!checkKarambaAndMeter(k, input, "Input")) {
        return 0;
    }

    return input->getFontSize();
}

/** InputBox/setInputFocus
*
* SYNOPSIS
*   boolean setInputFocus(widget, inputBox)
* DESCRIPTION
*   Sets the input focus to the Input Box.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to inputBox -- Input Box
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::setInputFocus(const Karamba *k, Input *input) const
{
    if (!checkKarambaAndMeter(k, input, "Input")) {
        return false;
    }

    input->setInputFocus();
    return true;
}

/** InputBox/clearInputFocus
*
* SYNOPSIS
*   boolean clearInputFocus(widget, inputBox)
* DESCRIPTION
*   Releases the input focus from the Input Box.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to inputBox -- pointer to Input Box
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::clearInputFocus(const Karamba *k, Input *input) const
{
    if (!checkKarambaAndMeter(k, input, "Input"))
        return false;

    input->clearInputFocus();
    return true;
}

/** InputBox/getInputFocus
*
* SYNOPSIS
*   boolean  getInputFocus(widget)
* DESCRIPTION
*   Gets the Input Box that is currently focused.
* ARGUMENTS
*   * reference to widget -- karamba
* RETURN VALUE
*   the input box or 0
*/
QObject* KarambaInterface::getInputFocus(const Karamba *k) const
{
    if (!checkKaramba(k)) {
        return 0;
    }

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
         k = d->karamba;
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

