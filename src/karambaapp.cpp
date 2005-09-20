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
#include <kdebug.h>
#include <khelpmenu.h>

#include <qtooltip.h>
#include <QByteArray>
#include <QtAlgorithms>

#include "themesdlg.h"
#include "karambainterface.h"
#include "karambaapp.h"
#include "dcopinterface_stub.h"
#include "karambawidget.h"
#include "karamba_python.h"
#include "superkarambasettings.h"

int KarambaApplication::fd = -1;

KarambaApplication::KarambaApplication()
    :   KUniqueApplication(),
        m_helpMenu(0),
        iface(0),
        themeListWindow(0),
        dcopIfaceStub(0),
        sysTrayIcon(0)
{
    // god get rid of this shit
    iface = new KarambaIface();
    // register ourselves as a dcop client
    dcopClient()->registerAs(name());
    dcopClient()->setDefaultObject(dcopIface()->objId());

// JESUS
    QStringList lst;
    QString mainAppId = getMainKaramba();
    if(!mainAppId.isEmpty())
    {
        initDcopStub(mainAppId.toAscii());
    }
    else
    {
        //Set up systray icon
        setUpSysTray(/*&about*/ 0);
        initDcopStub();
    }

    // Try to restore a previous session if applicable.
    if (isSessionRestored())
    {
        KConfig* config = sessionConfig();
        config->setGroup("General Options");
        QString restartThemes = config->readEntry("OpenThemes","");

        //Get themes that were running
        lst = QStringList::split(QString(";"), restartThemes);
    }


    if(lst.size() == 0)
    {
/*
        //Not a saved session - check for themes given on command line
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
*/

  //      app.checkCommandLine(args, lst);

        if(lst.size() == 0)
        {
            //No themes given on command line and no saved session.
            //Show welcome dialog.
            showThemeDialog();
        }
    }

//    args->clear();

    //qDebug("startThemes");
    if (!lst.isEmpty())
    {
        startThemes(lst);
    }

// End JESUS

    KarambaPython::initPython();
    connect( this, SIGNAL(lastWindowClosed()), SLOT(quit()));
}

KarambaApplication::~KarambaApplication()
{
    KarambaPython::shutdownPython();

    qDeleteAll(m_karambas);
    m_karambas.clear();

    delete iface;
    delete themeListWindow;
    delete dcopIfaceStub;
    //delete m_helpMenu;
}

void KarambaApplication::initDcopStub(QByteArray app)
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
    DCOPCStringList applst = dcopClient()->registeredApplications();
    DCOPCStringList::Iterator it;
    DCOPCString s;
    QStringList result;

    foreach (DCOPCString s, applst)
    {
        if (s.left(strlen(name())) == name())
        {
            result.append(s);

        }
    }
    return result;
}

void KarambaApplication::setUpSysTray(KAboutData* about)
{
    //kdDebug() << k_funcinfo << endl;
    KAction* action;

    //Create theme list window.
    //This will function as the main window for the tray icon
    themeListWindow = new ThemesDlg();

    //Set up systray icon
    sysTrayIcon = new KSystemTray(themeListWindow);

    KPopupMenu *menu = sysTrayIcon->contextMenu();
    menu->insertItem(SmallIconSet("superkaramba"),
                     i18n("Hide System Tray Icon"), this,
                     SLOT(globalHideSysTray()));
    menu->insertSeparator();

    m_helpMenu = new KHelpMenu(themeListWindow, about);
    action = KStdAction::help(m_helpMenu, SLOT(appHelpActivated()),
                              sysTrayIcon->actionCollection());
    action->plug(menu);
    action = KStdAction::aboutApp(m_helpMenu, SLOT(aboutApplication()),
                                  sysTrayIcon->actionCollection());
    action->plug(menu);
    action = KStdAction::aboutKDE(m_helpMenu, SLOT(aboutKDE()),
                                  sysTrayIcon->actionCollection());
    action->plug(menu);

    sysTrayIcon->setPixmap(sysTrayIcon->loadIcon("superkaramba"));
    setToolTip();

    if(SuperKarambaSettings::showSysTray())
        sysTrayIcon->show();
    else
        sysTrayIcon->hide();

    //Connect Systray icon's quit event
    QObject::connect(sysTrayIcon, SIGNAL(quitSelected()),
                     this, SLOT(globalQuitSuperKaramba()));
}

