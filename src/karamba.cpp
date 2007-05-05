/*
 * Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>
 * Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>
 * Copyright (C) 2004,2005 Luke Kenneth Casson Leighton <lkcl@lkcl.net>
 * Copyright (c) 2005 Ryan Nickell <p0z3r@earthlink.net>
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

#include "karamba.h"
#include "karamba.moc"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QStack>
#include <QDesktopWidget>
#include <QApplication>
#include <QX11Info>
#include <QBitmap>
#include <QSignalMapper>

#include <KDebug>
#include <KWindowSystem>
#include <KIcon>
#include <KLocale>
#include <KDirWatch>
#include <KMenu>
#include <KConfig>
#include <KToggleAction>
#include <KCmdLineArgs>

#include "meters/textfield.h"
#include "meters/richtextlabel.h"
#include "meters/bar.h"
#include "meters/graph.h"
#include "meters/textlabel.h"
#include "meters/input.h"
#include "meters/imagelabel.h"
#include "meters/clickarea.h"
#include "meters/clickmap.h"

#include "sensors/sensor.h"
#include "sensors/mem.h"
#include "sensors/disk.h"
#include "sensors/network.h"
#include "sensors/date.h"
#include "sensors/program.h"
#include "sensors/sensorparams.h"
#include "sensors/textfile.h"
#include "sensors/rss.h"
#include "sensors/uptime.h"
#include "sensors/lmsensor.h"
#include "sensors/cpu.h"

#include "python/karamba.h"

#include "karambaapp.h"
#include "systemtray.h"
#include "karambainterface.h"
#include "mainwidget.h"
#include "lineparser.h"
#include "themelocale.h"
#include "superkarambasettings.h"

Karamba::Karamba(const KUrl &themeFile, int instance, bool subTheme, const QPoint &startPos, bool reload)
        : QObject(),
        QGraphicsItemGroup(),
        m_scene(0),
        m_view(0),
        m_KWinModule(0),
        m_useKross(true),
        m_python(0),
        m_interface(0),
        m_foundKaramba(false),
        m_onTop(false),
        m_managed(false),
        m_info(0),
        m_desktop(0),
        m_interval(0),
        m_tempUnit('C'),
        m_defaultTextField(0),
        m_scaleStep(-1),
        m_showMenu(false),
        m_popupMenu(0),
        m_toggleLocked(0),
        m_themeConfMenu(0),
        m_toDesktopMenu(0),
        m_globalMenu(0),
        m_stepTimer(0),
        m_signalMapperConfig(0),
        m_signalMapperDesktop(0),
        m_config(0),
        m_instance(instance),
        m_wantRightButton(false),
        m_globalView(true),
        m_subTheme(subTheme),
        m_animation(0),
        m_timer(0),
        m_backgroundInterface(0)
{
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    if (args->isSet("usefallback")) {
        m_useKross = false;
    }

    if (m_view == 0 && m_scene == 0) {
        m_scene = new QGraphicsScene;
        m_scene->addItem(this);
        m_view = new MainWidget(m_scene);
        m_view->setRenderHints(QPainter::Antialiasing |
                               QPainter::SmoothPixmapTransform);
        m_globalView = false;
        m_view->show();
    }

    m_view->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

    if (!m_theme.set(themeFile)) {
        return;
    }

    KDirWatch *dirWatch = KDirWatch::self();
    connect(dirWatch, SIGNAL(dirty(const QString &)),
            SLOT(slotFileChanged(const QString &)));

    if (!dirWatch->contains(m_theme.file()))
        dirWatch->addFile(m_theme.file());

    if (!m_theme.isZipTheme() && m_theme.scriptModuleExists()) {
        QString scriptFile = m_theme.path() + '/' + m_theme.scriptModule();
        if (!dirWatch->contains(scriptFile))
            dirWatch->addFile(scriptFile);
    }

    if (m_prettyName.isEmpty())
        m_prettyName = QString("%1 - %2").arg(m_theme.name()).arg(m_instance);

    setObjectName("karamba - " + m_prettyName);

    m_info = new NETWinInfo(QX11Info::display(), m_view->winId(),
                            QX11Info::appRootWindow(), NET::WMState);

    m_defaultTextField = new TextField();

    m_KWinModule = KWindowSystem::self();
    connect(m_KWinModule, SIGNAL(currentDesktopChanged(int)), this,
            SLOT(currentDesktopChanged(int)));

    m_backgroundInterface = new org::kde::kdesktop::Background("org.kde.kdesktop", "/Background", QDBusConnection::sessionBus());
    connect(m_backgroundInterface, SIGNAL(backgroundChanged(int)), this,
            SLOT(currentWallpaperChanged(int)));

    setAcceptsHoverEvents(true);

    // Setup of the Task Manager Callbacks
    connect(TaskManager::self(), SIGNAL(activeTaskChanged(Task::TaskPtr)), this,
            SLOT(activeTaskChanged(Task::TaskPtr)));
    connect(TaskManager::self(), SIGNAL(taskAdded(Task::TaskPtr)), this,
            SLOT(taskAdded(Task::TaskPtr)));
    connect(TaskManager::self(), SIGNAL(taskRemoved(Task::TaskPtr)), this,
            SLOT(taskRemoved(Task::TaskPtr)));
    connect(TaskManager::self(), SIGNAL(startupAdded(Startup::StartupPtr)), this,
            SLOT(startupAdded(Startup::StartupPtr)));
    connect(TaskManager::self(), SIGNAL(startupRemoved(Startup::StartupPtr)), this,
            SLOT(startupRemoved(Startup::StartupPtr)));

    m_signalMapperConfig = new QSignalMapper(this);
    connect(m_signalMapperConfig, SIGNAL(mapped(QObject*)), this,
            SLOT(slotToggleConfigOption(QObject*)));

    m_signalMapperDesktop = new QSignalMapper(this);
    connect(m_signalMapperDesktop, SIGNAL(mapped(int)), this,
            SLOT(slotDesktopChanged(int)));

    preparePopupMenu();

    parseConfig();

    if (!(m_onTop || m_managed))
        KWindowSystem::lowerWindow(m_view->winId());
    /*
      m_view->setFocusPolicy(Qt::StrongFocus);
      KWindowSystem::setType(m_view->winId(), NET::Dock);
      KWindowSystem::setState(m_view->winId(), NET::KeepBelow);
      KWindowSystem::lowerWindow(m_view->winId());
    */
    QString instanceString;
    if (m_instance > 1)
        instanceString = QString("-%1").arg(m_instance);

    QString cfg = QDir::home().absolutePath() + "/.superkaramba/"
                  + m_theme.id() + instanceString + ".rc";
    kDebug() << cfg << endl;

    QFile themeConfigFile(cfg);
    // Tests if config file Exists
    if (!QFileInfo(themeConfigFile).exists()) {
        // Create config file
        themeConfigFile.open(QIODevice::ReadWrite);
        themeConfigFile.close();
    }

    m_config = new KConfig(cfg, KConfig::NoGlobals);

    // Karamba specific Config Entries
    KConfigGroup cg(m_config, "internal");
    bool locked = m_toggleLocked->isChecked();
    locked = cg.readEntry("lockedPosition", locked);
    m_toggleLocked->setChecked(locked);

    int desktop = 0;
    desktop = cg.readEntry("desktop", desktop);
    if (desktop > m_KWinModule->numberOfDesktops()) {
        desktop = 0;
    }

    slotDesktopChanged(desktop);

    cg.changeGroup("theme");
    if (cg.hasKey("widgetPosX") && cg.hasKey("widgetPosY")) {
        int xpos = cg.readEntry("widgetPosX", 0);
        int ypos = cg.readEntry("widgetPosY", 0);

        if (xpos < 0)
            xpos = 0;
        if (ypos < 0)
            ypos = 0;

        if (startPos.isNull()) {
            if (m_globalView) {
                    setPos(xpos, ypos);
            } else {
                    m_view->move(xpos, ypos);
            }
        }
    }

    if (!startPos.isNull()) {
        if (m_globalView) {
            setPos(startPos - m_themeCenter);
        } else {
            m_view->move(startPos - m_themeCenter);
        }

        m_toggleLocked->setChecked(false);
    }

    if (!reload) {
        m_timer = new QTimeLine(1000);
        m_timer->setFrameRange(0, 1000);

        m_animation = new QGraphicsItemAnimation;
        m_animation->setItem(this);
        m_animation->setTimeLine(m_timer);

        // Use 201 here because 200.0/200.0 < 1 => theme is still scaled
        for (int i = 0; i < 201; i++) {
            m_animation->setScaleAt(i / 200.0, 1 / 200.0 * i, 1 / 200.0 * i);
            QPointF animPos(
                boundingRect().width()/2*(1-m_animation->verticalScaleAt(i / 200.0)),
                boundingRect().height()/2*(1-m_animation->horizontalScaleAt(i / 200.0))
            );
                m_animation->setPosAt(i / 200.0, animPos);
        }

        m_timer->start();
    }

    QTimer::singleShot(0, this, SLOT(startKaramba()));
}

