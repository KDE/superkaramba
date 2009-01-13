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

#include "karambainterface.h"
#include "karambainterface.moc"

#include "karamba.h"
#include "karambaapp.h"
#include "lineparser.h"
#include "showdesktop.h"
#include "karambamanager.h"

#include <QGraphicsScene>
#include <QNetworkInterface>
#include <QTimer>
#include <QDomDocument>
#include <QFileInfo>
#include <QDesktopWidget>
#include <QGraphicsProxyWidget>
#include <QGraphicsView>

#include <KMenu>
#include <KDebug>
#include <KConfigGroup>
#include <KService>
#include <KServiceGroup>
#include <KLocale>
#include <KMessageBox>
#include <KCmdLineArgs>
#include <KAboutData>
#include <KIconLoader>

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
}

KarambaInterface::~KarambaInterface()
{
    delete d;
}

void KarambaInterface::startInterpreter()
{
    d->action->trigger();
    if( d->action->hadError() ) {
        const QString err = QString("%1\n\n%2").arg(d->action->errorMessage()).arg(d->action->errorTrace());
        d->karamba->emitError(err);
    }
}

bool KarambaInterface::initInterpreter()
{
    const ThemeFile &theme = d->karamba->theme();

    // Get the theme path
    QDir scriptDir;
    if (theme.isZipTheme()) {
        scriptDir = theme.extractArchive();
    } else {
        scriptDir = theme.path();
    }

    QString interpreter = Kross::Manager::self().interpreternameForFile(theme.scriptModule());
    if (interpreter.isEmpty()) {
        KMessageBox::sorry(0, i18n(
                "SuperKaramba cannot continue to run this theme."
                "One or more of the required components of the Kross scripting architecture is not installed. "
                "Please consult this theme's documentation and install the necessary Kross components."),
                i18n("Please install additional Kross components"));
        return false;
    }

    // Set up Interpreter
    QFileInfo fi(scriptDir, theme.scriptModule());
    if (fi.exists() && !interpreter.isEmpty()) {
        QString scriptFile = fi.absoluteFilePath();

        // the Kross::Action is our script container.
        d->action = new Kross::Action(this, scriptFile, fi.dir());

        // set the name of the interpreter that should be used for the
        // scripting code. This could be e.g. 'python' or 'ruby'.
        d->action->setInterpreter(interpreter);

        // publish this instance, so that scripts are able to access it with e.g.
        // import karamba or require 'karamba' depending on the chosen scripting
        // language.
        d->action->addObject(this, "karamba", Kross::ChildrenInterface::AutoConnectSignals);

        // If the karamba QObject has a child named "PlasmaApplet" publish that child
        // as well. This is used in the SuperKaramba Plasma Applet skapplet to pass
        // an own reference to an SuperKarambaAppletAdaptor instance to the script.
        if( QObject* appletadaptor = d->karamba->findChild<QObject*>("PlasmaApplet") ) {
            d->action->addObject(appletadaptor);
        }

        // this is for backward-compatibility and needed cause the prev python
        // implementation does provide the current path within the sys.path and
        // some scripts are using it to do custom stuff. So, while the preferred
        // way is now to use karamba.getThemePath() we still maintain support to
        // the old behaviour.
        if (interpreter == "python") {
            d->action->setCode(QString(
                "import karamba, sys\n"
                "sys.path.insert(0, karamba.getThemePath())\n"
                "sys.path.insert(0, '')\n"
                "execfile(\"%1\", globals(), locals())\n"
            ).arg(scriptFile).toLatin1());
        } else {
            d->action->setFile(scriptFile);
        }

        kDebug() << "Using " << interpreter << " script: " << scriptFile ;

        return true;
    }

    return false;
}

// Testing functions -----------------------

bool KarambaInterface::checkKaramba(const Karamba *k) const
{
    if (!k) {
        kWarning() << "Widget pointer was 0" ;
        return false;
    }

    if (!KarambaManager::self()->checkKaramba(k)) {
        kWarning() << "Widget " << (long)k << " invalid" ;
        return false;
    }

    return true;
}

