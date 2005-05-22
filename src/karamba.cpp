/*
 * Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>
 * Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>
 */

#include "karamba_python.h"
#include "dcopinterface_stub.h"
#include "richtextlabel.h"
#include "xpm.h"
#include "karamba.h"
#include "karambaapp.h"
#include "themelistwindow.h"

#include <kdebug.h>
#include <kzip.h>
#include <karchive.h>
#include <kmessagebox.h>
#include <krun.h>
#include <klocale.h>
#include <kwin.h>
#include <kdeversion.h>

#include <kparts/componentfactory.h>
#include <kparts/part.h>

#include <qdir.h>
#include <qwidgetlist.h>

// Menu IDs
#define EDITSCRIPT 1
#define THEMECONF  2

karamba::karamba(QString fn, bool reloading) :
    QWidget(0,"karamba", Qt::WGroupLeader | WStyle_Customize |
            WRepaintNoErase| WStyle_NoBorder | WDestructiveClose  ),
    tempUnit('C'), clickPos(0,0), pythonIface(0)
{
  //qDebug("karamba::karamba");
  QFileInfo fi(fn);

  if(isZipFile(fn))
  {
    themePath = fn;
    m_zipTheme = true;
  }
  else
  {
    themePath = fi.dirPath(true) + "/";
    m_zipTheme = false;
  }
  themeName = fi.baseName();
  themeFile = fn;
  setName("karamba - " + themeName);

  widgetUpdate = true;

  QFile themeConfigFile(QDir::home().absPath() + "/.superkaramba/" + themeName + ".rc");
  // Tests if config file Exists
  if (!QFileInfo(themeConfigFile).exists())
  {
    // Create config file
    themeConfigFile.open(IO_ReadWrite);
    themeConfigFile.close();
  }

  // Creates KConfig Object
  config = new KConfig(QFileInfo(themeConfigFile).filePath(), false, false);
  config -> sync();
  config -> setGroup("internal");

  m_reloading = reloading;
  QTimer::singleShot(0, this, SLOT(initPythonInterface()));

  //Matthew Kay: set window type to "dock" (plays better with taskbar themes this way)
  KWin::setType(winId(), NET::Dock);

#if defined(KDE_MAKE_VERSION)
# if KDE_VERSION >= KDE_MAKE_VERSION(3,1,9)
  //KDE 3.2 addition for the always on top issues
  KWin::setState(winId(), NET::KeepBelow);
# endif
#endif

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
  mid = toDesktopMenu -> insertItem ("&All Desktops",
                                     dslot = new DesktopChangeSlot(this,0),
                                     SLOT(receive()));
  dslot->setMenuId(mid);

  toDesktopMenu -> insertSeparator();
  for (int ndesktop=1; ndesktop <= kWinModule->numberOfDesktops(); ndesktop++)
  {
    QString name = "Desktop &";
    name += ('0' + ndesktop);

    mid = toDesktopMenu -> insertItem (name,
        dslot = new DesktopChangeSlot(this, ndesktop), SLOT(receive()));
    dslot->setMenuId(mid);
  }


  kpop = new KPopupMenu( this );
  kpop -> setCheckable(true);

  /*
  keditpop = new KPopupMenu ( this );
  keditpop -> insertItem(SmallIconSet("edit"),tr("Edit &theme"),this,
                         SLOT(editConfig()), CTRL+Key_E );
  keditpop -> insertItem(SmallIconSet("edit"),tr("Edit &script"),this,
                         SLOT(editScript()), ALT+Key_E, EDITSCRIPT);

  // Test if Theme Script exists
  QFileInfo scriptFile(themePath + "/" + themeName + ".py");
  keditpop -> setItemEnabled(EDITSCRIPT, scriptFile.exists());
  */

  accColl = new KActionCollection( this );
  menuAccColl = new KActionCollection( this );

  kpop->insertItem( SmallIconSet("reload"),tr("Update"), this,
                    SLOT(updateSensors()), Key_F5 );
  toggleLocked = new KToggleAction (  tr("Toggle &locked position"),
                                      SmallIconSet("locked"),
                                      CTRL+Key_L, this,
                                      SLOT( slotToggleLocked() ),
                                      accColl, "Locked position" );
  accColl->insert(toggleLocked);
  toggleLocked -> setChecked(true);

  toggleLocked->plug(kpop);

  toggleFastTransforms = new KToggleAction(tr("Use &fast image scaling"),
                         CTRL+Key_F, this,
                         SLOT( slotToggleFastTransforms() ),
                         accColl, "Fast transformations");

  accColl->insert(toggleFastTransforms);
  toggleFastTransforms -> setChecked(true);

  toggleFastTransforms -> plug(kpop);

  kpop->insertSeparator();

  kpop->insertItem( SmallIconSet("fileopen"),tr("&Open new theme"), this,
                    SLOT(startNewKaramba()), CTRL+Key_O );
  
  /*kpop->insertItem(tr("&Edit"), keditpop);*/
  
  kpop->insertItem(tr("Configure &theme"), themeConfMenu, THEMECONF);
  kpop->setItemEnabled(THEMECONF, false);
  kpop->insertItem(tr("To Des&ktop"), toDesktopMenu);

  kpop->insertItem( SmallIconSet("reload3"),tr("&Reload theme"),this,
                    SLOT(reloadConfig()), CTRL+Key_R );
  kpop->insertItem( SmallIconSet("fileclose"),tr("&Close this theme"), this,
                    SLOT(killWidget()), CTRL+Key_C );

  kpop->insertSeparator();
  kpop->insertItem( SmallIconSet("find"),tr("&Download more themes"), this,
                    SLOT(downloadThemes()), CTRL+Key_D );

  kpop->insertSeparator();

  kpop->insertItem( SmallIconSet("exit"),tr("&Quit"), this,
                    SLOT(quitKaramba()), CTRL+Key_Q );

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


  this->setBackgroundMode( NoBackground);
  if( !(onTop || managed))
    this->lower();


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
  if (!config -> readBoolEntry("lockedPosition", true))
  {
    toggleLocked -> setChecked(false);
    slotToggleLocked();
  }

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

  //Add self to list of open themes
  karambaApp->addKaramba(this, themeName);
}