Karamba::~Karamba()
{
    m_scene->removeItem(this);

    delete m_config;

    delete m_info;

    if (m_python) {
        delete m_python;
    }

    if (m_interface) {
        delete m_interface;
    }

    qDeleteAll(m_sensorList);
    m_sensorList.clear();

    delete m_toDesktopMenu;
    delete m_themeConfMenu,
    delete m_toggleLocked;
    delete m_popupMenu;

    delete m_stepTimer;

    delete m_animation;
    delete m_timer;

    delete m_backgroundInterface;

    if (!m_globalView) {
        delete m_view;
        delete m_scene;
    }
}

void Karamba::startKaramba()
{
    emit widgetStarted(this, true, false);

    if (m_theme.scriptModuleExists()) {
        kDebug() << "Loading script module: " << m_theme.scriptModule() << endl;

        m_stepTimer = new QTimer(this);
        m_stepTimer->setSingleShot(true);

        if (!m_useKross) {
            m_python = new KarambaPython(m_theme, false);
            m_python->initWidget(this);
        } else {
            m_interface = new KarambaInterface(this);
            m_interface->callInitWidget(this);
        }

        update();

        connect(m_stepTimer, SIGNAL(timeout()), SLOT(step()));
        m_stepTimer->start(m_interval);
    }
}

QString Karamba::prettyName() const
{
    return m_prettyName;
}

void Karamba::setPrettyName(const QString &prettyThemeName)
{
    m_prettyName = prettyThemeName;
}

void Karamba::step()
{
    m_stepTimer->start(m_interval);

    if (m_python)
        m_python->widgetUpdated(this);

    if (m_interface)
        m_interface->callWidgetUpdated(this);

    update();
}

void Karamba::redrawWidget()
{
    // Force the event loop to process the update() calls
    QCoreApplication::processEvents();
}