bool KarambaInterface::checkMeter(const Karamba *k, const Meter *m, const QString &type) const
{
    if (!m) {
        kWarning() << "Meter pointer was 0";
        return false;
    }

    if (!k->hasMeter(m)) {
        kWarning() << "Widget does not have meter" << (long)m;
        return false;
    }

    if (!m->inherits(type.toAscii().data())) {
        kWarning() << "Meter is not of type" << type;
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
        return QString();
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
            if (checkMeter(k, m, type)) {
                return m;
            }
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
        green, int blue, int alpha) const
{
    if (!checkKarambaAndMeter(k, m, type)) {
        return false;
    }

    m->setColor(QColor(red, green, blue, alpha));

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
    ret << color.alpha();

    return ret;
}

QString KarambaInterface::getMeterStringValue(const Karamba *k, const Meter *m, const QString &type)
    const
{
    if (!checkKarambaAndMeter(k, m, type)) {
        return QString();
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

void KarambaInterface::callMenuOptionChanged(Karamba *k, const QString &key, bool value)
{
    emit menuOptionChanged(k, key, value);
}

void KarambaInterface::callMenuItemClicked(Karamba* k, KMenu* menu, QAction *id)
{
    emit menuItemClicked(k, menu, id);
}

void KarambaInterface::callActiveTaskChanged(Karamba *k, Task* t)
{
    emit activeTaskChanged(k, t);
}

void KarambaInterface::callTaskAdded(Karamba *k, Task *t)
{
    emit taskAdded(k, t);
}

void KarambaInterface::callTaskRemoved(Karamba *k, Task *t)
{
    emit taskRemoved(k, t);
}

void KarambaInterface::callStartupAdded(Karamba *k, Startup *t)
{
    emit startupAdded(k, t);
}

void KarambaInterface::callStartupRemoved(Karamba *k, Startup *t)
{
    emit startupRemoved(k, t);
}

void KarambaInterface::callCommandFinished(Karamba *k, int pid)
{
    emit commandFinished(k, pid);
}

void KarambaInterface::callCommandOutput(Karamba *k, int pid, char* buffer)
{
    emit commandOutput(k, pid, QString(buffer));
}

void KarambaInterface::callItemDropped(Karamba *k, const QString &text, int x, int y)
{
    emit itemDropped(k, text, x, y);
}

void KarambaInterface::callMeterClicked(Karamba *k, Meter *m, int button)
{
    emit meterClicked(k, m, button);
}

void KarambaInterface::callMeterClicked(Karamba *k, const QString &str, int button)
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

void KarambaInterface::callKeyPressed(Karamba *k, Meter *meter, const QString &key)
{
    emit keyPressed(k, meter, key);
}

void KarambaInterface::callThemeNotify(Karamba *k, const QString &sender, const QString &data)
{
    emit themeNotify(k, sender, data);
}

void KarambaInterface::callWallpaperChanged(Karamba *k, int desktop)
{
    emit wallpaperChanged(k, desktop);
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
        return QString();
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
*   boolean setGraphColor(widget, graph, red, green, blue, alpha=255)
* DESCRIPTION
*   Sets current graph color.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to graph -- pointer to graph
*   * integer red -- red component of color
*   * integer green -- green component of color
*   * integer blue -- blue component of color
*   * integer alpha -- alpha component of color (optional)
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::setGraphColor(const Karamba *k, Graph *graph, int red, int green, int blue, int alpha)
    const
{
    return setMeterColor(k, graph, "Graph", red, green, blue, alpha);
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
*   (red, green, blue, alpha)
*/
QVariantList KarambaInterface::getGraphColor(const Karamba *k, const Graph *graph) const
{
    return getMeterColor(k, graph, "Graph");
}

/** Graph/setGraphFillColor
 *
 * SYNOPSIS
 *   boolean setGraphFillColor(widget, graph, red, green, blue, alpha=255)
 * DESCRIPTION
 *   Sets current graph fill color.
 * ARGUMENTS
 *   * reference to widget -- karamba
 *   * reference to graph -- pointer to graph
 *   * integer red -- red component of color
 *   * integer green -- green component of color
 *   * integer blue -- blue component of color
 *   * integer alpha -- alpha component of color (optional)
 * RETURN VALUE
 *   true if successful
 */
bool KarambaInterface::setGraphFillColor(const Karamba *k, Graph *graph, int red, int green, int blue, int alpha)
    const
{
  if (!checkKarambaAndMeter(k, graph, "Graph")) {
    return false;
  }

  graph->setFillColor(QColor(red, green, blue, alpha));
  return true;
}

/** Graph/getGraphFillColor
 *
 * SYNOPSIS
 *   array getGraphFillColor(widget, graph)
 * DESCRIPTION
 *   Gets the current graph fill color components.
 * ARGUMENTS
 *   * reference to widget -- karamba
 *   * reference to graph -- graph
 * RETURN VALUE
 *   (red, green, blue, alpha)
 */
QVariantList KarambaInterface::getGraphFillColor(const Karamba *k, const Graph *graph) const
{
     if (!checkKarambaAndMeter(k, graph, "Graph")) {
        return QVariantList();
      }

      QColor color = graph->getFillColor();

      QVariantList ret;
      ret << color.red();
      ret << color.green();
      ret << color.blue();
      ret << color.alpha();

      return ret;
}

/** Graph/getGraphScroll
 *
 * SYNOPSIS
 *   QString getGraphScroll(widget, graph)
 * DESCRIPTION
 *   get string description of the scroll direction
 * ARGUMENTS
 *   * reference to widget -- karamba
 *   * reference to graph -- graph
 * RETURN VALUE
 *   QString "left" or "right"
 */
QString KarambaInterface::getGraphScroll(const Karamba *k, const Graph *graph) const
{
  if (!checkKarambaAndMeter(k, graph, "Graph")) {
    return QString();
  }

  return graph->getScrollDirection();
}

/** Graph/getGraphScroll
 *
 * SYNOPSIS
 *   bool getGraphScroll(widget, graph)
 * DESCRIPTION
 *   get string description of the scroll direction of 'left' or 'right'
 * ARGUMENTS
 *   * reference to widget -- karamba
 *   * reference to graph -- graph
 *   * reference to new scroll value -- scroll
 * RETURN VALUE
 *   bool to indicate success
 */
bool KarambaInterface::setGraphScroll(const Karamba *k, Graph *graph, const QString &scroll) const
{
  if (!checkKarambaAndMeter(k, graph, "Graph")) {
    return false;
  }

  graph->setScrollDirection(scroll);
  return true;
}

/** Graph/getGraphPlot
 *
 * SYNOPSIS
 *   QString getGraphPlot(widget, graph)
 * DESCRIPTION
 *   get string description of the plot direction
 * ARGUMENTS
 *   * reference to widget -- karamba
 *   * reference to graph -- graph
 * RETURN VALUE
 *   QString "up" or "down"
 */
QString KarambaInterface::getGraphPlot(const Karamba *k, const Graph *graph) const
{
  if (!checkKarambaAndMeter(k, graph, "Graph")) {
    return QString();
  }

  return graph->getPlotDirection();
}

/** Graph/setGraphPlot
 *
 * SYNOPSIS
 *   bool setGraphPlot(widget, graph, plot)
 * DESCRIPTION
 *   sets the direction that the graph plots positive numbers to be 'up' or 'down'
 * ARGUMENTS
 *   * reference to widget -- karamba
 *   * reference to graph -- graph
 *   * reference to new plot direction value -- plot
 * RETURN VALUE
 *   bool to indicate success
 */
bool KarambaInterface::setGraphPlot(const Karamba *k, Graph *graph, const QString &plot) const
{
  if (!checkKarambaAndMeter(k, graph, "Graph")) {
    return false;
  }

  graph->setPlotDirection(plot);
  return true;
}

/** Graph/getGraphShouldFill
 *
 * SYNOPSIS
 *   QString getGraphShouldFill(widget, graph)
 * DESCRIPTION
 *   get bool describing whether or not the graph will draw itself filled
 * ARGUMENTS
 *   * reference to widget -- karamba
 *   * reference to graph -- graph
 * RETURN VALUE
 *   bool true if graph should draw itself filled
 */
bool KarambaInterface::getGraphShouldFill(const Karamba *k, const Graph *graph) const
{
  if (!checkKarambaAndMeter(k, graph, "Graph")) {
    return false;
  }

  return graph->shouldFill();
}

/** Graph/setGraphShouldFill
 *
 * SYNOPSIS
 *   bool setGraphShouldFill(widget, graph, shouldFill)
 * DESCRIPTION
 *   set whether or not the graph should draw itself filled
 * ARGUMENTS
 *   * reference to widget -- karamba
 *   * reference to graph -- graph
 *   * bool to dictate fill -- shouldFill
 * RETURN VALUE
 *   bool to indicate success
 */
bool KarambaInterface::setGraphShouldFill(const Karamba *k, Graph *graph, bool shouldFill) const
{
  if (!checkKarambaAndMeter(k, graph, "Graph")) {
    return false;
  }

  graph->shouldFill(shouldFill);
  return true;
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

/** Image/createEmptyImage
*
* SYNOPSIS
*   reference createEmptyImage(widget, x, y, w, h)
* DESCRIPTION
*   This creates an empty image on your widget at x, y. The empy image
*   allows you to draw the contents with the setPixel() function
*   without loading an image first.
* ARGUMENTS
*   * reference to widget -- karamba
*   * integer x -- x coordinate
*   * integer y -- y coordinate
*   * integer w -- width of the image
*   * integer h -- height of the image
* RETURN VALUE
*   reference to new image meter
*/
QObject* KarambaInterface::createEmptyImage(Karamba* k, int x, int y, int w, int h) const
{
    if (!checkKaramba(k)) {
        return NULL;
    }

    ImageLabel *tmp = new ImageLabel(k, x, y, w, h);

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
    if (!checkKarambaAndMeter(k, image, "ImageLabel")) {
        return false;
    }

    k->scaleImageLabel(image, width, height);

    return true;
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
*   Note, starting with SuperKaramba version 0.50 (KDE4.0) this
*   function accepts SVG.
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
*   true if successful
*/
bool KarambaInterface::changeImageToGray(const Karamba *k, ImageLabel *image, int ms) const
{
    if (!checkKarambaAndMeter(k, image, "ImageLabel")) {
        return false;
    }

    image->toGray(ms);

    return true;
}

/** Image/changeImageAlpha
*
* SYNOPSIS
*   boolean changeImageAlpha(widget, image, alpha, red=-1, green=-1, blue=-1, millisec=0)
* DESCRIPTION
*   This will change the image transparency to the given alpha value.
*   The R,G, or B value of -1 indicates that pixels of all intensities
*   in this channel will be assigned the given alpha.
*   If a color (R,G, or B) intensity is specified in the range of 0-255,
*   the alpha value is applied only to the pixels with that intensity.
*   This allows to make, for example, only black (R=0,G=0,B=0) pixels transparent,
*   while keeping all other pixel's alpha unchanged.
*   The value of millisec instructs how long the transparency effect should stay.
*   The value of 0 makes the alpha effect permanent.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to image -- image
*   * integer alpha -- alpha value to use
*   * integer red -- specific intensity of red component (optional) (defaults to "all intensities")
*   * integer green -- specific intensity of green component (optional) (defaults to "all intensities")
*   * integer blue -- blue component of color
*   * integer millisec -- specific intensity of blue component (optional) (defaults to "all intensities")
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::changeImageAlpha(const Karamba *k, ImageLabel *image, int a, int r, int g, int b, int ms) const
{
    if (!checkKarambaAndMeter(k, image, "ImageLabel")) {
        return false;
    }

    image->toAlpha(QColor(r, g, b), a, ms);

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
    tmp->setZValue(-1);

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
    QPixmap iconPixmap = KWindowSystem::icon(currTask->window());

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
*   boolean rotateImage(widget, image, deg)
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

/** Image/setPixel
*
* SYNOPSIS
*   boolean setPixel(widget, image, x, y, r, g, b, a=255)
* DESCRIPTION
*   This sets the pixel in the image at the position x, y to
*   the color r, g, b with the alpha channel a.
*   You can use this function to draw your own images with
*   your theme script.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to image -- image
*   * integer x -- x coordinate of the pixel
*   * integer y -- y coordinate of the pixel
*   * integer r -- red component of color
*   * integer g -- green component of color
*   * integer b -- blue component of color
*   * integer a -- alpha component of color (optional)
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::setPixel(Karamba *k, ImageLabel *image, int x, int y, int r, int g, int b, int a)
{
    if (!checkKarambaAndMeter(k, image, "ImageLabel")) {
        return false;
    }

    image->setPixel(QPoint(x, y), QColor(r,g,b,a));

    return true;
}

/** Image/changeImageAnimation
*
* SYNOPSIS
*   boolean changeImageAnimation(widget, image, enable)
* DESCRIPTION
*   This function can be used to enable or disable
*   animations in an SVG image. The animation is on by
*   default.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to image -- image
*   * boolean enable -- enable or disable animations
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::changeImageAnimation(Karamba *k, ImageLabel *image, bool enable) const
{
    if (!checkKarambaAndMeter(k, image, "ImageLabel")) {
        return false;
    }

    return image->enableAnimation(enable);
}

/** Image/getImageAnimation
*
* SYNOPSIS
*   boolean getImageAnimation(widget, image)
* DESCRIPTION
*   This function returns if the animation of
*   an SVG image is currently activated.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to image -- image
* RETURN VALUE
*   true if animation is activated or false
*   if it is deactivated or not an SVG image.
*/
bool KarambaInterface::getImageAnimation(Karamba *k, ImageLabel *image) const
{
    if (!checkKarambaAndMeter(k, image, "ImageLabel")) {
        return false;
    }

    return image->animationEnabled();
}

/** Image/setImageElement
*
* SYNOPSIS
*   boolean setImageElement(widget, image, element)
* DESCRIPTION
*   This function will only draw the named element
*   of an SVG image.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to image -- image
*   * string element -- ID of the element to draw
* RETURN VALUE
*   true if the element is available or false
*   if it is not an SVG image.
*/
bool KarambaInterface::setImageElement(Karamba* k, ImageLabel *image, const QString &element) const
{
    if (!checkKarambaAndMeter(k, image, "ImageLabel")) {
        return false;
    }

    return image->drawElement(element);
}

/** Image/setImageElementAll
*
* SYNOPSIS
*   boolean setImageElementAll(widget, image)
* DESCRIPTION
*   This function reset a previous set element.
*   A call to this function will result in a
*   completely drawn SVG image.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to image -- image
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::setImageElementAll(Karamba* k, ImageLabel *image) const
{
    if (!checkKarambaAndMeter(k, image, "ImageLabel")) {
        return false;
    }

    return image->drawElement(QString());
}

/** Image/getImageElement
*
* SYNOPSIS
*   string getImageElement(widget, image)
* DESCRIPTION
*   This function returns the ID of the currently drawn
*   element of an SVG image.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to image -- image
* RETURN VALUE
*   ID of the element or empty string if the image
*   is not an SVG image or the complete SVG image is drawn.
*/
QString KarambaInterface::getImageElement(Karamba *k, ImageLabel *image) const
{
    if (!checkKarambaAndMeter(k, image, "ImageLabel")) {
        return QString();
    }

    return image->elementDrawn();
}






/** Config/addMenuConfigOption
*
* SYNOPSIS
*   boolean addMenuConfigOption(widget, key, name)
* DESCRIPTION
*   SuperKaramba supports a simplistic configuration pop-up menu. This menu
*   appears when you right-click on a widget and choose Configure Theme.
*   Basically, it allows you to have check-able entries in the menu to allow
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
QVariant KarambaInterface::readConfigEntry(const Karamba *k, const QString &key) const
{
    if (!checkKaramba(k)) {
        return QString();
    }

    QString type = k->getConfig()->group("types").readEntry(key, QString());
    QVariant def(QVariant::nameToType(type.toAscii().data()));

    return k->getConfig()->group("theme").readEntry(key, def);
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
bool KarambaInterface::writeConfigEntry(const Karamba *k, const QString &key, const QVariant &value)
    const
{
    if (!checkKaramba(k)) {
        return false;
    }

    k->getConfig()->group("theme").writeEntry(key, value);

    k->getConfig()->group("types").writeEntry(key, QString(QVariant::typeToName(value.type())));

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

    k->makeActive();

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
*   The karamba widget is automatically set passive, when no more Input Boxes are on
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

    k->makePassive();

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
        return QString();
    }

    return input->getFont();
}

/** InputBox/changeInputBoxFontColor
*
* SYNOPSIS
*   boolean changeInputBoxFontColor(widget, inputBox, r, g, b, a=255)
* DESCRIPTION
*   This will change the color of a text of a Input Box widget.
*   InputBox is the reference to the text object to change
*   r, g, b, a are ints from 0 to 255 that represent red, green, blue and the alpha channel.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to inputBox -- Input Box
*   * integer red -- red component of color
*   * integer green -- green component of color
*   * integer blue -- blue component of color
*   * integer alpha -- alpha component of color (optional)
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::changeInputBoxFontColor(const Karamba *k, Input *input, int red, int green,
        int blue, int alpha) const
{
    if (!checkKarambaAndMeter(k, input, "Input")) {
        return false;
    }

    input->setFontColor(QColor(red, green, blue, alpha));
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
*   (red, green, blue, alpha)
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
    ret << color.alpha();

    return ret;
}

/** InputBox/changeInputBoxSelectionColor
*
* SYNOPSIS
*   boolean changeInputBoxSelectionColor(widget, inputBox, r, g, b, a=255)
* DESCRIPTION
*   This will change the color of the selection of a Input Box widget.
*   InputBox is the reference to the text object to change
*   r, g, b, a are ints from 0 to 255 that represent red, green, blue and the alpha channel.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to inputBox -- Input Box
*   * integer red -- red component of color
*   * integer green -- green component of color
*   * integer blue -- blue component of color
*   * integer alpha -- alpha component of color (optional)
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::changeInputBoxSelectionColor(const Karamba *k, Input *input, int red,
        int green, int blue, int alpha) const
{
    if (!checkKarambaAndMeter(k, input, "Input")) {
        return false;
    }

    input->setSelectionColor(QColor(red, green, blue, alpha));
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
*   (red, green, blue, alpha)
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
    ret << color.alpha();

    return ret;
}

/** InputBox/changeInputBoxBackgroundColor
*
* SYNOPSIS
*   reference changeInputBoxBackgroundColor(widget, inputBox, r, g, b, a=255)
* DESCRIPTION
*   This will change the background color of a Input Box widget.
*   InputBox is the reference to the text object to change
*   r, g, b are ints from 0 to 255 that represent red, green, blue and the alpha channel.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to inputBox -- Input Box
*   * integer red -- red component of color
*   * integer green -- green component of color
*   * integer blue -- blue component of color
*   * integer alpha -- alpha component of color
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::changeInputBoxBackgroundColor(const Karamba *k, Input *input, int red, int
        green, int blue, int alpha) const
{
    if (!checkKarambaAndMeter(k, input, "Input")) {
        return false;
    }

    input->setBGColor(QColor(red, green, blue, alpha));

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
*   (red, green, blue, alpha)
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
    ret << color.alpha();

    return ret;
}

/** InputBox/changeInputBoxFrameColor
*
* SYNOPSIS
*   boolean changeInputBoxFrameColor(widget, inputBox, r, g, b, a=255)
* DESCRIPTION
*   This will change the frame color of a Input Box widget.
*   InputBox is the reference to the text object to change
*   r, g, b are ints from 0 to 255 that represent red, green, blue and the alpha channel.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to inputBox -- Input Box
*   * integer red -- red component of color
*   * integer green -- green component of color
*   * integer blue -- blue component of color
*   * integer alpha -- alpha component of color (optional)
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::changeInputBoxFrameColor(const Karamba *k, Input *input, int red, int green,
        int blue, int alpha) const
{
    if (!checkKarambaAndMeter(k, input, "Input"))
        return false;

    input->setColor(QColor(red, green, blue, alpha));
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
*   (red, green, blue, alpha)
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
    ret << color.alpha();

    return ret;
}

/** InputBox/changeInputBoxSelectedTextColor
*
* SYNOPSIS
*   boolean changeInputBoxSelectedTextColor(widget, inputBox, r, g, b, a=255)
* DESCRIPTION
*   This will change the selected text color of a Input Box widget.
*   InputBox is the reference to the text object to change
*   r, g, b are ints from 0 to 255 that represent red, green, blue and alpha channel.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to inputBox -- Input Box
*   * integer red -- red component of color
*   * integer green -- green component of color
*   * integer blue -- blue component of color
*   * integer alpha -- alpha component of color (optional)
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::changeInputBoxSelectedTextColor(const Karamba *k, Input *input, int red, int
        green, int blue, int alpha) const
{
    if (!checkKarambaAndMeter(k, input, "Input")) {
        return false;
    }

    input->setSelectedTextColor(QColor(red, green, blue, alpha));

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
*   (red, green, blue, alpha)
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
    ret << color.alpha();

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
*   boolean getInputFocus(widget)
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

/** InputBox/getInputBoxTextWidth
*
* SYNOPSIS
*   integer getInputBoxTextWidth(widget, input)
* DESCRIPTION
*   Gets the width of the complete text of the Input Box.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to inputBox -- pointer to InputBox
* RETURN VALUE
*   the width or -1
*/
int KarambaInterface::getInputBoxTextWidth(const Karamba *k, Input *input) const
{
    if (!checkKarambaAndMeter(k, input, "Input")) {
        return 0;
    }

    return input->getTextWidth();
}

/** InputBox/changeInputBoxSelection
*
* SYNOPSIS
*   boolean changeInputBoxSelection(widget, inputBox, start, length)
* DESCRIPTION
*   This will set the selection of the input box. The range can be set
*   by the start cursor position and the length of the selection.
*   Both arguments are counted in characters and not pixel.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to inputBox -- inputBox
*   * integer start -- start of the selection
*   * integer length -- length of the selection
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::changeInputBoxSelection(const Karamba *k, Input *input, int start, int length) const
{
    if (!checkKarambaAndMeter(k, input, "Input")) {
        return false;
    }

    input->setSelection(start, length);

    return true;
}

/** InputBox/getInputBoxSelection
*
* SYNOPSIS
*   array getInputBoxSelection(widget, inputBox)
* DESCRIPTION
*   This will return the selection of the input box. If no selection
*   was made in the input box (-1, 0) will be returned for the start
*   and length of the selection.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to inputBox -- inputBox
* RETURN VALUE
*   array with the selection start and length in characters
*/
QVariantList KarambaInterface::getInputBoxSelection(const Karamba *k, Input *input) const
{
    if (!checkKarambaAndMeter(k, input, "Input")) {
        return QVariantList();
    }

    QVariantList ret;

    QTextLayout::FormatRange selection = input->getSelection();
    ret << selection.start;
    ret << selection.length;

    return ret;
}

/** InputBox/clearInputBoxSelection
*
* SYNOPSIS
*   boolean clearInputBoxSelection(widget, inputBox)
* DESCRIPTION
*   This will clear the selection of the input box.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to inputBox -- inputBox
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::clearInputBoxSelection(const Karamba *k, Input *input) const
{
    if (!checkKarambaAndMeter(k, input, "Input")) {
        return false;
    }

    input->clearSelection();

    return true;
}




//
// Helper function for menu API.
// It can not be called from scripts.
//
bool KarambaInterface::menuExists(const Karamba* k, const KMenu* menu) const
{
    return k->popupMenuExisting(menu);
}


/** Menu/addMenuItem
*
* SYNOPSIS
*   reference addMenuItem(widget, menu, text, icon)
* DESCRIPTION
*   This adds an entry to the given menu with label text and with given icon.
*   Icon can be just an application name in which case the user's current
*   icon set is used, or can be a path to a 16x16 png file.
*
*   The function returns the reference to the menu item, which identifies that popup
*   menu item uniquely among popupmenu items application-wide or returns 0
*   if the given menu doesn't exist.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to menu -- menu
*   * string text -- text for menu item
*   * string icon -- icon name or path
* RETURN VALUE
*   reference to the menu item
*/
QObject* KarambaInterface::addMenuItem(Karamba *k, KMenu *menu, const QString &text, const
        QString &icon) const
{
    if (!checkKaramba(k)) {
        return 0;
    }

    QAction *action = 0;
    if (menuExists(k, menu)) {
        action = k->addMenuItem(menu, text, icon);
    }

    return action;
}

/** Menu/addMenuSeparator
*
* SYNOPSIS
*   reference addMenuSeparator(widget, menu)
* DESCRIPTION
*   This adds an menu separator to the given menu.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to menu -- menu
* RETURN VALUE
*   reference to the separator
*/
QObject* KarambaInterface::addMenuSeparator(Karamba *k, KMenu *menu) const
{
    if (!checkKaramba(k)) {
        return 0;
    }

    QAction *sep = 0;
    if (menuExists(k, menu)) {
        sep = menu->addSeparator();
    }

    return sep;
}

/** Menu/createMenu
*
* SYNOPSIS
*   reference createMenu(widget)
* DESCRIPTION
*   This creates an empty popup menu and returns a reference to the menu.
* ARGUMENTS
*   * reference to widget -- karamba
* RETURN VALUE
*   reference to menu
*/
QObject* KarambaInterface::createMenu(Karamba *k) const
{
    if (!checkKaramba(k)) {
        return 0;
    }

    return k->addPopupMenu();
}

/** Menu/deleteMenu
*
* SYNOPSIS
*   boolean deleteMenu(widget, menu)
* DESCRIPTION
*   This deletes the referenced menu if that menu exists.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to menu -- pointer to menu
* RETURN VALUE
*   true if menu existed and was deleted, returns false otherwise.
*/
bool KarambaInterface::deleteMenu(Karamba *k, KMenu *menu) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    if (menuExists(k, menu)) {
        k->deletePopupMenu(menu);
        return true;
    }

    return false;
}


/** Menu/popupMenu
*
* SYNOPSIS
*   boolean popupMenu(widget, menu, x, y)
* DESCRIPTION
*   This pops up the given menu at the given co-ordinates. The co-ordinates
*   are relative to the widget, not the screen. You can use negative
*   co-ordinates to make a menu appear to the right of or above your theme.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to menu -- menu
*   * integer x -- x coordinate
*   * integer y -- y coordinate
* RETURN VALUE
*   true if the menu existed and was popped up, returns false otherwise.
*/
bool KarambaInterface::popupMenu(const Karamba *k, KMenu *menu, int x, int y) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    if (menuExists(k, menu)) {
        k->popupMenu(menu, QPoint(x, y));
        return true;
    }

    return false;
}

/** Menu/removeMenuItem
*
* SYNOPSIS
*   boolean removeMenuItem(widget, menu, id)
* DESCRIPTION
*   This removes the menu if it exists.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to menu -- pointer to menu
*   * reference to menu item -- menu item
* RETURN VALUE
*   1 if the menu item existed and was removed or returns zero otherwise.
*/
bool KarambaInterface::removeMenuItem(Karamba *k, KMenu *menu, QAction *action) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    if (menuExists(k, menu)) {
        k->deleteMenuItem(action);
        return true;
    }

    return false;
}






/** Misc/acceptDrops
*
* SYNOPSIS
*   boolean acceptDrops(widget)
* DESCRIPTION
*   Calling this enables your widget to receive Drop events. In other words,
*   the user will be able to drag icons from his/her desktop and drop them on
*   your widget. The "itemDropped" callback is called as a result with the
*   data about the icon that was dropped on your widget. This allows, for
*   example, icon bars where items are added to the icon bar by Drag and
*   Drop.
* ARGUMENTS
*   * reference to widget -- karamba
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::acceptDrops(Karamba *k) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    k->setAcceptDrops(true);

    return true;
}

/** Misc/attachClickArea
*
*   SYNOPSIS
*     boolean attachClickArea(widget, meter, lB, mB, rB)
*   DESCRIPTION
*     It is possible to attach a clickarea to a meter (image or text field),
*     which is moved and resized correctly if the meter is moved or resized.
*
*     There is also a callback meterClicked(widget, meter, button) which is
*     called whenever a meter is clicked (if something is attached to it).
*     Given an Image or a TextLabel, this call makes it clickable. When a mouse
*     click is detected, the callback meterClicked is called.
*
*     lB, mB, and rB are strings that specify what command is executed when
*     this meter is clicked with the left mouse button, middle mouse button,
*     and right mouse button respectively. If given, the appropriate command is
*     executed when the mouse click is received.
*
*     The keyword arguments are all optional. If command is an empty string
*     nothing is executed.
*
*     For now the command given to RightButton has obviosly no effect (because
*     that brings up the SuperKaramba menu).
*
*   ARGUMENTS
*     * reference to widget -- karamba
*     * reference to meter -- meter
*     * string lB -- command to left mouse button
*     * string mB -- command to middle mouse button
*     * string rB -- command to right mouse button
*
*   RETURN VALUE
*     true if successful
*/
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

/** Misc/callTheme
*
*  SYNOPSIS
*    boolean callTheme(widget, theme, info)
*  DESCRIPTION
*  Calls a theme - identified by the pretty name - and passes it a string to it.
*  This will work, despite superkaramba being multithreaded, because it
*  uses the DBus interface to contact the other theme.  If you need to
*  pass complex arguments (dictionaries, lists etc.) then use the python
*  "repr" and "eval" functions to marshall and unmarshall the data structure.
*  ARGUMENTS
*    * reference to widget -- karamba
*    * string theme -- pretty theme name
*    * string info -- a string containing the info to be passed to the theme
*  RETURN VALUE
*    true if successful
*/
bool KarambaInterface::callTheme(Karamba *k, const QString &theme, const QString &info) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    return k->sendDataToTheme(theme, info);
}

/** Misc/changeInterval
*
*  SYNOPSIS
*    boolean changeInterval(widget, interval)
*  DESCRIPTION
*    This function changes your widget's refresh rate (ms)
*  ARGUMENTS
*    * reference to widget -- karamba
*    * integer interval -- interval, in ms
*  RETURN VALUE
*    true if successful
*/
bool KarambaInterface::changeInterval(Karamba *k, int interval) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    k->changeInterval(interval);

    return true;
}

/** Misc/execute
*
* SYNOPSIS
*   integer execute(command)
* DESCRIPTION
*   This command simply executes a program or command on the system. This is
*   just for convience (IE you could accomplish this directly through python,
*   but sometimes threading problems crop up that way). The only option is a
*   string containing the command to execute.
* ARGUMENTS
*   * string command -- command to execute
* RETURN VALUE
*   process id of the executed command
*/
int KarambaInterface::execute(const QString &command) const
{
    return KRun::runCommand(command,0L);
}

/** Misc/createClickArea
*
* SYNOPSIS
*   reference createClickArea(widget, x, y, w, h, cmdToRun)
* DESCRIPTION
*    This creates a clickable area at x, y with width and height w, h. When
*    this area is clicked, cmd_to_run will be executed. The mouse will change over this area.
* ARGUMENTS
*   * reference to widget -- karamba
*   * integer x -- x coordinate
*   * integer y -- y coordinate
*   * integer w -- width
*   * integer h -- height
*   * string cmdToRun -- command to be run
* RETURN VALUE
*   reference to the new click area
*/
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

/** Misc/removeClickArea
*
*  SYNOPSIS
*    boolean removeClickArea(widget, clickarea)
*  DESCRIPTION
*     This function deletes a clickable area.
*  ARGUMENTS
*    * reference to widget -- karamba
*    * reference to clickarea -- click area
*  RETURN VALUE
*    true if successful
*/
bool KarambaInterface::removeClickArea(Karamba *k, ClickArea *area) const
{
    if (!checkKarambaAndMeter(k, area, "ClickArea")) {
        return false;
    }

    k->removeMeter(area);

    return true;
}

/** Misc/createServiceClickArea
*
*  SYNOPSIS
*    reference createServiceClickArea(widget, x, y, w, h, name_of_command, cmd_to_run, icon_to_display)
*  DESCRIPTION
*     This creates a clickable area at x, y with width and height w, h. When
*     this area is clicked, cmd_to_run will be executed. The mouse will change
*     to the clickable icon when over this area.  For more information on
*     the difference between createClickArea and createServiceClickArea,
*     see the KDE documentation about KService, and the difference
*     between KRun::run and KRun::runCommand.
*  ARGUMENTS
*    * reference to widget -- karamba
*    * integer x -- x coordinate
*    * integer y -- y coordinate
*    * integer w -- width
*    * integer h -- height
*    * string name_of_command -- name to be displayed
*    * string cmd_to_run -- command to be run
*    * string icon_to_display -- name of icon to be displayed
*  RETURN VALUE
*    reference to the new click area
*/
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

/** Misc/executeInteractive
*
* SYNOPSIS
*   integer executeInteractive(widget, command)
* DESCRIPTION
*   This command executes a program or command on the system. Additionally it allows
*   you to get any text that the program outputs. Futhermore, it won't freeze
*   up your widget while the command executes.
*
*   To use it, call executeInteractive with the reference to your widget and
*   a list of command options. The array is simply a list that contains the
*   command as the first entry, and each option as a separate list entry.
*   Output from the command is returned via the commandOutput callback.
*
*   The command returns the process number of the command. This is useful if
*   you want to run more than one program at a time. The number will allow
*   you to figure out which program is outputting in the commandOutput
*   callback.
*
*   Example: Run the command "ls -la *.zip"
*
*   myCommand = ["ls", "-la", "*.zip"]
*   karamba.executeInteractive(widget, myCommand)
* ARGUMENTS
*   * reference to widget -- karamba
*   * list command -- command to execute
* RETURN VALUE
*   process id of the executed command
*/
int KarambaInterface::executeInteractive(Karamba *k, const QStringList &command)
{
    if (!checkKaramba(k)) {
        return 0;
    }

    K3Process *process = new K3Process;
    k->setProcess(process);
    *process << command;

    connect(process,
                          SIGNAL(processExited(K3Process *)),
                          k,
                          SLOT(processExited(K3Process *)));

    connect(process,
                          SIGNAL(receivedStdout(K3Process *, char *, int)),
                          k,
                          SLOT(receivedStdout(K3Process *, char *, int)));

    process->start(K3Process::NotifyOnExit, K3Process::Stdout);

    return process->pid();
}

/** Misc/getIp
*
* SYNOPSIS
*   string getIp(widget, interface_name)
* DESCRIPTION
*   Gets the current IP address of the interface_name interface.
* ARGUMENTS
*   * reference to widget -- karamba
*   * string interface_name -- name of the interface
* RETURN VALUE
*   ip address as string
*/
QString KarambaInterface::getIp(const Karamba *k, const QString &interface) const
{
    if (!checkKaramba(k)) {
        return QString();
    }

    QNetworkInterface iface = QNetworkInterface::interfaceFromName(interface);
    if (iface.isValid()) {
        QNetworkInterface::InterfaceFlags flags = iface.flags();
        if (flags & QNetworkInterface::IsUp) {
            QList<QNetworkAddressEntry> list = iface.addressEntries();
            if (list.count() > 0) {
                return list[0].ip().toString();
            } else {
                return "Error";
            }
        } else {
            return "Disconnected";
        }
    } else {
        return "Error";
    }
}

/** Misc/getNumberOfDesktop
*
* SYNOPSIS
*   integer getNumberOfDesktop(widget)
* DESCRIPTION
*   Returns number of desktops
* ARGUMENTS
*   * reference to widget -- karamba
* RETURN VALUE
*   number of desktops
*/
int KarambaInterface::getNumberOfDesktops(const Karamba *k) const
{
    if (!checkKaramba(k)) {
        return 0;
    }

    return k->getNumberOfDesktops();
}

/** Misc/getPrettyThemeName
*
*  SYNOPSIS
*    string getPrettyName(theme)
*  DESCRIPTION
*    When a theme is created (with openNamedTheme), there is an
*    option to give the theme an alternative name.
*    This is useful if you open several widgets from the same theme:
*    you need to give them unique names in order to contact them
*    (for example, with callTheme or with setIncomingData)
*  ARGUMENTS
*    * string theme -- path to new theme
*  RETURN VALUE
*    the pretty name of the theme
*/
QString KarambaInterface::getPrettyThemeName(const Karamba *k) const
{
    if (!checkKaramba(k)) {
        return QString();
    }

    return k->prettyName();
}

/** Misc/getServiceGroups
*
* SYNOPSIS
*   list getServiceGroups(path)
* DESCRIPTION
*   This function returns a list of services and service groups
*   that are in the user's KDE Menu.  It is not a recursive
*   function, so if there are submenus (service groups) in the
*   returned results, you must call getServiceGroups with the
*   path of the submenu in order to obtain the information in
*   that submenu.
*   The return result is complex: it's a list of tuples.
*   The tuple contains two elements - a 1 if the second element
*   is a service, and a 0 if it's a service group.
*   The second element is a dictionary, with keys (if they exist)
*   of caption, comment, icon, and relpath if it's a service group,
*   and keys (if they exist) of exec, menuid, name, path, icon,
*   library, comment, type and genericname.
*   To fully understand the return results of this function,
*   it is thoroughly recommended //  that you look up the
*   KDE documentation on KServiceGroup and KService.
*  ARGUMENTS
*    * reference to widget -- karamba
*    * string path -- path to the Service Group you wish to retrieve
*  RETURN VALUE
*    List of Dictionaries of services and service groups
*/
QVariantList KarambaInterface::getServiceGroups(const QString &path) const
{
    QVariantList ret;

    KServiceGroup::Ptr root = KServiceGroup::group(path);

    if (!root || !root->isValid())
        return ret;

    KServiceGroup::List sl = root->entries(true, true, true, false);
    QStringList suppressGenericNames = root->suppressGenericNames();

    KServiceGroup::List::ConstIterator it = sl.constBegin();
    for (; it != sl.constEnd(); ++it) {
        KSycocaEntry *e = (KSycocaEntry*)(*it).data();

        QVariant subItem;
        if (e->isType(KST_KServiceGroup)) {
            KServiceGroup::Ptr g(static_cast<KServiceGroup *>(e));
            KServiceGroup::Ptr subMenuRoot = KServiceGroup::group(g->relPath());
            if (subMenuRoot->childCount() == 0 || g->name().at(0) == '.') {
                continue;
            }

            QVariantList typeMap;

            QMap<QString, QVariant> map;
            map["caption"] = g->caption();
            map["comment"] = g->comment();
            map["icon"] = g->icon();
            map["relpath"] = g->relPath();

            typeMap << 0;
            typeMap << QVariant(map);

            subItem = QVariant(typeMap);
        } else if (e->isType(KST_KService)) {
            KService::Ptr g(static_cast<KService *>(e));

            QVariantList typeMap;

            QMap<QString, QVariant> map;
            map["exec"] = g->exec();
            map["menuid"] = g->menuId();
            map["name"] = g->name();
            map["path"] = g->path();
            map["icon"] = g->icon();
            map["library"] = g->library();
            map["comment"] = g->comment();
            map["type"] = g->isApplication() ? "Application" : "Service";
            map["genericname"] = g->genericName();

            typeMap << 1;
            typeMap << QVariant(map);

            subItem = QVariant(typeMap);
        }

        if (subItem.isValid()) {
            ret << subItem;
        }
    }

    return ret;
}

/** Misc/getThemePath
*
* SYNOPSIS
*   string getThemePath(widget)
* DESCRIPTION
*   Returns a string containing the directory where your theme was loaded
*   from.
* ARGUMENTS
*   * reference to widget -- karamba
* RETURN VALUE
*   path to theme
*/
QString KarambaInterface::getThemePath(const Karamba *k) const
{
    if (!k) {
         k = d->karamba;
    }

    if (!checkKaramba(k)) {
        return QString();
    }

    return k->theme().path();
}

/** Misc/getUpdateTime
*
*  SYNOPSIS
*    decimal getUpdateTime(widget)
*  DESCRIPTION
*    Returns the last stored update time. Intended for use
*    so that the next refresh interval can work out how long ago
*    the mouse was last moved over the widget.
*  ARGUMENTS
*    * reference to widget -- karamba
*  RETURN VALUE
*    last stored update time (from setUpdateTime)
*/
double KarambaInterface::getUpdateTime(const Karamba *k) const
{
    if (!checkKaramba(k)) {
        return 0;
    }

    return k->getUpdateTime();
}

/** Misc/setUpdateTime
*
*  SYNOPSIS
*    boolean getUpdateTime(widget, updated_time)
*  DESCRIPTION
*    Sets the update time. Intended for use
*    so that the next refresh interval can work out how long ago
*    the mouse was last moved over the widget.
*  ARGUMENTS
*    * reference to widget -- karamba
*    * integer updated_time -- the update time to be associated with the widget
*  RETURN VALUE
*    true if successful
*/
bool KarambaInterface::setUpdateTime(Karamba *k, double updateTime) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    k->setUpdateTime(updateTime);

    return true;
}

/** Misc/hide
*
* SYNOPSIS
*   boolean hide(widget)
* DESCRIPTION
*   Hides the widget.
* ARGUMENTS
*   * reference to widget -- karamba
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::hide(Karamba *k) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    k->hide();

    return true;
}

/** Misc/show
*
* SYNOPSIS
*   boolean show(widget)
* DESCRIPTION
*   Shows the widget.
* ARGUMENTS
*   * reference to widget -- karamba
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::show(Karamba *k) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    k->show();

    return true;
}

/** Misc/language
*
* SYNOPSIS
*   string language(widget)
* DESCRIPTION
*   Returns a string containing the default language of a translation file.
* ARGUMENTS
*   * reference to widget -- karamba
* RETURN VALUE
*   default language or empty string if no translation files found.
*/
QString KarambaInterface::language(const Karamba *k) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    return k->theme().locale()->language();
}