karamba::~karamba()
{
  //qDebug("karamba::~karamba");
  //Remove self from list of open themes
  karambaApp->deleteKaramba(this, themeName);

  widgetClosed();
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

  if( toggleLocked != 0)
    delete toggleLocked;
  if( accColl != 0)
    delete accColl;
  if ( menuAccColl != 0)
    delete menuAccColl;
  /*
  if( keditpop != 0)
    delete keditpop;
  */
  if( themeConfMenu != 0)
    delete themeConfMenu;

  if( kpop != 0)
    delete kpop;

  if( widgetMask != 0)
    delete widgetMask;

  delete kWinModule;
  delete defaultTextField;

  delete pythonIface;
}

bool karamba::isZipFile(const QString& filename)
{
  QFile file(filename);

  if(file.open(IO_ReadOnly))
  {
    unsigned char buf[5];

    if(file.readBlock((char*)buf, 4) == 4)
    {
      if(buf[0] == 'P' && buf[1] == 'K' && buf[2] == 3 && buf[3] == 4)
        return true;
    }
  }
  return false;
}

QByteArray karamba::readZipFile(const QString& filename)
{
  QByteArray result;
  KZip zip(themeFile);
  const KArchiveDirectory* dir;
  const KArchiveEntry* entry;
  const KArchiveFile* file;

  if(!zip.open(IO_ReadOnly))
  {
    qDebug("Unable to open '%s' for reading.", themeFile.ascii());
    return result;
  }
  dir = zip.directory();
  if(dir == 0)
  {
    qDebug("Error reading directory contents of file %s", themeFile.ascii());
    return result;
  }

  entry = dir->entry(filename);
  if(entry == 0 || !entry->isFile())
  {
    qDebug("Error reading %s file from %s",
           filename.ascii(), themeFile.ascii());
    return false;
  }

  file = static_cast<const KArchiveFile*>(entry);
  result = file->data();
  zip.close();
  return result;
}