bool Karamba::parseConfig()
{
    //qDebug("karamba::parseConfig");
    bool passive = true;

    if (m_theme.open()) {
        QStack<QPoint> offsetStack;
        LineParser lineParser;
        int x = 0;
        int y = 0;
        int w = 0;
        int h = 0;

        offsetStack.push(QPoint(0, 0));

        while (m_theme.nextLine(lineParser)) {
            x = lineParser.getInt("X") + offsetStack.top().x();
            y = lineParser.getInt("Y") + offsetStack.top().y();
            w = lineParser.getInt("W");
            h = lineParser.getInt("H");

            if (lineParser.meter() == "KARAMBA" && !m_foundKaramba) {
                m_toggleLocked->setChecked(lineParser.getBoolean("LOCKED"));

                x = (x < 0) ? 0 : x;
                y = (y < 0) ? 0 : y;

                if (w == 0 ||  h == 0) {
                    w = 300;
                    h = 300;
                }

                // Store the center of the theme,
                // so it can be centered when a user
                // drags it from the theme dialog.
                m_themeCenter = QPoint(w/2, h/2);

                setFixedSize(w, h);
                if (!m_globalView)
                    m_view->setFixedSize(w + 5, h + 5);

                if (lineParser.getBoolean("RIGHT")) {
                    QDesktopWidget *d = QApplication::desktop();
                    x = d->width() - w;
                } else if (lineParser.getBoolean("LEFT")) {
                    x = 0;
                }

                if (lineParser.getBoolean("BOTTOM")) {
                    QDesktopWidget *d = QApplication::desktop();
                    y = d->height() - h;
                } else if (lineParser.getBoolean("TOP")) {
                    y = 0;
                }

                if (m_globalView)
                    setPos(x, y);
                else
                    m_view->move(x, y);

                if (lineParser.getBoolean("ONTOP")) {
                    m_onTop = true;
                    KWindowSystem::setState(m_view->winId(), NET::KeepAbove);
                }

                if (lineParser.getBoolean("MANAGED")) {
                    m_managed = true;
                    Qt::WindowFlags flags = Qt::Dialog;
                    flags & Qt::WA_DeleteOnClose;
                    m_view->setWindowFlags(flags);
                } else {
                    m_info->setState(NETWinInfo::SkipTaskbar
                                     | NETWinInfo::SkipPager, NETWinInfo::SkipTaskbar
                                     | NETWinInfo::SkipPager);
                    if (m_onTop) {
                        KWindowSystem::setState(m_view->winId(), NET::KeepAbove);
                    }
                }

                if (lineParser.getBoolean("ONALLDESKTOPS")) {
                    m_desktop = 200; // ugly
                }

                bool dfound = false;
                //int desktop = lineParser.getInt("DESKTOP", line, dfound);
                if (dfound) {
                    m_info->setDesktop(dfound);
                }

                if (lineParser.getBoolean("TOPBAR")) {
                    setPos(x, 0);
                    KWindowSystem::setStrut(m_view->winId(), 0, 0, h, 0);
                    //toggleLocked->setChecked(true);
                    //toggleLocked->setEnabled(false);
                }

                if (lineParser.getBoolean("BOTTOMBAR")) {
                    int dh = QApplication::desktop()->height();
                    setPos(x, dh - h);
                    KWindowSystem::setStrut(m_view->winId(), 0, 0, 0, h);
                    //toggleLocked->setChecked(true);
                    //toggleLocked->setEnabled(false);
                }

                if (lineParser.getBoolean("RIGHTBAR")) {
                    int dw = QApplication::desktop()->width();
                    setPos(dw - w, y);
                    KWindowSystem::setStrut(m_view->winId(), 0, w, 0, 0);
                    //toggleLocked->setChecked(true);
                    //toggleLocked->setEnabled(false);
                }

                if (lineParser.getBoolean("LEFTBAR")) {
                    setPos(0, y);
                    KWindowSystem::setStrut(m_view->winId(), w, 0, 0, 0);
                    //toggleLocked->setChecked( true );
                    //toggleLocked->setEnabled(false);
                }

                if (m_globalView)
                    setFlag(QGraphicsItem::ItemIsMovable,
                            m_toggleLocked->isChecked());


                /* Masking gone
                QString path = lineParser.getString("MASK");

                QFileInfo info(path);
                QString absPath;
                QBitmap bmMask;
                QByteArray ba;
                if( info.isRelative() )
                {
                  absPath = m_theme.path();
                  absPath.append(path);
                  ba = m_theme.readThemeFile(path);
                }
                else
                {
                  absPath = path;
                  ba = m_theme.readThemeFile(info.fileName());
                }
                if(m_theme.isZipTheme())
                {
                  bmMask.loadFromData(ba);
                }
                else
                {
                  bmMask.load(absPath);
                }
                m_view->setMask(bmMask);
                */

                m_interval = lineParser.getInt("INTERVAL");
                m_interval = (m_interval == 0) ? 1000 : m_interval;

                QString temp = lineParser.getString("TEMPUNIT", "C").toUpper();
                m_tempUnit = temp.at(0).toAscii();
                m_foundKaramba = true;
            }

            if (lineParser.meter() == "THEME") {
                QString path = lineParser.getString("PATH");
                QFileInfo info(path);
                if (info.isRelative())
                    path = m_theme.path() + '/' + path;

                new Karamba(path/*, m_view, m_scene*/);
            }

            if (lineParser.meter() == "<GROUP>") {
                int offsetX = offsetStack.top().x();
                int offsetY = offsetStack.top().y();
                offsetStack.push(QPoint(offsetX + lineParser.getInt("X"),
                                        offsetY + lineParser.getInt("Y")));
            }

            if (lineParser.meter() == "</GROUP>") {
                offsetStack.pop();
            }

            if (lineParser.meter() == "CLICKAREA") {
                m_view->setInteractive(true);

                bool preview = lineParser.getBoolean("PREVIEW");
                ClickArea *tmp = new ClickArea(this, preview, x, y, w, h);
                tmp->setOnClick(lineParser.getString("ONCLICK"));


                setSensor(lineParser, (Meter*)tmp);
            }

            if (lineParser.meter() == "SENSOR=PROGRAM") {
                setSensor(lineParser, 0);
            }

            if (lineParser.meter() == "IMAGE") {
                QString file = lineParser.getString("PATH");
                QString file_roll = lineParser.getString("PATHROLL");
                int xon = lineParser.getInt("XROLL");
                int yon = lineParser.getInt("YROLL");
                QString tiptext = lineParser.getString("TOOLTIP");
                QString name = lineParser.getString("NAME");
                bool bg = lineParser.getBoolean("BACKGROUND");
                xon = (xon <= 0) ? x : xon;
                yon = (yon <= 0) ? y : yon;

                ImageLabel *tmp = new ImageLabel(this, x, y, 0, 0);
                tmp->setValue(file);

                if (!file_roll.isEmpty())
                    tmp->parseImages(file, file_roll, x, y, xon, yon);

                tmp->setBackground(bg);

                if (!name.isEmpty())
                    tmp->setObjectName(name);

                if (!tiptext.isEmpty())
                    tmp->setTooltip(tiptext);

                setSensor(lineParser, (Meter*)tmp);
            }

            if (lineParser.meter() == "DEFAULTFONT") {
                delete m_defaultTextField;
                m_defaultTextField = new TextField();

                m_defaultTextField->setColor(lineParser.getColor("COLOR",
                                             QColor("black")));
                m_defaultTextField->setBGColor(lineParser.getColor("BGCOLOR",
                                               QColor("white")));
                m_defaultTextField->setFont(lineParser.getString("FONT", "Helvetica"));
                m_defaultTextField->setFontSize(lineParser.getInt("FONTSIZE", 12));
                m_defaultTextField->setAlignment(lineParser.getString("ALIGN",
                                                 "LEFT"));
                m_defaultTextField->setFixedPitch(lineParser.getBoolean("FIXEDPITCH",
                                                  false));
                m_defaultTextField->setShadow(lineParser.getInt("SHADOW", 0));
            }

            if (lineParser.meter() == "TEXT" ||
                    lineParser.meter() == "CLICKMAP" ||
                    lineParser.meter() == "RICHTEXT" ||
                    lineParser.meter() == "INPUT") {
                TextField defTxt;

                if (m_defaultTextField)
                    defTxt = *m_defaultTextField;

                TextField* tmpText = new TextField();

                tmpText->setColor(lineParser.getColor("COLOR", defTxt.getColor()));
                tmpText->setBGColor(lineParser.getColor("BGCOLOR",
                                                        defTxt.getBGColor()));
                tmpText->setFont(lineParser.getString("FONT", defTxt.getFont()));
                tmpText->setFontSize(lineParser.getInt("FONTSIZE",
                                                       defTxt.getFontSize()));
                tmpText->setAlignment(lineParser.getString("ALIGN",
                                      defTxt.getAlignmentAsString()));
                tmpText->setFixedPitch(lineParser.getInt("FIXEDPITCH",
                                       defTxt.getFixedPitch()));

                tmpText->setShadow(lineParser.getInt("SHADOW", defTxt.getShadow()));

                //////////////////////////////////////////////////////
                // Now handle the specifics
                if (lineParser.meter() == "TEXT") {
                    TextLabel *tmp = new TextLabel(this, x, y, w, h);
                    tmp->setValue(
                        m_theme.locale()->translate(lineParser.getString("VALUE")));
                    tmp->setTextProps(tmpText);

                    QString name = lineParser.getString("NAME");
                    if (!name.isEmpty())
                        tmp->setObjectName(name);

                    setSensor(lineParser, (Meter*)tmp);
                }

                if (lineParser.meter() == "CLICKMAP") {
                    m_view->setInteractive(true);

                    ClickMap *tmp = new ClickMap(this, x, y, w, h);
                    tmp->setTextProps(tmpText);

                    setSensor(lineParser, (Meter*)tmp);
                    // set all params
                    //clickList -> append(tmp);
                    //meterList->append( tmp );

                }

                if (lineParser.meter() == "RICHTEXT") {
                    RichTextLabel *tmp = new RichTextLabel(this, x, y, w, h);

                    bool dUl = lineParser.getBoolean("UNDERLINE");

                    tmp->setText(
                        m_theme.locale()->translate(lineParser.getString("VALUE")), dUl);
                    tmp->setTextProps(tmpText);
//          tmp->setWidth(w);
//          tmp->setHeight(h);

                    QString name = lineParser.getString("NAME");
                    if (!name.isEmpty())
                        tmp->setObjectName(name);

                    setSensor(lineParser, (Meter*)tmp);
                    //clickList->append(tmp);
                    //meterList->append(tmp);
                }

                if (lineParser.meter() == "INPUT") {
                    Input *tmp = new Input(this, x, y, w, h);

                    QString name = lineParser.getString("NAME");
                    //if (!name.isEmpty())
                    //tmp->setObjectName(name);

                    tmp->setTextProps(tmpText);
                    tmp->setValue(
                        m_theme.locale()->translate(lineParser.getString("VALUE")));

                    //meterList->append(tmp);
                    //passive = false;
                }

                delete tmpText;
            }

            if (lineParser.meter() == "BAR") {
                Bar *tmp = new Bar(this, x, y, w, h);

                tmp->setImage(lineParser.getString("PATH"));
                tmp->setVertical(lineParser.getBoolean("VERTICAL"));
                tmp->setMax(lineParser.getInt("MAX", 100));
                tmp->setMin(lineParser.getInt("MIN", 0));
                tmp->setValue(lineParser.getInt("VALUE"));

                QString name = lineParser.getString("NAME");
                if (!name.isEmpty())
                    tmp->setObjectName(name);

                setSensor(lineParser, (Meter*)tmp);
                //meterList->append(tmp);
            }

            if (lineParser.meter() == "GRAPH") {
                int points = lineParser.getInt("POINTS");

                Graph *tmp = new Graph(this, x, y, w, h, points);
                tmp->setMax(lineParser.getInt("MAX", 100));
                tmp->setMin(lineParser.getInt("MIN", 0));
                QString name = lineParser.getString("NAME");
                if (!name.isEmpty())
                    tmp->setObjectName(name);

                tmp->setColor(lineParser.getColor("COLOR"));

                setSensor(lineParser, (Graph*)tmp);
                //meterList->append(tmp);
            }
        }

        if (passive && !m_managed) {
            // Matthew Kay: set window type to "dock"
            // (plays better with taskbar themes this way)
            //KWindowSystem::setType(m_view->winId(), NET::Dock);
            //KDE 3.2 addition for the always on top issues
            //KWindowSystem::setState(m_view->winId(), NET::KeepBelow);
        }

        m_theme.close();
    }
    //qDebug("parseConfig ok: %d", foundKaramba);
    if (!m_foundKaramba) {
        //  interval = initKaramba( "", 0, 0, 0, 0 );
        //   this->close(true);
        //delete this;
        return false;
    } else {
        return true;
    }
}