/** Misc/managementPopup
*
*  SYNOPSIS
*    boolean managementPopup(widget)
*  DESCRIPTION
*    The right click menu of SuperKaramba allows themes
*    to be loaded, closed, moved to other screens.
*    If you want this popup menu to appear, call
*    this function.
*  ARGUMENTS
*    * reference to widget -- karamba
*  RETURN VALUE
*    true if successful
*/
bool KarambaInterface::managementPopup(const Karamba *k) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    k->popupGlobalMenu();

    return true;
}

/** Misc/openNamedTheme
*
*  SYNOPSIS
*    reference openNamedTheme(theme, pretty_name, is_sub_theme)
*  DESCRIPTION
*    Opens a new theme, giving it a pretty (alternative and by your
*    own choice _unique_) name.
*    If you do not want the theme to be loaded when SuperKaramba is
*    first started up (but instead want it to only be opened by
*    this function call) then set is_sub_theme to 1.
*    Themes opened with openNamedTheme will be unique.  If a theme
*    with the same pretty name already exists, openNamedTheme will
*    have no effect.  If you want duplicate themes (and a bit of a
*    mess), use openTheme, instead.
*  ARGUMENTS
*    * string theme -- path to new theme
*    * string pretty_name -- the name to be associated with the new widget
*    * boolean is_sub_theme -- sets persistence (save state) of the theme
*  RETURN VALUE
*    reference to the new widget
*/
Karamba* KarambaInterface::openNamedTheme(const QString &themePath, const QString &themeName,
        bool subTheme) const
{
    Q_UNUSED(themeName);

    Karamba *newTheme = 0;
    QFileInfo file(themePath);

    if (file.exists()) {
        Karamba *k = KarambaManager::self()->getKaramba(themeName);
        if (!k) {
            newTheme = new Karamba(KUrl(themePath), 0, -1, subTheme);
        }
    }
    return newTheme;
}

