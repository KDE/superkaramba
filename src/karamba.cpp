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

#include "config-superkaramba.h"

#include "karambamanager.h"

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

#ifdef PLASMASENSOR_ENABLED
    #include "sensors/plasmaengine.h"
#endif

#include "python/karamba.h"

#include "karambaapp.h"
#include "systemtray.h"
#include "karambainterface.h"
#include "mainwidget.h"
#include "lineparser.h"
#include "themelocale.h"
#include "superkarambasettings.h"

extern "C" {
    KDE_EXPORT QGraphicsItemGroup* startKaramba(const KUrl &theme, QGraphicsView *view)
    {
        return new Karamba(theme, view);
    }
}

class Karamba::Private
{
    public:
        ThemeFile theme;
        QGraphicsScene *scene;
        QGraphicsView *view;

        KWindowSystem *KWinModule;

        bool useKross;
#ifdef PYTHON_INCLUDE_PATH
        KarambaPython *python;
#endif
        KarambaInterface *interface;

        bool foundKaramba;
        bool onTop;
        bool managed;

        NETWinInfo *info;

        QRect size;

        u_int desktop;

        u_int interval;

        char tempUnit;

        TextField *defaultTextField;

        int scaleStep;
        bool showMenu;

        QList<Sensor*> sensorList;
        QMap<QString, Sensor*> sensorMap;

        KMenu *popupMenu;
        KToggleAction *toggleLocked;
        KAction* reloadTheme;
        KMenu *themeConfMenu;
        KMenu *toDesktopMenu;
        KMenu *globalMenu;

        QTimer stepTimer;

        QSignalMapper *signalMapperConfig;
        QSignalMapper *signalMapperDesktop;

        KConfig *config;

        int instance;

        QList<KMenu*> menuList;

        QString prettyName;

        QString storedData;

        double updateTime;

        bool wantRightButton;

        QPoint mouseClickPos;

        bool globalView;

        bool subTheme;

        QPoint themeCenter;

        QGraphicsItemAnimation *animation;
        QTimeLine *timer;

        //OrgKdeKdesktopBackgroundInterface* backgroundInterface;

        bool useFancyEffects;
        bool useAntialiasing;

        bool errorInInit;

        K3Process *currProcess;
        Systemtray *systray;

        Private(QGraphicsView *view, int instance, bool subTheme) :
            scene(view ? view->scene() : 0),
            view(view),
            KWinModule(0),
            useKross(true),
#ifdef PYTHON_INCLUDE_PATH
            python(0),
#endif
            interface(0),
            foundKaramba(false),
            onTop(false),
            managed(false),
            info(0),
            desktop(0),
            interval(0),
            tempUnit('C'),
            defaultTextField(0),
            scaleStep(-1),
            showMenu(false),
            popupMenu(0),
            toggleLocked(0),
            reloadTheme(0),
            themeConfMenu(0),
            toDesktopMenu(0),
            globalMenu(0),
            signalMapperConfig(0),
            signalMapperDesktop(0),
            config(0),
            instance(instance),
            storedData(""),
            wantRightButton(false),
            globalView(view ? true : false),
            subTheme(subTheme),
            animation(0),
            timer(0),
            //backgroundInterface(0),
            useFancyEffects(true),
            useAntialiasing(true),
            errorInInit(false),
            currProcess(0),
            systray(0)
        {
        }

        ~Private()
        {
            delete config;

            delete info;

#ifdef PYTHON_INCLUDE_PATH
            if (python) {
                delete python;
            }
#endif

            if (interface) {
                delete interface;
            }

            qDeleteAll(sensorList);
            sensorList.clear();

            delete toDesktopMenu;
            delete themeConfMenu,
            delete toggleLocked;
            delete reloadTheme;
            delete popupMenu;

            delete animation;
            delete timer;

            //delete backgroundInterface;

            if (!globalView) {
                delete view;
                delete scene;
            }

            delete currProcess;
            delete systray;
        }
};

