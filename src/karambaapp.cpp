/***************************************************************************
 *   Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>              *
 *   Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#include <qstring.h>
#include <qstringlist.h>
#include <qdir.h>
#include <kfiledialog.h>
#include <kcmdlineargs.h>
#include <fcntl.h>
#include <klocale.h>
#include <kmessagebox.h>

#include <qtooltip.h>

#include "themesdlg.h"
#include "karambainterface.h"
#include "karambaapp.h"
#include "dcopinterface_stub.h"
#include "karamba.h"
#include "superkarambasettings.h"

int KarambaApplication::fd = -1;

KarambaApplication::KarambaApplication() :
    iface(0), themeListWindow(0), dcopIfaceStub(0), sysTrayIcon(0)
{
  iface = new KarambaIface();
  karambaList = new QObjectList();
  // register ourselves as a dcop client
  dcopClient()->registerAs(name());
  dcopClient()->setDefaultObject(dcopIface()->objId());
}

KarambaApplication::~KarambaApplication()
{
  delete iface;
  delete karambaList;
  delete themeListWindow;
  delete dcopIfaceStub;
}

void KarambaApplication::initDcopStub(QCString app)
{
  if(app.isEmpty())
    app = dcopClient()->appId();
  dcopIfaceStub = new dcopIface_stub(app, iface->objId());
}

QString KarambaApplication::getMainKaramba()
{
  QStringList karambas = getKarambas();
  QStringList::Iterator it;

  for (it = karambas.begin(); it != karambas.end(); ++it)
  {
    if((*it).ascii() == dcopClient()->appId())
      continue;
    dcopIface_stub dcop((*it).ascii(), iface->objId());
    if (dcop.isMainKaramba())
      return *it;
  }
  return QString::null;
}

void KarambaApplication::setupKaramba(QCString app)
{
  if(app.isEmpty())
    return;

  dcopIface_stub dcop(app, iface->objId());

  dcop.hideSystemTray(false);
}

QStringList KarambaApplication::getKarambas()
{
  QCStringList applst = dcopClient()->registeredApplications();
  QCStringList::Iterator it;
  QCString s;
  QStringList result;

  for (it = applst.begin(); (s = *it) != 0; ++it)
  {
    if (s.left(strlen(name())) == name())
      result.append(s);
  }
  return result;
}

void KarambaApplication::checkSuperKarambaDir()
{
  // Create ~/.superkaramba if necessary
  QDir configDir(QDir::home().absPath() + "/.superkaramba");
  if (!configDir.exists())
  {
    qWarning("~/.superkaramba doesn't exist");
    if(!configDir.mkdir(QDir::home().absPath() + "/.superkaramba"))
    {
        qWarning("Couldn't create Directory ~/.superkaramba");
    }
    else
    {
        qWarning("created ~/.superkaramba");
    }
  }
}

void KarambaApplication::setUpSysTray()
{
  //Create theme list window.
  //This will function as the main window for the tray icon
  themeListWindow = new ThemesDlg();

  //Set up systray icon
  sysTrayIcon = new KSystemTray(themeListWindow);

  KPopupMenu *menu = sysTrayIcon->contextMenu();
  menu->insertItem(SmallIconSet("superkaramba"),
                   i18n("Hide System Tray Icon"), this,
                   SLOT(hideSysTray()));

  sysTrayIcon->setPixmap(sysTrayIcon->loadIcon("superkaramba"));
  setToolTip();

  bool showSysTrayIcon = SuperKarambaSettings::showSysTray();

  if(showSysTrayIcon)
  {
    sysTrayIcon->show();
  }

  //Connect Systray icon's quit event
  QObject::connect(sysTrayIcon, SIGNAL(quitSelected()),
                   this, SLOT(quitSuperKaramba()));
}

void KarambaApplication::hideSysTray(bool hide)
{
  if(hide)
  {
    KMessageBox::information(0,
        i18n("<qt>Hiding the system tray icon will keep SuperKaramba running "
             "in background. To show it again use the theme menu.</qt>"),
             i18n("Hiding System Tray Icon"), "hideIcon");

    showKarambaMenuExtension();
    sysTrayIcon->hide();
  }
  else
  {
    showKarambaMenuExtension(false);
    sysTrayIcon->show();
  }
  SuperKarambaSettings::setShowSysTray(!hide);
  SuperKarambaSettings::writeConfig();
}

void KarambaApplication::showKarambaMenuExtension(bool show)
{
  QObject *k;

  if(show)
  {
    for (k = karambaList->first(); k; k = karambaList->next())
    {
      ((karamba*)k)->showMenuExtension();
    }
  }
  else
  {
    for (k = karambaList->first(); k; k = karambaList->next())
    {
      ((karamba*)k)->hideMenuExtension();
    }
  }
}

void KarambaApplication::setToolTip(const QString &tip)
{
  QToolTip::remove(sysTrayIcon);
  if(tip.isNull())
    QToolTip::add(sysTrayIcon, i18n("SuperKaramba"));
  else
    QToolTip::add(sysTrayIcon, tip);
}

void KarambaApplication::buildToolTip()
{
  if(karambaList->isEmpty())
  {
    setToolTip();
    return;
  }
  
  QString toolTip("<b><center>" + i18n("SuperKaramba") + "</center></b>");

  toolTip += "<table width=300>";
  
  bool firstRun = true;
  QObject *k;
  for (k = karambaList->first(); k; k = karambaList->next())
  {
    if(firstRun) 
    {
      toolTip += "<tr><td align=right>" + i18n("Running Theme:", "Running Themes:", karambaList->count()) + 
        "</td><td align=left>" + ((karamba*)k)->theme().name() + "</td></tr>";
      firstRun = false;
    }
    else
    {
      toolTip += "<tr><td></td><td align=left>" + ((karamba*)k)->theme().name() + "</td></tr>";
    }
  }

  toolTip += "</table>";
  
  setToolTip(toolTip);
}

bool KarambaApplication::sysTrayIconShown()
{
  if(sysTrayIcon)
    return sysTrayIcon->isVisible();
  return false;
}

void KarambaApplication::checkPreviousSession(KApplication &app, QStringList &lst)
{
  /******
  Try to restore a previous session if applicable.
  */
  if (app.isSessionRestored())
  {
    KConfig* config = app.sessionConfig();
    config->setGroup("General Options");
    QString restartThemes = config->readEntry("OpenThemes","");

    //Get themes that were running
    lst = QStringList::split(QString(";"), restartThemes);
  }
}

