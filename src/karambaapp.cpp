/***************************************************************************
 *   Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>              *
 *   Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "karambaapp.h"
#include "dcopinterface_stub.h"
#include "karamba.h"
#include "qstring.h"
#include <ksystemtray.h>
#include <qstringlist.h>
#include "karambainterface.h"
#include <qdir.h>
#include "welcomeform.h"
#include "themelistwindow.h"
#include <kfiledialog.h>
#include <kcmdlineargs.h>
#include <fcntl.h>
#include <klocale.h>

#include "skicon.xpm"

int KarambaApplication::fd = -1;

KarambaApplication::KarambaApplication() :
  iface(0), themeListWindow(0), dcopIfaceStub(0)
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
  themeListWindow = new ThemeListWindow();

  //Set up systray icon
  KSystemTray* sysTrayIcon = new KSystemTray(themeListWindow);
  sysTrayIcon->setPixmap( QPixmap(skicon_xpm));
#if defined(KDE_MAKE_VERSION)
# if KDE_VERSION >= KDE_MAKE_VERSION(3,1,9)
  sysTrayIcon->contextMenu()->insertItem(i18n("Open Theme..."),
      themeListWindow, SLOT(openTheme()));
# endif
#endif
  sysTrayIcon->show();

  //Connect Systray icon's quit event
  QObject::connect(sysTrayIcon, SIGNAL( quitSelected() ), themeListWindow,
                  SLOT(quitSuperKaramba()) );
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
          lst.push_back(args->arg(i));
      }
    }
}

void KarambaApplication::showWelcomeDialog(QStringList &lst)
{
  //No themes given on command line and no saved session.
  //Show welcome dialog.

  WelcomeForm* wcmFrm = new WelcomeForm(themeListWindow,0,true,0);
  if (wcmFrm->exec() == QDialog::Accepted)
  {
    lst = KFileDialog::getOpenFileNames(QString::null,
                                        i18n("*.theme *.ctheme *.skz|Themes"),
                                        0,
                                        i18n("Open Themes"));
  }
}

bool KarambaApplication::startThemes(QStringList &lst)
{
  bool result = false;

  for(QStringList::Iterator it = lst.begin(); it != lst.end(); ++it )
  {
    QFileInfo file( *it );
    if(file.exists() && !file.isDir())
    {
      karamba *mainWin = 0;

      mainWin = new karamba(*it , false);
      mainWin->show();
      result = true;
    }
    else
      qWarning("%s theme not found.", (*it).ascii());
  }
  return result;
}

void KarambaApplication::addKaramba(karamba* k, const QString& themeName)
{
  if(karambaApp->dcopStub())
    karambaApp->dcopStub()->themeAdded(
      karambaApp->dcopClient()->appId(), themeName);
  karambaList->append(k);
}

void KarambaApplication::deleteKaramba(karamba* k, const QString& themeName)
{
  if(karambaApp->dcopStub())
    karambaApp->dcopStub()->themeClosed(
      karambaApp->dcopClient()->appId(), themeName);
  karambaList->removeRef(k);
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