Karamba::Karamba(const KUrl &themeFile, QGraphicsView *view, int instance, bool subTheme, const QPoint &startPos, bool reload, bool startkaramba)
        : QObject(),
        QGraphicsItemGroup(0, view ? view->scene() : 0),
        d(new Private(view, instance, subTheme))
{
#ifdef PYTHON_INCLUDE_PATH
    if (!d->globalView) {
        KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
        if (args->isSet("usefallback")) {
            d->useKross = false;
        }
    }
#endif

    QString environment = getenv("SK_FANCY");
    if (!environment.compare("false", Qt::CaseInsensitive)) {
        d->useFancyEffects = false;
    }

    environment = getenv("SK_AA");
    if (!environment.compare("false", Qt::CaseInsensitive)) {
        d->useAntialiasing = false;
    }

    if (!d->globalView) {
        d->scene = new QGraphicsScene;
        d->scene->addItem(this);
        d->view = new MainWidget(d->scene);

        if (d->useAntialiasing) {
            d->view->setRenderHints(QPainter::Antialiasing |
                               QPainter::SmoothPixmapTransform);
        }

        d->scene->setItemIndexMethod(QGraphicsScene::NoIndex);

        d->view->show();
    }

    hide();

    if (!d->globalView) {
        d->view->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    }

    if (!d->theme.set(themeFile)) {
        kDebug() << "Could not load theme file" ;
        d->errorInInit = true;
        if (startkaramba)
            QTimer::singleShot(0, this, SLOT(startKaramba()));
        return;
    }

    KDirWatch *dirWatch = KDirWatch::self();
    connect(dirWatch, SIGNAL(dirty(const QString &)),
            SLOT(slotFileChanged(const QString &)));

    if (!dirWatch->contains(d->theme.file()))
        dirWatch->addFile(d->theme.file());

    if (!d->theme.isZipTheme() && d->theme.scriptModuleExists()) {
        QString scriptFile = d->theme.path() + '/' + d->theme.scriptModule();
        if (!dirWatch->contains(scriptFile))
            dirWatch->addFile(scriptFile);
    }

    if (d->prettyName.isEmpty())
        d->prettyName = QString("%1 - %2").arg(d->theme.name()).arg(d->instance);

    setObjectName("karamba - " + d->prettyName);

    if (!d->globalView) {
        d->info = new NETWinInfo(QX11Info::display(), d->view->winId(),
                            QX11Info::appRootWindow(), NET::WMState);
    }

    d->defaultTextField = new TextField();

    d->KWinModule = KWindowSystem::self();
    connect(d->KWinModule, SIGNAL(currentDesktopChanged(int)), this,
            SLOT(currentDesktopChanged(int)));
//TODO port it

#if 0
    d->backgroundInterface = new org::kde::kdesktop::Background("org.kde.kdesktop", "/Background", QDBusConnection::sessionBus());
    connect(d->backgroundInterface, SIGNAL(backgroundChanged(int)), this,
            SLOT(currentWallpaperChanged(int)));
#endif
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

    d->signalMapperConfig = new QSignalMapper(this);
    connect(d->signalMapperConfig, SIGNAL(mapped(QObject*)), this,
            SLOT(slotToggleConfigOption(QObject*)));

    d->signalMapperDesktop = new QSignalMapper(this);
    connect(d->signalMapperDesktop, SIGNAL(mapped(int)), this,
            SLOT(slotDesktopChanged(int)));

    preparePopupMenu();

    parseConfig();

    QString instanceString;
    if (d->instance > 1)
        instanceString = QString("-%1").arg(d->instance);

    QString cfg = QDir::home().absolutePath() + "/.superkaramba/"
                  + d->theme.id() + instanceString + ".rc";
    kDebug() << cfg ;

    QFile themeConfigFile(cfg);
    // Tests if config file Exists
    if (!QFileInfo(themeConfigFile).exists()) {
        // Create config file
        themeConfigFile.open(QIODevice::ReadWrite);
        themeConfigFile.close();
    }

    d->config = new KConfig(cfg, KConfig::NoGlobals);

    // Karamba specific Config Entries
    KConfigGroup cg(d->config, "internal");
    bool locked = d->toggleLocked->isChecked();
    locked = cg.readEntry("lockedPosition", locked);
    d->toggleLocked->setChecked(locked);

    int desktop = 0;
    desktop = cg.readEntry("desktop", desktop);
    if (desktop > d->KWinModule->numberOfDesktops()) {
        desktop = 0;
    }

    slotDesktopChanged(desktop);

    cg = KConfigGroup(d->config, "theme");
    if (cg.hasKey("widgetPosX") && cg.hasKey("widgetPosY")) {
        int xpos = cg.readEntry("widgetPosX", 0);
        int ypos = cg.readEntry("widgetPosY", 0);

        if (xpos < 0)
            xpos = 0;
        if (ypos < 0)
            ypos = 0;

        if (startPos.isNull()) {
            moveToPos(QPoint(xpos, ypos));
        }
    }

    if (!startPos.isNull()) {
        moveToPos(startPos - d->themeCenter);

        d->toggleLocked->setChecked(false);
    }

    if (!d->globalView && !reload && d->useFancyEffects) {
        d->timer = new QTimeLine(1000);
        d->timer->setFrameRange(0, 1000);

        d->animation = new QGraphicsItemAnimation;
        d->animation->setItem(this);
        d->animation->setTimeLine(d->timer);

        // Use 201 here because 200.0/200.0 < 1 => theme is still scaled
        for (int i = 0; i < 201; i++) {
            d->animation->setScaleAt(i / 200.0, 1 / 200.0 * i, 1 / 200.0 * i);
            QPointF animPos(
                boundingRect().width()/2*(1-d->animation->verticalScaleAt(i / 200.0)),
                boundingRect().height()/2*(1-d->animation->horizontalScaleAt(i / 200.0))
            );
                d->animation->setPosAt(i / 200.0, animPos);
        }

        d->timer->start();
    }

    if (startkaramba)
        QTimer::singleShot(0, this, SLOT(startKaramba()));

    if (!(d->onTop || d->managed) && !d->globalView) {
        KWindowSystem::setState(d->view->winId(), NET::KeepBelow);
        KWindowSystem::lowerWindow(d->view->winId());
    }
}

Karamba::~Karamba()
{
    if (d->toggleLocked) // may NULL e.g. if "Could not load theme file"
        writeConfigData();

    d->scene->removeItem(this);

    delete d;
}

void Karamba::startKaramba()
{
    if (d->errorInInit) {
        deleteLater();
        return;
    }

    KarambaManager::self()->addKaramba(this);

    if (d->theme.scriptModuleExists()) {
        kDebug() << "Loading script module: " << d->theme.scriptModule() ;

        d->stepTimer.setSingleShot(true);

        if (!d->useKross) {
#ifdef PYTHON_INCLUDE_PATH
            d->python = new KarambaPython(d->theme, false);
            d->python->initWidget(this);
#endif
        } else {
            d->interface = new KarambaInterface(this);
            bool interpreterStarted = d->interface->initInterpreter();

            if (!interpreterStarted) {
                delete d->interface;
                d->interface = 0;
            } else {
                d->interface->startInterpreter();
                d->interface->callInitWidget(this);
            }
        }

        update();

        connect(&d->stepTimer, SIGNAL(timeout()), SLOT(step()));
        d->stepTimer.start(d->interval);
    }

    show();
}

QString Karamba::prettyName() const
{
    return d->prettyName;
}

void Karamba::setPrettyName(const QString &prettyThemeName)
{
    d->prettyName = prettyThemeName;
}

void Karamba::step()
{
    d->stepTimer.start(d->interval);

#ifdef PYTHON_INCLUDE_PATH
    if (d->python)
        d->python->widgetUpdated(this);
#endif

    if (d->interface)
        d->interface->callWidgetUpdated(this);

    update();
}

void Karamba::redrawWidget()
{
    // Force the event loop to process the update() calls
    // QCoreApplication::processEvents();
}