TextField* Karamba::getDefaultTextProps()
{
    return m_defaultTextField;
}

QString Karamba::findSensorFromMap(const Sensor* sensor) const
{
    //qDebug("karamba::findSensorFromMap");
    QMap<QString, Sensor*>::ConstIterator it;
    QMap<QString, Sensor*>::ConstIterator end(m_sensorMap.end());
    for (it = m_sensorMap.begin(); it != end; ++it) {
        if (it.value() == sensor)
            return it.key();
    }
    return "";
}

Sensor *Karamba::findSensorFromList(const Meter* meter) const
{
    foreach(Sensor *sensor, m_sensorList) {
        if (sensor->hasMeter(meter))
            return sensor;
    }

    return 0;
}

void Karamba::deleteMeterFromSensors(Meter* meter)
{
    //qDebug("karamba::deleteMeterFromSensors");
    Sensor* sensor = findSensorFromList(meter);

    if (sensor) {
        sensor->deleteMeter(meter);
        if (sensor->isEmpty()) {
            QString s = findSensorFromMap(sensor);
            m_sensorMap.remove(s);
            m_sensorList.removeAll(sensor);
            delete sensor;
        }
    }
}

QString Karamba::getSensor(const Meter* meter) const
{
    Sensor* sensor = findSensorFromList(meter);

    if (sensor)
        return findSensorFromMap(sensor);

    return QString::null;
}

bool Karamba::removeMeter(Meter* meter)
{
    QList<QGraphicsItem*> items = QGraphicsItemGroup::children();

    if (items.contains(meter)) {
        deleteMeterFromSensors(meter);
        delete meter;
        return true;
    } else
        return false;
}