bool karamba::parseConfig()
{
  //qDebug("karamba::parseConfig");
  QTimer *m_sysTimer = new QTimer(this);

  QFile file(themeFile);
  QString line;
  QString meter;
  int interval = 0;
  QTextStream *t = 0;        // use a text stream
  bool ok = false;

  if(m_zipTheme)
  {
    QByteArray ba = readZipFile(themeName + ".theme");
    if(ba.size() > 0)
    {
      t = new QTextStream(ba, IO_ReadOnly);
      ok = true;
    }
  }
  else
  {
    if(file.open(IO_ReadOnly|IO_Translate))
    {
      t = new QTextStream(&file);        // use a text stream
      ok = true;
    }
  }

  if(ok)
  {
    QValueStack<QPoint> offsetStack;
    offsetStack.push(QPoint(0,0));

    int x=0;
    int y=0;
    int w=0;
    int h=0;
    while( ( line = t->readLine() ) !=0 )
    {
      QRegExp rx("^\\s*(\\S+)");
      rx.search(line);
      meter = rx.cap(1).upper();

      x = getInt("X",line)+offsetStack.top().x();
      y = getInt("Y",line)+offsetStack.top().y();
      w = getInt("W",line);
      h = getInt("H",line);


      if( meter == "KARAMBA" && !foundKaramba )
      {
        //qDebug("karamba found");
        toggleLocked->setChecked( getBoolean( "LOCKED", line ) );
        slotToggleLocked();

        x = ( x < 0 ) ? 0:x;
        y = ( y < 0 ) ? 0:y;

        if( w == 0 ||  h == 0)
        {
          w = 300;
          h = 300;
        }
        setFixedSize(w,h);

        if( getBoolean( "RIGHT", line ) )
        {
          QDesktopWidget *d = QApplication::desktop();
          x = d->width() - w;
        }
        else
          if( getBoolean( "LEFT", line ) )
          {
            x = 0;
          }

        if( getBoolean( "BOTTOM", line ) )
        {
          QDesktopWidget *d = QApplication::desktop();
          y = d->height() - h;
        }
        else
          if( getBoolean( "TOP", line ) )
          {
            y = 0;
          }

        move(x,y);
        pm = QPixmap(size());

        if( getBoolean("ONTOP", line ) )
        {
          onTop = true;
          KWin::setState( winId(), NET::StaysOnTop );
        }

        if( getBoolean("MANAGED", line ) )
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

        if (getBoolean("ONALLDESKTOPS", line))
        {
          desktop = 200; // ugly
        }


        bool dfound=false;
        //int desktop = getInt("DESKTOP", line, dfound);
        if (dfound)
        {
          info->setDesktop( dfound );
        }
        if( getBoolean("TOPBAR", line ) )
        {
          move(x,0);
          KWin::setStrut( winId(), 0, 0, h, 0 );
          toggleLocked->setChecked( true );
          slotToggleLocked();
          toggleLocked->setEnabled(false);

        }

        if( getBoolean("BOTTOMBAR", line ) )
        {
          int dh = QApplication::desktop()->height();
          move( x, dh - h );
          KWin::setStrut( winId(), 0, 0, 0, h );
          toggleLocked->setChecked( true );
          slotToggleLocked();
          toggleLocked->setEnabled(false);


        }

        if( getBoolean("RIGHTBAR", line ) )
        {
          int dw = QApplication::desktop()->width();
          move( dw - w, y );
          KWin::setStrut( winId(), 0, w, 0, 0 );
          toggleLocked->setChecked( true );
          slotToggleLocked();
          toggleLocked->setEnabled(false);

        }
        if( getBoolean("LEFTBAR", line) )
        {
          //int dw = QApplication::desktop()->width();
          move( 0, y );
          KWin::setStrut( winId(), w, 0, 0, 0 );
          toggleLocked->setChecked( true );
          slotToggleLocked();
          toggleLocked->setEnabled(false);
        }

        bool found = false;
        QString path = getString( "MASK", line, found );

        if( 1 )
        {

          QFileInfo info(path);
          if( info.isRelative())
            path = themePath +"/" + path;

          widgetMask = new QBitmap(path);

          setMask(*widgetMask);

        }

        interval = getInt("INTERVAL",line);
        interval = ( interval == 0 ) ? 5000 : interval;

        QString temp = getString("TEMPUNIT", line, found).upper();
        if(found && !temp.isEmpty())
          tempUnit = temp.ascii()[0];
        foundKaramba = true;
      }

      if( meter == "THEME" )
      {
        QString path = getString( "PATH", line );
        QFileInfo info(path);
        if( info.isRelative())
          path = themePath +"/" + path;
        //qDebug("new theme from theme. path: " + path);
        (new karamba( path, false ))->show();
      }

      if( meter == "<GROUP>" )
      {
        int offsetX = offsetStack.top().x();
        int offsetY = offsetStack.top().y();
        offsetStack.push( QPoint( offsetX + getInt("X",line),
                                  offsetY + getInt("Y",line) ) );
      }

      if( meter == "</GROUP>" )
      {
        offsetStack.pop();
      }

      if( meter == "CLICKAREA" )
      {
        if( !hasMouseTracking() )
          setMouseTracking(true);
        ClickArea *tmp = new ClickArea(this, x, y, w, h );
        tmp->setOnClick( getString( "ONCLICK", line ) );

        setSensor( line, (Meter*) tmp );
        clickList->append( tmp );
        if( getBoolean( "PREVIEW", line ) )
          meterList->append( tmp );
      }

      // program sensor without a meter
      if(meter == "SENSOR=PROGRAM")
      {
        setSensor(line, 0 );
      }

      if(meter == "IMAGE")
      {
        QString file = getString("PATH",line);
        QString file_roll = getString("PATHROLL",line);
        int xon = getInt("XROLL",line);
        int yon = getInt("YROLL",line);
        QString tiptext = getString("TOOLTIP",line);
        QString name = getString("NAME",line);
        bool bg = getBoolean("BACKGROUND", line);
        xon = ( xon <= 0 ) ? x:xon;
        yon = ( yon <= 0 ) ? y:yon;

        ImageLabel *tmp = new ImageLabel(this, x, y, 0, 0);
        tmp->setValue(file);
        if(!file_roll.isEmpty())
          tmp->parseImages(file, file_roll, x,y, xon, yon);
        tmp->setBackground(bg);
        if (name != "")
          tmp->setName(name.ascii());
        if (!tiptext.isEmpty())
          tmp->setTooltip(tiptext);

        connect(tmp, SIGNAL(pixmapLoaded()), this, SLOT(externalStep()));
        setSensor(line, (Meter*) tmp );
        meterList->append (tmp );
        imageList->append (tmp );
      }

      if( meter == "DEFAULTFONT" )
      {
        int r,g,b;

        delete defaultTextField;
        defaultTextField = new TextField( );

        get3Int( "COLOR", line, r, g, b );
        defaultTextField->setColor(r, g, b);
        get3Int( "BGCOLOR", line, r, g, b );
        defaultTextField->setBGColor( r, g, b );

        defaultTextField->setFont( getString( "FONT", line ) );
        defaultTextField->setFontSize( getInt( "FONTSIZE", line ) );
        defaultTextField->setAlignment( getString( "ALIGN", line ) );
        defaultTextField->setFixedPitch( getBoolean( "FIXEDPITCH", line ) );
        defaultTextField->setShadow( getInt( "SHADOW", line ) );

      }


      if(meter == "TEXT" || meter == "CLICKMAP" || meter == "RICHTEXT")
      {
        int r,g,b;

        TextField* tmpText = new TextField();
        bool paramFound = false;

        get3Int( "COLOR", line, r, g, b, paramFound );
        if ( paramFound || !defaultTextField )
        {
          tmpText->setColor ( r, g, b );
        }
        else
          if ( defaultTextField )
          {
            defaultTextField->getColor().rgb ( &r, &g, &b );
            tmpText->setColor( r, g, b );
          }

        get3Int( "BGCOLOR", line, r, g, b, paramFound );
        if ( paramFound  || !defaultTextField )
          tmpText->setBGColor( r, g, b );
        else
          if ( defaultTextField )
          {
            defaultTextField->getBGColor().rgb ( &r, &g, &b );
            tmpText->setBGColor( r, g, b );
          }

        QString dFont = getString("FONT", line, paramFound);
        if( paramFound || !defaultTextField )
          tmpText->setFont( dFont );
        else
          if( defaultTextField )
            tmpText->setFont( defaultTextField->getFont() );


        int dFontSize = getInt( "FONTSIZE", line, paramFound );
        if( paramFound || !defaultTextField )
          tmpText->setFontSize( dFontSize );
        else
          if( defaultTextField )
            tmpText->setFontSize( defaultTextField->getFontSize() );

        QString dAlign = getString( "ALIGN", line, paramFound );
        if( paramFound || !defaultTextField )
          tmpText->setAlignment( dAlign );
        else
          if( defaultTextField )
            tmpText->setAlignment( defaultTextField->getAlignment() );

        bool dFp = getBoolean( "FIXEDPITCH", line, paramFound );
        if( paramFound || !defaultTextField )
          tmpText->setFixedPitch( dFp );
        else
          if( defaultTextField )
            tmpText->setFixedPitch( defaultTextField->getFixedPitch() );

        int dShadow = getInt( "SHADOW", line, paramFound );
        if( paramFound || !defaultTextField )
          tmpText->setShadow( dShadow );
        else
          if( defaultTextField )
            tmpText->setShadow( defaultTextField->getShadow() );

        // ////////////////////////////////////////////////////
        // Now handle the specifics
        if( meter == "TEXT" )
        {

          TextLabel *tmp = new TextLabel(this, x, y, w, h );
          tmp->setTextProps( tmpText );
          tmp->setValue( getString( "VALUE", line ) );

          QString name = getString( "NAME", line);
          if (name != "")
            tmp->setName(name.ascii());

          setSensor( line, (Meter*)tmp );
          meterList->append ( tmp );
        }

        if(meter == "CLICKMAP")
        {
          if( !hasMouseTracking() )
            setMouseTracking(true);
          ClickMap *tmp = new ClickMap(this, x, y, w, h);
          tmp->setTextProps( tmpText );

          setSensor( line, (Meter*) tmp );
          // set all params
          clickList -> append(tmp);
          meterList->append( tmp );

        }

        if( meter == "RICHTEXT" )
        {
          RichTextLabel *tmp = new RichTextLabel(this, x, y, w, h);

          bool dUl = getBoolean( "UNDERLINE", line, paramFound );
          if( !paramFound )
            dUl = false;

          tmp->setText( getString( "VALUE", line ), dUl );
          tmp->setTextProps( tmpText );
          tmp->setWidth(w);
          tmp->setHeight(h);

          QString name = getString( "NAME", line);
          if (name != "")
            tmp->setName(name.ascii());

          setSensor( line, (Meter*)tmp );
          clickList -> append(tmp);
          meterList->append ( tmp );
        }
      }

      if(meter == "BAR")
      {
        Bar *tmp = new Bar(this, x, y, w, h );
        tmp->setImage( getString( "PATH", line ) );
        tmp->setVertical( getBoolean( "VERTICAL", line ) );
        bool maxFound = false;
        int max = getInt( "MAX", line, maxFound );
        if( maxFound )
          tmp->setMax( max );
        bool minFound = false;
        int min = getInt( "MIN", line, minFound );
        if( minFound )
          tmp->setMin( min );
        tmp->setValue( getInt( "VALUE", line ) );
        QString name = getString("NAME",line);
        if (name != "")
          tmp->setName(name.ascii());
        setSensor( line, (Meter*)tmp );
        meterList->append ( tmp );
      }

      if(meter == "GRAPH")
      {
        int r, g, b;
        int points = getInt("POINTS",line);
        get3Int("COLOR", line, r, g, b);

        Graph *tmp = new Graph(this, x, y, w, h, points );
        bool maxFound = false;
        int max = getInt( "MAX", line, maxFound );
        if( maxFound )
          tmp->setMax( max );
        bool minFound = false;
        int min = getInt( "MIN", line, minFound );
        if( minFound )
          tmp->setMin( min );
        QString name = getString("NAME",line);
        if (name != "")
          tmp->setName(name.ascii());

        tmp->setColor( r , g, b );

        setSensor( line, (Graph*)tmp );
        meterList->append ( tmp );
      }
    }
    delete t;
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
    connect(m_sysTimer, SIGNAL(timeout()), SLOT(step()));

    interval = interval == 0 ? 1000 : interval;
    m_sysTimer->start(interval);

    //Start the widget running
    QTimer::singleShot( 0, this, SLOT(step()) );

    if( !(onTop || managed) )
      lowerTimer.start();

    return true;
  }
}