bool Karamba::parseConfig()
{
    //qDebug("karamba::parseConfig");
    bool passive = true;

    if (d->theme.open()) {
        QStack<QPoint> offsetStack;
        LineParser lineParser;
        int x = 0;
        int y = 0;
        int w = 0;
        int h = 0;

        offsetStack.push(QPoint(0, 0));

        while (d->theme.nextLine(lineParser)) {
            x = lineParser.getInt("X") + offsetStack.top().x();
            y = lineParser.getInt("Y") + offsetStack.top().y();
            w = lineParser.getInt("W");
            h = lineParser.getInt("H");

            bool hidden = lineParser.getBoolean("HIDDEN");

            if (lineParser.meter() == "KARAMBA" && !d->foundKaramba) {
                d->toggleLocked->setChecked(lineParser.getBoolean("LOCKED"));

                x = (x < 0) ? 0 : x;
                y = (y < 0) ? 0 : y;

                if (w == 0 ||  h == 0) {
                    w = 300;
                    h = 300;
                }

                // Store the center of the theme,
                // so it can be centered when a user
                // drags it from the theme dialog.
                d->themeCenter = QPoint(w/2, h/2);

                setFixedSize(w, h);
                if (!d->globalView) {
                    d->view->setFixedSize(w + 5, h + 5);
                }

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

                moveToPos(QPoint(x, y));

                if (lineParser.getBoolean("ONTOP")) {
                    d->onTop = true;
                    if (!d->globalView) {
                        KWindowSystem::setState(d->view->winId(), NET::StaysOnTop);
                   }
                }

                if (lineParser.getBoolean("MANAGED")) {
                    d->managed = true;
                    if (!d->globalView) {
                        Qt::WindowFlags flags = Qt::Dialog;
                        flags & Qt::WA_DeleteOnClose;
                        d->view->setWindowFlags(flags);
                        d->view->show();
                    }
                } else {
                    if (!d->globalView) {
                        d->info->setState(NETWinInfo::SkipTaskbar
                                     | NETWinInfo::SkipPager, NETWinInfo::SkipTaskbar
                                     | NETWinInfo::SkipPager);
                    }
                    if (d->onTop && !d->globalView) {
                        KWindowSystem::setState(d->view->winId(), NET::KeepAbove);
                    }
                }

                if (lineParser.getBoolean("ONALLDESKTOPS")) {
                    d->desktop = 200; // ugly
                }

                bool dfound = false;
                //int desktop = lineParser.getInt("DESKTOP", line, dfound);
                if (dfound && !d->globalView) {
                    d->info->setDesktop(dfound);
                }

                if (lineParser.getBoolean("TOPBAR")) {
                    if (!d->globalView) {
                        KWindowSystem::setStrut(d->view->winId(), 0, 0, h, 0);
                        d->view->move(x, 0);
                    } else {
                        moveToPos(QPoint(x, 0));
                    }

                    d->toggleLocked->setChecked(true);
                    d->toggleLocked->setEnabled(false);
                }

                if (lineParser.getBoolean("BOTTOMBAR")) {
                    int dh = QApplication::desktop()->height();
                    if (d->globalView) {
                        moveToPos(QPoint(x, dh - h));
                    } else {
                        KWindowSystem::setStrut(d->view->winId(), 0, 0, 0, h);
                        d->view->move(x, dh - h);
                    }
                    d->toggleLocked->setChecked(true);
                    d->toggleLocked->setEnabled(false);
                }

                if (lineParser.getBoolean("RIGHTBAR")) {
                    int dw = QApplication::desktop()->width();
                    if (d->globalView) {
                        moveToPos(QPoint(dw - w, y));
                    } else {
                        KWindowSystem::setStrut(d->view->winId(), 0, w, 0, 0);
                        d->view->move(dw - w, y);
                    }
                    d->toggleLocked->setChecked(true);
                    d->toggleLocked->setEnabled(false);
                }

                if (lineParser.getBoolean("LEFTBAR")) {
                    if (d->globalView) {
                        moveToPos(QPoint(0, y));
                    } else {
                        KWindowSystem::setStrut(d->view->winId(), w, 0, 0, 0);
                        d->view->move(0, y);
                    }
                    d->toggleLocked->setChecked( true );
                    d->toggleLocked->setEnabled(false);
                }

                if (d->globalView)
                    setFlag(QGraphicsItem::ItemIsMovable,
                            d->toggleLocked->isChecked());


                /* Masking gone
                QString path = lineParser.getString("MASK");

                QFileInfo info(path);
                QString absPath;
                QBitmap bmMask;
                QByteArray ba;
                if( info.isRelative() )
                {
                  absPath = d->theme.path();
                  absPath.append(path);
                  ba = d->theme.readThemeFile(path);
                }
                else
                {
                  absPath = path;
                  ba = d->theme.readThemeFile(info.fileName());
                }
                if(d->theme.isZipTheme())
                {
                  bmMask.loadFromData(ba);
                }
                else
                {
                  bmMask.load(absPath);
                }
                d->view->setMask(bmMask);
                */

                d->interval = lineParser.getInt("INTERVAL");
                d->interval = (d->interval == 0) ? 1000 : d->interval;

                QString temp = lineParser.getString("TEMPUNIT", "C").toUpper();
                d->tempUnit = temp.at(0).toAscii();
                d->foundKaramba = true;
            }

            if (lineParser.meter() == "THEME") {
                QString path = lineParser.getString("PATH");
                QFileInfo info(path);
                if (info.isRelative())
                    path = d->theme.path() + '/' + path;

                new Karamba(path/*, d->view, d->scene*/);
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
                if (!d->globalView) {
                    d->view->setInteractive(true);
                }

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
                if ( hidden )
                    tmp->hide();

                setSensor(lineParser, (Meter*)tmp);
            }

            if (lineParser.meter() == "DEFAULTFONT") {
                delete d->defaultTextField;
                d->defaultTextField = new TextField();

                d->defaultTextField->setColor(lineParser.getColor("COLOR",
                                             QColor("black")));
                d->defaultTextField->setBGColor(lineParser.getColor("BGCOLOR",
                                               QColor("white")));
                d->defaultTextField->setFont(lineParser.getString("FONT", "Helvetica"));
                d->defaultTextField->setFontSize(lineParser.getInt("FONTSIZE", 12));
                d->defaultTextField->setAlignment(lineParser.getString("ALIGN",
                                                 "LEFT"));
                d->defaultTextField->setFixedPitch(lineParser.getBoolean("FIXEDPITCH",
                                                  false));
                d->defaultTextField->setShadow(lineParser.getInt("SHADOW", 0));
            }

            if (lineParser.meter() == "TEXT" ||
                    lineParser.meter() == "CLICKMAP" ||
                    lineParser.meter() == "RICHTEXT" ||
                    lineParser.meter() == "INPUT") {
                TextField defTxt;

                if (d->defaultTextField)
                    defTxt = *d->defaultTextField;

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
                        d->theme.locale()->translate(lineParser.getString("VALUE")));
                    tmp->setTextProps(tmpText);

                    QString name = lineParser.getString("NAME");
                    if (!name.isEmpty())
                        tmp->setObjectName(name);

                    if ( hidden )
                        tmp->hide();

                    setSensor(lineParser, (Meter*)tmp);
                }

                if (lineParser.meter() == "CLICKMAP") {
                    if (!d->globalView) {
                        d->view->setInteractive(true);
                    }

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
                        d->theme.locale()->translate(lineParser.getString("VALUE")), dUl);
                    tmp->setTextProps(tmpText);
//          tmp->setWidth(w);
//          tmp->setHeight(h);

                    QString name = lineParser.getString("NAME");
                    if (!name.isEmpty())
                        tmp->setObjectName(name);

                    if ( hidden )
                        tmp->hide();

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
                        d->theme.locale()->translate(lineParser.getString("VALUE")));

                    //meterList->append(tmp);
                    passive = false;
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

                if ( hidden )
                    tmp->hide();

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

                tmp->setPlotDirection(lineParser.getString("PLOT"));
                tmp->setScrollDirection(lineParser.getString("SCROLL"));
                tmp->setColor(lineParser.getColor("COLOR"));

                QString fillString = lineParser.getString("FILLCOLOR");
                if ( ! fillString.isEmpty() )
                  tmp->setFillColor(lineParser.getColor("FILLCOLOR"));

                if ( hidden )
                    tmp->hide();

                setSensor(lineParser, (Graph*)tmp);
                //meterList->append(tmp);
            }
        }

        if (passive && !d->managed && !d->onTop && !d->globalView) {
            KWindowSystem::setType(d->view->winId(), NET::Dock);
            KWindowSystem::setState(d->view->winId(), NET::KeepBelow);
        }

        d->theme.close();
    }
    //qDebug("parseConfig ok: %d", foundKaramba);
    if (!d->foundKaramba) {
        //  interval = initKaramba( "", 0, 0, 0, 0 );
        //   this->close(true);
        //delete this;
        return false;
    } else {
        return true;
    }
}