void Karamba::setSensor(const LineParser& lineParser, Meter* meter)
{
    Sensor* sensor = 0;

    deleteMeterFromSensors(meter);

    QString sens = lineParser.getString("SENSOR").toUpper();

    if (sens == "CPU") {
        QString cpuNbr = lineParser.getString("CPU");
        sensor = m_sensorMap["CPU"+cpuNbr];

        if (sensor == 0) {
            int interval = lineParser.getInt("INTERVAL");
            interval = (interval == 0) ? 1000 : interval;
            sensor = (m_sensorMap["CPU" + cpuNbr] = new CPUSensor(cpuNbr, interval));
            m_sensorList.append(sensor);
        }

        SensorParams *sp = new SensorParams(meter);
        sp->addParam("FORMAT",
                     m_theme.locale()->translate(lineParser.getString("FORMAT")));

        sp->addParam("DECIMALS", lineParser.getString("DECIMALS"));

        sensor->addMeter(sp);
        sensor->setMaxValue(sp);
    }

    if (sens == "MEMORY") {
        sensor = m_sensorMap["MEMORY"];
        if (sensor == 0) {
            int interval = lineParser.getInt("INTERVAL");
            interval = (interval == 0) ? 3000 : interval;
            sensor = (m_sensorMap["MEMORY"] = new MemSensor(interval));
            m_sensorList.append(sensor);
        }

        SensorParams *sp = new SensorParams(meter);
        sp->addParam("FORMAT",
                     m_theme.locale()->translate(lineParser.getString("FORMAT")));

        sensor->addMeter(sp);
        sensor->setMaxValue(sp);
    }

    if (sens == "DISK") {
        sensor = m_sensorMap["DISK"];

        if (sensor == 0) {
            int interval = lineParser.getInt("INTERVAL");
            interval = (interval == 0) ? 5000 : interval;
            sensor = (m_sensorMap["DISK"] = new DiskSensor(interval));
            m_sensorList.append(sensor);
        }
        // meter->setMax( ((DiskSensor*)sensor)->getTotalSpace(mntPt)/1024 );
        SensorParams *sp = new SensorParams(meter);
        QString mntPt = lineParser.getString("MOUNTPOINT");
        if (mntPt.isEmpty()) {
            mntPt = "/";
        }
        // remove any trailing '/' from mount points in the .theme config, our
        // mntMap doesn't like trailing '/'s for matching in DiskSensor
        if (mntPt.length() > 1 && mntPt.endsWith("/")) {
            mntPt.remove(mntPt.length() - 1, 1);
        }
        sp->addParam("MOUNTPOINT", mntPt);
        sp->addParam("FORMAT",
                     m_theme.locale()->translate(lineParser.getString("FORMAT")));
        sensor->addMeter(sp);
        sensor->setMaxValue(sp);
    }

    if (sens == "NETWORK") {
        int interval = lineParser.getInt("INTERVAL");
        interval = (interval == 0) ? 2000 : interval;
        QString device = lineParser.getString("DEVICE");
        sensor = m_sensorMap["NETWORK"+device];

        if (sensor == 0) {
            sensor = (m_sensorMap["NETWORK"+device] =
                          new NetworkSensor(device, interval));
            m_sensorList.append(sensor);
        }

        SensorParams *sp = new SensorParams(meter);
        sp->addParam("FORMAT",
                     m_theme.locale()->translate(lineParser.getString("FORMAT")));

        sp->addParam("DECIMALS", lineParser.getString("DECIMALS"));
        sensor->addMeter(sp);
    }

    if (sens == "UPTIME") {
        sensor = m_sensorMap["UPTIME"];
        if (sensor == 0) {
            int interval = lineParser.getInt("INTERVAL");
            interval = (interval == 0) ? 60000 : interval;
            sensor = (m_sensorMap["UPTIME"] = new UptimeSensor(interval));
            m_sensorList.append(sensor);
        }

        SensorParams *sp = new SensorParams(meter);
        sp->addParam("FORMAT",
                     m_theme.locale()->translate(lineParser.getString("FORMAT")));

        sensor->addMeter(sp);
    }

    if (sens == "SENSOR") {
        sensor = m_sensorMap["SENSOR"];

        if (sensor == 0) {
            int interval = lineParser.getInt("INTERVAL");
            interval = (interval == 0) ? 30000 : interval;
            sensor = (m_sensorMap["SENSOR"] = new SensorSensor(interval, m_tempUnit));
            m_sensorList.append(sensor);
        }

        SensorParams *sp = new SensorParams(meter);
        sp->addParam("FORMAT",
                     m_theme.locale()->translate(lineParser.getString("FORMAT")));
        sp->addParam("TYPE", lineParser.getString("TYPE"));
        sensor->addMeter(sp);
    }

    if (sens == "TEXTFILE") {
        QString path = lineParser.getString("PATH");
        bool rdf = lineParser.getBoolean("RDF");
        sensor = m_sensorMap["TEXTFILE"+path];

        if (sensor == 0) {
            int interval = lineParser.getInt("INTERVAL");
            interval = (interval == 0) ? 60000 : interval;
            QString encoding = lineParser.getString("ENCODING");

            sensor = (m_sensorMap["TEXTFILE"+path] =
                          new TextFileSensor(path, rdf, interval, encoding));
            m_sensorList.append(sensor);
        }

        SensorParams *sp = new SensorParams(meter);
        sp->addParam("LINE", QString::number(lineParser.getInt("LINE")));
    }

    if (sens == "TIME") {
        sensor = m_sensorMap["DATE"];
        if (sensor == 0) {
            int interval = lineParser.getInt("INTERVAL");
            interval = (interval == 0) ? 60000 : interval;
            sensor = (m_sensorMap["DATE"] = new DateSensor(interval));
            m_sensorList.append(sensor);
        }
        SensorParams *sp = new SensorParams(meter);

        sp->addParam("FORMAT",
                     m_theme.locale()->translate(lineParser.getString("FORMAT")));
        sp->addParam("CALWIDTH", lineParser.getString("CALWIDTH"));
        sp->addParam("CALHEIGHT", lineParser.getString("CALHEIGHT"));

        sensor->addMeter(sp);
    }

#ifdef HAVE_XMMS

    if (sens == "XMMS") {
        sensor = sensorMap["XMMS"];
        if (sensor == 0) {
            int interval = lineParser.getInt("INTERVAL");
            interval = (interval == 0) ? 1000 : interval;
            QString encoding = lineParser.getString("ENCODING");

            sensor = (sensorMap["XMMS"] = new XMMSSensor(interval, encoding));
            sensorList->append(sensor);
        }
        SensorParams *sp = new SensorParams(meter);
        sp->addParam("FORMAT",
                     m_theme.locale()->translate(lineParser.getString("FORMAT")));
        sensor->addMeter(sp);
        sensor->setMaxValue(sp);
    }
#endif // HAVE_XMMS


    /*
    Not used in KDE4

      if( sens == "NOATUN" )
      {
        sensor = sensorMap["NOATUN"];
        if (sensor == 0)
        {
          int interval = lineParser.getInt("INTERVAL");
          interval = (interval == 0)?1000:interval;
          sensor = ( sensorMap["NOATUN"] = new NoatunSensor( interval, client ) );
          sensorList->append( sensor );
        }
        SensorParams *sp = new SensorParams(meter);
        sp->addParam("FORMAT",
                     m_theme.locale()->translate(lineParser.getString("FORMAT").ascii()));
        sensor->addMeter(sp);
        sensor->setMaxValue(sp);
      }
    */

    if (sens == "PROGRAM") {
        QString progName = lineParser.getString("PROGRAM");
        sensor = m_sensorMap["PROGRAM"+progName];

        if (sensor == 0) {
            int interval = lineParser.getInt("INTERVAL");

            interval = (interval == 0) ? 3600000 : interval;
            QString encoding = lineParser.getString("ENCODING");

            sensor = (m_sensorMap["PROGRAM"+progName] =
                          new ProgramSensor(progName, interval, encoding));
            m_sensorList.append(sensor);
        }

        SensorParams *sp = new SensorParams(meter);
        sp->addParam("LINE", QString::number(lineParser.getInt("LINE")));
        sp->addParam("THEMAPATH", m_theme.path());
        sensor->addMeter(sp);
    }

    if (sens == "RSS") {
        QString source = lineParser.getString("SOURCE");
        QString format =
            m_theme.locale()->translate(lineParser.getString("FORMAT"));

        sensor = m_sensorMap["RSS"+source];
        if (sensor == 0) {
            int interval = lineParser.getInt("INTERVAL");
            interval = (interval == 0) ? 60000 : interval;
            QString encoding = lineParser.getString("ENCODING");

            sensor = (m_sensorMap["RSS"+source] =
                          new RssSensor(source, interval, format, encoding));
            m_sensorList.append(sensor);
        }
        SensorParams *sp = new SensorParams(meter);
        sp->addParam("SOURCE", lineParser.getString("SOURCE"));
        sensor->addMeter(sp);
    }

    if (sensor != 0) {
        sensor->update();
        update();

        sensor->start();
    }
}