/** Misc/openTheme
*
* SYNOPSIS
*   reference openTheme(theme)
* DESCRIPTION
*   Opens a new theme.
* ARGUMENTS
*   * string theme -- path to theme that will be opened
* RETURN VALUE
*   reference to the new widget
*/
Karamba* KarambaInterface::openTheme(const QString &themePath) const
{
    Karamba *newTheme = 0;
    QFileInfo file(themePath);

    if (file.exists()) {
        newTheme = new Karamba(KUrl(themePath));
    }

    return newTheme;
}

/** Misc/readThemeFile
*
* SYNOPSIS
*   string readThemeFile(widget, file)
* DESCRIPTION
*   Returns a string with the contents of the theme file
* ARGUMENTS
*   * reference to widget -- karamba
*   * string file -- name of the file to read
* RETURN VALUE
*   file contents
*/
QString KarambaInterface::readThemeFile(const Karamba *k, const QString &file) const
{
    if (!checkKaramba(k)) {
        return QString();
    }

    return k->theme().readThemeFile(file);
}

/** Misc/reloadTheme
*
* SYNOPSIS
*   boolean reloadTheme(theme)
* DESCRIPTION
*   Reloads the current theme.
* ARGUMENTS
*   * reference to widget -- karamba
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::reloadTheme(Karamba *k) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    k->reloadConfig();

    return true;
}

/** Misc/run
*
*  SYNOPSIS
*    boolean run(name, command, icon, list_of_args)
*  DESCRIPTION
*    This command simply executes a program or command on the system.
*    The difference between run and execute is that run takes arguments,
*    and the name of the icon to be displayed.
*  ARGUMENTS
*    * string name -- name to be displayed
*    * string command -- command to execute
*    * string icon -- name of icon to be displayed
*    * string list_of_args -- arguments to be passed to the command
*  RETURN VALUE
*    true if successful
*/
bool KarambaInterface::run(const QString &appName, const QString &command, const QString &icon,
        const QStringList &arguments)
{
    KService service(appName, command, icon);

    KRun::run(service, arguments, 0);

    return true;
}