void karamba::startNewKaramba()
{
  //qDebug("karamba::startNewKaramba");
  QStringList fileNames;
  fileNames = KFileDialog::getOpenFileNames(QString::null,
                                            "*.theme *.ctheme *.skz|Themes",
                                            0, "Open Themes");
  for ( QStringList::Iterator it = fileNames.begin();
        it != fileNames.end();
        ++it )
  {
    QFileInfo file( *it );
    if( file.exists() )
    {
      (new karamba( *it, false ))->show();
    }
  }
}

void karamba::popupNotify(int)
{
  //qDebug("karamba::popupNotify");
}

void karamba::reloadConfig()
{
  //qDebug("karamba::reloadConfig: %s", themeFile.ascii());
  writeConfigData();
  QFileInfo file( themeFile );
  if( file.exists() )
  {
    (new karamba( themeFile, true ))->show();
  }
  killWidget();
}

void karamba::killWidget()
{
  //qDebug("karamba::killWidget");
  close();
}

void karamba::initPythonInterface()
{
  pythonIface = new KarambaPython(themePath, themeName, m_reloading);
}

void karamba::quitKaramba()
{
  //qDebug("karamba::quitKaramba.");
  qApp -> closeAllWindows();
  qApp -> quit();
}

void karamba::editConfig()
{
  //qDebug("karamba::editConfig");
  QFileInfo fileInfo( themeFile );
  QString path;

  if( fileInfo.isRelative() )
  {
    path = themePath + "/" + themeFile;
  }
  else
  {
    path = themeFile;
  }

  KRun::runURL( KURL( path ), "text/plain" );
}