void KarambaApplication::showKarambaMenuExtension(bool show)
{
    if(show)
    {
        foreach (QObject *k, m_karambas)
        {
            ((KarambaWidget*)k)->showMenuExtension();
        }
    }
    else
    {
        foreach (QObject *k, m_karambas)
        {
            ((KarambaWidget*)k)->hideMenuExtension();
        }
    }
}

void KarambaApplication::setToolTip(const QString &tip)
{
    QToolTip::remove
        (sysTrayIcon);
    if(tip.isNull())
        QToolTip::add
            (sysTrayIcon, i18n("SuperKaramba"));
    else
        QToolTip::add
            (sysTrayIcon, tip);
}

void KarambaApplication::buildToolTip()
{
    if(!sysTrayIcon || !themeListWindow)
        return;

    QStringList list = themeListWindow->runningThemes();

    if(list.isEmpty())
    {
        setToolTip();
        return;
    }

    QString toolTip("<b><center>" + i18n("SuperKaramba") + "</center></b>");
    toolTip += "<table width=300>";

    bool firstRun = true;
    for(QStringList::Iterator it = list.begin(); it != list.end(); ++it )
    {
        if(firstRun)
        {
            toolTip +=
                "<tr><td align=right>" +
                i18n("1 Running Theme:", "%n Running Themes:", list.count()) +
                "</td><td align=left>" + (*it) + "</td></tr>";
            firstRun = false;
        }
        else
        {
            toolTip += "<tr><td></td><td align=left>" + (*it) + "</td></tr>";
        }
    }

    toolTip += "</table>";

    setToolTip(toolTip);
}

bool KarambaApplication::startThemes(QStringList &lst)
{
    bool result = false;

    for(QStringList::Iterator it = lst.begin(); it != lst.end(); ++it )
    {
        KarambaWidget *mainWin = 0;

        mainWin = new KarambaWidget(*it , false);
        mainWin->show();
        result = true;
    }

    buildToolTip();
    return result;
}

void KarambaApplication::addKaramba(KarambaWidget* k, bool reloading)
{
    if(!reloading && karambaApp->dcopStub())
    {
        int instance = karambaApp->dcopStub()->themeAdded(
                           karambaApp->dcopClient()->appId(), k->theme().file());
        k->setInstance(instance);
    }
    m_karambas.append(k);
}

void KarambaApplication::deleteKaramba(KarambaWidget* k, bool reloading)
{
    if(!reloading && karambaApp->dcopStub())
        karambaApp->dcopStub()->themeClosed(
            karambaApp->dcopClient()->appId(), k->theme().file(), k->instance());
    m_karambas.removeAll(k);
}

bool KarambaApplication::hasKaramba(KarambaWidget* k)
{
    return m_karambas.count(k) > 0;
}

void KarambaApplication::hideSysTray(bool hide)
{
    //kdDebug() << k_funcinfo << endl;
    if(hide)
    {
        if(sysTrayIcon)
        {
            KMessageBox::information(0,
                                     i18n("<qt>Hiding the system tray icon will keep SuperKaramba running "
                                          "in background. To show it again use the theme menu.</qt>"),
                                     i18n("Hiding System Tray Icon"), "hideIcon");
            sysTrayIcon->hide();
        }
        showKarambaMenuExtension();
    }
    else
    {
        showKarambaMenuExtension(false);
        if(sysTrayIcon)
            sysTrayIcon->show();
    }
}

void KarambaApplication::showThemeDialog()
{
    //kdDebug() << k_funcinfo << endl;
    if(themeListWindow)
        themeListWindow->show();
}

void KarambaApplication::quitSuperKaramba()
{
    if(themeListWindow)
    {
        themeListWindow->saveUserAddedThemes();
    }

    closeAllWindows();
    quit();
}

void KarambaApplication::globalQuitSuperKaramba()
{
    QStringList apps = getKarambas();
    QStringList::Iterator it;

    for (it = apps.begin(); it != apps.end(); ++it)
    {
        dcopIface_stub dcop((*it).ascii(), dcopIface()->objId());
        dcop.quit();
    }
}

void KarambaApplication::globalHideSysTray(bool hide)
{
    //kdDebug() << k_funcinfo << endl;
    QStringList apps = getKarambas();
    QStringList::Iterator it;

    SuperKarambaSettings::setShowSysTray(!hide);
    SuperKarambaSettings::writeConfig();

    for (it = apps.begin(); it != apps.end(); ++it)
    {
        dcopIface_stub dcop((*it).ascii(), dcopIface()->objId());
        dcop.hideSystemTray(hide);
    }
}

#include "karambaapp.moc"