void Karamba::updateSensors()
{
    foreach(Sensor *sensor, m_sensorList)
    sensor->update();
}

void Karamba::slotToggleLocked()
{
    if (m_globalView)
        setFlag(QGraphicsItem::ItemIsMovable,
                m_toggleLocked->isChecked());
}

void Karamba::closeWidget()
{
    if (m_python)
        m_python->widgetClosed(this);

    if (m_interface)
        m_interface->callWidgetClosed(this);

    writeConfigData();

    emit widgetClosed(this);
}

KConfig* Karamba::getConfig() const
{
    return m_config;
}

void Karamba::writeConfigData()
{
    KConfigGroup cg(m_config, "internal");
    cg.writeEntry("lockedPosition", m_toggleLocked-> isChecked());
    cg.writeEntry("desktop", m_desktop);

    cg.changeGroup("theme");

    // Widget Position
    if (!m_globalView) {
        cg.writeEntry("widgetPosX", m_view->x());
        cg.writeEntry("widgetPosY", m_view->y());
    } else {
        cg.writeEntry("widgetPosX", x());
        cg.writeEntry("widgetPosY", y());
    }

    // Widget Size
    cg.writeEntry("widgetWidth", boundingRect().width());
    cg.writeEntry("widgetHeight", boundingRect().height());

    // write changes to DiskSensor
    m_config->sync();
}

void Karamba::reloadConfig()
{
    writeConfigData();

    Karamba *k = 0;

    if (m_globalView)
        k = new Karamba(m_theme.getUrlPath(), -1, false, QPoint(), true);
    else
        k = new Karamba(m_theme.getUrlPath(), -1, false, QPoint(), true);

    if (k != 0) {
        emit widgetStarted(k, true, true);
    }

    closeWidget();
}

void Karamba::setOnTop(bool stayOnTop)
{
    if (stayOnTop) {
        KWindowSystem::setState(m_view->winId(), NET::KeepAbove);
    } else {
        KWindowSystem::setState(m_view->winId(), NET::KeepBelow);
    }

    m_onTop = stayOnTop;
}

void Karamba::preparePopupMenu()
{
    m_popupMenu = new KMenu();

    m_popupMenu->addAction(KIcon("view-refresh"), i18n("Update"), this,
                           SLOT(updateSensors()), Qt::Key_F5);

    m_toggleLocked = new KToggleAction(KIcon("move"), i18n("Toggle &Locked Position"), this);
    m_toggleLocked->setShortcut(KShortcut(Qt::CTRL + Qt::Key_L));
    m_toggleLocked->setCheckedState(KGuiItem("Toggle &Locked Position",
                                    KIcon("system-lock-screen")));
    connect(m_toggleLocked, SIGNAL(triggered()), this, SLOT(slotToggleLocked()));
    m_popupMenu->addAction(m_toggleLocked);

    m_popupMenu->addSeparator();

    m_themeConfMenu = new KMenu();
    m_themeConfMenu->setTitle(i18n("Configure &Theme"));
    QAction *newAC = m_popupMenu->addMenu(m_themeConfMenu);
    newAC->setVisible(false);

    m_toDesktopMenu = new KMenu();
    m_toDesktopMenu->setTitle(i18n("To Des&ktop"));
    m_popupMenu->addMenu(m_toDesktopMenu);

    QAction *allDesktop = m_toDesktopMenu->addAction((i18n("&All Desktops")));
    connect(allDesktop, SIGNAL(triggered()), m_signalMapperDesktop, SLOT(map()));
    allDesktop->setCheckable(true);
    m_signalMapperDesktop->setMapping(allDesktop, 0);

    for (int desktop = 1; desktop <= m_KWinModule->numberOfDesktops(); desktop++) {
        QString name = i18n("Desktop");
        name += QString(" &%1").arg(desktop);
        QAction* action = m_toDesktopMenu->addAction(name);
        action->setCheckable(true);
        connect(action, SIGNAL(triggered()), m_signalMapperDesktop, SLOT(map()));
        m_signalMapperDesktop->setMapping(action, desktop);
    }

    m_popupMenu->addAction(KIcon("reload3"), i18n("&Reload Theme"), this,
                           SLOT(reloadConfig()), Qt::CTRL + Qt::Key_R);
    m_popupMenu->addAction(KIcon("window-close"), i18n("&Close This Theme"), this,
                           SLOT(closeWidget()), Qt::CTRL + Qt::Key_C);
}

void Karamba::slotDesktopChanged(int desktop)
{
    QList<QAction*> actions = m_toDesktopMenu->actions();

    for (int i = 0; i < actions.count(); i++) {
        if (i == desktop)
            actions[i]->setChecked(true);
        else
            actions[i]->setChecked(false);
    }

    if (desktop)
        m_info->setDesktop(desktop);
    else
        m_info->setDesktop(NETWinInfo::OnAllDesktops);
}

void Karamba::currentWallpaperChanged(int desktop)
{
    if (m_python) {
        m_python->wallpaperChanged(this, desktop);
    }

    if (m_interface) {
        m_interface->callWallpaperChanged(this, desktop);
    }
}

void Karamba::addMenuConfigOption(const QString &key, const QString &name)
{
    m_themeConfMenu->menuAction()->setVisible(true);

    KAction *newAction = new KToggleAction(name, this);

    newAction->setObjectName(key);
    connect(newAction, SIGNAL(triggered()), m_signalMapperConfig, SLOT(map()));
    m_signalMapperConfig->setMapping(newAction, newAction);
    m_themeConfMenu->addAction(newAction);

    newAction->setChecked(m_config->group("config menu").readEntry(key, false));
}

void Karamba::slotToggleConfigOption(QObject* sender)
{
    KToggleAction *action = (KToggleAction*)sender;

    m_config->group("config menu").writeEntry(action->objectName(), action->isChecked());

    if (m_python)
        m_python->menuOptionChanged(this, action->objectName(), action->isChecked());

    if (m_interface)
        m_interface->callMenuOptionChanged(this, action->objectName(), action->isChecked());
}

