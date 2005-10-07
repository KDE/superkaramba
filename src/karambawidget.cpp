/*
 * Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>
 * Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>
 * Copyright (c) 2005 Ryan Nickell <p0z3r@earthlink.net>
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
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ****************************************************************************/

#include "karamba_python.h"
#include "dcopinterface_stub.h"
#include "richtextlabel.h"
#include "karambawidget.h"
#include "karambaapp.h"
#include "themesdlg.h"
#include "lineparser.h"
#include "themelocale.h"
#include "superkarambasettings.h"

#include <kdebug.h>
#include <kmessagebox.h>
#include <krun.h>
#include <klocale.h>
#include <kwin.h>
#include <kdeversion.h>
#include <kdirwatch.h>

#include <kparts/componentfactory.h>
#include <kparts/part.h>

#include <QDir>
#include <QStack>
#include <QDesktopWidget>
#include <QtAlgorithms>
#include <Q3TextDrag>
#include <QX11Info>
#include <krootpixmap.h>

// Menu IDs
#define EDITSCRIPT 1
#define THEMECONF  2

KarambaWidget::KarambaWidget(QString fn, bool reloading, int instance, QWidget *parent)
    :   QWidget(parent, Qt::FramelessWindowHint ),
        kpop(0), widgetMask(0),
        config(0), kWinModule(0), tempUnit('C'), m_instance(instance),
        themeConfMenu(0), toDesktopMenu(0), kglobal(0), clickPos(0, 0), accColl(0),
        menuAccColl(0), toggleLocked(0), pythonIface(0), defaultTextField(0),
        trayMenuSeperatorId(-1), trayMenuQuitId(-1), trayMenuToggleId(-1),
        trayMenuThemeId(-1)
{
    setAttribute(Qt::WA_GroupLeader);
    setAttribute(Qt::WA_DeleteOnClose);
    KURL url;
    
    /* Testing of passed filename as relative/absolute or pre filename */
    if(fn.find('/') == -1)
        url.setFileName(fn);
    else
        url = fn;
    
    /* if themefile is not set, close the theme */
    if(!m_theme.set(url))
    {
        setFixedSize(0, 0);
        QTimer::singleShot(100, this, SLOT(killWidget()));
        return;
    }
    kdDebug() << "Starting theme: " << m_theme.name() << endl;
    QString qName = "karamba - " + m_theme.name();
    setObjectName(qName.ascii());

    /*Add self to list of open themes to karambaApplication. reloading indicated whether this is
    first start or we are reloading from RMB */
    karambaApp->addKaramba(this, reloading);
    
    //So that, if user changes the theme file/python script, the theme is reloaded
    KDirWatch *dirWatch = new KDirWatch( this );
    connect(dirWatch, SIGNAL( dirty( const QString & ) ),
            SLOT( slotFileChanged( const QString & ) ) );
    dirWatch->addFile(m_theme.file());
    if(!m_theme.isZipTheme() && m_theme.pythonModuleExists())
    {
        QString pythonFile = m_theme.path() + "/" + m_theme.pythonModule() + ".py";
        dirWatch->addFile(pythonFile);
    }

    widgetUpdate = true;

    // Creates KConfig Object
    QString instanceString;
    if(m_instance > 1)
        instanceString = QString("-%1").arg(m_instance);
    QString cfg = QDir::home().absPath() + "/.superkaramba/"
                  + m_theme.id() + instanceString + ".rc";
    kdDebug() << cfg << endl;
    QFile themeConfigFile(cfg);
    // Tests if config file Exists
    if (!QFileInfo(themeConfigFile).exists())
    {
        // Create config file
        themeConfigFile.open(IO_ReadWrite);
        themeConfigFile.close();
    }

    config = new KConfig(cfg, false, false);
    config -> sync();
    config -> setGroup("internal");

    m_reloading = reloading;
    /* Loading of python module the initialization of its interface */
    if(m_theme.pythonModuleExists())
    {
        kdDebug() << "Loading python module: " << m_theme.pythonModule() << endl;
        QTimer::singleShot(0, this, SLOT(initPythonInterface()));
    }

    widgetMask = 0;
    info = new NETWinInfo( QX11Info::display(), winId(), QX11Info::appRootWindow(), NET::WMState );

    // could be replaced with TaskManager
    kWinModule = new KWinModule();
    desktop = 0;
    
    /* To let python module know that desktop is changed, and repaint of faked transparency */
    connect( kWinModule,SIGNAL(currentDesktopChanged(int)), this,
             SLOT(currentDesktopChanged(int)) );
    connect( kapp, SIGNAL(backgroundChanged(int)), this,
             SLOT(currentWallpaperChanged(int)));

    // Setup of the Task Manager Callbacks
    connect(&taskManager, SIGNAL(activeTaskChanged(Task*)), this,
            SLOT(activeTaskChanged(Task*)) );
    connect(&taskManager, SIGNAL(taskAdded(Task*)), this,
            SLOT(taskAdded(Task*)) );
    connect(&taskManager, SIGNAL(taskRemoved(Task*)), this,
            SLOT(taskRemoved(Task*)) );
    connect(&taskManager, SIGNAL(startupAdded(Startup*)), this,
            SLOT(startupAdded(Startup*)) );
    connect(&taskManager, SIGNAL(startupRemoved(Startup*)), this,
            SLOT(startupRemoved(Startup*)) );

    themeConfMenu = new KPopupMenu( this);
    themeConfMenu -> setCheckable(true);

    /* XXX - need to be able to delete all these DesktopChangeSlot objects */
    /* these are the submenu "To Dekstop"*/
    DesktopChangeSlot *dslot;

    int mid;

    toDesktopMenu = new KPopupMenu (this);
    toDesktopMenu -> setCheckable(true);
    mid = toDesktopMenu -> insertItem (i18n("&All Desktops"),
                                       dslot = new DesktopChangeSlot(this,0),
                                       SLOT(receive()));
    dslot->setMenuId(mid);

    toDesktopMenu -> insertSeparator();
    for (int ndesktop=1; ndesktop <= kWinModule->numberOfDesktops(); ndesktop++)
    {
        QString name = i18n("Desktop &");
        name += ('0' + ndesktop);

        mid = toDesktopMenu -> insertItem (name,
                                           dslot = new DesktopChangeSlot(this, ndesktop), SLOT(receive()));
        dslot->setMenuId(mid);
    }


    kpop = new KPopupMenu( this );
    kpop -> setCheckable(true);

    accColl = new KActionCollection( this );
    menuAccColl = new KActionCollection( this );

    kpop->insertItem( SmallIconSet("reload"),i18n("Update"), this,
                      SLOT(updateSensors()), Qt::Key_F5 );
    toggleLocked = new KToggleAction (  i18n("Toggle &Locked Position"),
                                        SmallIconSet("locked"),
                                        Qt::CTRL + Qt::Key_L, this,
                                        SLOT( slotToggleLocked() ),
                                        accColl, "Locked position" );
    accColl->insert(toggleLocked);
    toggleLocked -> setChecked(true);

    toggleLocked->plug(kpop);

    toggleFastTransforms = new KToggleAction(i18n("Use &Fast Image Scaling"),
                           Qt::CTRL + Qt::Key_F, this,
                           SLOT( slotToggleFastTransforms() ),
                           accColl, "Fast transformations");

    accColl->insert(toggleFastTransforms);
    toggleFastTransforms -> setChecked(true);

    toggleFastTransforms -> plug(kpop);

    kpop->insertSeparator();

    kpop->insertItem(i18n("Configure &Theme"), themeConfMenu, THEMECONF);
    kpop->setItemEnabled(THEMECONF, false);
    kpop->insertItem(i18n("To Des&ktop"), toDesktopMenu);

    kpop->insertItem( SmallIconSet("reload3"),i18n("&Reload Theme"),this,
                      SLOT(reloadConfig()), Qt::CTRL + Qt::Key_R );
    kpop->insertItem( SmallIconSet("fileclose"),i18n("&Close This Theme"), this,
                      SLOT(killWidget()), Qt::CTRL + Qt::Key_C );

    if(!SuperKarambaSettings::showSysTray())
        showMenuExtension();

    kpop->polish();

    numberOfConfMenuItems = 0;

    systray = 0;
    foundKaramba = false;
    onTop = false;
    managed = false;
    fixedPosition = false;
    defaultTextField = new TextField();

    client = kapp->dcopClient();
    if (!client->isAttached())
        client->attach();
    appId = client->registerAs(qApp->name());
    //to send it to desktop

    if( !(onTop || managed))
        this->lower();

    /* if *.theme file could not be parsed correctly, close the theme */
    if( !parseConfig() )
    {
        setFixedSize(0,0);
        QTimer::singleShot( 100, this, SLOT(killWidget()) );
        qWarning("Could not read config file.");
    }
    else
    {
        /* the application background handling for faked transparency */
       KRootPixmap *root = new KRootPixmap((QWidget*)this);
       root->start();
    }
    // Karamba specific Config Entries
    bool locked = toggleLocked->isChecked();
    locked = config->readBoolEntry("lockedPosition", locked);
    toggleLocked->setChecked(locked);
    slotToggleLocked();

    if (!config -> readBoolEntry("fastTransforms", true))
    {
        toggleFastTransforms -> setChecked(false);
        slotToggleFastTransforms();
    }

    desktop = config -> readNumEntry("desktop", desktop);
    if (desktop > kWinModule->numberOfDesktops())
    {
        desktop = 0;
    }

    if (desktop)
    {
        info->setDesktop( desktop );
    }
    else
        info->setDesktop( NETWinInfo::OnAllDesktops);

    // Read Themespecific Config Entries
    config -> setGroup("theme");
    if (config -> hasKey("widgetPosX") && config -> hasKey("widgetPosY"))
    {
        int xpos = config -> readNumEntry("widgetPosX");
        int ypos = config -> readNumEntry("widgetPosY");

        if (xpos < 0)
            xpos = 0;
        if (ypos < 0)
            ypos = 0;
        move(xpos, ypos);
    }

    haveUpdated = 0;
    this->setMouseTracking(true);


    setFocusPolicy(Qt::StrongFocus);
    repaint(true);
}