/** Misc/getIncomingData
*
*  SYNOPSIS
*    string getIncomingData(widget)
*  DESCRIPTION
*    Obtains the last data received by any other theme that set the
*    "incoming data" of this theme. This isn't particularly sophisticated
*    and could benefit from the data being placed in an FIFO queue instead.
*  ARGUMENTS
*    * reference to widget -- karamba
*  RETURN VALUE
*    string containing the last information received from setIncomingData
*/
QString KarambaInterface::getIncomingData(const Karamba *k) const
{
    if (!checkKaramba(k)) {
        return QString();
    }

    return k->retrieveReceivedData();
}

/** Misc/setIncomingData
*
*  SYNOPSIS
*    boolean setIncomingData(widget, theme, info)
*  DESCRIPTION
*    Contacts a theme - identified by the pretty name - and stores a string
*    to be associated with the remote theme. The difference between
*    setIncomingData and callTheme is that the theme is not notified
*    by setIncomingData that the data has arrived. Previous information,
*    if any, is overwritten. Use getIncomingData to retrieve the last
*    stored information.
*    setIncomingData is not very sophisticated, and could benefit from
*    having info passed to it put into a queue, instead of being overwritten.
*  ARGUMENTS
*    * reference to widget -- karamba
*    * string theme -- path to theme to have information passed to it.
*    * string info -- a string containing the info to be passed to the theme
*  RETURN VALUE
*    true if successful
*/

// Is the theme path or the pretty name required?
bool KarambaInterface::setIncomingData(Karamba *k, const QString &prettyThemeName, const QString &data)
    const
{
    if (!checkKaramba(k)) {
        return false;
    }

    return k->sendData(prettyThemeName, data);
}

/** Misc/toggleShowDesktop
*
* SYNOPSIS
*   boolean toggleShowDesktop(widget)
* DESCRIPTION
*   This shows/hides the current desktop just like the Show Desktop button on
*   kicker. Basically, it minimizes all the windows on the current desktop.
*   Call it once to show the desktop and again to hide it.
* ARGUMENTS
*   * reference to widget -- karamba
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::toggleShowDesktop(const Karamba *k) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    ShowDesktop::self()->toggle();

    return true;
}

/** Misc/translateAll
*
* SYNOPSIS
*   boolean translateAll(widget, relative_x, relative_y)
* DESCRIPTION
*   Moves all widgets within a theme in a particular direction relative from
*   the previous spot without moving the parent theme widget.
* ARGUMENTS
*   * reference to widget -- karamba
*   * integer translate_x -- move horizontally
*   * integer translate_y -- move vertically
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::translateAll(const Karamba *k, int x, int y) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    QList<QGraphicsItem*> items = ((QGraphicsItemGroup*)k)->children();

    foreach(QGraphicsItem *item, items) {
        Meter *meter = dynamic_cast<Meter*>(item);
        if (meter)
            meter->setSize(meter->getX() + x,
                           meter->getY() + y,
                           meter->getWidth(),
                           meter->getHeight());
    }

    return true;
}

/** Misc/userLanguage
*
* SYNOPSIS
*   string userLanguage(widget)
* DESCRIPTION
*   Returns a string containing the global KDE user language.
* ARGUMENTS
*   * reference to widget -- karamba
* RETURN VALUE
*   user language or empty string
*/
QString KarambaInterface::userLanguage(const Karamba *k) const
{
    if (!checkKaramba(k)) {
        return QString();
    }

    return KGlobal::locale()->language();
}

/** Misc/userLanguages
*
* SYNOPSIS
*   array userLanguages(widget)
* DESCRIPTION
*   Returns an array containing the language name abbreviations for the
*   preferred interface languages user chose for KDE session in Region &
*   Language settings.
*   Having the whole array of preferred languages available is important for
*   cases when you cannot provide interface translation for the 1st preferred
*   language, but can for consecutive languages.
*   (Implemented in version 0.42.)
* ARGUMENTS
*   * reference to widget -- karamba
* RETURN VALUE
*   list (array) with user languages in the order of preference.
*/
QStringList KarambaInterface::userLanguages(const Karamba *k) const
{
    if (!checkKaramba(k)) {
        return QStringList();
    }

    return KGlobal::locale()->languageList();
}

/** Misc/setWantRightButton
*
*  SYNOPSIS
*    boolean setWantRightButton(widget, want_receive_right_button)
*  DESCRIPTION
*    The right click menu of SuperKaramba allows themes
*    to be loaded, closed, moved to other screens.
*    Not all themes will want the right click menu.
*    Call karamba.wantRightButton(widget, 1)
*    if you want to receive MouseUpdate button notifications.
*  ARGUMENTS
*    * reference to widget -- karamba
*    * boolean want_receive_right_button -- whether the widget will receive right clicks
*  RETURN VALUE
*    true if successful
*/
bool KarambaInterface::setWantRightButton(Karamba *k, bool enable) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    k->setWantRightButton(enable);

    return true;
}

/** Misc/version
*
* SYNOPSIS
*   array version()
* DESCRIPTION
*   Returns the (API) version of the running SuperKaramba.
*   This allows theme writers to use new functionality with keeping
*   backward compatibility.
* RETURN VALUE
*   array with SuperKaramba version and API version
*/
QStringList KarambaInterface::version() const
{
    QStringList version;

    version << KCmdLineArgs::aboutData()->version();
    version << "0.1";

    return version;
}

/** Misc/desktopSize
*
* SYNOPSIS
*   array desktopSize()
* DESCRIPTION
*   Returns the screen size of the display.
* RETURN VALUE
*   array with width and height of the screen
*/
QVariantList KarambaInterface::desktopSize() const
{
    QVariantList ret;

    QRect screenSize = QApplication::desktop()->screenGeometry();
    ret << screenSize.width();
    ret << screenSize.height();

    return ret;
}

/** Misc/getIconByName
*
* SYNOPSIS
*   strint getIconByName(name, size)
* DESCRIPTION
*   Returns the absolute path to the icon.
*   This function respects the current used icon
*   set.
*  ARGUMENTS
*    * string name -- name of the icon
*    * integer size -- Size of the icon (16, 22, 32, 48, 64, 128 available)
* RETURN VALUE
*   string with the absolute path to the icon.
*/
QString KarambaInterface::getIconByName(const QString &icon, int size) const
{
    return KIconLoader::global()->iconPath(icon, -size);
}

/** Misc/getExecutingApplication
*
* SYNOPSIS
*   strint getExecutingApplication()
* DESCRIPTION
*   Returns the name of the application that runs the
*   theme. It will return "superkaramba" if it runs in
*   SuperKaramba or "plasma" if the theme runs in the
*   Plasma desktop.
* RETURN VALUE
*   string with the application name
*/
QString KarambaInterface::getExecutingApplication() const
{
    return QApplication::applicationName();
}





/** RichText/createRichText
*
* SYNOPSIS
*   reference createRichText(widget, text, underlineLinks)
* DESCRIPTION
*   This creates creates a new rich text meter. underlineLinks is a boolean that
*   determines if html links will be automatically underlined so that the
*   user knows that the links can be clicked on. You need to save the return
*   value of this function to call other functions on your rich text field,
*   such as changeRichText().
*
*   The differance between Rich Text and a regular text field is that rich
*   text fields can display HTML code embedded in your text.
*
*   In a <a href="command"> ... </a> tag command is executed if the link is
*   click with the left mouse button.
*
*   Except if command starts with an '#' (ie: href="#value" ) the callback
*   meterClicked is called with value (without the #) as the meter argument.
*
*   Also inline images work. Unfortunately currently only when using absolute
*   paths.
* ARGUMENTS
*   * reference to widget -- karamba
*   * string text -- text for richtext
*   * boolean underlineLinks -- should the links be underlined
* RETURN VALUE
*   reference to new richtext meter
*/
QObject* KarambaInterface::createRichText(Karamba* k, const QString &text, bool underline) const
{
    if (!checkKaramba(k)) {
        return NULL;
    }

    RichTextLabel *tmp = new RichTextLabel(k);
    tmp->setText(text, underline);
    tmp->setTextProps(k->getDefaultTextProps());

    k->addToGroup(tmp);

    return tmp;
}