bool Karamba::setMenuConfigOption(const QString &key, bool value)
{
    QList<QAction*> actions = m_themeConfMenu->actions();
    QAction *action;
    foreach(action, actions) {
        if (action->objectName() == key) {
            action->setChecked(value);
            return true;
        }
    }

    return false;
}

bool Karamba::readMenuConfigOption(const QString &key) const
{
    QList<QAction*> actions = m_themeConfMenu->actions();
    QAction *action;
    foreach(action, actions) {
        if (action->objectName() == key) {
            return action->isChecked();
        }
    }

    return false;
}

KMenu *Karamba::addPopupMenu()
{
    KMenu *tmp = new KMenu();

    connect(tmp, SIGNAL(triggered(QAction*)), this,
            SLOT(passMenuItemClicked(QAction*)));

    m_menuList.append(tmp);
    return tmp;
}

QAction* Karamba::addMenuItem(KMenu *menu, const QString &text, const QString &icon)
{
    QAction *action = menu->addAction(KIcon(icon), text);
    return action;
}

void Karamba::popupMenu(KMenu *menu, const QPoint &pos) const
{
//  QPoint diff = mapToGlobal(QGraphicsItemGroup::pos().toPoint()).toPoint();
    menu->popup(m_view->pos() + pos + boundingRect().toRect().topLeft());
}

void Karamba::deletePopupMenu(KMenu *menu)
{
    int index = m_menuList.indexOf(menu);
    m_menuList.takeAt(index);

    delete menu;
}

void Karamba::deleteMenuItem(QAction *action)
{
    foreach(KMenu* menu, m_menuList) {
        QList<QAction*> actions = menu->actions();
        if (actions.contains(action)) {
            menu->removeAction(action);
            delete action;
        }
    }
}

bool Karamba::popupMenuExisting(const KMenu *menu) const
{
    return m_menuList.contains(const_cast<KMenu*>(menu));
}

void Karamba::scaleImageLabel(Meter *meter, int width,
                              int height) const
{
    if (ImageLabel *image = dynamic_cast<ImageLabel*>(meter)) {
        image->smoothScale(width, height);
    }
}

void Karamba::moveMeter(Meter *meter, int x, int y) const
{
    meter->setSize(x, y,
                   meter->getWidth(),
                   meter->getHeight());
}


void Karamba::passMenuItemClicked(QAction* action)
{
    if (m_python)
        m_python->menuItemClicked(this, (KMenu*)action->parentWidget(), (long)action);

    if (m_interface)
        m_interface->callMenuItemClicked(this, (KMenu*)action->parentWidget(), action);

}

void Karamba::popupGlobalMenu() const
{
    m_popupMenu->popup(QCursor::pos());
}

bool Karamba::hasMeter(const Meter* meter) const
{
    QList<QGraphicsItem*> items = QGraphicsItemGroup::children();
    return items.contains(const_cast<Meter*>(meter));
}

QRectF Karamba::boundingRect() const
{
    return size;
}

void Karamba::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                    QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
    //painter->drawText(0,0,"test");
    /*
    if(m_showMenu || m_scaleStep > 0)
    {
      setZValue(1);
      painter->setOpacity(0.075*m_scaleStep);
      painter->setBrush(Qt::gray);
      painter->setPen(Qt::gray);

      QRect frame(150,150,150,150);
      frame.moveCenter(boundingRect().center().toPoint());
      painter->drawRoundRect(frame);
    }
    */
}

void Karamba::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    /*
      QGraphicsItem *item;
      QList<QGraphicsItem*> items = m_view->items
                                    (mapToScene(boundingRect()).toPolygon());
      foreach(item, items)
      {
        ((Meter*)item)->setOpacity(1-0.05*m_scaleStep);
      }

      if(m_showMenu)
        m_scaleStep++;
      else
        m_scaleStep--;

      update();

      if((m_scaleStep == -1) || (m_scaleStep == 10))
        killTimer(event->timerId());
    */
}

void Karamba::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if (!m_wantRightButton) {
        m_popupMenu->exec(event->screenPos());
    }
}

void Karamba::activeTaskChanged(Task::TaskPtr t)
{
    if (m_python)
        m_python->activeTaskChanged(this, t.data());

    if (m_interface)
        m_interface->callActiveTaskChanged(this, t.data());
}

void Karamba::taskAdded(Task::TaskPtr t)
{
    if (m_python)
        m_python->taskAdded(this, t.data());

    if (m_interface)
        m_interface->callTaskAdded(this, t.data());
}

void Karamba::taskRemoved(Task::TaskPtr t)
{
    if (m_python)
        m_python->taskRemoved(this, t.data());

    if (m_interface)
        m_interface->callTaskRemoved(this, t.data());
}

void Karamba::startupAdded(Startup::StartupPtr t)
{
    if (m_python)
        m_python->startupAdded(this, t.data());

    if (m_interface)
        m_interface->callStartupAdded(this, t.data());
}

void Karamba::startupRemoved(Startup::StartupPtr t)
{
    if (m_python)
        m_python->startupRemoved(this, t.data());

    if (m_interface)
        m_interface->callStartupRemoved(this, t.data());
}

void Karamba::processExited(K3Process* proc)
{
    if (m_python)
        m_python->commandFinished(this, (int)proc->pid());

    if (m_interface)
        m_interface->callCommandFinished(this, (int)proc->pid());
}

void Karamba::receivedStdout(K3Process *proc, char *buffer, int)
{
    if (m_python)
        m_python->commandOutput(this, (int)proc->pid(), buffer);

    if (m_interface)
        m_interface->callCommandOutput(this, (int)proc->pid(), buffer);
}

void Karamba::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    //event->accept(QTextDrag::canDecode(event));
    if (event->mimeData()->hasText())
        event->acceptProposedAction();
}

void Karamba::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasText()) {
        if (m_python)
            m_python->itemDropped(this, event->mimeData()->text(),
                                  (int)event->pos().x(), (int)event->pos().y());

        if (m_interface)
            m_interface->callItemDropped(this, event->mimeData()->text(),
                                         (int)event->pos().x(), (int)event->pos().y());

    }
}

void Karamba::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_mouseClickPos = event->pos().toPoint();

    if (!m_toggleLocked->isChecked())
        return;

    int button = passEvent(event);

    if (m_python)
        m_python->widgetClicked(this, (int)event->pos().x(),
                                (int)event->pos().y(), button);

    if (m_interface)
        m_interface->callWidgetClicked(this, (int)event->pos().x(),
                                       (int)event->pos().y(), button);
}

void Karamba::setWantRightButton(bool enable)
{
    m_wantRightButton = enable;
}