void karamba::editScript()
{
  //qDebug("karamba::editScript");
  QFileInfo fileInfo( themeFile );
  QString path;

  if( fileInfo.isRelative() )
  {
      path = themePath + "/" + themeName + ".py";
  }
  else
  {
      path = QFileInfo(themeFile).dirPath() + "/" + themeName + ".py";
  }
  KRun::runURL( KURL( path ), "text/plain" );
}

int karamba::getInt(QString w, QString &line )
{
  //qDebug("karamba::getInt");
  bool b;
  return getInt( w, line, b );
}

int karamba::getInt(QString w, QString &line, bool &found )
{
  //qDebug("karamba::getInt (2)");
  QRegExp rx( "\\W+" + w +"=([-]?\\d+)", false );
  found = (rx.search(line)==-1)?false:true;
  return rx.cap(1).toInt();
}

void karamba::get3Int(QString w, QString &line, int &a, int &b, int &c )
{
  //qDebug("karamba::get3Int");
  bool bo;
  get3Int ( w, line, a, b, c, bo );
}

void karamba::get3Int(QString w, QString &line, int &a, int &b, int &c, bool &found )
{
  //qDebug("karamba::get3Int (2)");
  QRegExp rx( "\\W+" + w + "=([-]?\\d+),([-]?\\d+),([-]?\\d+)", false );
  found = (rx.search(line)==-1)?false:true;
  a = rx.cap(1).toInt();
  b = rx.cap(2).toInt();
  c = rx.cap(3).toInt();
}