/** RichText/deleteRichText
*
* SYNOPSIS
*   boolean deleteRichText(widget, richtext)
* DESCRIPTION
*   This removes the rich text from the widget. Please do not call
*   functions on "text" after calling deleteRichText, as it does not exist
*   anymore and that could cause crashes in some cases.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to widget -- richtext
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::deleteRichText(Karamba *k, RichTextLabel *label) const
{
    if (!checkKarambaAndMeter(k, label, "RichTextLabel")) {
        return false;
    }

    return k->removeMeter(label);
}

/** RichText/moveRichText
*
* SYNOPSIS
*   boolean moveRichText(widget, richtext, x, y)
* DESCRIPTION
*   This moves the rich text to a new x, y relative to your widget. In other
*   words, (0,0) is the top corner of your widget, not the screen.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference richtext -- richtext
*   * integer x -- x coordinate
*   * integer y -- y coordinate
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::moveRichText(Karamba *k, RichTextLabel *label, int x, int y) const
{
    return moveMeter(k, label, "RichTextLabel", x, y);
}

/** RichText/getRichTextPos
*
* SYNOPSIS
*   array getRichTextPos(widget, richtext)
* DESCRIPTION
*   Given a reference to a richtext object, this will return an array
*   containing the x and y coordinate of a richtext object.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to richtext -- richtext
* RETURN VALUE
*   x and y coordinates
*/
QVariantList KarambaInterface::getRichTextPos(const Karamba *k, const RichTextLabel *label) const
{
    return getMeterPos(k, label, "RichTextLabel");
}

/** RichText/resizeRichText
*
* SYNOPSIS
*   boolean resizeRichText(widget, richtext, w, h)
* DESCRIPTION
*   This will resize richtext to the new height and width.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to richtext -- richtext
*   * integer w -- new width
*   * integer h -- new height
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::resizeRichText(const Karamba *k, RichTextLabel *label, int width, int height)
    const
{
    return resizeMeter(k, label, "RichTextLabel", width, height);
}

/** RichText/setRichTextWidth
*
* SYNOPSIS
*   boolean setRichTextWidth(widget, richtext, width)
* DESCRIPTION
*   Given a reference to a rich text object, this function changes it's width
*   to the specified value in pixels.
*
*   The height adjusts automatically as the contents are changed with
*   changeRichText.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to richtext -- richtext
*   * integer width -- new width in pixels
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::setRichTextWidth(const Karamba *k, RichTextLabel *label, int width) const
{
    if (!checkKarambaAndMeter(k, label, "RichTextLabel")) {
        return false;
    }

    label->setWidth(width);

    return true;
}

/** RichText/getRichTextSize
*
* SYNOPSIS
*   array getRichTextSize(widget, richtext)
* DESCRIPTION
*   Given a reference to a richtext object, this will return a tuple
*   containing the height and width of a richtext object.
* ARGUMENTS
*   * long widget -- karamba
*   * long richtext -- richtext
* RETURN VALUE
*   width and height of the richtext
*/
QVariantList KarambaInterface::getRichTextSize(const Karamba *k, const RichTextLabel *label) const
{
    return getMeterSize(k, label, "RichTextLabel");
}

/** RichText/changeRichText
*
* SYNOPSIS
*   reference changeRichText(widget, richtext, value)
* DESCRIPTION
*   This will change the contents of a rich text meter. richText is the
*   reference to the text object to change that you saved from the
*   createRichText() call. text is a string containing the new value for the
*   rich text object.
*
*   The differance between Rich Text and a regular text field is that rich
*   text fields can display HTML code embedded in your text.
*
*   In a <a href="command"> ... </a> tag command is executed if the link is
*   click with the left mouse button.
*
*   Except if command starts with an '#' (ie: href="#value" ) the callback
*   meterClicked is called with value (without the #) as the meter argument.
*
*   Also inline images work. Unfortunately currently only when using absolute
*   paths.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to richtext -- richtext
*   * string value -- new text
* RETURN VALUE
*   true if successful
*/
QObject* KarambaInterface::changeRichText(const Karamba *k, RichTextLabel *label, const QString
        &text) const
{
    return setMeterStringValue(k, label, "RichTextLabel", text);
}

/** RichText/getRichTextValue
*
* SYNOPSIS
*   string getRichTextValue(widget, richtext)
* DESCRIPTION
*   Returns the current richtext value.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to richtext -- richtext
* RETURN VALUE
*   text that is displayed in the rich text
*/
QString KarambaInterface::getRichTextValue(const Karamba *k, const RichTextLabel *label) const
{
    return getMeterStringValue(k, label, "RichTextLabel");
}

/** RichText/getThemeRichText
*
* SYNOPSIS
*   reference getThemeRichText(widget, name)
* DESCRIPTION
*   You can reference richtext in your python code that was created in the
*   theme file. Basically, you just add a NAME= value to the GRAPH line in
*   the .theme file. Then if you want to use that object, instead of calling
*   createRichText, you can call this function.
*
*   The name you pass to the function is the same one that you gave it for
*   the NAME= parameter in the .theme file.
* ARGUMENTS
*   * long widget -- karamba
*   * string name -- name of the richtext in the theme file
* RETURN VALUE
*   reference to richtext
*/
QObject* KarambaInterface::getThemeRichText(const Karamba *k, const QString &meter) const
{
    return getThemeMeter(k, meter, "RichTextLabel");
}

/** RichText/hideRichText
*
* SYNOPSIS
*   boolean hideRichText(widget, richtext)
* DESCRIPTION
*   This hides the richtext. In other words, during subsequent calls to
*   widgetUpdate(), this richtext will not be drawn.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to richtext -- richtext
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::hideRichText(const Karamba *k, RichTextLabel *label) const
{
    return hideMeter(k, label, "RichTextLabel");
}

/** RichText/showRichText
*
* SYNOPSIS
*   boolean showRichText(widget, richtext)
* DESCRIPTION
*   This shows the richtext. In other words, during subsequent calls to
*   widgetUpdate(), this richtext will be drawn.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to richtext -- richtext
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::showRichText(const Karamba *k, RichTextLabel *label) const
{
    return showMeter(k, label, "RichTextLabel");
}

/** RichText/changeRichTextFont
*
* SYNOPSIS
*   long changeRichTextFont(widget, richtext, font)
* DESCRIPTION
*   This will change the font of a richtext meter.
* ARGUMENTS
*   * long widget -- karamba
*   * long richtext -- richtext
*   * string font -- name of the new font
* RETURN VALUE
*   1 if successful
*/
bool KarambaInterface::changeRichTextFont(const Karamba *k, RichTextLabel *label, const QString
        &font) const
{
    if (!checkKarambaAndMeter(k, label, "RichTextLabel")) {
        return false;
    }

    label->setFont(font);
    return true;
}

/** RichText/getRichTextFont
*
* SYNOPSIS
*   string getRichTextFont(widget, richtext)
* DESCRIPTION
*   Gets the font name of a richtext meter.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to richtext -- richtext
* RETURN VALUE
*   font name
*/
QString KarambaInterface::getRichTextFont(const Karamba *k, const RichTextLabel *label) const
{
    if (!checkKarambaAndMeter(k, label, "RichTextLabel")) {
        return QString();
    }

    return label->getFont();
}

/** RichText/changeRichTextSize
*
* SYNOPSIS
*   boolean changeRichTextSize(widget, richtext, size)
* DESCRIPTION
*   Sets the font size of a richtext meter.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to richtext -- richtext
*   * long size -- new font point size
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::changeRichTextSize(const Karamba *k, RichTextLabel *label, int size) const
{
    if (!checkKarambaAndMeter(k, label, "RichTextLabel")) {
        return false;
    }

    label->setFontSize(size);

    return true;
}

/** RichText/getRichTextFontSize
*
* SYNOPSIS
*   integer getRichTextFontSize(widget, richtext)
* DESCRIPTION
*   Gets the font size of the richtext meter.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to richtext -- richtext
* RETURN VALUE
*   font point size
*/
int KarambaInterface::getRichTextFontSize(const Karamba *k, const RichTextLabel *label) const
{
    if (!checkKarambaAndMeter(k, label, "RichTextLabel")) {
        return 0;
    }

    return label->getFontSize();
}

/** RichText/setRichTextSensor
*
* SYNOPSIS
*   boolean setRichTextSensor(widget, richtext, sensor)
* DESCRIPTION
*   Sets a new sensor string
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to richtext -- richtext
*   * string sensor -- new sensor string as in theme files
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::setRichTextSensor(Karamba *k, RichTextLabel *label, const QString &sensor)
    const
{
    return setMeterSensor(k, label, "RichTextLabel", sensor);
}

/** RichText/getRichTextSensor
*
* SYNOPSIS
*   string getRichTextSensor(widget, richtext)
* DESCRIPTION
*   Gets the current sensor string
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to richtext -- richtext
* RETURN VALUE
*   current sensor string
*/
QString KarambaInterface::getRichTextSensor(const Karamba *k, const RichTextLabel *label) const
{
    return getMeterSensor(k, label, "RichTextLabel");
}




QObject* KarambaInterface::createSystray(const Karamba *k, int x, int y, int w, int h) const
{
    Q_UNUSED(x);
    Q_UNUSED(y);
    Q_UNUSED(w);
    Q_UNUSED(h);

    if (!checkKaramba(k)) {
        return 0;
    }

    static bool firstMessage = false;

    if (!firstMessage) {
        kWarning() << "Call to \"createSystray\" not available in this version of SuperKaramba" ;
        firstMessage = true;
    }

    return 0;
}

int KarambaInterface::getCurrentWindowCount(const Karamba *k) const
{
    if (!checkKaramba(k)) {
        return -1;
    }

    static bool firstMessage = false;

    if (!firstMessage) {
        kWarning() << "Call to \"getCurrentWindowCount\" not available in this version of SuperKaramba" ;
        firstMessage = true;
    }

    return -1;
}

bool KarambaInterface::hideSystray(const Karamba *k) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    static bool firstMessage = false;

    if (!firstMessage) {
        kWarning() << "Call to \"hideSystray\" not available in this version of SuperKaramba" ;
        firstMessage = true;
    }

    return false;
}

bool KarambaInterface::moveSystray(const Karamba *k, int x, int y, int w, int h) const
{
    Q_UNUSED(x);
    Q_UNUSED(y);
    Q_UNUSED(w);
    Q_UNUSED(h);

    if (!checkKaramba(k)) {
        return false;
    }

    static bool firstMessage = false;

    if (!firstMessage) {
        kWarning() << "Call to \"moveSystray\" not available in this version of SuperKaramba" ;
        firstMessage = true;
    }

    return false;
}

bool KarambaInterface::showSystray(const Karamba *k) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    static bool firstMessage = false;

    if (!firstMessage) {
        kWarning() << "Call to \"showSystray\" not available in this version of SuperKaramba" ;
        firstMessage = true;
    }

    return false;
}

bool KarambaInterface::updateSystrayLayout(const Karamba *k) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    static bool firstMessage = false;

    if (!firstMessage) {
        kWarning() << "Call to \"updateSystrayLayout\" not available in this version of SuperKaramba" ;
        firstMessage = true;
    }

    return false;
}

bool KarambaInterface::getSystraySize(const Karamba *k) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    static bool firstMessage = false;

    if (!firstMessage) {
        kWarning() << "Call to \"getSystraySize\" not available in this version of SuperKaramba" ;
        firstMessage = true;
    }

    return false;
}



/** Task/getStartupInfo
*
* SYNOPSIS
*   list getStartupInfo(widget, task)
* DESCRIPTION
*   This returns all of the info about a certain starting task in the form of
*   a list. Widget is a reference to the current widget. task is a
*   reference to the window you want info about which you obtain by calling
*   getStartupList().
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to task -- task
* RETURN VALUE
*   Here is the format of the returned list by index value:
*   * 0 = Task name (The full name of the window)
*   * 1 = Icon name
*   * 2 = Executable name
*   * 3 = A reference back to the task you got info on
*/
QVariantList KarambaInterface::getStartupInfo(const Karamba *k, const Startup *startup)
    const
{
    if (!checkKaramba(k)) {
        return QVariantList();
    }

    QVariantList ret;

    ret << startup->text();
    ret << startup->icon();
    ret << startup->bin();
    ret << qVariantFromValue((QObject*)startup);

    return ret;
}