void KarambaApplication::checkCommandLine(KCmdLineArgs *args, QStringList &lst)
{
  /******
    Not a saved session - check for themes given on command line
  */
  if(args->count() > 0)
  {
    for(int i = 0; i < (args->count()); i++)
    {
      if( args->arg(i) != "" )
      {
        KURL url = args->url(i);

        lst.push_back(url.path());
      }
    }
  }
}

void KarambaApplication::showWelcomeDialog()
{
  //No themes given on command line and no saved session.
  //Show welcome dialog.

  if(themeListWindow)
    themeListWindow->show();
}

bool KarambaApplication::startThemes(QStringList &lst)
{
  bool result = false;

  for(QStringList::Iterator it = lst.begin(); it != lst.end(); ++it )
  {
    karamba *mainWin = 0;

    mainWin = new karamba(*it , false);
    mainWin->show();
    result = true;
  }
  
  buildToolTip();
  
  return result;
}

void KarambaApplication::addKaramba(karamba* k, bool reloading)
{
  if(!reloading && karambaApp->dcopStub())
  {
    int instance = karambaApp->dcopStub()->themeAdded(
        karambaApp->dcopClient()->appId(), k->theme().file());
    k->setInstance(instance);
  }
  karambaList->append(k);
  
  buildToolTip();
}

void KarambaApplication::deleteKaramba(karamba* k, bool reloading)
{
  if(!reloading && karambaApp->dcopStub())
    karambaApp->dcopStub()->themeClosed(
        karambaApp->dcopClient()->appId(), k->theme().file(), k->instance());
  karambaList->removeRef(k);
  
  buildToolTip();
}

bool KarambaApplication::hasKaramba(karamba* k)
{
  return karambaList->containsRef(k) > 0;
}

// XXX: I guess this should be made with mutex/semaphores
// but this is good for now...

bool KarambaApplication::lockKaramba()
{
  QString file = QDir::home().absPath() + "/.superkaramba/.lock";

  fd = open(file.ascii(), O_CREAT | O_RDWR);
  if (fd < 0)
  {
    qWarning("Open failed in lock.");
    return false;
  }
  //qDebug("lock %d", getpid());
  if(lockf(fd, F_LOCK, 0))
  {
    qWarning("Lock failed.");
    return false;
  }
  return true;
}

void KarambaApplication::unlockKaramba()
{
  if(fd > 0)
  {
    lockf(fd, F_ULOCK, 0);
    //qDebug("Unlock %d", getpid());
    close(fd);
    fd = -1;
  }
}

void KarambaApplication::quitSuperKaramba()
{
  //qDebug("quitSuperKaramba called");

  themeListWindow->quitSuperKaramba();
}

#include "karambaapp.moc"