void Karamba::makeActive()
{
    if (!d->globalView) {
        KWindowSystem::setType(d->view->winId(), NET::Normal);
        KWindowSystem::setState(d->view->winId(), NET::Modal);
    }
}

void Karamba::makePassive()
{
    if (d->managed) {
        return;
    }

    QList<QGraphicsItem*> items = QGraphicsItemGroup::children();
    foreach (QGraphicsItem* item, items) {
        if (dynamic_cast<Input*>(item) != 0) {
            return;
        }
    }

    if (!d->globalView) {
        KWindowSystem::setType(d->view->winId(), NET::Dock);
        KWindowSystem::setState(d->view->winId(), NET::KeepBelow);
    }
}

TextField* Karamba::getDefaultTextProps()
{
    return d->defaultTextField;
}

QString Karamba::findSensorFromMap(const Sensor* sensor) const
{
    //qDebug("karamba::findSensorFromMap");
    QMap<QString, Sensor*>::ConstIterator it;
    QMap<QString, Sensor*>::ConstIterator end(d->sensorMap.constEnd());
    for (it = d->sensorMap.constBegin(); it != end; ++it) {
        if (it.value() == sensor)
            return it.key();
    }
    return "";
}

Sensor *Karamba::findSensorFromList(const Meter* meter) const
{
    foreach(Sensor *sensor, d->sensorList) {
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
            d->sensorMap.remove(s);
            d->sensorList.removeAll(sensor);
            delete sensor;
        }
    }
}

QString Karamba::getSensor(const Meter* meter) const
{
    Sensor* sensor = findSensorFromList(meter);

    if (sensor)
        return findSensorFromMap(sensor);

    return QString();
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
        sensor = d->sensorMap["CPU"+cpuNbr];

        if (sensor == 0) {
            int interval = lineParser.getInt("INTERVAL");
            interval = (interval == 0) ? 1000 : interval;
            sensor = (d->sensorMap["CPU" + cpuNbr] = new CPUSensor(cpuNbr, interval));
            d->sensorList.append(sensor);
        }

        SensorParams *sp = new SensorParams(meter);
        sp->addParam("FORMAT",
                     d->theme.locale()->translate(lineParser.getString("FORMAT")));

        sp->addParam("DECIMALS", lineParser.getString("DECIMALS"));

        sensor->addMeter(sp);
        sensor->setMaxValue(sp);
    }

    if (sens == "MEMORY") {
        sensor = d->sensorMap["MEMORY"];
        if (sensor == 0) {
            int interval = lineParser.getInt("INTERVAL");
            interval = (interval == 0) ? 3000 : interval;
            sensor = (d->sensorMap["MEMORY"] = new MemSensor(interval));
            d->sensorList.append(sensor);
        }

        SensorParams *sp = new SensorParams(meter);
        sp->addParam("FORMAT",
                     d->theme.locale()->translate(lineParser.getString("FORMAT")));

        sensor->addMeter(sp);
        sensor->setMaxValue(sp);
    }

    if (sens == "DISK") {
        sensor = d->sensorMap["DISK"];

        if (sensor == 0) {
            int interval = lineParser.getInt("INTERVAL");
            interval = (interval == 0) ? 5000 : interval;
            sensor = (d->sensorMap["DISK"] = new DiskSensor(interval));
            d->sensorList.append(sensor);
        }
        // meter->setMax( ((DiskSensor*)sensor)->getTotalSpace(mntPt)/1024 );
        SensorParams *sp = new SensorParams(meter);
        QString mntPt = lineParser.getString("MOUNTPOINT");
        if (mntPt.isEmpty()) {
            mntPt = "/";
        }
        // remove any trailing '/' from mount points in the .theme config, our
        // mntMap doesn't like trailing '/'s for matching in DiskSensor
        if (mntPt.length() > 1 && mntPt.endsWith('/')) {
            mntPt.remove(mntPt.length() - 1, 1);
        }
        sp->addParam("MOUNTPOINT", mntPt);
        sp->addParam("FORMAT",
                     d->theme.locale()->translate(lineParser.getString("FORMAT")));
        sensor->addMeter(sp);
        sensor->setMaxValue(sp);
    }

    if (sens == "NETWORK") {
        int interval = lineParser.getInt("INTERVAL");
        interval = (interval == 0) ? 2000 : interval;
        QString device = lineParser.getString("DEVICE");
        sensor = d->sensorMap["NETWORK"+device];

        if (sensor == 0) {
            sensor = (d->sensorMap["NETWORK"+device] =
                          new NetworkSensor(device, interval));
            d->sensorList.append(sensor);
        }

        SensorParams *sp = new SensorParams(meter);
        sp->addParam("FORMAT",
                     d->theme.locale()->translate(lineParser.getString("FORMAT")));

        sp->addParam("DECIMALS", lineParser.getString("DECIMALS"));
        sensor->addMeter(sp);
    }

    if (sens == "UPTIME") {
        sensor = d->sensorMap["UPTIME"];
        if (sensor == 0) {
            int interval = lineParser.getInt("INTERVAL");
            interval = (interval == 0) ? 1000 : interval;
            sensor = (d->sensorMap["UPTIME"] = new UptimeSensor(interval));
            d->sensorList.append(sensor);
        }

        SensorParams *sp = new SensorParams(meter);
        sp->addParam("FORMAT",
                     d->theme.locale()->translate(lineParser.getString("FORMAT")));

        sensor->addMeter(sp);
    }

    if (sens == "SENSOR") {
        sensor = d->sensorMap["SENSOR"];

        if (sensor == 0) {
            int interval = lineParser.getInt("INTERVAL");
            interval = (interval == 0) ? 5000 : interval;
            sensor = (d->sensorMap["SENSOR"] = new SensorSensor(interval, d->tempUnit));
            d->sensorList.append(sensor);
        }

        SensorParams *sp = new SensorParams(meter);
        sp->addParam("FORMAT",
                     d->theme.locale()->translate(lineParser.getString("FORMAT")));
        sp->addParam("TYPE", lineParser.getString("TYPE"));
        sensor->addMeter(sp);
    }

    if (sens == "TEXTFILE") {
        QString path = lineParser.getString("PATH");
        bool rdf = lineParser.getBoolean("RDF");
        sensor = d->sensorMap["TEXTFILE"+path];

        if (sensor == 0) {
            int interval = lineParser.getInt("INTERVAL");
            interval = (interval == 0) ? 60000 : interval;
            QString encoding = lineParser.getString("ENCODING");

            sensor = (d->sensorMap["TEXTFILE"+path] =
                          new TextFileSensor(path, rdf, interval, encoding));
            d->sensorList.append(sensor);
        }

        SensorParams *sp = new SensorParams(meter);
        sp->addParam("LINE", QString::number(lineParser.getInt("LINE")));
    }

    if (sens == "TIME") {
        sensor = d->sensorMap["DATE"];
        if (sensor == 0) {
            int interval = lineParser.getInt("INTERVAL");
            interval = (interval == 0) ? 1000 : interval;
            sensor = (d->sensorMap["DATE"] = new DateSensor(interval));
            d->sensorList.append(sensor);
        }
        SensorParams *sp = new SensorParams(meter);

        sp->addParam("FORMAT",
                     d->theme.locale()->translate(lineParser.getString("FORMAT")));
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
                     d->theme.locale()->translate(lineParser.getString("FORMAT")));
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
                     d->theme.locale()->translate(lineParser.getString("FORMAT").ascii()));
        sensor->addMeter(sp);
        sensor->setMaxValue(sp);
      }
    */

    if (sens == "PROGRAM") {
        QString progName = lineParser.getString("PROGRAMNAME");
        if (progName.isEmpty()) {
            progName = lineParser.getString("PROGRAM");
        }

        sensor = d->sensorMap["PROGRAM"+progName];

        if (sensor == 0) {
            int interval = lineParser.getInt("INTERVAL");

            interval = (interval == 0) ? 60000 : interval;
            QString encoding = lineParser.getString("ENCODING");

            QString prog = lineParser.getString("PROGRAM");
            sensor = (d->sensorMap["PROGRAM"+progName] =
                          new ProgramSensor(this, prog, interval, encoding));
            d->sensorList.append(sensor);
        }

        SensorParams *sp = new SensorParams(meter);
        sp->addParam("LINE", QString::number(lineParser.getInt("LINE")));
        sp->addParam("THEMAPATH", d->theme.path());
        sp->addParam("FORMAT", d->theme.locale()->translate(lineParser.getString("FORMAT")));
        sensor->addMeter(sp);
    }

    if (sens == "RSS") {
        QString source = lineParser.getString("SOURCE");
        QString format =
            d->theme.locale()->translate(lineParser.getString("FORMAT"));

        sensor = d->sensorMap["RSS"+source];
        if (sensor == 0) {
            int interval = lineParser.getInt("INTERVAL");
            interval = (interval == 0) ? 60000 : interval;
            QString encoding = lineParser.getString("ENCODING");

            sensor = (d->sensorMap["RSS"+source] =
                          new RssSensor(source, interval, format, encoding));
            d->sensorList.append(sensor);
        }
        SensorParams *sp = new SensorParams(meter);
        sp->addParam("SOURCE", lineParser.getString("SOURCE"));
        sensor->addMeter(sp);
    }