void Karamba::currentDesktopChanged(int i)
{
    if (m_python)
        m_python->desktopChanged(this, i);

    if (m_interface)
        m_interface->callDesktopChanged(this, i);
}

int Karamba::passEvent(QEvent *e)
{
    QList<QGraphicsItem*> items;
    QPointF pos;
    int button = 0;

    if (QGraphicsSceneMouseEvent *event = dynamic_cast<QGraphicsSceneMouseEvent*>(e)) {
        items = m_scene->items(mapToScene(event->pos()));
        pos = event->pos();

        if (event->button() == Qt::LeftButton)
            button = 1;
        else if (event->button() == Qt::MidButton)
            button = 2;
        else if (event->button() == Qt::RightButton)
            button = 3;
    } else if (QGraphicsSceneWheelEvent *event = dynamic_cast<QGraphicsSceneWheelEvent*>(e)) {
        items = m_scene->items(mapToScene(event->pos()));
        pos = event->pos();

        if (event->delta() > 0)
            button = 4;
        else
            button = 5;
    }

    if (button == 3 && !m_wantRightButton) {
        return 0;
    }

    foreach(QGraphicsItem *item, items) {
        bool pass = false;
        bool allowClick = false;

        if (item == this)
            continue;

        else if (ImageLabel* image = dynamic_cast<ImageLabel*>(item)) {
            allowClick = image->clickable();
            pass = image->mouseEvent(e);
        } else if (TextLabel* text = dynamic_cast<TextLabel*>(item)) {
            allowClick = text->clickable();
            pass = text->mouseEvent(e);
        } else if (ClickArea* area = dynamic_cast<ClickArea*>(item)) {
            pass = area->mouseEvent(e);
        } else if (RichTextLabel* rich = dynamic_cast<RichTextLabel*>(item)) {
            allowClick = false;
            pass = rich->mouseEvent(e);
            if (pass) {
                QString anchor = rich->getAnchor(pos);
                if (m_python)
                    m_python->meterClicked(this, anchor.toAscii().data(), button);
                if (m_interface)
                    m_interface->callMeterClicked(this, anchor, button);
            }
        }

        if (Input *input = dynamic_cast<Input*>(item)) {
            input->setFocus();
            input->mouseEvent(e);
        }

        if (pass && allowClick) {
            if (m_python)
                m_python->meterClicked(this, (Meter*)item, button);

            if (m_interface)
                m_interface->callMeterClicked(this, (Meter*)item, button);
        }
    }

    return button;
}

void Karamba::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    int button = passEvent(event);

    if (m_python)
        m_python->widgetClicked(this, (int)event->pos().x(), (int)event->pos().y(), button);

    if (m_interface)
        m_interface->callWidgetClicked(this, (int)event->pos().x(), (int)event->pos().y(), button);
}

void Karamba::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    if (m_python)
        m_python->widgetMouseMoved(this, (int)event->pos().x(), (int)event->pos().y(), 0/*button*/);

    if (m_interface)
        m_interface->callWidgetMouseMoved(this, (int)event->pos().x(), (int)event->pos().y(), 0/*button*/);

}

QGraphicsScene* Karamba::getScene() const
{
    return m_scene;
}

QGraphicsView* Karamba::getView() const
{
    return m_view;
}

int Karamba::getNumberOfDesktops() const
{
    return m_KWinModule->numberOfDesktops();
}

void Karamba::changeInterval(u_int newInterval)
{
    m_interval = newInterval;
}

double Karamba::getUpdateTime() const
{
    return m_updateTime;
}

void Karamba::setUpdateTime(double newTime)
{
    m_updateTime = newTime;
}

void Karamba::keyPressed(const QString& s, const Meter* meter)
{
    if (s.isEmpty()) {
      return;
    }

    if (m_python) {
        m_python->keyPressed(this, meter, s);
    }

    if (m_interface) {
        m_interface->callKeyPressed(this, (Meter*)meter, s);
    }
}

void Karamba::setFixedSize(u_int w, u_int h)
{
    size.setWidth(w);
    size.setHeight(h);
}

const ThemeFile& Karamba::theme() const
{
    return m_theme;
}

void Karamba::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!m_globalView) {
        if (!m_toggleLocked->isChecked())
            m_view->move(event->screenPos() - m_mouseClickPos);
    } else {
        event->ignore();
        QGraphicsItemGroup::mouseMoveEvent(event);
    }
}

void Karamba::keyPressEvent(QKeyEvent *event)
{
    QGraphicsItem *item = m_scene->focusItem();

    if (Input* input = dynamic_cast<Input*>(item)) {
        if (input && input->hasFocus()) {
            input->keyPress(event);
        }
    }

    keyPressed(event->text(), (Meter*)item);
}

void Karamba::slotFileChanged(const QString &file)
{
    QString pythonFile = m_theme.path() + '/' + m_theme.scriptModule();
    if (file == m_theme.file() || file == pythonFile)
        reloadConfig();
}

void Karamba::setMenuExtension(KMenu *menu)
{
    m_popupMenu->addSeparator();

    m_globalMenu = menu;
    m_popupMenu->addMenu(menu);
}

void Karamba::removeMenuExtension()
{
    m_popupMenu->removeAction(m_globalMenu->menuAction());
    m_globalMenu = 0;       // m_globalMenu is deleted in removeAction
}

bool Karamba::hasMenuExtension() const
{
    return m_globalMenu != 0;
}

int Karamba::instance()
{
    return m_instance;
}

void Karamba::setInstance(int instance)
{
    m_instance = instance;
}

void Karamba::moveToPos(QPoint pos)
{
    if (!m_globalView)
        m_view->move(pos);
    else
        setPos(pos);
}

void Karamba::resizeTo(int width, int height)
{
    if (!m_globalView)
        m_view->setFixedSize(width, height);

    setFixedSize(width, height);
}

QPoint Karamba::getPosition() const
{
    if (!m_globalView)
        return m_view->pos();
    else
        return pos().toPoint();
}

void Karamba::setIncomingData(const QString &data)
{
    m_storedData = data;
}

void Karamba::notifyTheme(const QString &sender, const QString &data)
{
    m_interface->callThemeNotify(this, sender, data);
}

bool Karamba::sendDataToTheme(const QString &prettyThemeName, const QString &data) const
{
    Karamba *k = karambaApp->getKaramba(prettyThemeName);
    if (k == 0) {
        return false;
    }

    k->notifyTheme(m_prettyName, data);

    return true;
}

QString Karamba::retrieveReceivedData() const
{
    return m_storedData;
}

bool Karamba::sendData(const QString &prettyThemeName, const QString &data) const
{
    Karamba *k = karambaApp->getKaramba(prettyThemeName);
    if (k == 0) {
        return false;
    }

    k->setIncomingData(data);

    return true;
}

bool Karamba::isSubTheme() const
{
    return m_subTheme;
}