QString karamba::getString(QString w, QString &line)
{
  //qDebug("karamba::getString");
  bool b;
  return getString ( w, line, b );
}

QString karamba::getString(QString w, QString &line, bool &found)
{
  //qDebug("karamba::getString (2)");
  QRegExp rx( "\\W+" + w + "=\"([^\"]*)\"", false );
  found = (rx.search(line)==-1)?false:true;
  if (rx.cap(1).isEmpty())
  {
    rx = QRegExp(w + "=(\\S+)", false);
    found = (rx.search(line)==-1)?false:true;
    return rx.cap(1);
  }
  else
  {
    return rx.cap(1);
  }
}

bool karamba::getBoolean( QString w, QString &line )
{
  //qDebug("karamba::getBoolean");
  bool b;
  return getBoolean ( w, line, b );
}

bool karamba::getBoolean( QString w, QString &line, bool &found )
{
  //qDebug("karamba::getBoolean (2)");
  return  ( getString( w, line, found ).lower() == "true")?true:false;
}

QString karamba::findSensorFromMap(Sensor* sensor)
{
  //qDebug("karamba::findSensorFromMap");
  QMap<QString,Sensor*>::Iterator it;

  for ( it = sensorMap.begin(); it != sensorMap.end(); ++it )
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

void karamba::setSensor(QString &line, Meter* meter)
{
  //qDebug("karamba::setSensor");
  Sensor* sensor = 0;

  deleteMeterFromSensors(meter);

  QString sens = getString("SENSOR",line).upper();

  if( sens == "CPU" )
  {
    QString cpuNbr = getString( "CPU", line );
    sensor = sensorMap["CPU"+cpuNbr];
    if (sensor == 0)
    {
      int interval = getInt("INTERVAL",line);
      interval = (interval == 0)?1000:interval;
      sensor = ( sensorMap["CPU"+cpuNbr] = new CPUSensor( cpuNbr, interval ) );
      sensorList->append( sensor );
    }
    SensorParams *sp = new SensorParams(meter);
    sp->addParam("FORMAT",getString("FORMAT",line));
    sp->addParam("DECIMALS",getString("DECIMALS",line));

    sensor->addMeter(sp);
    sensor->setMaxValue(sp);

  }

  if( sens == "MEMORY" )
  {
    sensor = sensorMap["MEMORY"];
    if (sensor == 0)
    {
      int interval = getInt("INTERVAL",line);
      interval = (interval == 0)?3000:interval;
      sensor = ( sensorMap["MEMORY"] = new MemSensor( interval ) );
      sensorList->append( sensor );
    }
    SensorParams *sp = new SensorParams(meter);
    sp->addParam("FORMAT",getString("FORMAT",line));

    sensor->addMeter(sp);
    sensor->setMaxValue(sp);
  }


  if( sens == "DISK" )
  {
    sensor = sensorMap["DISK"];
    if (sensor == 0)
    {
      int interval = getInt("INTERVAL",line);
      interval = (interval == 0)?5000:interval;
      sensor = ( sensorMap["DISK"] = new DiskSensor( interval ) );
      connect( sensor, SIGNAL(initComplete()), this, SLOT(externalStep()) );
      sensorList->append( sensor );
    }
    // meter->setMax( ((DiskSensor*)sensor)->getTotalSpace(mntPt)/1024 );
    SensorParams *sp = new SensorParams(meter);
    QString mntPt = getString("MOUNTPOINT",line);
    if( mntPt == ""  )
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
    sp->addParam("FORMAT",getString("FORMAT",line));
    sensor->addMeter(sp);
    sensor->setMaxValue(sp);
  }

  if( sens == "NETWORK")
  {
    int interval = getInt("INTERVAL",line );
    interval = (interval == 0)?2000:interval;
    QString device = getString("DEVICE", line );
    sensor = sensorMap["NETWORK"+device];
    if (sensor == 0)
    {
      sensor = ( sensorMap["NETWORK"+device] = new NetworkSensor( device, interval ) );
      sensorList->append( sensor );
    }
    SensorParams *sp = new SensorParams(meter);
    sp->addParam( "FORMAT", getString( "FORMAT", line ) );
    sp->addParam( "DECIMALS", getString( "DECIMALS", line ) );
    sensor->addMeter(sp);
  }

  if( sens == "UPTIME" )
  {
    sensor = sensorMap["UPTIME"];
    if (sensor == 0)
    {
      int interval = getInt("INTERVAL",line);
      interval = (interval == 0)?60000:interval;
      sensor = ( sensorMap["UPTIME"] = new UptimeSensor( interval ));
      sensorList->append( sensor );

    }
    SensorParams *sp = new SensorParams(meter);
    sp->addParam( "FORMAT", getString( "FORMAT", line ) );
    sensor->addMeter(sp);
  }

  if( sens == "SENSOR" )
  {
    sensor = sensorMap["SENSOR"];
    if (sensor == 0)
    {
      int interval = getInt("INTERVAL",line);
      interval = (interval == 0)?30000:interval;
      sensor = (sensorMap["SENSOR"] = new SensorSensor(interval, tempUnit));
      sensorList->append( sensor );
    }
    SensorParams *sp = new SensorParams(meter);
    sp->addParam("FORMAT",getString("FORMAT",line));
    sp->addParam("TYPE",getString("TYPE",line));
    sensor->addMeter(sp);
  }


  if( sens == "TEXTFILE" )
  {
    QString path = getString( "PATH", line );
    bool rdf = getBoolean( "RDF", line );
    sensor = sensorMap["TEXTFILE"+path];
    if (sensor == 0)
    {
      int interval = getInt( "INTERVAL", line );
      interval = ( interval == 0 )?60000:interval;
      QString encoding = getString( "ENCODING", line );

      sensor = ( sensorMap["TEXTFILE"+path] =
                   new TextFileSensor( path, rdf, interval, encoding ) );
      sensorList->append( sensor );
    }
    SensorParams *sp = new SensorParams(meter);
    sp->addParam("LINE",QString::number(getInt("LINE",line)));
    sensor->addMeter(sp);
  }


  if( sens == "TIME")
  {
    sensor = sensorMap["DATE"];
    if (sensor == 0)
    {
      int interval = getInt("INTERVAL",line);
      interval = (interval == 0)?60000:interval;
      sensor = ( sensorMap["DATE"] = new DateSensor( interval ) );
      sensorList->append( sensor );
      timeList->append( sensor );
    }
    SensorParams *sp = new SensorParams(meter);
    sp->addParam("FORMAT",getString("FORMAT",line));
    sp->addParam("CALWIDTH",getString("CALWIDTH",line));
    sp->addParam("CALHEIGHT",getString("CALHEIGHT",line));
    sensor->addMeter(sp);
  }

#ifdef HAVE_XMMS

  if( sens == "XMMS" )
  {
    sensor = sensorMap["XMMS"];
    if (sensor == 0)
    {
      int interval = getInt("INTERVAL",line);
      interval = (interval == 0)?1000:interval;
      QString encoding = getString( "ENCODING", line );

      sensor = ( sensorMap["XMMS"] = new XMMSSensor( interval, encoding ) );
      sensorList->append( sensor );
    }
    SensorParams *sp = new SensorParams(meter);
    sp->addParam("FORMAT",getString("FORMAT",line));
    sensor->addMeter(sp);
    sensor->setMaxValue(sp);
  }
#endif // HAVE_XMMS


  if( sens == "NOATUN" )
  {
    sensor = sensorMap["NOATUN"];
    if (sensor == 0)
    {
      int interval = getInt("INTERVAL",line);
      interval = (interval == 0)?1000:interval;
      sensor = ( sensorMap["NOATUN"] = new NoatunSensor( interval, client ) );
      sensorList->append( sensor );
    }
    SensorParams *sp = new SensorParams(meter);
    sp->addParam("FORMAT",getString("FORMAT",line));
    sensor->addMeter(sp);
    sensor->setMaxValue(sp);
  }

  if( sens == "PROGRAM")
  {
    QString progName = getString("PROGRAM",line);
    sensor = sensorMap["PROGRAM"+progName];
    if (sensor == 0)
    {
      int interval = getInt("INTERVAL",line);
      interval = (interval == 0)?3600000:interval;
      QString encoding = getString( "ENCODING", line );

      sensor = (sensorMap["PROGRAM"+progName] =
                  new ProgramSensor( progName, interval, encoding ) );
      sensorList->append( sensor );
    }
    SensorParams *sp = new SensorParams(meter);
    sp->addParam( "LINE", QString::number( getInt( "LINE", line ) ) );
    sp->addParam( "THEMAPATH", themePath );
    sensor->addMeter(sp);
  }

  if( sens == "RSS" )
  {
    QString source = getString( "SOURCE", line );
    QString format = getString( "FORMAT", line );

    sensor = sensorMap["RSS"+source];
    if (sensor == 0)
    {
      int interval = getInt( "INTERVAL", line );
      interval = ( interval == 0 )?60000:interval;
      QString encoding = getString( "ENCODING", line );

      sensor = ( sensorMap["RSS"+source] =
                   new RssSensor( source, interval, format, encoding ) );
      sensorList->append( sensor );
    }
    SensorParams *sp = new SensorParams(meter);
    sp->addParam("SOURCE",getString("SOURCE",line));
    sensor->addMeter(sp);
  }

  if (sensor != 0)
  {
    QTimer::singleShot( 0, sensor, SLOT(update()) );
    sensor->start();
  }
}

void karamba::passMenuOptionChanged(QString key, bool value)
{
  //Everything below is to call the python callback function
  if (pythonIface && pythonIface->isExtensionLoaded())
    pythonIface->menuOptionChanged(this, key, value);
}

void karamba::meterClicked(QMouseEvent* e, Meter* meter)
{
  //qWarning("karamba::meterClicked");
  if (pythonIface->isExtensionLoaded() && haveUpdated)
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
      pythonIface->meterClicked(this, richText->anchorAt(e->x(), e->y()), button);
    }
    else
    {
      pythonIface->meterClicked(this, meter, button);
    }
  }
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
  if (pythonIface->isExtensionLoaded() && haveUpdated)
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
  if (pythonIface->isExtensionLoaded() && haveUpdated)
  {
    int button = 0;

    if( e->delta() > 0 )
      button = 4;
    else
      button = 5;

    pythonIface->widgetClicked(this, e->x(), e->y(), button);
  }
}