#ifdef PLASMASENSOR_ENABLED
    if (sens == "PLASMA") {
        QString engine = lineParser.getString("ENGINE");
        QString source = lineParser.getString("SOURCE");
        kDebug()<<"PlasmaEngineSensor engine="<<engine<<" source="<<source<<endl;
        sensor = d->sensorMap["PLASMA." + engine + '.' + source];
        if (sensor == 0) {
            PlasmaSensor* plasmasensor = new PlasmaSensor();
            plasmasensor->setEngine(engine);
            if( ! source.isEmpty() ) {
                QObject* connector = plasmasensor->connectSource(source, meter);
                if( PlasmaSensorConnector* c = dynamic_cast<PlasmaSensorConnector*>(connector) ) {
                    c->setFormat( lineParser.getString("FORMAT") );
                }
            }

            QString propertiesLine = lineParser.getString("PROPERTIES");
            const QStringList properties = propertiesLine.split(",");
            foreach (const QString &property, properties) {
                QStringList options = property.split(":");
                if (options.count() == 2) {
                    plasmasensor->setProperty(options[0].toLatin1(), options[1]);
                }
            }

            sensor = plasmasensor;
            d->sensorMap["PLASMA." + engine + '.' + source] = sensor;
            d->sensorList.append(sensor);
        }

        SensorParams *sp = new SensorParams(meter);
        sp->addParam("THEMAPATH", d->theme.path());
        sp->addParam("ENGINE", engine);
        sp->addParam("SOURCE", source);
        sensor->addMeter(sp);
    }
#endif

    if (sensor != 0) {
        sensor->update();
        update();

        sensor->start();
    }
}

void Karamba::updateSensors()
{
    foreach(Sensor *sensor, d->sensorList)
    sensor->update();
}

void Karamba::slotToggleLocked()
{
    if (d->globalView)
        setFlag(QGraphicsItem::ItemIsMovable,
                d->toggleLocked->isChecked());
}

void Karamba::closeWidget()
{
#ifdef PYTHON_INCLUDE_PATH
    if (d->python)
        d->python->widgetClosed(this);
#endif

    if (d->interface)
        d->interface->callWidgetClosed(this);

    KarambaManager::self()->removeKaramba(this);
}

KConfig* Karamba::getConfig() const
{
    return d->config;
}

void Karamba::writeConfigData()
{
    KConfigGroup cg(d->config, "internal");
    cg.writeEntry("lockedPosition", d->toggleLocked-> isChecked());
    cg.writeEntry("desktop", d->desktop);

    cg = KConfigGroup(d->config, "theme");

    // Widget Position
    if (!d->globalView) {
        cg.writeEntry("widgetPosX", d->view->x());
        cg.writeEntry("widgetPosY", d->view->y());
    } else {
        if (parentItem()) {
            cg.writeEntry("widgetPosX", parentItem()->x());
            cg.writeEntry("widgetPosY", parentItem()->y());
        } else {
            cg.writeEntry("widgetPosX", x());
            cg.writeEntry("widgetPosY", y());
        }
    }

    // Widget Size
    cg.writeEntry("widgetWidth", boundingRect().width());
    cg.writeEntry("widgetHeight", boundingRect().height());

    // write changes to DiskSensor
    d->config->sync();
}

void Karamba::reloadConfig()
{
    writeConfigData();

    Karamba *k = 0;

    if (d->globalView)
        k = new Karamba(d->theme.getUrlPath(), d->view, -1, false, QPoint(), true);
    else
        k = new Karamba(d->theme.getUrlPath(), 0, -1, false, QPoint(), true);

    closeWidget();
}

void Karamba::setOnTop(bool stayOnTop)
{
    if (stayOnTop) {
        if (!d->globalView) {
            KWindowSystem::setState(d->view->winId(), NET::KeepAbove);
        }
    } else {
        if (!d->globalView) {
            KWindowSystem::setState(d->view->winId(), NET::KeepBelow);
        }
    }

    d->onTop = stayOnTop;
}

