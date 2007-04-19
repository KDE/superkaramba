/*
 * Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>
 * Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>
 * Copyright (C) 2004,2005 Luke Kenneth Casson Leighton <lkcl@lkcl.net>
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 ****************************************************************************/

#include "karamba_python.h"
#include "dcopinterface_stub.h"
#include "richtextlabel.h"
#include "karamba.h"
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

#include <qdir.h>
#include <qwidgetlist.h>

// Menu IDs
#define EDITSCRIPT 1
#define THEMECONF  2

karamba::karamba(QString fn, QString name, bool reloading, int instance,
    bool sub_theme):
    QWidget(0,"karamba", Qt::WGroupLeader | WStyle_Customize |
            WRepaintNoErase| WStyle_NoBorder | WDestructiveClose  ),
    meterList(0), imageList(0), clickList(0), kpop(0), widgetMask(0),
    config(0), kWinModule(0), tempUnit('C'), m_instance(instance),
    sensorList(0), timeList(0),
    themeConfMenu(0), toDesktopMenu(0), kglobal(0), clickPos(0, 0), accColl(0),
    menuAccColl(0), toggleLocked(0), pythonIface(0), defaultTextField(0),
    trayMenuSeperatorId(-1), trayMenuQuitId(-1), trayMenuToggleId(-1),
    trayMenuThemeId(-1),
    m_sysTimer(NULL)
{
  themeStarted = false;
  want_right_button = false;
  want_meter_wheel_event = false;
  prettyName = name;
  m_sub_theme = sub_theme;

  KURL url;

  if(fn.find('/') == -1)
    url.setFileName(fn);
  else
    url = fn;
  if(!m_theme.set(url))
  {
    setFixedSize(0, 0);
    QTimer::singleShot(100, this, SLOT(killWidget()));
    return;
  }
  // Add self to list of open themes
  // This also updates instance number
  karambaApp->addKaramba(this, reloading);

  if(prettyName.isEmpty())
    prettyName = QString("%1 - %2").arg(m_theme.name()).arg(m_instance);

  kdDebug() << "Starting theme: " << m_theme.name()
            << " pretty name: " << prettyName << endl;
  QString qName = "karamba - " + prettyName;
  setName(qName.ascii());

  KDirWatch *dirWatch = KDirWatch::self();
  connect(dirWatch, SIGNAL( dirty( const QString & ) ),
          SLOT( slotFileChanged( const QString & ) ) );

  if(!dirWatch->contains(m_theme.file()))
    dirWatch->addFile(m_theme.file());

  if(!m_theme.isZipTheme() && m_theme.pythonModuleExists())
  {
    QString pythonFile = m_theme.path() + "/" + m_theme.pythonModule() + ".py";
    if(!dirWatch->contains(pythonFile))
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
  if(m_theme.pythonModuleExists())
  {
    kdDebug() << "Loading python module: " << m_theme.pythonModule() << endl;
    QTimer::singleShot(0, this, SLOT(initPythonInterface()));
  }

  widgetMask = 0;
  info = new NETWinInfo( qt_xdisplay(), winId(), qt_xrootwin(), NET::WMState );

  // could be replaced with TaskManager
  kWinModule = new KWinModule();
  desktop = 0;

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
                    SLOT(updateSensors()), Key_F5 );
  toggleLocked = new KToggleAction (  i18n("Toggle &Locked Position"),
                                      SmallIconSet("locked"),
                                      CTRL+Key_L, this,
                                      SLOT( slotToggleLocked() ),
                                      accColl, "Locked position" );
  accColl->insert(toggleLocked);
  toggleLocked -> setChecked(true);

  toggleLocked->plug(kpop);

  toggleFastTransforms = new KToggleAction(i18n("Use &Fast Image Scaling"),
                         CTRL+Key_F, this,
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
                    SLOT(reloadConfig()), CTRL+Key_R );
  kpop->insertItem( SmallIconSet("fileclose"),i18n("&Close This Theme"), this,
                    SLOT(killWidget()), CTRL+Key_C );

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

  meterList = new QObjectList();
  meterList->setAutoDelete( true );
  sensorList = new QObjectList();
  sensorList->setAutoDelete( true );
  clickList = new QObjectList();
  timeList = new QObjectList();
  imageList = new QObjectList();
  menuList = new QObjectList();
  menuList->setAutoDelete( true );

  client = kapp->dcopClient();
  if (!client->isAttached())
    client->attach();
  appId = client->registerAs(qApp->name());


  setBackgroundMode( NoBackground);
  if( !(onTop || managed))
    KWin::lowerWindow( winId() );

  if( !parseConfig() )
  {
    setFixedSize(0,0);
    QTimer::singleShot( 100, this, SLOT(killWidget()) );
    qWarning("Could not read config file.");
  }
  else
  {
    kroot = new KarambaRootPixmap((QWidget*)this);
    kroot->start();
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


  setFocusPolicy(QWidget::StrongFocus);
}

karamba::~karamba()
{
  //qDebug("karamba::~karamba");
  //Remove self from list of open themes
  karambaApp->deleteKaramba(this, m_reloading);

  widgetClosed();
  if(m_theme.isValid())
    writeConfigData();

  delete config;

  if(meterList != 0)
  {
    meterList->clear();
    delete meterList;
  }

  if( sensorList != 0 )
  {
    sensorList->clear();
    delete sensorList;
  }

  if( imageList != 0 )
  {
    imageList->clear();
    delete imageList;
  }

  if( clickList != 0 )
  {
    clickList->clear();
    delete clickList;
  }

  if( timeList != 0 )
  {
    timeList->clear();
    delete timeList;
  }

  delete toggleLocked;
  delete accColl;
  delete menuAccColl;
  delete themeConfMenu;
  delete kpop;
  delete widgetMask;
  delete kWinModule;
  delete defaultTextField;
  if (pythonIface != NULL)
    delete pythonIface;
}

bool karamba::parseConfig()
{
  //qDebug("karamba::parseConfig");
  bool passive = true;

  if(m_theme.open())
  {
    QValueStack<QPoint> offsetStack;
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
        //qDebug("karamba found");
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
          reparent(0, Qt::WType_Dialog | WStyle_Customize | WStyle_NormalBorder
                      |  WRepaintNoErase | WDestructiveClose, pos());
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
        (new karamba( path, QString() ))->show();
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

      if(lineParser.meter() == "CLICKAREA")
      {
        if( !hasMouseTracking() )
          setMouseTracking(true);
        ClickArea *tmp = new ClickArea(this, x, y, w, h );
        tmp->setOnClick(lineParser.getString("ONCLICK"));

        setSensor(lineParser, (Meter*)tmp);
        clickList->append( tmp );
        if( lineParser.getBoolean("PREVIEW"))
          meterList->append( tmp );
      }

      // program sensor without a meter
      if(lineParser.meter() == "SENSOR=PROGRAM")
      {
        setSensor(lineParser, 0 );
      }

      if(lineParser.meter() == "IMAGE")
      {
        QString file = lineParser.getString("PATH");
        QString file_roll = lineParser.getString("PATHROLL");
        int xon = lineParser.getInt("XROLL");
        int yon = lineParser.getInt("YROLL");
        QString tiptext = lineParser.getString("TOOLTIP");
        QString name = lineParser.getString("NAME");
        bool bg = lineParser.getBoolean("BACKGROUND");
        xon = ( xon <= 0 ) ? x:xon;
        yon = ( yon <= 0 ) ? y:yon;

        ImageLabel *tmp = new ImageLabel(this, x, y, 0, 0);
        tmp->setValue(file);
        if(!file_roll.isEmpty())
          tmp->parseImages(file, file_roll, x,y, xon, yon);
        tmp->setBackground(bg);
        if (!name.isEmpty())
          tmp->setName(name.ascii());
        if (!tiptext.isEmpty())
          tmp->setTooltip(tiptext);

        connect(tmp, SIGNAL(pixmapLoaded()), this, SLOT(externalStep()));
        setSensor(lineParser, (Meter*) tmp );
        meterList->append (tmp );
        imageList->append (tmp );
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

      if(lineParser.meter() == "TEXT" ||
         lineParser.meter() == "CLICKMAP" ||
         lineParser.meter() == "RICHTEXT" ||
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
        if(lineParser.meter() == "TEXT")
        {

          TextLabel *tmp = new TextLabel(this, x, y, w, h );
          tmp->setTextProps(tmpText);
          tmp->setValue(
              m_theme.locale()->translate(lineParser.getString("VALUE")));

          QString name = lineParser.getString("NAME");
          if (!name.isEmpty())
            tmp->setName(name.ascii());

          setSensor(lineParser, (Meter*)tmp);
          meterList->append ( tmp );
        }

        if(lineParser.meter() == "CLICKMAP")
        {
          if( !hasMouseTracking() )
            setMouseTracking(true);
          ClickMap *tmp = new ClickMap(this, x, y, w, h);
          tmp->setTextProps( tmpText );

          setSensor(lineParser, (Meter*)tmp);
          // set all params
          clickList -> append(tmp);
          meterList->append( tmp );

        }

        if(lineParser.meter() == "RICHTEXT")
        {
          RichTextLabel *tmp = new RichTextLabel(this, x, y, w, h);

          bool dUl = lineParser.getBoolean("UNDERLINE");

          tmp->setText(
              m_theme.locale()->translate(lineParser.getString("VALUE").ascii()), dUl);
          tmp->setTextProps( tmpText );
          tmp->setWidth(w);
          tmp->setHeight(h);

          QString name = lineParser.getString("NAME");
          if (!name.isEmpty())
            tmp->setName(name.ascii());

          setSensor(lineParser, (Meter*)tmp);
          clickList -> append(tmp);
          meterList->append ( tmp );
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

          meterList->append(tmp);
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
        meterList->append ( tmp );
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
        meterList->append ( tmp );
      }
    }

    if(passive && !managed)
    {
      // Matthew Kay: set window type to "dock"
      // (plays better with taskbar themes this way)
      KWin::setType(winId(), NET::Dock);
      #if defined(KDE_MAKE_VERSION)
        #if KDE_VERSION >= KDE_MAKE_VERSION(3,1,9)
          //KDE 3.2 addition for the always on top issues
          KWin::setState(winId(), NET::KeepBelow);
        #endif
      #endif
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

void karamba::start()
{
  m_sysTimer = new QTimer(this);

  connect(m_sysTimer, SIGNAL(timeout()), SLOT(step()));

  m_sysTimer->start(m_interval);

    //Start the widget running
  QTimer::singleShot( 0, this, SLOT(step()) );

  if( !(onTop || managed) )
    lowerTimer.start();
}

void karamba::makeActive()
{
  KWin::setType(winId(), NET::Normal);

  #if defined(KDE_MAKE_VERSION)
    #if KDE_VERSION >= KDE_MAKE_VERSION(3,1,9)
      //KDE 3.2 addition for the always on top issues
      KWin::setState(winId(), NET::Modal);
    #endif
  #endif
}

void karamba::makePassive()
{
  if(managed)
    return;

  QObject *meter;
  for (meter = meterList->first(); meter; meter = meterList->next())
  {
    if((meter)->isA("Input"))
      return;
  }

  // Matthew Kay: set window type to "dock" (plays better with taskbar themes
  // this way)
  KWin::setType(winId(), NET::Dock);
  #if defined(KDE_MAKE_VERSION)
    #if KDE_VERSION >= KDE_MAKE_VERSION(3,1,9)
      //KDE 3.2 addition for the always on top issues
      KWin::setState(winId(), NET::KeepBelow);
    #endif
  #endif
}

void karamba::popupNotify(int)
{
  //qDebug("karamba::popupNotify");
}

void karamba::reloadConfig()
{
  //qDebug("karamba::reloadConfig: %s", m_theme.file().ascii());
  writeConfigData();
  if(m_theme.exists())
  {
    QFileInfo fileInfo( m_theme.file() );
    (new karamba(m_theme.file(), fileInfo.baseName(), true, m_instance))->show();
  }
  closeTheme(true);
}

void karamba::closeTheme(bool reloading)
{
  m_reloading = reloading;
  close();
}

void karamba::killWidget()
{
  closeTheme();
}

void karamba::initPythonInterface()
{
  pythonIface = new KarambaPython(m_theme, m_reloading);
}

void karamba::editConfig()
{
  //qDebug("karamba::editConfig");
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

void karamba::editScript()
{
  //qDebug("karamba::editScript");
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

QString karamba::findSensorFromMap(Sensor* sensor)
{
  //qDebug("karamba::findSensorFromMap");
  QMap<QString,Sensor*>::ConstIterator it;
  QMap<QString,Sensor*>::ConstIterator end( sensorMap.end() );
  for ( it = sensorMap.begin(); it != end; ++it )
  {
    if (it.data() == sensor)
      return it.key();
  }
  return "";
}

Sensor* karamba::findSensorFromList(Meter* meter)
{
  //qDebug("karamba::findSensorFromList");
  QObjectListIt it( *sensorList ); // iterate over meters

  while ( it != 0 )
  {
    if (((Sensor*) *it)->hasMeter(meter))
      return ((Sensor*)*it);
    ++it;
  }
  return NULL;
}

QString karamba::getSensor(Meter* meter)
{
  //qDebug("karamba::getSensor");
  QString s;
  Sensor* sensor = findSensorFromList(meter);
  if (sensor)
    s = findSensorFromMap(sensor);
  return s;
}

void karamba::deleteMeterFromSensors(Meter* meter)
{
  //qDebug("karamba::deleteMeterFromSensors");
  Sensor* sensor = findSensorFromList(meter);

  if (sensor)
  {
    sensor->deleteMeter(meter);
    if (sensor->isEmpty())
    {
      QString s = findSensorFromMap(sensor);
      sensorMap.erase(s);
      sensorList->removeRef(sensor);
    }
  }
}

void karamba::setSensor(const LineParser& lineParser, Meter* meter)
{
  //qDebug("karamba::setSensor");
  Sensor* sensor = 0;

  deleteMeterFromSensors(meter);

  QString sens = lineParser.getString("SENSOR").upper();

  if( sens == "CPU" )
  {
    QString cpuNbr = lineParser.getString("CPU");
    sensor = sensorMap["CPU"+cpuNbr];
    if (sensor == 0)
    {
      int interval = lineParser.getInt("INTERVAL");
      interval = (interval == 0)?1000:interval;
      sensor = ( sensorMap["CPU"+cpuNbr] = new CPUSensor( cpuNbr, interval ) );
      sensorList->append( sensor );
    }
    SensorParams *sp = new SensorParams(meter);
    sp->addParam("FORMAT",
                 m_theme.locale()->translate(lineParser.getString("FORMAT").ascii()));
    sp->addParam("DECIMALS",lineParser.getString("DECIMALS"));

    sensor->addMeter(sp);
    sensor->setMaxValue(sp);

  }

  if( sens == "MEMORY" )
  {
    sensor = sensorMap["MEMORY"];
    if (sensor == 0)
    {
      int interval = lineParser.getInt("INTERVAL");
      interval = (interval == 0)?3000:interval;
      sensor = ( sensorMap["MEMORY"] = new MemSensor( interval ) );
      sensorList->append( sensor );
    }
    SensorParams *sp = new SensorParams(meter);
    sp->addParam("FORMAT",
        m_theme.locale()->translate(lineParser.getString("FORMAT").ascii()));

    sensor->addMeter(sp);
    sensor->setMaxValue(sp);
  }


  if( sens == "DISK" )
  {
    sensor = sensorMap["DISK"];
    if (sensor == 0)
    {
      int interval = lineParser.getInt("INTERVAL");
      interval = (interval == 0)?5000:interval;
      sensor = ( sensorMap["DISK"] = new DiskSensor( interval ) );
      connect( sensor, SIGNAL(initComplete()), this, SLOT(externalStep()) );
      sensorList->append( sensor );
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
    sp->addParam("MOUNTPOINT",mntPt);
    sp->addParam("FORMAT",
                 m_theme.locale()->translate(lineParser.getString("FORMAT").ascii()));
    sensor->addMeter(sp);
    sensor->setMaxValue(sp);
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
          new NetworkSensor(device, interval));
      sensorList->append( sensor );
    }
    SensorParams *sp = new SensorParams(meter);
    sp->addParam("FORMAT",
                 m_theme.locale()->translate(lineParser.getString("FORMAT").ascii()));
    sp->addParam("DECIMALS", lineParser.getString("DECIMALS"));
    sensor->addMeter(sp);
  }

  if( sens == "UPTIME" )
  {
    sensor = sensorMap["UPTIME"];
    if (sensor == 0)
    {
      int interval = lineParser.getInt("INTERVAL");
      interval = (interval == 0)?60000:interval;
      sensor = ( sensorMap["UPTIME"] = new UptimeSensor( interval ));
      sensorList->append( sensor );

    }
    SensorParams *sp = new SensorParams(meter);
    sp->addParam("FORMAT",
                 m_theme.locale()->translate(lineParser.getString("FORMAT").ascii()));
    sensor->addMeter(sp);
  }

  if( sens == "SENSOR" )
  {
    sensor = sensorMap["SENSOR"];
    if (sensor == 0)
    {
      int interval = lineParser.getInt("INTERVAL");
      interval = (interval == 0)?30000:interval;
      sensor = (sensorMap["SENSOR"] = new SensorSensor(interval, tempUnit));
      sensorList->append( sensor );
    }
    SensorParams *sp = new SensorParams(meter);
    sp->addParam("FORMAT",
                 m_theme.locale()->translate(lineParser.getString("FORMAT").ascii()));
    sp->addParam("TYPE", lineParser.getString("TYPE"));
    sensor->addMeter(sp);
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
      sensorList->append( sensor );
    }
    SensorParams *sp = new SensorParams(meter);
    sp->addParam("LINE",QString::number(lineParser.getInt("LINE")));
    sensor->addMeter(sp);
  }


  if( sens == "TIME")
  {
    sensor = sensorMap["DATE"];
    if (sensor == 0)
    {
      int interval = lineParser.getInt("INTERVAL");
      interval = (interval == 0)?60000:interval;
      sensor = ( sensorMap["DATE"] = new DateSensor( interval ) );
      sensorList->append( sensor );
      timeList->append( sensor );
    }
    SensorParams *sp = new SensorParams(meter);
    sp->addParam("FORMAT",
                 m_theme.locale()->translate(lineParser.getString("FORMAT").ascii()));
    sp->addParam("CALWIDTH",lineParser.getString("CALWIDTH"));
    sp->addParam("CALHEIGHT",lineParser.getString("CALHEIGHT"));
    sensor->addMeter(sp);
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
      sensorList->append( sensor );
    }
    SensorParams *sp = new SensorParams(meter);
    sp->addParam("FORMAT",
                 m_theme.locale()->translate(lineParser.getString("FORMAT").ascii()));
    sensor->addMeter(sp);
    sensor->setMaxValue(sp);
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
      sensorList->append( sensor );
    }
    SensorParams *sp = new SensorParams(meter);
    sp->addParam("FORMAT",
                 m_theme.locale()->translate(lineParser.getString("FORMAT").ascii()));
    sensor->addMeter(sp);
    sensor->setMaxValue(sp);
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
      sensorList->append( sensor );
    }
    SensorParams *sp = new SensorParams(meter);
    sp->addParam( "LINE", QString::number(lineParser.getInt("LINE")));
    sp->addParam( "THEMAPATH", m_theme.path() );
    sensor->addMeter(sp);
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
      sensorList->append( sensor );
    }
    SensorParams *sp = new SensorParams(meter);
    sp->addParam("SOURCE",lineParser.getString("SOURCE"));
    sensor->addMeter(sp);
  }

  if (sensor != 0)
  {
    QTimer::singleShot( 0, sensor, SLOT(update()) );
    sensor->start();
  }
}

void karamba::slotFileChanged( const QString & file)
{
  //kdDebug() << "fileChanged: " << file << endl;

  QString pythonFile = m_theme.path() + "/" + m_theme.pythonModule() + ".py";

  if(file == m_theme.file() || file == pythonFile)
    reloadConfig();
}

void karamba::passMenuOptionChanged(QString key, bool value)
{
  //Everything below is to call the python callback function
  if (pythonIface && pythonIface->isExtensionLoaded())
    pythonIface->menuOptionChanged(this, key, value);
}

void karamba::setIncomingData(QString theme, QString obj)
{
  KarambaApplication* app = (KarambaApplication*)qApp;

  kdDebug() << "karamba::setIncomingData " << theme << obj << endl;
   //QByteArray data;
   //QDataStream dataStream( data, IO_WriteOnly );
   //dataStream << theme;
   //dataStream << txt;

   //kapp->dcopClient()->send( app->dcopClient()->appId(), "KarambaIface", "themeNotify(QString,QString)", data );

  DCOPClient *c = kapp->dcopClient();
  if (!c->isAttached())
    c->attach();

  if(app->dcopStub())
    app->dcopStub()->setIncomingData(theme, obj);
}

void karamba::callTheme(QString theme, QString txt)
{
  KarambaApplication* app = (KarambaApplication*)qApp;
  kdDebug() << "karamba::callTheme " << theme << txt << endl;
  //qWarning("karamba::callTheme");
   //QByteArray data;
   //QDataStream dataStream( data, IO_WriteOnly );
   //dataStream << theme;
   //dataStream << txt;

   //kapp->dcopClient()->send( app->dcopClient()->appId(), "KarambaIface", "themeNotify(QString,QString)", data );

  DCOPClient *c = kapp->dcopClient();
  if (!c->isAttached())
    c->attach();

  if(app->dcopStub())
    app->dcopStub()->themeNotify(theme, txt);
}

void karamba::themeNotify(QString theme, QString txt)
{
  kdDebug() << "karamba::themeNotify" << theme << txt << endl;
  if (pythonIface->isExtensionLoaded())
  {
      pythonIface->themeNotify(this, theme.ascii(), txt.ascii());
  }
}

void karamba::meterClicked(QMouseEvent* e, Meter* meter)
{
  //qWarning("karamba::meterClicked");
  if (pythonIface && pythonIface->isExtensionLoaded() && haveUpdated)
  {
    int button = 0;

    if( e->button() == Qt::LeftButton )
      button = 1;
    else if( e->button() == Qt::MidButton )
      button = 2;
    else if( e->button() == Qt::RightButton )
      button = 3;

    if (RichTextLabel* richText = dynamic_cast<RichTextLabel*>(meter))
    {
      pythonIface->meterClicked(this, richText->anchorAt(e->x(), e->y()),
                                button);
    }
    else
    {
      pythonIface->meterClicked(this, meter, button);
    }
  }
}

void karamba::changeInterval(int interval)
{
  if (m_sysTimer != NULL)
    m_sysTimer->changeInterval(interval);
}

void karamba::passClick(QMouseEvent *e)
{
  //qDebug("karamba::passClick");
  QObjectListIt it2( *timeList ); // iterate over meters
  while ( it2 != 0 )
  {
    (( DateSensor* ) *it2)->toggleCalendar( e );
    ++it2;
  }


  // We create a temporary click list here because original
  // can change during the loop (infinite loop Bug 994359)
  QObjectList clickListTmp(*clickList);
  QObjectListIt it(clickListTmp);
  while (it != 0)
  {
    Meter* meter = (Meter*)(*it);
    // Check if meter is still in list
    if (clickList->containsRef(meter) && meter->click(e))
    {
      // callback
      meterClicked(e, meter);
    }
    ++it;
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
  }
}

void karamba::passWheelClick( QWheelEvent *e )
{
  //qDebug("karamba::passWheelClick");
  //Everything below is to call the python callback function
  if (pythonIface && pythonIface->isExtensionLoaded() && haveUpdated)
  {
    int button = 0;

    if( e->delta() > 0 )
      button = 4;
    else
      button = 5;

    // We create a temporary click list here because original
    // can change during the loop (infinite loop Bug 994359)
    if (want_meter_wheel_event)
    {
      QObjectList clickListTmp(*clickList);
      QObjectListIt it(clickListTmp);

      QMouseEvent fakeEvent(QEvent::MouseButtonPress, e->pos(), e->globalPos(), button, e->state());

      while (it != 0)
      {
        Meter* meter = (Meter*)(*it);
        // Check if meter is still in list
        if (clickList->containsRef(meter) && meter->click(&fakeEvent))
        {
          if (RichTextLabel* richText = dynamic_cast<RichTextLabel*>(meter))
          {
            pythonIface->meterClicked(this, richText->anchorAt(fakeEvent.x(), fakeEvent.y()),
                                    button);
          }
          else
          {
            pythonIface->meterClicked(this, meter, button);
          }
        }
        ++it;
      }
    }

    pythonIface->widgetClicked(this, e->x(), e->y(), button);
  }
}

void karamba::management_popup( void )
{
  kpop->popup(QCursor::pos());
}

void karamba::mousePressEvent( QMouseEvent *e )
{
  //qDebug("karamba::mousePressEvent");
  if( e->button() == RightButton && !want_right_button )
  {
    management_popup();
  }
  else
  {
    clickPos = e->pos();
    if( toggleLocked -> isChecked() )
      passClick( e );
    if( !(onTop || managed))
      KWin::lowerWindow( winId() );
  }
}

void karamba::wheelEvent( QWheelEvent *e )
{
  //qDebug("karamba::wheelEvent");
  passWheelClick( e );
}

void karamba::mouseReleaseEvent( QMouseEvent *e )
{
  //qDebug("karamba::mouseReleaseEvent");
  clickPos = e->pos();
}

void karamba::mouseDoubleClickEvent( QMouseEvent *e )
{
  //qDebug("karamba::mouseDoubleClickEvent");
  if( !toggleLocked -> isChecked() )
  {
    passClick( e );
  }
}

void karamba::keyPressEvent(QKeyEvent *e)
{
  //qDebug("karamba::keyPressEvent");
  keyPressed(e->text(), 0);
}

void karamba::keyPressed(const QString& s, const Meter* meter)
{
  if (pythonIface && pythonIface->isExtensionLoaded())
    pythonIface->keyPressed(this, meter, s);
}

void karamba::mouseMoveEvent( QMouseEvent *e )
{
  //qDebug("karamba::mouseMoveEvent");
  if( e->state() !=  0 && e->state() < 16 && !toggleLocked -> isChecked() )
  {
    move( e->globalPos() - clickPos );
  }
  else
  {
    // Change cursor over ClickArea
    QObjectListIt it(*clickList);
    bool insideArea = false;

    while (it != 0)
    {
      insideArea = ((Meter*)(*it)) -> insideActiveArea(e -> x(), e ->y());
      if (insideArea)
      {
         break;
      }
      ++it;
    }

    if(insideArea)
    {
      if( cursor().shape() != PointingHandCursor )
        setCursor( PointingHandCursor );
    }
    else
    {
      if( cursor().shape() != ArrowCursor )
        setCursor( ArrowCursor );
    }

    QObjectListIt image_it( *imageList);        // iterate over image sensors
    while ( image_it != 0 )
    {
      ((ImageLabel*) *image_it)->rolloverImage(e);
      ++image_it;
    }
  }

  if (pythonIface && pythonIface->isExtensionLoaded())
  {
    int button = 0;

    //Modified by Ryan Nickell (p0z3r@mail.com) 03/16/2004
    // This will work now, but only when you move at least
    // one pixel in any direction with your mouse.
    //if( e->button() == Qt::LeftButton )
    if( e->state() == LeftButton)
      button = 1;
    //else if( e->button() == Qt::MidButton )
    else if( e->state() == MidButton )
      button = 2;
    //else if( e->button() == Qt::RightButton )
    else if( e->state() == RightButton )
      button = 3;

    pythonIface->widgetMouseMoved(this, e->x(), e->y(), button);
  }
}

void karamba::closeEvent ( QCloseEvent *  qc)
{
  //qDebug("karamba::closeEvent");
  qc->accept();
  //  close(true);
  //  delete this;
}

void karamba::paintEvent ( QPaintEvent *e)
{
  //kdDebug() << k_funcinfo << pm.size() << endl;
  if(pm.width() == 0)
    return;
  if( !(onTop || managed))
  {
    if( lowerTimer.elapsed() > 100 )
    {
      KWin::lowerWindow( winId() );
      lowerTimer.restart();
    }
  }
  QRect rect = e->rect();
  bitBlt(this,rect.topLeft(),&pm,rect,Qt::CopyROP);
}

void karamba::updateSensors()
{
  //qDebug("karamba::updateSensors");
  QObjectListIt it( *sensorList ); // iterate over meters
  while ( it != 0 )
  {
    ((Sensor*) *it)->update();
    ++it;
  }
  QTimer::singleShot( 500, this, SLOT(step()) );
}

void karamba::updateBackground(KSharedPixmap* kpm)
{
  //kdDebug() << k_funcinfo << pm.size() << endl;
  // if pm width == 0 this is the first time we come here and we should start
  // the theme. This is because we need the background before starting.
  //if(pm.width() == 0)
  if( !themeStarted )
  {
    themeStarted = true;
    start();
  }
  background = QPixmap(*kpm);

  QPixmap buffer = QPixmap(size());

  pm = QPixmap(size());
  buffer.fill(Qt::black);

  QObjectListIt it( *imageList ); // iterate over meters
  p.begin(&buffer);
  bitBlt(&buffer,0,0,&background,0,Qt::CopyROP);

  while ( it != 0 )
  {
    if (((ImageLabel*) *it)->background == 1)
    {
      ((ImageLabel*) *it)->mUpdate(&p, 1);
    }
    ++it;
  }
  p.end();

  bitBlt(&pm,0,0,&buffer,0,Qt::CopyROP);
  background = pm;

  QPixmap buffer2 = QPixmap(size());

  pm = QPixmap(size());
  buffer2.fill(Qt::black);

  QObjectListIt it2( *meterList ); // iterate over meters
  p.begin(&buffer2);
  bitBlt(&buffer2,0,0,&background,0,Qt::CopyROP);

  while ( it2 != 0 )
  {
    ((Meter*) *it2)->mUpdate(&p);
    ++it2;
  }
  p.end();

  bitBlt(&pm,0,0,&buffer2,0,Qt::CopyROP);
  if (systray != 0)
  {
    systray->updateBackgroundPixmap(buffer2);
  }
  repaint();
}

void karamba::currentDesktopChanged( int i )
{
  //qDebug("karamba::currentDesktopChanged");
  kroot->repaint( true );
  if (pythonIface && pythonIface->isExtensionLoaded())
    pythonIface->desktopChanged(this, i);
}

void karamba::currentWallpaperChanged(int i )
{
  //qDebug("karamba::currentWallpaperChanged");
  kroot->repaint( true );
  if (pythonIface && pythonIface->isExtensionLoaded())
    pythonIface->wallpaperChanged(this, i);
}

void karamba::externalStep()
{
  //kdDebug() << k_funcinfo << pm.size() << endl;
  if (widgetUpdate)
  {
    QPixmap buffer = QPixmap(size());

    pm = QPixmap(size());
    buffer.fill(Qt::black);

    QObjectListIt it( *meterList ); // iterate over meters
    p.begin(&buffer);
    bitBlt(&buffer,0,0,&background,0,Qt::CopyROP);

    while ( it != 0 )
    {
      ((Meter*) *it)->mUpdate(&p);
      ++it;
    }
    p.end();

    bitBlt(&pm,0,0,&buffer,0,Qt::CopyROP);
    repaint();
  }
}

void karamba::step()
{
  //kdDebug() << k_funcinfo << pm.size() << endl;
  if (widgetUpdate && haveUpdated)
  {
    pm = QPixmap(size());
    QPixmap buffer = QPixmap(size());
    buffer.fill(Qt::black);

    QObjectListIt it( *meterList ); // iterate over meters
    p.begin(&buffer);

    bitBlt(&buffer,0,0,&background,0,Qt::CopyROP);

    while (it != 0)
    {
      ((Meter*) *it)->mUpdate(&p);
      ++it;
    }
    p.end();

    bitBlt(&pm,0,0,&buffer,0,Qt::CopyROP);
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

void karamba::widgetClosed()
{
  //qDebug("karamba::widgetClosed");
  if (pythonIface && pythonIface->isExtensionLoaded())
    pythonIface->widgetClosed(this);
}

void karamba::slotToggleLocked()
{
  //qDebug("karamba::slotToggleLocked");
  if(toggleLocked->isChecked())
  {
    toggleLocked->setIconSet(SmallIconSet("lock"));
  }
  else
  {
    toggleLocked->setIconSet(SmallIconSet("move"));
  }
}

void karamba::slotToggleFastTransforms()
{
  //qDebug("karamba::slotToggleFastTransforms");
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


bool karamba::useSmoothTransforms()
{
  //qDebug("karamba::useSmoothTransforms");
  return !toggleFastTransforms -> isChecked();
}

void karamba::writeConfigData()
{
  //qDebug("karamba::writeConfigData");
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

void karamba::slotToggleConfigOption(QString key, bool value)
{
  //qDebug("karamba::slotToggleConfigOption");
  config -> setGroup("config menu");
  config -> writeEntry(key, value);
  passMenuOptionChanged(key, value);
}

void karamba::addMenuConfigOption(QString key, QString name)
{
  //qDebug("karamba::addMenuConfigOption");
  kpop -> setItemEnabled(THEMECONF, true);

  SignalBridge* action = new SignalBridge(this, key, menuAccColl);
  KToggleAction* confItem = new KToggleAction (name, KShortcut::null(),
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

bool karamba::setMenuConfigOption(QString key, bool value)
{
  //qDebug("karamba::setMenuConfigOption");
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

bool karamba::readMenuConfigOption(QString key)
{
  //qDebug("karamba::readMenuConfigOption");
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

void karamba::passMenuItemClicked(int id)
{
  //qDebug("karamba::passMenuItemClicked");
  //Everything below is to call the python callback function
  if (pythonIface && pythonIface->isExtensionLoaded())
  {
    KPopupMenu* menu = 0;
    for(int i = 0; i < (int)menuList->count(); i++)
    {
      KPopupMenu* tmp;
      if(i==0)
      {
        tmp = (KPopupMenu*) menuList->first();
      }
      else
      {
        tmp = (KPopupMenu*) menuList->next();
      }
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

void karamba::activeTaskChanged(Task* t)
{
  //qDebug("karamba::activeTaskChanged");
  //Everything below is to call the python callback function
  if (pythonIface && pythonIface->isExtensionLoaded())
    pythonIface->activeTaskChanged(this, t);
}

void karamba::taskAdded(Task* t)
{
  //qDebug("karamba::taskAdded");
  //Everything below is to call the python callback function
  if (pythonIface && pythonIface->isExtensionLoaded())
    pythonIface->taskAdded(this, t);
}

void karamba::taskRemoved(Task* t)
{
  //qDebug("karamba::taskRemoved");
  //Everything below is to call the python callback function
  if (pythonIface && pythonIface->isExtensionLoaded())
    pythonIface->taskRemoved(this, t);
}

void karamba::startupAdded(Startup* t)
{
  //qDebug("karamba::startupAdded");
  //Everything below is to call the python callback function
  if (pythonIface && pythonIface->isExtensionLoaded())
    pythonIface->startupAdded(this, t);
}

void karamba::startupRemoved(Startup* t)
{
  //qDebug("karamba::startupRemoved");
  //Everything below is to call the python callback function
  if (pythonIface && pythonIface->isExtensionLoaded())
    pythonIface->startupRemoved(this, t);
}

void  karamba::processExited (KProcess* proc)
{
  //qDebug("karamba::processExited");
  if (pythonIface && pythonIface->isExtensionLoaded())
    pythonIface->commandFinished(this, (int)proc->pid());
}

void  karamba::receivedStdout (KProcess *proc, char *buffer, int)
{
  //qDebug("karamba::receivedStdout");
  //Everything below is to call the python callback function
  if (pythonIface && pythonIface->isExtensionLoaded())
    pythonIface->commandOutput(this, (int)proc->pid(), buffer);
}

//For KDE session management
void karamba::saveProperties(KConfig* config)
{
  //qDebug("karamba::saveProperties");
  config->setGroup("session");
  config->writeEntry("theme", m_theme.file());
  writeConfigData();
}

//For KDE session management
void karamba::readProperties(KConfig* config)
{
  //qDebug("karamba::readProperties");
  config->setGroup("session");
  QString atheme = config->readEntry("theme");
}

//Register types of events that can be dragged on our widget
void karamba::dragEnterEvent(QDragEnterEvent* event)
{
  //qDebug("karamba::dragEnterEvent");
  event->accept(QTextDrag::canDecode(event));
}

//Handle the drop part of a drag and drop event.
void karamba::dropEvent(QDropEvent* event)
{
  //qDebug("karamba::dropEvent");
  QString text;

  if ( QTextDrag::decode(event, text) )
  {
    //Everything below is to call the python callback function
    if (pythonIface && pythonIface->isExtensionLoaded())
    {
      const QPoint &p = event->pos();
      pythonIface->itemDropped(this, text, p.x(), p.y());
    }
  }
}

void karamba::toDesktop(int id, int menuid)
{
  //qDebug("karamba::toDesktop");
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

void karamba::systrayUpdated()
{
  //qDebug("karamba::systrayUpdated");
  if (pythonIface && pythonIface->isExtensionLoaded())
    pythonIface->systrayUpdated(this);
}

void karamba::toggleWidgetUpdate( bool b)
{
  //qDebug("karamba::toggleWidgetUpdate");
  if (pythonIface && pythonIface->isExtensionLoaded())
    widgetUpdate = b;
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
  karamba *k = (karamba *)parent();

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

void karamba::showMenuExtension()
{
  kglobal = new KPopupMenu(this);

  trayMenuToggleId = kglobal->insertItem(SmallIconSet("superkaramba"),
                                         i18n("Show System Tray Icon"), this,
                                         SLOT(slotToggleSystemTray()),
                                         CTRL+Key_S);

  trayMenuThemeId = kglobal->insertItem(SmallIconSet("knewstuff"),
                                        i18n("&Manage Themes..."), this,
                                        SLOT(slotShowTheme()), CTRL+Key_M);

  trayMenuQuitId = kglobal->insertItem(SmallIconSet("exit"),
                                       i18n("&Quit SuperKaramba"), this,
                                       SLOT(slotQuit()), CTRL+Key_Q);

  kglobal->polish();

  trayMenuSeperatorId = kpop->insertSeparator();
  kpop->insertItem("SuperKaramba", kglobal);
}

void karamba::hideMenuExtension()
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

void karamba::slotToggleSystemTray()
{
  karambaApp->globalHideSysTray(false);
}

void karamba::slotQuit()
{
  karambaApp->globalQuitSuperKaramba();
}

void karamba::slotShowTheme()
{
  karambaApp->globalShowThemeDialog();
}

void karamba::setAlwaysOnTop(bool stay)
{
    if(stay)
    {
        onTop = true;
        KWin::setState( winId(), NET::KeepAbove );
    }
    else
    {
        onTop = false;
        KWin::setState( winId(), NET::KeepBelow );
    }
}

#include "karamba.moc"