/** Task/getStartupList
*
* SYNOPSIS
*   list getTaskList(widget)
* DESCRIPTION
*   This returns a list with references to all the current
*   windows that are in the process of loading on this system. You can then
*   call getStartupInfo() on any of the entries in the list.
* ARGUMENTS
*   * reference to widget -- karamba
* RETURN VALUE
*   list with references to startups
*/
QVariantList KarambaInterface::getStartupList(const Karamba *k) const
{
    if (!checkKaramba(k)) {
        return QVariantList();
    }

    QVariantList ret;

    Startup::List startupList = TaskManager::self()->startups();
    foreach (Startup::StartupPtr startup, startupList) {
        ret << qVariantFromValue((QObject*)startup.data());
    }

    return ret;
}

/** Task/getTaskInfo
*
* SYNOPSIS
*   list getTaskInfo(widget, task)
* DESCRIPTION
*   This returns all of the info about a certain task in the form of a
*   list. widget is a reference to the current widget. Task is a reference to
*   the window you want info about which you obtain by calling getTaskList().
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to task -- task
* RETURN VALUE
*   Here is the format of the returned list by index value:
*   * 0 = Task name (The full name of the window)
*   * 1 = Icon name
*   * 2 = Class name - This is for grouping tasks. All tasks with the same
*         name can be grouped together because they are instances of the same
*         program.
*   * 3 = Desktop number - The desktop number this window is on
*   * 4 = Is this window maximized? false = no, true = yes
*   * 5 = Is this window iconified (minimized)? false = no, true = yes
*   * 6 = Is this window shaded (rolled up)? false = no, true = yes
*   * 7 = Is this window focused? fale = no, true = yes
*   * 8 = A reference back to the task you got info on
*/
QVariantList KarambaInterface::getTaskInfo(const Karamba *k, Task* task) const
{
    if (!checkKaramba(k)) {
        return QVariantList();
    }

    QVariantList ret;

    ret << task->name();
    ret << task->info().iconName();
    ret << task->className();
    ret << task->desktop();
    ret << task->isMaximized();
    ret << task->isIconified();
    ret << task->isShaded();
    ret << task->isActive();
    ret << qVariantFromValue((QObject*)task);

    return ret;
}

/** Task/getTaskList
*
* SYNOPSIS
*   list getTaskList(widget)
* DESCRIPTION
*   This returns a list with references to all the current
*   windows open on this system. You can then call performTaskAction() or
*   getTaskInfo() on any of the entries in the list.
* ARGUMENTS
*   * reference to widget -- karamba
* RETURN VALUE
*   list with references to the tasks
*/
QVariantList KarambaInterface::getTaskList(const Karamba *k) const
{
    if (!checkKaramba(k)) {
        return QVariantList();
    }

    QVariantList ret;

    QList<Task::TaskPtr> taskList = TaskManager::self()->tasks().values();

    Task::TaskPtr task;
    foreach(task, taskList) {
        ret << qVariantFromValue((QObject*)task.data());
    }

    return ret;
}

/** Task/getTaskNames
*
* SYNOPSIS
*   list getTaskNames(widget)
* DESCRIPTION
*   This returns a list containing the String names of all open
*   windows on the system. This is for convience if you want to list open
*   windows or see if a window by a certain name exists. Anything else
*   requires the reference to the window you would obtain from getTaskList()
* ARGUMENTS
*   * reference to widget -- karamba
* RETURN VALUE
*   list with task names
*/
QStringList KarambaInterface::getTaskNames(const Karamba *k) const
{
    if (!checkKaramba(k)) {
        return QStringList();
    }

    QStringList ret;

    QList<Task::TaskPtr> taskList = TaskManager::self()->tasks().values();

    Task::TaskPtr task;
    foreach(task, taskList) {
        ret << task->name();
    }

    return ret;
}

/** Task/performTaskAction
*
* SYNOPSIS
*   boolean performTaskAction(widget, task, action)
* DESCRIPTION
*   This peforms the given action on a task object. widget is a reference to
*   the current widget. Task is a reference to a task object you got from
*   getTaskList(). Action is a number from 1 to 10. See the list below.
*
*   Possible actions:
*   * 1 = Maximize the window
*   * 2 = Restore the window (use on iconified windows)
*   * 3 = Iconify the window (minimize it)
*   * 4 = Close the window
*   * 5 = Activate (give focus to) the window
*   * 6 = Raise the window
*   * 7 = Lower the window
*   * 8 = Smart Focus/Minimize - This will what the KDE taskbar does when you
*         click on a window. If it is iconified, raise it. If it has focus,
*         iconify it.
*   * 9 = Toggle whether this window is always on top
*   * 10 = Toggle whether this window is shaded (rolled up)
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to task -- task
*   * integer action -- action number
* RETURN VALUE
*   boolean if successful
*/
bool KarambaInterface::performTaskAction(const Karamba *k, Task* task, int action)
    const
{
    if (!checkKaramba(k)) {
        return false;
    }

    switch (action) {
        case 1:
            task->setMaximized(true);
            break;

        case 2:
            task->restore();
            break;

        case 3:
            task->setIconified(true);
            break;

        case 4:
            task->close();
            break;

        case 5:
            task->activate();
            break;

        case 6:
            task->raise();
            break;

        case 7:
            task->lower();
            break;

        case 8:
            task->activateRaiseOrIconify();
            break;

        case 9:
            task->toggleAlwaysOnTop();
            break;

        case 10:
            task->toggleShaded();
            break;

        default:
            kWarning() << "You are trying to perform an invalid "
                    << "action in performTaskAction" << endl;
            return false;

    }

    return true;
}




/** Text/createText
*
* SYNOPSIS
*   reference createText(widget, x, y, w, h, text)
* DESCRIPTION
*   This creates a new text at x, y with width and height w, h. You need to save
*   the return value of this function to call other functions on your text
*   field, such as changeText()
* ARGUMENTS
*   * reference to widget -- karamba
*   * integer x -- x coordinate
*   * integer y -- y coordinate
*   * integer w -- width
*   * integer h -- height
*   * string text -- text for the textlabel
* RETURN VALUE
*   Pointer to new text meter
*/
QObject* KarambaInterface::createText(Karamba* k, int x, int y, int width, int height, const QString
        &text) const
{
    if (!checkKaramba(k)) {
        return NULL;
    }

    TextLabel *tmp = new TextLabel(k, x, y, width, height);
    tmp->setValue(text);
    tmp->setTextProps(k->getDefaultTextProps());

    k->addToGroup(tmp);

    return tmp;
}

/** Text/deleteText
*
* SYNOPSIS
*   boolean deleteText(widget, text)
* DESCRIPTION
*   This removes a text object from the widget. Please do not call functions on
*   "text" after calling deleteText, as it does not exist anymore and that
*   could cause crashes in some cases.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to widget -- text meter
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::deleteText(Karamba *k, TextLabel *text) const
{
    if (!checkKarambaAndMeter(k, text, "TextLabel")) {
        return false;
    }

    return k->removeMeter(text);
}

/** Text/moveText
*
* SYNOPSIS
*   boolean moveText(widget, text, x, y)
* DESCRIPTION
*   This moves a text object to a new x, y relative to your widget. In other
*   words, (0,0) is the top corner of your widget, not the screen.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to text -- text meter
*   * integer x -- x coordinate
*   * integer y -- y coordinate
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::moveText(Karamba *k, TextLabel *text, int x, int y) const
{
    return moveMeter(k, text, "TextLabel", x, y);
}

/** Text/getTextPos
*
* SYNOPSIS
*   array getTextPos(widget, text)
* DESCRIPTION
*   Given a reference to a text object, this will return an array
*   containing the x and y coordinate of the text object.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to text -- text meter
* RETURN VALUE
*   x and y coordinate
*/
QVariantList KarambaInterface::getTextPos(const Karamba *k, const TextLabel *text) const
{
    return getMeterPos(k, text, "TextLabel");
}

/** Text/setTextSensor
*
* SYNOPSIS
*   boolean setTextSensor(widget, text, sensor)
* DESCRIPTION
*   Sets a new sensor string
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to text -- text meter
*   * string sensor -- new sensor string as in theme files
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::setTextSensor(Karamba *k, TextLabel *text, const QString &sensor) const
{
    return setMeterSensor(k, text, "TextLabel", sensor);
}

/** Text/getTextSensor
*
* SYNOPSIS
*   string getTextSensor(widget, text)
* DESCRIPTION
*   Gets the current sensor string
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to text -- text meter
* RETURN VALUE
*   currently used sensor string
*/
QString KarambaInterface::getTextSensor(const Karamba *k, const TextLabel *text) const
{
    return getMeterSensor(k, text, "TextLabel");
}

/** Text/resizeText
*
* SYNOPSIS
*   boolean resizeText(widget, text, w, h)
* DESCRIPTION
*   This will resize text to the new height and width.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to text -- text meter
*   * integer w -- new width
*   * integer h -- new height
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::resizeText(const Karamba *k, TextLabel *text, int width, int height) const
{
    return resizeMeter(k, text, "TextLabel", width, height);
}

/** Text/getTextSize
*
* SYNOPSIS
*   array getTextSize(widget, text)
* DESCRIPTION
*   Given a reference to a text object, this will return an array
*   containing the height and width of a text object.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to text -- text meter
* RETURN VALUE
*   height and width of the text
*/
QVariantList KarambaInterface::getTextSize(const Karamba *k, const TextLabel *text) const
{
    return getMeterSize(k, text, "TextLabel");
}

/** Text/getThemeText
*
* SYNOPSIS
*   reference getThemeText(widget, name)
* DESCRIPTION
*   You can reference text in your python code that was created in the
*   theme file. Basically, you just add a NAME= value to the TEXT line in
*   the .theme file. Then if you want to use that object, instead of calling
*   createText, you can call this function.
*
*   The name you pass to the function is the same one that you gave it for
*   the NAME= parameter in the .theme file.
* ARGUMENTS
*   * long widget -- karamba
*   * string name -- name of the text in the theme file
* RETURN VALUE
*   reference to text
*/
QObject* KarambaInterface::getThemeText(const Karamba *k, const QString &meter) const
{
    return getThemeMeter(k, meter, "TextLabel");
}

/** Text/hideText
*
* SYNOPSIS
*   boolean hideText(widget, text)
* DESCRIPTION
*   Hides text that is visible. You need to call redrawWidget() afterwords
*   to actually hide the text on screen.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to text -- text meter
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::hideText(const Karamba *k, TextLabel *text) const
{
    return hideMeter(k, text, "TextLabel");
}

/** Text/showText
*
* SYNOPSIS
*   boolean showText(widget, text)
* DESCRIPTION
*   Shows text that has been hidden with hideText().
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to text -- text meter
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::showText(const Karamba *k, TextLabel *text) const
{
    return showMeter(k, text, "TextLabel");
}

/** Text/changeText
*
* SYNOPSIS
*   reference changeText(widget, text, value)
* DESCRIPTION
*   This will change the contents of a text widget.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to text -- text meter
*   * long value -- new text to display
* RETURN VALUE
*   true if successful
*/
QObject* KarambaInterface::changeText(const Karamba *k, TextLabel *label, const QString &text) const
{
    return setMeterStringValue(k, label, "TextLabel", text);
}

/** Text/getTextValue
*
* SYNOPSIS
*   string getTextValue(widget, text)
* DESCRIPTION
*   Returns there current text value.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to text -- text meter
* RETURN VALUE
*   value
*/
QString KarambaInterface::getTextValue(const Karamba *k, const TextLabel *label) const
{
    return getMeterStringValue(k, label, "TextLabel");
}