void Karamba::preparePopupMenu()
{
    d->popupMenu = new KMenu();

    d->popupMenu->addAction(KIcon("view-refresh"), i18n("Update"), this,
                           SLOT(updateSensors()), Qt::Key_F5);

    d->toggleLocked = new KToggleAction(i18n("&Locked Position"), this);
    d->toggleLocked->setObjectName("lockedAction");
    d->toggleLocked->setShortcut(KShortcut(Qt::CTRL + Qt::Key_L));
    d->toggleLocked->setCheckedState(KGuiItem(i18n("&Locked Position")));
    connect(d->toggleLocked, SIGNAL(triggered()), this, SLOT(slotToggleLocked()));
    d->popupMenu->addAction(d->toggleLocked);

    d->popupMenu->addSeparator();

    d->themeConfMenu = new KMenu();
    d->themeConfMenu->setTitle(i18n("Configure &Theme"));
    QAction *newAC = d->popupMenu->addMenu(d->themeConfMenu);
    newAC->setObjectName("configureTheme");
    newAC->setParent(this);
    newAC->setVisible(false);

    if (d->globalView) {
        return;
    }

    d->toDesktopMenu = new KMenu();
    d->toDesktopMenu->setTitle(i18n("To Des&ktop"));
    d->popupMenu->addMenu(d->toDesktopMenu);

    QAction *allDesktop = d->toDesktopMenu->addAction((i18n("&All Desktops")));
    connect(allDesktop, SIGNAL(triggered()), d->signalMapperDesktop, SLOT(map()));
    allDesktop->setCheckable(true);
    d->signalMapperDesktop->setMapping(allDesktop, 0);

    for (int desktop = 1; desktop <= d->KWinModule->numberOfDesktops(); desktop++) {
        QString name = i18n("Desktop");
        name += QString(" &%1").arg(desktop);
        QAction* action = d->toDesktopMenu->addAction(name);
        action->setCheckable(true);
        connect(action, SIGNAL(triggered()), d->signalMapperDesktop, SLOT(map()));
        d->signalMapperDesktop->setMapping(action, desktop);
    }

    d->reloadTheme = new KAction(KIcon("view-refresh"), i18n("&Reload Theme"), this);
    d->reloadTheme->setObjectName("reloadAction");
    d->reloadTheme->setShortcut(KShortcut(Qt::CTRL + Qt::Key_R));
    connect(d->reloadTheme, SIGNAL(triggered()), this, SLOT(reloadConfig()));
    d->popupMenu->addAction(d->reloadTheme);

    d->popupMenu->addAction(KIcon("window-close"), i18n("&Close This Theme"), this,
                           SLOT(closeWidget()), Qt::CTRL + Qt::Key_C);
}

void Karamba::slotDesktopChanged(int desktop)
{
    if (d->globalView) {
        return;
    }

    QList<QAction*> actions = d->toDesktopMenu->actions();

    for (int i = 0; i < actions.count(); i++) {
        if (i == desktop)
            actions[i]->setChecked(true);
        else
            actions[i]->setChecked(false);
    }

    if (desktop) {
        d->info->setDesktop(desktop);
    } else {
        d->info->setDesktop(NETWinInfo::OnAllDesktops);
    }
}

void Karamba::currentWallpaperChanged(int desktop)
{
#ifdef PYTHON_INCLUDE_PATH
    if (d->python) {
        d->python->wallpaperChanged(this, desktop);
    }
#endif

    if (d->interface) {
        d->interface->callWallpaperChanged(this, desktop);
    }
}

void Karamba::addMenuConfigOption(const QString &key, const QString &name)
{
    d->themeConfMenu->menuAction()->setVisible(true);

    KAction *newAction = new KToggleAction(name, this);

    newAction->setObjectName(key);
    connect(newAction, SIGNAL(triggered()), d->signalMapperConfig, SLOT(map()));
    d->signalMapperConfig->setMapping(newAction, newAction);
    d->themeConfMenu->addAction(newAction);

    newAction->setChecked(d->config->group("config menu").readEntry(key, false));
}

void Karamba::slotToggleConfigOption(QObject* sender)
{
    KToggleAction *action = (KToggleAction*)sender;

    d->config->group("config menu").writeEntry(action->objectName(), action->isChecked());

#ifdef PYTHON_INCLUDE_PATH
    if (d->python)
        d->python->menuOptionChanged(this, action->objectName(), action->isChecked());
#endif

    if (d->interface)
        d->interface->callMenuOptionChanged(this, action->objectName(), action->isChecked());
}

bool Karamba::setMenuConfigOption(const QString &key, bool value)
{
    QList<QAction*> actions = d->themeConfMenu->actions();
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
    QList<QAction*> actions = d->themeConfMenu->actions();
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

    d->menuList.append(tmp);
    return tmp;
}

QAction* Karamba::addMenuItem(KMenu *menu, const QString &text, const QString &icon)
{
    QAction *action = menu->addAction(KIcon(icon), text);
    return action;
}

void Karamba::popupMenu(KMenu *menu, const QPoint &pos) const
{
    if (!d->globalView) {
        menu->popup(d->view->pos() + pos + boundingRect().toRect().topLeft());
    } else {
        menu->popup(mapToScene(pos).toPoint());
    }
}

void Karamba::deletePopupMenu(KMenu *menu)
{
    int index = d->menuList.indexOf(menu);
    d->menuList.takeAt(index);

    menu->deleteLater();
}

void Karamba::deleteMenuItem(QAction *action)
{
    foreach(KMenu* menu, d->menuList) {
        QList<QAction*> actions = menu->actions();
        if (actions.contains(action)) {
            menu->removeAction(action);
            delete action;
        }
    }
}

bool Karamba::popupMenuExisting(const KMenu *menu) const
{
    return d->menuList.contains(const_cast<KMenu*>(menu));
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
#ifdef PYTHON_INCLUDE_PATH
    if (d->python)
        d->python->menuItemClicked(this, (KMenu*)action->parentWidget(), (long)action);
#endif

    if (d->interface)
        d->interface->callMenuItemClicked(this, (KMenu*)action->parentWidget(), action);

}

void Karamba::popupGlobalMenu() const
{
    d->popupMenu->popup(QCursor::pos());
}

bool Karamba::hasMeter(const Meter* meter) const
{
    QList<QGraphicsItem*> items = QGraphicsItemGroup::children();
    return items.contains(const_cast<Meter*>(meter));
}

QRectF Karamba::boundingRect() const
{
    return d->size;
}

void Karamba::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                    QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
    /*
    if(d->showMenu || d->scaleStep > 0)
    {
      setZValue(1);
      painter->setOpacity(0.075*d->scaleStep);
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
      QList<QGraphicsItem*> items = d->view->items
                                    (mapToScene(boundingRect()).toPolygon());
      foreach(item, items)
      {
        ((Meter*)item)->setOpacity(1-0.05*d->scaleStep);
      }

      if(d->showMenu)
        d->scaleStep++;
      else
        d->scaleStep--;

      update();

      if((d->scaleStep == -1) || (d->scaleStep == 10))
        killTimer(event->timerId());
    */
}

void Karamba::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if (!d->wantRightButton) {
        d->popupMenu->exec(event->screenPos());
    }
}