void karamba::mousePressEvent( QMouseEvent *e )
{
  //qDebug("karamba::mousePressEvent");
  if( e->button() == RightButton )
  {
    kpop->exec(QCursor::pos());
  }
  else
  {
    clickPos = e->pos();
    if( toggleLocked -> isChecked() )
      passClick( e );
    if( !(onTop || managed))
      lower();
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
  //Everything below is to call the python callback function
  if (pythonIface && pythonIface->isExtensionLoaded())
    pythonIface->keyPressed(this, e->text());
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
                insideArea = ((Meter*)(*it)) -> insideActiveArea(e -> x(), e -> y());
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
  //  this->close(true);
  //  delete this;
}

void karamba::paintEvent ( QPaintEvent *e)
{
  //qDebug("karamba::paintEvent");
  if( !(onTop || managed))
  {
    if( lowerTimer.elapsed() > 100 )
    {
      this->lower();
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
  //qDebug("karamba::updateBackground");
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
    ((ImageLabel*) *it)->mUpdate(&p, 1);
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
  //qDebug("karamba::externalStep");
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
  //qDebug("karamba::step");
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
  //qWarning("Config File ~/.superkaramba/%s.rc written.", themeName.ascii());
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
                                               menuAccColl, key);
  confItem -> setName(key);

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
  KToggleAction* menuAction = ((KToggleAction*)menuAccColl -> action(key));
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
  KToggleAction* menuAction = ((KToggleAction*)menuAccColl -> action(key));
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

void  karamba::processExited (KProcess*)
{
  //qDebug("karamba::processExited");
  // Nothing needed here at the moment.
  // Could add a "commandFinished" callback here if it becomes
  // needed at some point.
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
  config->writeEntry("theme", themeFile);
  writeConfigData();
}

//For KDE session management
void karamba::readProperties(KConfig* config)
{
  //qDebug("karamba::readProperties");
  config->setGroup("session");
  QString atheme = config->readEntry("theme","");
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
      pythonIface->itemDropped(this, text.ascii());
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

//Eventually maybe add a theme browser here
void karamba::downloadThemes()
{
  //qDebug("karamba::downloadThemes");
  //Go to SK Themes webpage for now
  KRun::runURL( KURL( "http://www.superkaramba.com/" ), "text/html" );
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
  : QObject(parent, name), collection(ac)
{
  setName(name);
}

void SignalBridge::receive()
{
  emit enabled(name(), ((KToggleAction*)collection -> action(name())) -> isChecked());
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

#include "karamba.moc"