/** Text/changeTextShadow
*
* SYNOPSIS
*   boolean changeTextShadow(widget, text, shadow)
* DESCRIPTION
*    This will change the shadow size of a text widget (only ones you
*    created through python currently). textToChange is the reference to the
*    text object to change that you saved from the createText() call. size
*    is the offset of the shadow in pixels. 1 or 2 is a good value in most
*    cases.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to text -- text meter
*   * integer shadow -- shadow offset
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::changeTextShadow(const Karamba *k, TextLabel *label, int shadow) const
{
    if (!checkKarambaAndMeter(k, label, "TextLabel")) {
        return false;
    }

    label->setShadow(shadow);
    return true;
}

/** Text/getTextShadow
*
* SYNOPSIS
*   integer getTextShadow(widget, text)
* DESCRIPTION
*   Gets the current shadow offset
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to text -- text meter
* RETURN VALUE
*   shadow offset
*/
int KarambaInterface::getTextShadow(const Karamba *k, const TextLabel *text) const
{
    if (!checkKarambaAndMeter(k, text, "TextLabel")) {
        return -1;
    }

    return text->getShadow();
}

/** Text/changeTextFont
*
* SYNOPSIS
*   boolean changeTextFont(widget, text, font)
* DESCRIPTION
*   This will change the font of a text widget (only ones you created
*   through python currently). Text is the reference to the text
*   object to change that you saved from the createText() call. Font is a
*   string with the name of the font to use.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to text -- text meter
*   * string font -- font name
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::changeTextFont(const Karamba *k, TextLabel *text, const QString &font) const
{
    if (!checkKarambaAndMeter(k, text, "TextLabel")) {
        return false;
    }

    text->setFont(font);

    return true;
}

/** Text/getTextFont
*
* SYNOPSIS
*   string getTextFont(widget, text)
* DESCRIPTION
*   Gets the current text font name
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to text -- text meter
* RETURN VALUE
*   font name
*/
QString KarambaInterface::getTextFont(const Karamba *k, const TextLabel *text) const
{
    if (!checkKarambaAndMeter(k, text, "TextLabel")) {
        return false;
    }

    return text->getFont();
}

/** Text/changeTextColor
*
* SYNOPSIS
*   boolean changeTextColor(widget, text, r, g, b, a=255)
* DESCRIPTION
*   This will change the color of a text widget (only ones you created
*   through python currently). textToChange is the reference to the text
*   object to change that you saved from the createText() call. r, g, b are
*   ints from 0 to 255 that represent red, green, blue and the alpha channel.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to text -- text meter
*   * integer red -- red component of color
*   * integer green -- green component of color
*   * integer blue -- blue component of color
*   * integer alpha -- alpha component of color (optional)
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::changeTextColor(const Karamba *k, TextLabel *text, int red, int green, int
        blue, int alpha) const
{
    return setMeterColor(k, text, "TextLabel", red, green, blue, alpha);
}

/** Text/getTextColor
*
* SYNOPSIS
*   array getTextColor(widget, text)
* DESCRIPTION
*   Gets the current text color
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to text -- text meter
* RETURN VALUE
*   (red, green, blue, alpha)
*/
QVariantList KarambaInterface::getTextColor(const Karamba *k, const TextLabel *text) const
{
    return getMeterColor(k, text, "TextLabel");
}

/** Text/changeTextSize
*
* SYNOPSIS
*   boolean changeTextSize(widget, text, size)
* DESCRIPTION
*   This will change the font size of a text widget (only ones you created
*   through python currently). text is the reference to the text
*   object to change that you saved from the createText() call. size is the
*   new font point size.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to text -- text meter
*   * integer size -- new size for text
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::changeTextSize(const Karamba *k, TextLabel *text, int size) const
{
    if (!checkKarambaAndMeter(k, text, "TextLabel")) {
        return false;
    }

    text->setFontSize(size);

    return true;
}

/** Text/getTextFontSize
*
* SYNOPSIS
*   integer getTextFontSize(widget, text)
* DESCRIPTION
*   Gets the current text font size.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to text -- text meter
* RETURN VALUE
*   text font size
*/
int KarambaInterface::getTextFontSize(const Karamba *k, const TextLabel *text) const
{
    if (!checkKarambaAndMeter(k, text, "TextLabel")) {
        return -1;
    }

    return text->getFontSize();
}

/** Text/getTextAlign
*
* SYNOPSIS
*   string getTextAlign(widget, text)
* DESCRIPTION
*   Gets the current text alignment.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to text -- text meter
* RETURN VALUE
*   LEFT, CENTER or RIGHT as string
*/
QString KarambaInterface::getTextAlign(const Karamba *k, const TextLabel *text) const
{
    if (!checkKarambaAndMeter(k, text, "TextLabel")) {
        return false;
    }

    return text->getAlignment();
}

/** Text/setTextAlign
*
* SYNOPSIS
*   boolean setTextAlign(widget, text, align)
* DESCRIPTION
*   Sets the text label align.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to text -- text meter
*   * string align -- LEFT, CENTER or RIGHT as string
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::setTextAlign(const Karamba *k, TextLabel *text, const QString &alignment)
    const
{
    if (!checkKarambaAndMeter(k, text, "TextLabel")) {
        return false;
    }

    text->setAlignment(alignment);

    return true;
}

bool KarambaInterface::setTextScroll(const Karamba *k, TextLabel *text, const QString &type, int x,
        int y, int gap, int pause) const
{
    if (!checkKarambaAndMeter(k, text, "TextLabel")) {
        return false;
    }

    text->setScroll(type, QPoint(x, y), gap, pause);

    return true;
}

/** Text/getTextTextWidth
*
* SYNOPSIS
*   integer getTextTextWidth(widget, text)
* DESCRIPTION
*   Gets the current text width.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to text -- text meter
* RETURN VALUE
*   text width
*/
int KarambaInterface::getTextTextWidth(const Karamba *k, const TextLabel *text) const
{
    if (!checkKarambaAndMeter(k, text, "TextLabel")) {
        return -1;
    }

    return text->getTextWidth();
}





/** Widget/createWidgetMask
*
* SYNOPSIS
*   long createWidgetMask(widget, mask)
* DESCRIPTION
*   This function doesn't work currently due to a bug in KDE. Please use
*   MASK= in your .theme file for the time being.
* WARNING
*   This function does nothing in SuperKaramba 0.50 and later
* ARGUMENTS
*   * reference to widget -- karamba
*   * string mask --  The path to the widget mask file.
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::createWidgetMask(const Karamba *k, const QString &mask) const
{
    Q_UNUSED(k);
    Q_UNUSED(mask);

    return true;
}

/** Widget/getWidgetPosition
*
* SYNOPSIS
*   array getWidgetPosition(widget)
* DESCRIPTION
*   Returns an array containing the x and y position of you widget.
*   widget is a reference to the current widget.
* ARGUMENTS
*   * reference to widget -- karamba
* RETURN VALUE
*   position of the widget
*/
QVariantList KarambaInterface::getWidgetPosition(const Karamba *k) const
{
    if (!checkKaramba(k)) {
        return QVariantList();
    }

    QPoint pos = k->getPosition();

    QVariantList ret;
    ret << pos.x();
    ret << pos.y();

    return ret;
}

/** Widget/moveWidget
*
* SYNOPSIS
*   boolean moveWidget(widget, x, y)
* DESCRIPTION
*   Moves the widget to the new screen location.
* ARGUMENTS
*   * reference to widget -- karamba
*   * integer x -- x coordinate
*   * integer y -- y coordinate
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::moveWidget(Karamba *k, int x, int y) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    k->moveToPos(QPoint(x, y));

    return true;
}

/** Widget/redrawWidget
*
* SYNOPSIS
*   boolean redrawWidget(widget)
* DESCRIPTION
*   This is THE most important function. After you do a bunch of other calls
*   (moving images, adding images or text, etc), you call this to update the
*   widget display area. You will not see your changes until you call this.
*   Redraws all meters.
* ARGUMENTS
*   * reference to widget -- karamba
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::redrawWidget(Karamba *k) const
{
    if (!checkKaramba(k))
        return false;

    k->redrawWidget();

    return true;
}

/** Widget/redrawWidgetBackground
*
* SYNOPSIS
*   boolean redrawWidgetBackground(widget)
* DESCRIPTION
*   Redraws the widget background.
* WARNING
*   This function does nothing in SuperKaramba 0.50 and later
* ARGUMENTS
*   * reference to widget -- karamba
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::redrawWidgetBackground(const Karamba *k) const
{
    Q_UNUSED(k);

    return true;
}

/** Widget/resizeWidget
*
* SYNOPSIS
*   boolean resizeWidget(widget, w, h)
* DESCRIPTION
*   Resizes the widget to the new width and height.
* ARGUMENTS
*   * reference to widget -- karamba
*   * integer w -- width
*   * integer h -- height
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::resizeWidget(Karamba *k, int width, int height) const
{
    if (!checkKaramba(k)) {
        return false;
    }

    k->resizeTo(width, height);

    return true;
}

/** Widget/toggleWidgetRedraw
*
* SYNOPSIS
*   boolean toggleWidgetRedraw(widget, b)
* DESCRIPTION
*   Toggles widget redraw.
** WARNING
*   This function does nothing in SuperKaramba 0.50 and later
* ARGUMENTS
*   * reference to widget -- karamba
*   * boolean b -- 1 = widget is drawn
* RETURN VALUE
*   true if successful
*/
bool KarambaInterface::toggleWidgetRedraw(const Karamba *k, bool enable) const
{
    Q_UNUSED(k);
    Q_UNUSED(enable);

    return true;
}

/** Sensor/getPlasmaSensor
*
* SYNOPSIS
*   dataengine getPlasmaSensor(widget, engine, source)
* DESCRIPTION
*   Returns a sensor for Plasma::DataEngine objects.
* ARGUMENTS
*   * reference to widget -- karamba
*   * string engine -- The name of the dataengine. This could be for example "time" for the Plasma TimeEngine.
*   * string source -- The source. E.g. "Local" for the Plasma TimeEngine.
* RETURN VALUE
*   the reference to an instance of the SuperKaramba PlasmaSensor class. See also http://websvn.kde.org/trunk/kdeutils/superkaramba/src/sensors/plasmaengine.h
*/
QObject* KarambaInterface::getPlasmaSensor(Karamba *k, const QString& engine, const QString& source)
{
    if (!checkKaramba(k)) {
        return 0;
    }
    return k->getPlasmaSensor(engine, source);
}

/** CanvasWidget/createCanvasWidget
*
* SYNOPSIS
*   canvaswidget createCanvasWidget(widget, widget)
* DESCRIPTION
*   Adds a QWidget to the scene and returns a canvaswidget object.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to widget -- the QWidget which should be added to the QGraphicsScene.
* RETURN VALUE
*   the reference to an instance of a QGraphicsProxyWidget canvaswidget object.
*/
QObject* KarambaInterface::createCanvasWidget(Karamba *k, QWidget* widget)
{
    if (!checkKaramba(k)) {
        return 0;
    }
    QGraphicsProxyWidget* proxy = k->getScene()->addWidget(widget);
    proxy->setGeometry(k->boundingRect());
    proxy->setVisible(true);
    return proxy;
}

/** CanvasWidget/moveCanvasWidget
*
* SYNOPSIS
*   boolean moveCanvasWidget(widget, canvaswidget, x, y, w, h)
* DESCRIPTION
*   Moves the canvaswidget object to a new position.
* ARGUMENTS
*   * reference to widget -- karamba
*   * reference to widget -- the QGraphicsProxyWidget canvaswidget object which should be moved.
*   * integer x -- x coordinate
*   * integer y -- y coordinate
*   * integer w -- width
*   * integer h -- height
* RETURN VALUE
*   true if the move was successful done else false.
*/
bool KarambaInterface::moveCanvasWidget(Karamba *k, QObject* canvaswidget, int x, int y, int w, int h)
{
    QGraphicsProxyWidget* proxy = dynamic_cast<QGraphicsProxyWidget*>(canvaswidget);
    if (!checkKaramba(k) || !proxy) {
        return false;
    }
    proxy->setGeometry(QRectF(x, y, w, h));
    return true;
}