void Karamba::activeTaskChanged(Task::TaskPtr t)
{
#ifdef PYTHON_INCLUDE_PATH
    if (d->python)
        d->python->activeTaskChanged(this, t.data());
#endif

    if (d->interface)
        d->interface->callActiveTaskChanged(this, t.data());
}

void Karamba::taskAdded(Task::TaskPtr t)
{
#ifdef PYTHON_INCLUDE_PATH
    if (d->python)
        d->python->taskAdded(this, t.data());
#endif

    if (d->interface)
        d->interface->callTaskAdded(this, t.data());
}

void Karamba::taskRemoved(Task::TaskPtr t)
{
#ifdef PYTHON_INCLUDE_PATH
    if (d->python)
        d->python->taskRemoved(this, t.data());
#endif

    if (d->interface)
        d->interface->callTaskRemoved(this, t.data());
}

void Karamba::startupAdded(Startup::StartupPtr t)
{
#ifdef PYTHON_INCLUDE_PATH
    if (d->python)
        d->python->startupAdded(this, t.data());
#endif

    if (d->interface)
        d->interface->callStartupAdded(this, t.data());
}

void Karamba::startupRemoved(Startup::StartupPtr t)
{
#ifdef PYTHON_INCLUDE_PATH
    if (d->python)
        d->python->startupRemoved(this, t.data());
#endif

    if (d->interface)
        d->interface->callStartupRemoved(this, t.data());
}

void Karamba::processExited(K3Process* proc)
{
#ifdef PYTHON_INCLUDE_PATH
    if (d->python)
        d->python->commandFinished(this, (int)proc->pid());
#endif

    if (d->interface)
        d->interface->callCommandFinished(this, (int)proc->pid());
}

void Karamba::receivedStdout(K3Process *proc, char *buffer, int)
{
#ifdef PYTHON_INCLUDE_PATH
    if (d->python)
        d->python->commandOutput(this, (int)proc->pid(), buffer);
#endif

    if (d->interface)
        d->interface->callCommandOutput(this, (int)proc->pid(), buffer);
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
        #ifdef PYTHON_INCLUDE_PATH
        if (d->python)
            d->python->itemDropped(this, event->mimeData()->text(),
                                  (int)event->pos().x(), (int)event->pos().y());
        #endif

        if (d->interface)
            d->interface->callItemDropped(this, event->mimeData()->text(),
                                         (int)event->pos().x(), (int)event->pos().y());

    }
}

void Karamba::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    d->mouseClickPos = event->pos().toPoint();

    if (!(d->onTop || d->managed) && !d->globalView) {
        KWindowSystem::lowerWindow(d->view->winId());
    }

    if (!d->toggleLocked->isChecked()) {
        return;
    }

    int button = passEvent(event);

#ifdef PYTHON_INCLUDE_PATH
    if (d->python) {
        d->python->widgetClicked(this, (int)event->pos().x(),
                                (int)event->pos().y(), button);
    }
#endif

    if (d->interface) {
        d->interface->callWidgetClicked(this, (int)event->pos().x(),
                                       (int)event->pos().y(), button);
    }
}

void Karamba::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (!d->toggleLocked->isChecked())
        return;

    QList<QGraphicsItem*>items = d->scene->items(mapToScene(event->pos()));
    foreach (QGraphicsItem *item, items) {
        if (Input *input = dynamic_cast<Input*>(item)) {
            input->mouseEventRelease(event);
        }
    }
}

void Karamba::setWantRightButton(bool enable)
{
    d->wantRightButton = enable;
}

void Karamba::currentDesktopChanged(int i)
{
#ifdef PYTHON_INCLUDE_PATH
    if (d->python)
        d->python->desktopChanged(this, i);
#endif

    if (d->interface)
        d->interface->callDesktopChanged(this, i);
}

int Karamba::passEvent(QEvent *e)
{
    QList<QGraphicsItem*> items;
    QPointF pos;
    int button = 0;

    if (QGraphicsSceneMouseEvent *event = dynamic_cast<QGraphicsSceneMouseEvent*>(e)) {
        items = d->scene->items(mapToScene(event->pos()));
        pos = event->pos();

        if (event->button() == Qt::LeftButton)
            button = 1;
        else if (event->button() == Qt::MidButton)
            button = 2;
        else if (event->button() == Qt::RightButton)
            button = 3;
    } else if (QGraphicsSceneWheelEvent *event = dynamic_cast<QGraphicsSceneWheelEvent*>(e)) {
        items = d->scene->items(mapToScene(event->pos()));
        pos = event->pos();

        if (event->delta() > 0)
            button = 4;
        else
            button = 5;
    }

    if (button == 3 && !d->wantRightButton) {
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
                #ifdef PYTHON_INCLUDE_PATH
                if (d->python)
                    d->python->meterClicked(this, anchor.toAscii().data(), button);
                #endif
                if (d->interface)
                    d->interface->callMeterClicked(this, anchor, button);
            }
        }

        if (Input *input = dynamic_cast<Input*>(item)) {
            input->setFocus();
            input->mouseEvent(e);
        }

        if (pass && allowClick) {
            #ifdef PYTHON_INCLUDE_PATH
            if (d->python)
                d->python->meterClicked(this, (Meter*)item, button);
            #endif

            if (d->interface)
                d->interface->callMeterClicked(this, (Meter*)item, button);
        }
    }

    return button;
}

void Karamba::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    int button = passEvent(event);

#ifdef PYTHON_INCLUDE_PATH
    if (d->python)
        d->python->widgetClicked(this, (int)event->pos().x(), (int)event->pos().y(), button);
#endif

    if (d->interface)
        d->interface->callWidgetClicked(this, (int)event->pos().x(), (int)event->pos().y(), button);
}

void Karamba::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QList<QGraphicsItem*>items = d->scene->items(mapToScene(event->pos()));
    foreach (QGraphicsItem *item, items) {
        if (Input *input = dynamic_cast<Input*>(item)) {
            input->mouseEventMove(event);
        }
    }

    foreach (QGraphicsItem *item, QGraphicsItemGroup::children()) {
        if (ImageLabel *image = dynamic_cast<ImageLabel*>(item)) {
            image->rolloverImage(event);
        }
    }

#ifdef PYTHON_INCLUDE_PATH
    if (d->python)
        d->python->widgetMouseMoved(this, (int)event->pos().x(), (int)event->pos().y(), 0/*button*/);
#endif

    if (d->interface)
        d->interface->callWidgetMouseMoved(this, (int)event->pos().x(), (int)event->pos().y(), 0/*button*/);

}

QGraphicsScene* Karamba::getScene() const
{
    return d->scene;
}

QGraphicsView* Karamba::getView() const
{
    return d->view;
}

int Karamba::getNumberOfDesktops() const
{
    return d->KWinModule->numberOfDesktops();
}

void Karamba::changeInterval(u_int newInterval)
{
    d->interval = newInterval;
}