KarambaWidget::~KarambaWidget()
{
    //qDebug("KarambaWidget::~karamba");
    //Remove self from list of open themes
    karambaApp->deleteKaramba(this, m_reloading);

    widgetClosed();
    if(m_theme.isValid())
        writeConfigData();

    delete config;
    /* TODO Remove these list. There is no need to keep a list, when childrens can be easiliy queried
    directly by Qt functions */
    qDeleteAll(meterList);
    meterList.clear();
    qDeleteAll(sensorList);
    sensorList.clear();
    
    qDeleteAll(timeList);
    timeList.clear();

    delete toggleLocked;
    delete accColl;
    delete menuAccColl;
    delete themeConfMenu;
    delete kpop;
    delete widgetMask;
    delete kWinModule;
    delete defaultTextField;
    delete pythonIface;
}

bool KarambaWidget::parseConfig()
{
    //qDebug("KarambaWidget::parseConfig");
    bool passive = true;

    if(m_theme.open())
    {
        QStack<QPoint> offsetStack;
        LineParser lineParser;
        int x=0;
        int y=0;
        int w=0;
        int h=0;

        offsetStack.push(QPoint(0,0));

        while(m_theme.nextLine(lineParser))
        {
            x = lineParser.getInt("X") + offsetStack.top().x();
            y = lineParser.getInt("Y") + offsetStack.top().y();
            w = lineParser.getInt("W");
            h = lineParser.getInt("H");

            if(lineParser.meter() == "KARAMBA" && !foundKaramba )
            {
                toggleLocked->setChecked(lineParser.getBoolean("LOCKED"));
                slotToggleLocked();

                x = ( x < 0 ) ? 0:x;
                y = ( y < 0 ) ? 0:y;

                if( w == 0 ||  h == 0)
                {
                    w = 300;
                    h = 300;
                }
                setFixedSize(w,h);

                if(lineParser.getBoolean("RIGHT"))
                {
                    QDesktopWidget *d = QApplication::desktop();
                    x = d->width() - w;
                }
                else if(lineParser.getBoolean("LEFT"))
                {
                    x = 0;
                }

                if(lineParser.getBoolean("BOTTOM"))
                {
                    QDesktopWidget *d = QApplication::desktop();
                    y = d->height() - h;
                }
                else if(lineParser.getBoolean("TOP"))
                {
                    y = 0;
                }

                move(x,y);
                //pm = QPixmap(size());

                if(lineParser.getBoolean("ONTOP"))
                {
                    onTop = true;
                    KWin::setState( winId(), NET::StaysOnTop );
                }

                if(lineParser.getBoolean("MANAGED"))
                {
                    managed = true;
                    reparent(0, Qt::Dialog , pos());
                    setAttribute(Qt::WA_DeleteOnClose);
                }
                else
                {
                    info->setState( NETWinInfo::SkipTaskbar
                                    | NETWinInfo::SkipPager,NETWinInfo::SkipTaskbar
                                    | NETWinInfo::SkipPager );
                    if (onTop)
                    {
                        KWin::setState( winId(), NET::StaysOnTop );

                    }
                }

                if (lineParser.getBoolean("ONALLDESKTOPS"))
                {
                    desktop = 200; // ugly
                }


                bool dfound=false;
                //int desktop = lineParser.getInt("DESKTOP", line, dfound);
                if (dfound)
                {
                    info->setDesktop( dfound );
                }
                if(lineParser.getBoolean("TOPBAR"))
                {
                    move(x,0);
                    KWin::setStrut( winId(), 0, 0, h, 0 );
                    toggleLocked->setChecked( true );
                    slotToggleLocked();
                    toggleLocked->setEnabled(false);

                }

                if(lineParser.getBoolean("BOTTOMBAR"))
                {
                    int dh = QApplication::desktop()->height();
                    move( x, dh - h );
                    KWin::setStrut( winId(), 0, 0, 0, h );
                    toggleLocked->setChecked( true );
                    slotToggleLocked();
                    toggleLocked->setEnabled(false);
                }

                if(lineParser.getBoolean("RIGHTBAR"))
                {
                    int dw = QApplication::desktop()->width();
                    move( dw - w, y );
                    KWin::setStrut( winId(), 0, w, 0, 0 );
                    toggleLocked->setChecked( true );
                    slotToggleLocked();
                    toggleLocked->setEnabled(false);
                }

                if(lineParser.getBoolean("LEFTBAR"))
                {
                    move( 0, y );
                    KWin::setStrut( winId(), w, 0, 0, 0 );
                    toggleLocked->setChecked( true );
                    slotToggleLocked();
                    toggleLocked->setEnabled(false);
                }

                QString path = lineParser.getString("MASK");

                QFileInfo info(path);
                QString absPath;
                QBitmap bmMask;
                QByteArray ba;
                if( info.isRelative() )
                {
                    absPath.setAscii(m_theme.path().ascii());
                    absPath.append(path.ascii());
                    ba = m_theme.readThemeFile(path);
                }
                else
                {
                    absPath.setAscii(path.ascii());
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
                setMask(bmMask);

                m_interval = lineParser.getInt("INTERVAL");
                m_interval = (m_interval == 0) ? 1000 : m_interval;

                QString temp = lineParser.getString("TEMPUNIT", "C").upper();
                tempUnit = temp.ascii()[0];
                foundKaramba = true;
            }

            if(lineParser.meter() == "THEME")
            {
                QString path = lineParser.getString("PATH");
                QFileInfo info(path);
                if( info.isRelative())
                    path = m_theme.path() +"/" + path;
                (new KarambaWidget( path, false ))->show();
            }

            if(lineParser.meter() == "<GROUP>")
            {
                int offsetX = offsetStack.top().x();
                int offsetY = offsetStack.top().y();
                offsetStack.push( QPoint( offsetX + lineParser.getInt("X"),
                                          offsetY + lineParser.getInt("Y")));
            }

            if(lineParser.meter() == "</GROUP>")
            {
                offsetStack.pop();
            }
/*
            if(lineParser.meter() == "CLICKAREA")
            {
                if( !hasMouseTracking() )
                    setMouseTracking(true);
                ClickArea *tmp = new ClickArea(this, x, y, w, h );
                tmp->setOnClick(lineParser.getString("ONCLICK"));

                setSensor(lineParser, (Meter*)tmp);
                clickList.append( (ClickMap *)tmp );
                if( lineParser.getBoolean("PREVIEW"))
                    meterList.append( tmp );
            }*/

            // program sensor without a meter
            if(lineParser.meter() == "SENSOR=PROGRAM")
            {
                setSensor(lineParser, 0 );
            }

            if(lineParser.meter() == "IMAGE")
            {
                QString file = lineParser.getString("PATH");
                /*specific functionality like rollover image should be programmed by theme writer themselves. if we really want to provide this function, at least this is not the place for that we should make separate convenience function for that    */
//                 QString file_roll = lineParser.getString("PATHROLL");
//                 int xon = lineParser.getInt("XROLL");
//                 int yon = lineParser.getInt("YROLL");
                QString tiptext = lineParser.getString("TOOLTIP");
                QString name = lineParser.getString("NAME");
                bool bg = lineParser.getBoolean("BACKGROUND");
//                 xon = ( xon <= 0 ) ? x:xon;
//                 yon = ( yon <= 0 ) ? y:yon;

                ImageLabel *tmp = new ImageLabel(this, x, y, 0, 0);
                tmp->setValue(file);
//                 if(!file_roll.isEmpty())
//                     tmp->parseImages(file, file_roll, x,y, xon, yon);
                tmp->setBackground(bg);
                if (!name.isEmpty())
                    tmp->setObjectName(name.ascii());
                if (!tiptext.isEmpty())
                    tmp->setTooltip(tiptext);

               // connect(tmp, SIGNAL(pixmapLoaded()), this, SLOT(externalStep()));
                setSensor(lineParser, (Meter*) tmp );
                meterList.append (tmp );
                imageList.append (tmp );
            }

            if(lineParser.meter() == "DEFAULTFONT" )
            {
                delete defaultTextField;
                defaultTextField = new TextField( );

                defaultTextField->setColor(lineParser.getColor("COLOR",
                                           QColor("black")));
                defaultTextField->setBGColor(lineParser.getColor("BGCOLOR",
                                             QColor("white")));
                defaultTextField->setFont(lineParser.getString("FONT", "Helvetica"));
                defaultTextField->setFontSize(lineParser.getInt("FONTSIZE", 12));
                defaultTextField->setAlignment(lineParser.getString("ALIGN",
                                               "LEFT"));
                defaultTextField->setFixedPitch(lineParser.getBoolean("FIXEDPITCH",
                                                false));
                defaultTextField->setShadow(lineParser.getInt("SHADOW", 0));
            }

            if(lineParser.meter() == "RICHTEXT" ||
                    lineParser.meter() == "INPUT")
            {
                TextField defTxt;

                if(defaultTextField)
                    defTxt = *defaultTextField;

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

                // ////////////////////////////////////////////////////
                // Now handle the specifics
//                 if(lineParser.meter() == "TEXT")
//                 {
// 
//                     TextLabel *tmp = new TextLabel(this, x, y, w, h );
//                     tmp->setTextProps(tmpText);
//                     tmp->setValue(
//                         m_theme.locale()->translate(lineParser.getString("VALUE").ascii()));
// 
//                     QString name = lineParser.getString("NAME");
//                     if (!name.isEmpty())
//                         tmp->setName(name.ascii());
// 
//                     setSensor(lineParser, (Meter*)tmp);
//                     meterList.append ( tmp );
//                 }

//                 if(lineParser.meter() == "CLICKMAP")
//                 {
//                     if( !hasMouseTracking() )
//                         setMouseTracking(true);
//                     ClickMap *tmp = new ClickMap(this, x, y, w, h);
//                     tmp->setTextProps( tmpText );
// 
//                     setSensor(lineParser, (Meter*)tmp);
//                     // set all params
//                     clickList.append(tmp);
//                     meterList.append( tmp );
// 
//                 }

                if(lineParser.meter() == "RICHTEXT")
                {
                    RichTextLabel *tmp = new RichTextLabel(this, x, y, w, h);

                    bool dUl = lineParser.getBoolean("UNDERLINE");

                    tmp->setText(
                        m_theme.locale()->translate(lineParser.getString("VALUE").ascii()));
                    tmp->setTextProps( tmpText );
                    tmp->resize(w,h);
                    
                    QString name = lineParser.getString("NAME");
                    if (!name.isEmpty())
                        tmp->setName(name.ascii());

                    setSensor(lineParser, (Meter*)tmp);
                    meterList.append ( tmp );
                }

                if(lineParser.meter() == "INPUT")
                {
                    Input *tmp = new Input(this, x, y, w, h);

                    QString name = lineParser.getString("NAME");
                    if (!name.isEmpty())
                        tmp->setName(name.ascii());

                    tmp->setTextProps(tmpText);
                    tmp->setValue(
                        m_theme.locale()->translate(lineParser.getString("VALUE").ascii()));

                    meterList.append(tmp);
                    passive = false;
                }
            }

            if(lineParser.meter() == "BAR")
            {
                Bar *tmp = new Bar(this, x, y, w, h );
                tmp->setImage(lineParser.getString("PATH").ascii());
                tmp->setVertical(lineParser.getBoolean("VERTICAL"));
                tmp->setMax(lineParser.getInt("MAX", 100));
                tmp->setMin(lineParser.getInt("MIN", 0));
                tmp->setValue(lineParser.getInt("VALUE"));
                QString name = lineParser.getString("NAME");
                if (!name.isEmpty())
                    tmp->setName(name.ascii());
                setSensor(lineParser, (Meter*)tmp );
                meterList.append ( tmp );
            }

            if(lineParser.meter() == "GRAPH")
            {
                int points = lineParser.getInt("POINTS");

                Graph *tmp = new Graph(this, x, y, w, h, points);
                tmp->setMax(lineParser.getInt("MAX", 100));
                tmp->setMin(lineParser.getInt("MIN", 0));
                QString name = lineParser.getString("NAME");
                if (!name.isEmpty())
                    tmp->setName(name.ascii());

                tmp->setColor(lineParser.getColor("COLOR"));

                setSensor(lineParser, (Graph*)tmp);
                meterList.append ( tmp );
            }
        }

        if(passive && !managed)
        {
            // Matthew Kay: set window type to "dock"
            // (plays better with taskbar themes this way)
            KWin::setType(winId(), NET::Dock);

            //KDE 3.2 addition for the always on top issues
            KWin::setState(winId(), NET::KeepBelow);
        }

        m_theme.close();
    }
    //qDebug("parseConfig ok: %d", foundKaramba);
    if( !foundKaramba )
    {
        //  interval = initKaramba( "", 0, 0, 0, 0 );
        //   this->close(true);
        //delete this;
        return false;
    }
    else
    {
        return true;
    }
}
/*
void KarambaWidget::start()
{
    QTimer *m_sysTimer = new QTimer(this);

    connect(m_sysTimer, SIGNAL(timeout()), SLOT(step()));

    m_sysTimer->start(m_interval);

    //Start the widget running
    QTimer::singleShot( 0, this, SLOT(step()) );

    if( !(onTop || managed) )
        lowerTimer.start();
}*/

void KarambaWidget::makeActive()
{
    KWin::setType(winId(), NET::Normal);
    KWin::setState(winId(), NET::Modal);
}

 void KarambaWidget::makePassive()
 {
//     if(managed)
//         return;
// 
//     foreach (QObject *meter, meterList)
//     {
//         if((meter)->isA("Input"))
//             return;
//     }
// 
//     // Matthew Kay: set window type to "dock" (plays better with taskbar themes
//     // this way)
//     KWin::setType(winId(), NET::Dock);
//     //KDE 3.2 addition for the always on top issues
//     KWin::setState(winId(), NET::KeepBelow);
// 
 }

void KarambaWidget::popupNotify(int)
{
    //qDebug("KarambaWidget::popupNotify");
}

void KarambaWidget::reloadConfig()
{
    //qDebug("KarambaWidget::reloadConfig: %s", m_theme.file().ascii());
    writeConfigData();
    if(m_theme.exists())
    {
        (new KarambaWidget(m_theme.file(), true, m_instance))->show();
    }
    closeTheme(true);
}

void KarambaWidget::closeTheme(bool reloading)
{
    m_reloading = reloading;
    close();
}

void KarambaWidget::killWidget()
{
    closeTheme();
}

void KarambaWidget::initPythonInterface()
{
    pythonIface = new KarambaPython(m_theme, m_reloading);
}

void KarambaWidget::editConfig()
{
    //qDebug("KarambaWidget::editConfig");
    QFileInfo fileInfo( m_theme.file() );
    QString path;

    if( fileInfo.isRelative() )
    {
        path = m_theme.path() + "/" + m_theme.file();
    }
    else
    {
        path = m_theme.file();
    }

    KRun::runURL( KURL( path ), "text/plain" );
}

void KarambaWidget::editScript()
{
    //qDebug("KarambaWidget::editScript");
    QFileInfo fileInfo( m_theme.file() );
    QString path;

    if( fileInfo.isRelative() )
    {
        path = m_theme.path() + "/" + m_theme.name() + ".py";
    }
    else
    {
        path = QFileInfo(m_theme.file()).dirPath() + "/" + m_theme.name() + ".py";
    }
    KRun::runURL( KURL( path ), "text/plain" );
}

 QString KarambaWidget::findSensorFromMap(Sensor* sensor)
 {
//     //qDebug("KarambaWidget::findSensorFromMap");
//     QMap<QString,Sensor*>::ConstIterator it;
//     QMap<QString,Sensor*>::ConstIterator end( sensorMap.end() );
//     for ( it = sensorMap.begin(); it != end; ++it )
//     {
//         if (it.data() == sensor)
//             return it.key();
//     }
//     return "";
// }

// Sensor* KarambaWidget::findSensorFromList(Meter* meter)
// {
//     //qDebug("KarambaWidget::findSensorFromList");
//     return meter->getSensor();
 }
// 
 QString KarambaWidget::getSensor(Meter* meter)
 {
//     //qDebug("KarambaWidget::getSensor");
//     QString s;
//     Sensor* sensor = meter->getSensor();
//     if(sensor)
//         s = findSensorFromMap(sensor);
//     return s;
 }

void KarambaWidget::deleteMeterFromSensors(Meter* meter)
{
    meter->detachFromSensor();
    //qDebug("KarambaWidget::deleteMeterFromSensors");
    /*Sensor* sensor = findSensorFromList(meter);

    if (sensor)
    {
      sensor->deleteMeter(meter);
      if (sensor->isEmpty())
      {
        QString s = findSensorFromMap(sensor);
        sensorMap.erase(s);
        sensorList.removeAll(sensor);
      }
    }*/
}

void KarambaWidget::setSensor(const LineParser& lineParser, Meter* meter)
{
    //qDebug("KarambaWidget::setSensor");
    Sensor* sensor = 0;

    deleteMeterFromSensors(meter);

    QString sens = lineParser.getString("SENSOR").upper();

    if( sens == "CPU" )
    {
//         QString cpuNbr = lineParser.getString("CPU");
//         sensor = sensorMap["CPU"+cpuNbr];
//         if (sensor == 0)
//         {
            int interval = lineParser.getInt("INTERVAL");
            interval = (interval == 0)?1000:interval;
            sensor = ( /*sensorMap["CPU"+cpuNbr] =*/ CPUSensor::self());
            sensorList.append( sensor );
//         }
        /*SensorParams *sp = new SensorParams(meter);
        sp->addParam("FORMAT",
                     m_theme.locale()->translate(lineParser.getString("FORMAT").ascii()));
        sp->addParam("DECIMALS",lineParser.getString("DECIMALS"));*/
        meter->setFormat(m_theme.locale()->translate(lineParser.getString("FORMAT").ascii()));
        meter->attachToSensor(sensor);
        //sensor->setMaxValue(sp);

    }

    if( sens == "MEMORY" )
    {
        sensor = sensorMap["MEMORY"];
        if (sensor == 0)
        {
            int interval = lineParser.getInt("INTERVAL");
            interval = (interval == 0)?3000:interval;
            sensor = ( sensorMap["MEMORY"] = MemSensor::self() );
            sensorList.append( sensor );
        }
        /*SensorParams *sp = new SensorParams(meter);
        sp->addParam("FORMAT",
            m_theme.locale()->translate(lineParser.getString("FORMAT").ascii()));
        */
        meter->setFormat(m_theme.locale()->translate(lineParser.getString("FORMAT").ascii()));
        meter->attachToSensor(sensor);
        //sensor->setMaxValue(sp);
    }


    if( sens == "DISK" )
    {
        sensor = sensorMap["DISK"];
        if (sensor == 0)
        {
            int interval = lineParser.getInt("INTERVAL");
            interval = (interval == 0)?5000:interval;
            sensor = ( sensorMap["DISK"] = DiskSensor::self() );
            connect( sensor, SIGNAL(initComplete()), this, SLOT(externalStep()) );
            sensorList.append( sensor );
        }
        // meter->setMax( ((DiskSensor*)sensor)->getTotalSpace(mntPt)/1024 );
        SensorParams *sp = new SensorParams(meter);
        QString mntPt = lineParser.getString("MOUNTPOINT");
        if( mntPt.isEmpty()  )
        {
            mntPt = "/";
        }
        // remove any trailing '/' from mount points in the .theme config, our
        // mntMap doesn't like trailing '/'s for matching in DiskSensor
        if( mntPt.length() > 1 && mntPt.endsWith("/") )
        {
            mntPt.remove( mntPt.length()-1, 1 );
        }
        /*sp->addParam("MOUNTPOINT",mntPt);
        sp->addParam("FORMAT",
                     m_theme.locale()->translate(lineParser.getString("FORMAT").ascii()));*/
        meter->setFormat(m_theme.locale()->translate(lineParser.getString("FORMAT").ascii()));
        meter->attachToSensor(sensor);
        //sensor->setMaxValue(sp);
    }

    if( sens == "NETWORK")
    {
        int interval = lineParser.getInt("INTERVAL");
        interval = (interval == 0)?2000:interval;
        QString device = lineParser.getString("DEVICE");
        sensor = sensorMap["NETWORK"+device];
        if (sensor == 0)
        {
            sensor = ( sensorMap["NETWORK"+device] =
                    NetworkSensor::self());
            sensorList.append( sensor );
        }
        /*SensorParams *sp = new SensorParams(meter);
        sp->addParam("FORMAT",
                     m_theme.locale()->translate(lineParser.getString("FORMAT").ascii()));
        sp->addParam("DECIMALS", lineParser.getString("DECIMALS"));*/
        meter->setFormat(m_theme.locale()->translate(lineParser.getString("FORMAT").ascii()));
        meter->attachToSensor(sensor);
    }

    if( sens == "UPTIME" )
    {
        sensor = sensorMap["UPTIME"];
        if (sensor == 0)
        {
            int interval = lineParser.getInt("INTERVAL");
            interval = (interval == 0)?60000:interval;
            sensor = ( sensorMap["UPTIME"] = UptimeSensor::self());
            sensorList.append( sensor );

        }
        /*SensorParams *sp = new SensorParams(meter);
        sp->addParam("FORMAT",
                     m_theme.locale()->translate(lineParser.getString("FORMAT").ascii()));*/
        meter->setFormat(m_theme.locale()->translate(lineParser.getString("FORMAT").ascii()));
        meter->attachToSensor(sensor);
    }

    if( sens == "SENSOR" )
    {
        sensor = sensorMap["SENSOR"];
        if (sensor == 0)
        {
            int interval = lineParser.getInt("INTERVAL");
            interval = (interval == 0)?30000:interval;
            sensor = (sensorMap["SENSOR"] = SensorSensor::self());
            sensorList.append( sensor );
        }
        /*SensorParams *sp = new SensorParams(meter);
        sp->addParam("FORMAT",
                     m_theme.locale()->translate(lineParser.getString("FORMAT").ascii()));
        sp->addParam("TYPE", lineParser.getString("TYPE"));*/
        meter->setFormat(m_theme.locale()->translate(lineParser.getString("FORMAT").ascii()));
        meter->attachToSensor(sensor);
    }


    if( sens == "TEXTFILE" )
    {
        QString path = lineParser.getString("PATH");
        bool rdf = lineParser.getBoolean("RDF");
        sensor = sensorMap["TEXTFILE"+path];
        if (sensor == 0)
        {
            int interval = lineParser.getInt("INTERVAL");
            interval = ( interval == 0 )?60000:interval;
            QString encoding = lineParser.getString("ENCODING");

            sensor = ( sensorMap["TEXTFILE"+path] =
                           new TextFileSensor( path, rdf, interval, encoding ) );
            sensorList.append( sensor );
        }
        /*SensorParams *sp = new SensorParams(meter);
        sp->addParam("LINE",QString::number(lineParser.getInt("LINE")));*/
        meter->attachToSensor(sensor);
    }


    if( sens == "TIME")
    {
        sensor = sensorMap["DATE"];
        if (sensor == 0)
        {
            int interval = lineParser.getInt("INTERVAL");
            interval = (interval == 0)?60000:interval;
            sensor = ( sensorMap["DATE"] = DateSensor::self());
            sensorList.append( sensor );
            timeList.append( sensor );
        }
        /*SensorParams *sp = new SensorParams(meter);
        sp->addParam("FORMAT",
                     m_theme.locale()->translate(lineParser.getString("FORMAT").ascii()));
        sp->addParam("CALWIDTH",lineParser.getString("CALWIDTH"));
        sp->addParam("CALHEIGHT",lineParser.getString("CALHEIGHT"));*/
        meter->setFormat(m_theme.locale()->translate(lineParser.getString("FORMAT").ascii()));
        meter->attachToSensor(sensor);
    }

#ifdef HAVE_XMMS

    if( sens == "XMMS" )
    {
        sensor = sensorMap["XMMS"];
        if (sensor == 0)
        {
            int interval = lineParser.getInt("INTERVAL");
            interval = (interval == 0)?1000:interval;
            QString encoding = lineParser.getString("ENCODING");

            sensor = ( sensorMap["XMMS"] = new XMMSSensor( interval, encoding ) );
            sensorList.append( sensor );
        }
        /*SensorParams *sp = new SensorParams(meter);
        sp->addParam("FORMAT",
                     m_theme.locale()->translate(lineParser.getString("FORMAT").ascii()));*/
        meter->setFormat(m_theme.locale()->translate(lineParser.getString("FORMAT").ascii()));
        meter->attachToSensor(sensor);
        //sensor->setMaxValue(sp);
    }
#endif // HAVE_XMMS


    if( sens == "NOATUN" )
    {
        sensor = sensorMap["NOATUN"];
        if (sensor == 0)
        {
            int interval = lineParser.getInt("INTERVAL");
            interval = (interval == 0)?1000:interval;
            sensor = ( sensorMap["NOATUN"] = new NoatunSensor( interval, client ) );
            sensorList.append( sensor );
        }
        /*SensorParams *sp = new SensorParams(meter);
        sp->addParam("FORMAT",
                     m_theme.locale()->translate(lineParser.getString("FORMAT").ascii()));*/
        meter->setFormat(m_theme.locale()->translate(lineParser.getString("FORMAT").ascii()));
        meter->attachToSensor(sensor);
        //sensor->setMaxValue(sp);
    }

    if( sens == "PROGRAM")
    {
        QString progName = lineParser.getString("PROGRAM");
        sensor = sensorMap["PROGRAM"+progName];
        if (sensor == 0)
        {
            int interval = lineParser.getInt("INTERVAL");
            interval = (interval == 0)?3600000:interval;
            QString encoding = lineParser.getString("ENCODING");

            sensor = (sensorMap["PROGRAM"+progName] =
                          new ProgramSensor( progName, interval, encoding ) );
            sensorList.append( sensor );
        }
        /*SensorParams *sp = new SensorParams(meter);
        sp->addParam( "LINE", QString::number(lineParser.getInt("LINE")));
        sp->addParam( "THEMAPATH", m_theme.path() );*/
        meter->attachToSensor(sensor);
    }

    if( sens == "RSS" )
    {
        QString source = lineParser.getString("SOURCE");
        QString format =
            m_theme.locale()->translate(lineParser.getString("FORMAT").ascii());

        sensor = sensorMap["RSS"+source];
        if (sensor == 0)
        {
            int interval = lineParser.getInt( "INTERVAL");
            interval = ( interval == 0 )?60000:interval;
            QString encoding = lineParser.getString("ENCODING");

            sensor = ( sensorMap["RSS"+source] =
                           new RssSensor( source, interval, format, encoding ) );
            sensorList.append( sensor );
        }
        /*SensorParams *sp = new SensorParams(meter);
        sp->addParam("SOURCE",lineParser.getString("SOURCE"));*/
        meter->attachToSensor(sensor);
    }

    if (sensor != 0)
    {
//        QTimer::singleShot( 0, sensor, SLOT(update()) );
//        sensor->start();
    }
}

void KarambaWidget::slotFileChanged( const QString & )
{
    reloadConfig();
}

void KarambaWidget::passMenuOptionChanged(QString key, bool value)
{
    //Everything below is to call the python callback function
    if (pythonIface && pythonIface->isExtensionLoaded())
        pythonIface->menuOptionChanged(this, key, value);
}

//FIXME remove this function

void KarambaWidget::meterClicked(QMouseEvent* e, Meter* meter)
{
    //qWarning("KarambaWidget::meterClicked");
//     if (pythonIface && pythonIface->isExtensionLoaded() && haveUpdated)
//     {
//         int button = 0;
// 
//         if( e->button() == Qt::LeftButton )
//             button = 1;
//         else if( e->button() == Qt::MidButton )
//             button = 2;
//         else if( e->button() == Qt::RightButton )
//             button = 3;
// 
//         if (RichTextLabel* richText = dynamic_cast<RichTextLabel*>(meter))
//         {
//             pythonIface->meterClicked(this, richText->anchorAt(e->x(), e->y()),
//                                       button);
//         }
//         else
//         {
//             pythonIface->meterClicked(this, meter, button);
//         }
//     }
}
//FIXME remove this function
void KarambaWidget::passClick(QMouseEvent *e)
{
    /*//qDebug("KarambaWidget::passClick");
    QListIterator<Sensor *> it2( timeList ); // iterate over meters
    while ( it2.hasNext() )
    {
        //(( DateSensor* ) it2.next())->toggleCalendar( e );
    }


    // We create a temporary click list here because original
    // can change during the loop (infinite loop Bug 994359)
    QList<ClickMap *> clickListTmp(clickList);
    QListIterator<ClickMap *> it(clickListTmp);
    while (it.hasNext())
    {
        Meter* meter = (Meter*)(it.next());
        // Check if meter is still in list
        if (clickList.count((ClickMap *)meter) && meter->click(e))
        {
            // callback
            meterClicked(e, meter);
        }
    }

    //Everything below is to call the python callback function
    if (pythonIface && pythonIface->isExtensionLoaded() && haveUpdated)
    {
        int button = 0;

        if( e->button() == Qt::LeftButton )
            button = 1;
        else if( e->button() == Qt::MidButton )
            button = 2;
        else if( e->button() == Qt::RightButton )
            button = 3;

        pythonIface->widgetClicked(this, e->x(), e->y(), button);
    }*/
}

//FIXME remove this function
void KarambaWidget::passWheelClick( QWheelEvent *e )
{
 /*   //qDebug("KarambaWidget::passWheelClick");
    //Everything below is to call the python callback function
    if (pythonIface && pythonIface->isExtensionLoaded() && haveUpdated)
    {
        int button = 0;

        if( e->delta() > 0 )
            button = 4;
        else
            button = 5;

        pythonIface->widgetClicked(this, e->x(), e->y(), button);
    }*/
}

//FIXME remove lower() and use only KeepBelow
void KarambaWidget::mousePressEvent( QMouseEvent *e )
{
    //qDebug("KarambaWidget::mousePressEvent");
    if( e->button() == Qt::RightButton )
    {
        kpop->exec(QCursor::pos());
    }
    else
    {
        clickPos = e->pos();
//         if( toggleLocked -> isChecked() )
//             passClick( e );
        if( !(onTop || managed))
            lower();
    }
}


//FIXME remove this function
void KarambaWidget::wheelEvent( QWheelEvent *e )
{
    //qDebug("KarambaWidget::wheelEvent");
//     passWheelClick( e );
}

void KarambaWidget::mouseReleaseEvent( QMouseEvent *e )
{
    //qDebug("KarambaWidget::mouseReleaseEvent");
    clickPos = e->pos();
}

//FIXME remove this function
void KarambaWidget::mouseDoubleClickEvent( QMouseEvent *e )
{
    //qDebug("KarambaWidget::mouseDoubleClickEvent");
//     if( !toggleLocked -> isChecked() )
//     {
//         passClick( e );
//     }
}
//FIXME remove this function

void KarambaWidget::keyPressEvent(QKeyEvent *e)
{
    //qDebug("KarambaWidget::keyPressEvent");
//     keyPressed(e->text(), 0);
}

//FIXME remove this function
void KarambaWidget::keyPressed(const QString& s, const Meter* meter)
{
//     if (pythonIface && pythonIface->isExtensionLoaded())
//         pythonIface->keyPressed(this, meter, s);
}

void KarambaWidget::mouseMoveEvent( QMouseEvent *e )
{
    //qDebug("KarambaWidget::mouseMoveEvent");
    if( e->state() !=  0 && e->state() < 16 && !toggleLocked -> isChecked() )
    {
        move( e->globalPos() - clickPos );
    }
    else
//     {
//         // Change cursor over ClickArea
// //         QListIterator<ClickMap *> it(clickList);
//         bool insideArea = false;
// 
//         while (it.hasNext())
//         {
//             insideArea = ((Meter*) it.next()) -> insideActiveArea(e -> x(), e ->y());
//             if (insideArea)
//             {
//                 break;
//             }
//         }
// 
//         if(insideArea)
//         {
//             if( cursor().shape() != Qt::PointingHandCursor )
//                 setCursor( Qt::PointingHandCursor );
//         }
//         else
//         {
//             if( cursor().shape() != Qt::ArrowCursor )
//                 setCursor( Qt::ArrowCursor );
//         }
// 
//         QListIterator<ImageLabel *> image_it( imageList);        // iterate over image sensors
//         while ( image_it.hasNext() )
//         {
//             ((ImageLabel*) image_it.next())->rolloverImage(e);
//         }
//     }

    if (pythonIface && pythonIface->isExtensionLoaded())
    {
        int button = 0;

        //Modified by Ryan Nickell (p0z3r@mail.com) 03/16/2004
        // This will work now, but only when you move at least
        // one pixel in any direction with your mouse.
        //if( e->button() == Qt::LeftButton )
        if( e->state() == Qt::LeftButton)
            button = 1;
        //else if( e->button() == Qt::MidButton )
        else if( e->state() == Qt::MidButton )
            button = 2;
        //else if( e->button() == Qt::RightButton )
        else if( e->state() == Qt::RightButton )
            button = 3;

        pythonIface->widgetMouseMoved(this, e->x(), e->y(), button);
    }
}

void KarambaWidget::closeEvent ( QCloseEvent *  qc)
{
    //qDebug("KarambaWidget::closeEvent");
    qc->accept();
    //  this->close(true);
    //  delete this;
}
//FIXME remove the double buffering over here. Not needed now.

void KarambaWidget::paintEvent ( QPaintEvent *e)
{
    //kdDebug() << k_funcinfo << pm.size() << endl;
    if(pm.width() == 0)
    {
        //return;
    }
    if( !(onTop || managed))
    {
        if( lowerTimer.elapsed() > 100 )
        {
            this->lower();
            lowerTimer.restart();
        }
    }
    QRect rect = e->rect();
    QPainter p(this);
    p.drawPixmap(rect.topLeft(), pm, rect);
    p.end();
    if (pythonIface && pythonIface->isExtensionLoaded())
    {
        if (haveUpdated == 0)
        {
            pythonIface->initWidget(this);
            haveUpdated = 1;
        }
        else
        {
            pythonIface->widgetUpdated(this);
        }
    }
}

//FIXME remove this function
void KarambaWidget::updateSensors()
{
    //qDebug("KarambaWidget::updateSensors");
/*    QListIterator<Sensor *> it( sensorList ); // iterate over meters
    while ( it.hasNext() )
    {
        ((Sensor*) it.next())->update();
    }
    QTimer::singleShot( 500, this, SLOT(step()) );
    */
}

//FIXME remove this function
void KarambaWidget::updateBackground(KSharedPixmap* kpm)
{
    //kdDebug() << k_funcinfo << pm.size() << endl;
    // if pm width == 0 this is the first time we come here and we should start
    // the theme. This is because we need the background before starting.
//     if(pm.width() == 0)
//         start();
//     background = QPixmap(*kpm);
// 
//     QPixmap buffer = QPixmap(size());
// 
//     pm = QPixmap(size());
//     buffer.fill(Qt::black);
// 
//     QListIterator<ImageLabel *> it( imageList ); // iterate over meters
//     p.begin(&buffer);
//     p.drawPixmap(0, 0, background);
// 
//     while ( it.hasNext() )
//     {
//         ImageLabel *tmp = qobject_cast<ImageLabel *>(it.next());
//         if (tmp->background == 1)
//         {
//   //          tmp->mUpdate(&p, 1);
//         }
//     }
//     p.end();
// 
//     p.begin(&pm);
//     p.drawPixmap(0, 0, buffer);
//     p.end();
// 
//     background = pm;
// 
//     QPixmap buffer2 = QPixmap(size());
// 
//     pm = QPixmap(size());
//     buffer2.fill(Qt::black);
// 
//     QListIterator<QObject *> it2( meterList ); // iterate over meters
//     p.begin(&buffer2);
//     p.drawPixmap(0, 0, background);
// 
//     while ( it2.hasNext() )
//     {
//  //       ((Meter*) it2.next())->mUpdate(&p);
//     }
//     p.end();
// 
//     p.begin(&pm);
//     p.drawPixmap(0, 0, buffer2);
//     p.end();
// 
//     if (systray != 0)
//     {
//         systray->updateBackgroundPixmap(buffer2);
//     }
//     repaint();
}

void KarambaWidget::currentDesktopChanged( int i )
{
    //qDebug("KarambaWidget::currentDesktopChanged");
//    kroot->repaint( true );
    if (pythonIface && pythonIface->isExtensionLoaded())
        pythonIface->desktopChanged(this, i);
}

void KarambaWidget::currentWallpaperChanged(int i )
{
    //qDebug("KarambaWidget::currentWallpaperChanged");
//    kroot->repaint( true );
    if (pythonIface && pythonIface->isExtensionLoaded())
        pythonIface->wallpaperChanged(this, i);
}

//FIXME remove externalStep() function. Not needed now.

void KarambaWidget::externalStep()
{
    //kdDebug() << k_funcinfo << pm.size() << endl;
    if (widgetUpdate)
    {
        QPixmap buffer = QPixmap(size());

        pm = QPixmap(size());
        buffer.fill(Qt::black);

        QListIterator<QObject *> it( meterList ); // iterate over meters
        p.begin(&buffer);
        p.drawPixmap(0, 0, background);

        while ( it.hasNext()  )
        {
          //  ((Meter*) it.next())->mUpdate(&p);
        }
        p.end();

        p.begin(&pm);
        p.drawPixmap(0, 0, buffer);
        p.end();

        repaint();
    }
}

//FIXME remove this function. no need to update at intervals.

void KarambaWidget::step()
{
    //kdDebug() << k_funcinfo << pm.size() << endl;
    if (widgetUpdate && haveUpdated)
    {
        pm = QPixmap(size());
        QPixmap buffer = QPixmap(size());
        buffer.fill(Qt::black);

        QListIterator<QObject *> it( meterList ); // iterate over meters
        p.begin(&buffer);
        p.drawPixmap(0, 0, background);

        while (it.hasNext())
        {
    //        ((Meter*) it.next())->mUpdate(&p);
        }
        p.end();

        p.begin(&pm);
        p.drawPixmap(0, 0, buffer);
        p.end();

        update();
    }

    if (pythonIface && pythonIface->isExtensionLoaded())
    {
        if (haveUpdated == 0)
            pythonIface->initWidget(this);
        else
            pythonIface->widgetUpdated(this);
    }

    if (haveUpdated == 0)
        haveUpdated = 1;
}

void KarambaWidget::widgetClosed()
{
    //qDebug("KarambaWidget::widgetClosed");
    if (pythonIface && pythonIface->isExtensionLoaded())
        pythonIface->widgetClosed(this);
}

void KarambaWidget::slotToggleLocked()
{
    //qDebug("KarambaWidget::slotToggleLocked");
    if(toggleLocked->isChecked())
    {
        toggleLocked->setIconSet(SmallIconSet("lock"));
    }
    else
    {
        toggleLocked->setIconSet(SmallIconSet("move"));
    }
}
/*FIXME should we really give transform as an option to user? IMHO this is theme developer's
  job to recognise where smooth transform is enough efficient.so basically it should be a
  property in meters  */

void KarambaWidget::slotToggleFastTransforms()
{
    //qDebug("KarambaWidget::slotToggleFastTransforms");
    //    bool fastTransforms = toggleFastTransforms -> isChecked();
    //    if (toggleFastTransforms -> isChecked())
    //    {
    //     toggleFastTransforms -> setIconSet(SmallIconSet("ok"));
    //    }
    //    else
    //    {
    //     QPixmap ok_disabled;
    //            toggleFastTransforms -> setIconSet(ok_disabled);
    //    }
    //config.setGroup("internal");
    //config.writeEntry("fastTransforms", toggleFastTransforms -> isChecked());
}

//FIXME see above
bool KarambaWidget::useSmoothTransforms()
{
    //qDebug("KarambaWidget::useSmoothTransforms");
    return !toggleFastTransforms -> isChecked();
}

void KarambaWidget::writeConfigData()
{
    //qDebug("KarambaWidget::writeConfigData");
    config -> setGroup("internal");
    config -> writeEntry("lockedPosition", toggleLocked -> isChecked() );
    config -> writeEntry("fastTransforms", toggleFastTransforms -> isChecked() );
    config -> writeEntry("desktop", desktop );
    config -> setGroup("theme");
    // Widget Position
    config -> writeEntry("widgetPosX", x());
    config -> writeEntry("widgetPosY", y());
    // Widget Size
    config -> writeEntry("widgetWidth", width());
    config -> writeEntry("widgetHeight", height());

    // write changes to DiskSensor
    config -> sync();
    //qWarning("Config File ~/.superkaramba/%s.rc written.",
    //         m_theme.name().ascii());
}

void KarambaWidget::slotToggleConfigOption(QString key, bool value)
{
    //qDebug("KarambaWidget::slotToggleConfigOption");
    config -> setGroup("config menu");
    config -> writeEntry(key, value);
    passMenuOptionChanged(key, value);
}

void KarambaWidget::addMenuConfigOption(QString key, QString name)
{
    //qDebug("KarambaWidget::addMenuConfigOption");
    kpop -> setItemEnabled(THEMECONF, true);

    SignalBridge* action = new SignalBridge(this, key, menuAccColl);
    KToggleAction* confItem = new KToggleAction (name.ascii(), KShortcut::null(),
                              action, SLOT(receive()),
                              menuAccColl, key.ascii());
    confItem -> setName(key.ascii());

    menuAccColl -> insert(confItem);

    connect(action, SIGNAL( enabled(QString, bool) ),
            this, SLOT( slotToggleConfigOption(QString, bool) ));

    config -> setGroup("config menu");
    confItem -> setChecked(config -> readBoolEntry(key));

    confItem -> plug(themeConfMenu);

    numberOfConfMenuItems++;
}

bool KarambaWidget::setMenuConfigOption(QString key, bool value)
{
    //qDebug("KarambaWidget::setMenuConfigOption");
    KToggleAction* menuAction = ((KToggleAction*)menuAccColl -> action(key.ascii()));
    if (menuAction == NULL)
    {
        qWarning("Menu action %s not found.", key.ascii());
        return false;
    }
    else
    {
        menuAction -> setChecked(value);
        return true;
    }
}

bool KarambaWidget::readMenuConfigOption(QString key)
{
    //qDebug("KarambaWidget::readMenuConfigOption");
    KToggleAction* menuAction = ((KToggleAction*)menuAccColl -> action(key.ascii()));
    if (menuAction == NULL)
    {
        qWarning("Menu action %s not found.", key.ascii());
        return false;
    }
    else
    {
        return menuAction -> isChecked();
    }
}

void KarambaWidget::passMenuItemClicked(int id)
{
    //qDebug("KarambaWidget::passMenuItemClicked");
    //Everything below is to call the python callback function
    if (pythonIface && pythonIface->isExtensionLoaded())
    {
        KPopupMenu* menu = 0;
        for(int i = 0; i < (int)menuList.count(); i++)
        {
            KPopupMenu* tmp;
            tmp = (KPopupMenu*) menuList.at(i);

            if(tmp != 0)
            {
                if(tmp->isItemVisible(id))
                {
                    menu = tmp;
                    break;
                }
            }
        }
        pythonIface->menuItemClicked(this, menu, id);
    }
}

/*FIXME all these taskmanager stuff needs to be separated from karambawidget. 
    on the first thought, I think, pointer to pythonIface/AbstractIface should
    be available to classes other than karambawidget too. Then these signals 
    should be emitted from there. as meters are now separate widgets, they also 
    emit Iface signals. May be, we should make the Taskmanager class singleton, then it 
    will have the iface QList, which can be populated by individual KarambaWidgets.*/

void KarambaWidget::activeTaskChanged(Task* t)
{
    //qDebug("KarambaWidget::activeTaskChanged");
    //Everything below is to call the python callback function
    if (pythonIface && pythonIface->isExtensionLoaded())
        pythonIface->activeTaskChanged(this, t);
}

void KarambaWidget::taskAdded(Task* t)
{
    //qDebug("KarambaWidget::taskAdded");
    //Everything below is to call the python callback function
    if (pythonIface && pythonIface->isExtensionLoaded())
        pythonIface->taskAdded(this, t);
}

void KarambaWidget::taskRemoved(Task* t)
{
    //qDebug("KarambaWidget::taskRemoved");
    //Everything below is to call the python callback function
    if (pythonIface && pythonIface->isExtensionLoaded())
        pythonIface->taskRemoved(this, t);
}

void KarambaWidget::startupAdded(Startup* t)
{
    //qDebug("KarambaWidget::startupAdded");
    //Everything below is to call the python callback function
    if (pythonIface && pythonIface->isExtensionLoaded())
        pythonIface->startupAdded(this, t);
}

void KarambaWidget::startupRemoved(Startup* t)
{
    //qDebug("KarambaWidget::startupRemoved");
    //Everything below is to call the python callback function
    if (pythonIface && pythonIface->isExtensionLoaded())
        pythonIface->startupRemoved(this, t);
}

void  KarambaWidget::processExited (KProcess* proc)
{
    //qDebug("KarambaWidget::processExited");
    if (pythonIface && pythonIface->isExtensionLoaded())
        pythonIface->commandFinished(this, (int)proc->pid());
}

void  KarambaWidget::receivedStdout (KProcess *proc, char *buffer, int)
{
    //qDebug("KarambaWidget::receivedStdout");
    //Everything below is to call the python callback function
    if (pythonIface && pythonIface->isExtensionLoaded())
        pythonIface->commandOutput(this, (int)proc->pid(), buffer);
}

//For KDE session management
void KarambaWidget::saveProperties(KConfig* config)
{
    //qDebug("KarambaWidget::saveProperties");
    config->setGroup("session");
    config->writeEntry("theme", m_theme.file());
    writeConfigData();
}

//For KDE session management
void KarambaWidget::readProperties(KConfig* config)
{
    //qDebug("KarambaWidget::readProperties");
    config->setGroup("session");
    QString atheme = config->readEntry("theme","");
}

//Register types of events that can be dragged on our widget
void KarambaWidget::dragEnterEvent(QDragEnterEvent* event)
{
    //qDebug("KarambaWidget::dragEnterEvent");
    event->accept(Q3TextDrag::canDecode(event));
}

//Handle the drop part of a drag and drop event.
void KarambaWidget::dropEvent(QDropEvent* event)
{
    //qDebug("KarambaWidget::dropEvent");
    QString text;

    if ( Q3TextDrag::decode(event, text) )
    {
        //Everything below is to call the python callback function
        if (pythonIface && pythonIface->isExtensionLoaded())
            pythonIface->itemDropped(this, text.ascii());
    }
}

void KarambaWidget::toDesktop(int id, int menuid)
{
    //qDebug("KarambaWidget::toDesktop");
    int i;

    desktop = id;
    for (i=0; ; i++)
    {
        int mid = toDesktopMenu->idAt(i);
        if (mid == -1)
            break;

        toDesktopMenu->setItemChecked(mid, false);
    }
    toDesktopMenu->setItemChecked(menuid, true);

    if (desktop)
        info->setDesktop( desktop);
    else
        info->setDesktop( NETWinInfo::OnAllDesktops );
}

void KarambaWidget::systrayUpdated()
{
    //qDebug("KarambaWidget::systrayUpdated");
    if (pythonIface && pythonIface->isExtensionLoaded())
        pythonIface->systrayUpdated(this);
}
/*FIXME do we really need update functions should not the update be 
     called by only meters, when they are updated? */

void KarambaWidget::toggleWidgetUpdate( bool b)
{
    //qDebug("KarambaWidget::toggleWidgetUpdate");
    if (pythonIface && pythonIface->isExtensionLoaded())
        widgetUpdate = b;
}

/* FIXME why do we need the meterlist to have with karambawidget? The meters are searched by name
   here in meterlist, which we can always do with findChild() findChildren() directly with onle line ? */

void KarambaWidget::addMeter(Meter* meter)
{
    meterList.append(meter);
}

SignalBridge::SignalBridge(QObject* parent, QString name, KActionCollection* ac)
        : QObject(parent, name.ascii()), collection(ac)
{
    setName(name.ascii());
}

void SignalBridge::receive()
{
    emit enabled(name(), ((KToggleAction*)collection -> action(name())) ->
                 isChecked());
}

DesktopChangeSlot::DesktopChangeSlot(QObject *parent, int id)
        : QObject(parent, "")
{
    desktopid = id;
}

void DesktopChangeSlot::receive()
{
    KarambaWidget *k = (KarambaWidget *)parent();

    // XXX - check type cast

    k->toDesktop(desktopid, menuid);
}

void DesktopChangeSlot::setMenuId(int id)
{
    menuid = id;
}

int DesktopChangeSlot::menuId()
{
    return menuid;
}

void KarambaWidget::showMenuExtension()
{
    kglobal = new KPopupMenu(this);

    trayMenuToggleId = kglobal->insertItem(SmallIconSet("superkaramba"),
                                           i18n("Show System Tray Icon"), this,
                                           SLOT(slotToggleSystemTray()),
                                           Qt::CTRL + Qt::Key_S);

    trayMenuThemeId = kglobal->insertItem(SmallIconSet("knewstuff"),
                                          i18n("&Manage Themes..."), this,
                                          SLOT(slotShowTheme()), Qt::CTRL + Qt::Key_M);

    trayMenuQuitId = kglobal->insertItem(SmallIconSet("exit"),
                                         i18n("&Quit SuperKaramba"), this,
                                         SLOT(slotQuit()), Qt::CTRL + Qt::Key_Q);

    kglobal->polish();

    trayMenuSeperatorId = kpop->insertSeparator();
    kpop->insertItem("SuperKaramba", kglobal);
}

void KarambaWidget::hideMenuExtension()
{
    if(kglobal)
    {
        kpop->removeItem(trayMenuSeperatorId);
        kglobal->removeItem(trayMenuToggleId);
        kglobal->removeItem(trayMenuThemeId);
        kglobal->removeItem(trayMenuQuitId);

        delete kglobal;
        kglobal = 0;
    }
}

void KarambaWidget::slotToggleSystemTray()
{
    karambaApp->globalHideSysTray(false);
}

void KarambaWidget::slotQuit()
{
    karambaApp->globalQuitSuperKaramba();
}

void KarambaWidget::slotShowTheme()
{
    karambaApp->showThemeDialog();
}

#include "karambawidget.moc"