double Karamba::getUpdateTime() const
{
    return d->updateTime;
}

void Karamba::setUpdateTime(double newTime)
{
    d->updateTime = newTime;
}

void Karamba::keyPressed(const QString& s, const Meter* meter)
{
    if (s.isEmpty()) {
      return;
    }

#ifdef PYTHON_INCLUDE_PATH
    if (d->python) {
        d->python->keyPressed(this, meter, s);
    }
#endif

    if (d->interface) {
        d->interface->callKeyPressed(this, (Meter*)meter, s);
    }
}

void Karamba::setFixedSize(u_int w, u_int h)
{
    d->size.setWidth(w);
    d->size.setHeight(h);
    emit sizeChanged();
}

const ThemeFile& Karamba::theme() const
{
    return d->theme;
}

void Karamba::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!d->globalView) {
        if (!d->toggleLocked->isChecked()) {
            d->view->move(event->screenPos() - d->mouseClickPos);
        }
    } else {
        if (!d->toggleLocked->isChecked()) {
            QPointF diff =  event->pos() - d->mouseClickPos;
            parentItem()->moveBy(diff.x(), diff.y());
        }
    }
}

void Karamba::keyPressEvent(QKeyEvent *event)
{
    QGraphicsItem *item = d->scene->focusItem();

    if (Input* input = dynamic_cast<Input*>(item)) {
        if (input && input->hasFocus()) {
            input->keyPress(event);
        }
    }

    keyPressed(event->text(), (Meter*)item);
}

QVariant Karamba::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if(change == QGraphicsItem::ItemPositionHasChanged) {
        emit positionChanged();
    }
    return QGraphicsItemGroup::itemChange(change, value);
}

void Karamba::slotFileChanged(const QString &file)
{
    QString pythonFile = d->theme.path() + '/' + d->theme.scriptModule();
    if (file == d->theme.file() || file == pythonFile) {
        reloadConfig();
    }
}

void Karamba::setMenuExtension(KMenu *menu)
{
    d->popupMenu->addSeparator();

    d->globalMenu = menu;
    d->popupMenu->addMenu(menu);
}

void Karamba::removeMenuExtension()
{
    d->popupMenu->removeAction(d->globalMenu->menuAction());
    d->globalMenu = 0;       // d->globalMenu is deleted in removeAction
}

bool Karamba::hasMenuExtension() const
{
    return d->globalMenu != 0;
}

int Karamba::instance()
{
    return d->instance;
}

void Karamba::setInstance(int instance)
{
    d->instance = instance;
}

void Karamba::moveToPos(QPoint pos)
{
    if (!d->globalView) {
        d->view->move(pos);
    } else {
        if (parentItem()) {
            setPos(0,0);
            parentItem()->setPos(pos);
        } else {
            setPos(pos);
        }
    }
    emit positionChanged();
}

void Karamba::resizeTo(int width, int height)
{
    if (!d->globalView) {
        d->view->setFixedSize(width, height);
    }

    setFixedSize(width, height);
}

QPoint Karamba::getPosition() const
{
    if (!d->globalView) {
        return d->view->pos();
    } else {
        if (parentItem()) {
            return parentItem()->pos().toPoint();
        } else {
            return pos().toPoint();
        }
    }
}

void Karamba::setIncomingData(const QString &data)
{
    d->storedData = data;
}

void Karamba::notifyTheme(const QString &sender, const QString &data)
{
    d->interface->callThemeNotify(this, sender, data);
}

bool Karamba::sendDataToTheme(const QString &prettyThemeName, const QString &data)
{
    Karamba *k = KarambaManager::self()->getKarambaByName(prettyThemeName);
    if (k == 0) {
        return false;
    }

    k->notifyTheme(d->prettyName, data);

    return true;
}

QString Karamba::retrieveReceivedData() const
{
    return d->storedData;
}

bool Karamba::sendData(const QString &prettyThemeName, const QString &data)
{
    Karamba *k = KarambaManager::self()->getKarambaByName(prettyThemeName);
    if (k == 0) {
        return false;
    }

    k->setIncomingData(data);

    return true;
}

bool Karamba::isSubTheme() const
{
    return d->subTheme;
}

void Karamba::setProcess(K3Process *process)
{
    delete d->currProcess;
    d->currProcess = process;
}

K3Process* Karamba::process() const
{
    return d->currProcess;
}

void Karamba::setSystemTray(Systemtray *systray)
{
    d->systray = systray;
}

Systemtray* Karamba::systemTray()
{
    return d->systray;
}

QObject* Karamba::getPlasmaSensor(const QString& engine, const QString& source)
{
#ifdef PLASMASENSOR_ENABLED
    Sensor* sensor = d->sensorMap["PLASMA." + engine + '.' + source];
    if (sensor == 0) {
        PlasmaSensor* plasmasensor = new PlasmaSensor();
        plasmasensor->setEngine(engine);
        sensor = plasmasensor;
        d->sensorMap["PLASMA." + engine + '.' + source] = sensor;
        d->sensorList.append(sensor);
    }
    return sensor;
#else
    Q_UNUSED(engine);
    Q_UNUSED(source);
    return 0;
#endif
}

QString Karamba::getMeterValue(const QString& name)
{
  if ( ! name.isNull() ) {
    Meter* meter = (Meter*) getMeter(name);
    if(meter != 0) {
      QString retVal = meter->getStringValue();
      if( retVal.isEmpty() ) {
        int intVal = meter->getValue();
        // if intVal > minimum, consider it a valid value
        if ( intVal >= meter->getMin() ) {
          retVal = QString::number(intVal);
        }
      }
      return retVal;
    }
  }
  return QString("");
}

void Karamba::replaceNamedValues(QString* source)
{
  //kDebug() << "%named replacement before:'" + *source + "'";
  QRegExp rx ("%named:(\\w+)", Qt::CaseInsensitive);
  int pos = 0;
  while ( pos >= 0 ) {
    pos = rx.indexIn(*source, pos);
    if ( pos >= 0 ) {
      QString namedReplacement = rx.cap(1);
      if( !namedReplacement.isEmpty() ) {
        QString replacementValue = getMeterValue(namedReplacement);
        if( replacementValue.isNull()) {
          replacementValue = QString("");
        }
        source->replace(QRegExp("%named:" + namedReplacement, Qt::CaseInsensitive), replacementValue);
      }
    }
  }
  //kDebug() << "%named replacement  after:'" + *source + "'";
}

Meter* Karamba::getMeter(const QString& name)
{
  //meterList...how I miss thee
  QList<QGraphicsItem*> items = QGraphicsItemGroup::children();
  foreach (QGraphicsItem *item, items) {
    if (Meter* meter = dynamic_cast<Meter*>(item)) {
      if(name == meter->objectName()){
        return (Meter*) meter;
      }
    }
  }
  return 0;
}

void Karamba::emitError(const QString& errormessage)
{
  emit error(errormessage);
}
