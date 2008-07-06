/***************************************************************************
*   Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>              *
*   Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>                 *
*   Copyright (C) 2007 Alexander Wiedenbruch <mail@wiedenbruch.de>        *
*   Copyright (C) 2007 Matt Broadstone <mbroadst@gmail.com>               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/
#include "karambaapp.h"
#include "karambaapp.moc"

#include "superkarambasettings.h"
#include "mainwidget.h"
#include "themesdlg.h"
#include "karamba.h"
#include "superkarambaadaptor.h"
#include "karambamanager.h"

#include <QDesktopWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMenu>
#include <QDBusConnection>

#include <KUrl>
#include <KAction>
#include <KMessageBox>
#include <KDebug>
#include <KCmdLineArgs>
#include <KSystemTrayIcon>
#include <KHelpMenu>
#include <KStandardAction>
#include <KActionCollection>
#include <KMenu>

KarambaApplication::KarambaApplication(Display *display, Qt::HANDLE visual, Qt::HANDLE colormap)
        :   KUniqueApplication(display, visual, colormap),
        m_themesDialog(0)
{
    connect(KarambaManager::self(), SIGNAL(karambaStarted(QGraphicsItemGroup*)), this, SLOT(karambaStarted(QGraphicsItemGroup*)));
    connect(KarambaManager::self(), SIGNAL(karambaClosed(QGraphicsItemGroup*)), this, SLOT(karambaClosed(QGraphicsItemGroup*)));

    new SuperKarambaAdaptor(this);
    QDBusConnection dbus = QDBusConnection::sessionBus();
    dbus.registerObject("/SuperKaramba", this);
}

KarambaApplication::~KarambaApplication()
{
    delete m_themesDialog;
}

void KarambaApplication::checkCommandLine(KCmdLineArgs *args, QList<KUrl> &lst)
{
    /******
    *     Not a saved session - check for themes given on command line
    **/

    if (args->count() > 0) {
        for (int i = 0; i < (args->count()); i++) {
            if (!args->arg(i).isEmpty()) {
                KUrl url = args->url(i);
                lst.append(url);
            }
        }
    }
}

void KarambaApplication::startThemes(const QList<KUrl> &lst)
{
    foreach(const KUrl &url, lst) {
        new Karamba(url);
    }
}

void KarambaApplication::karambaStarted(QGraphicsItemGroup *item)
{
   Karamba *k = dynamic_cast<Karamba*>(item);
    if (!k) {
        return;
    }

   int instance = m_themesDialog->addTheme(k->prettyName(), k->theme().file());
    k->setInstance(instance);

    if (!SuperKarambaSettings::showSysTray()) {
        showKarambaMenuExtension();
    }

    buildToolTip();
}

void KarambaApplication::karambaClosed(QGraphicsItemGroup *item)
{
    Karamba *k = dynamic_cast<Karamba*>(item);
    if (!k) {
        return;
    }

    m_themesDialog->removeTheme(k->prettyName(), k->theme().file(), k->instance());

    buildToolTip();
}

void KarambaApplication::checkPreviousSession(QList<KUrl> &lst)
{
    if (isSessionRestored()) {
        KSharedConfigPtr config = KGlobal::config();
        KConfigGroup group(config, "Session");
        const QList<QString> themePaths = group.readEntry("OpenThemes", QList<QString>());
        /*
        KConfig *config = sessionConfig();
        KConfigGroup group(config, "Session");
        QList<QString> themePaths = group.readEntry("OpenThemes", QList<QString>());
        */

        foreach(const QString &path, themePaths) {
            lst.append(KUrl(path));
        }
    }
}

int KarambaApplication::newInstance()
{
    KCmdLineArgs *args = 0;
    QList<KUrl> lst;

    if (restoringSession()) {
        checkPreviousSession(lst);
    } else {
        args = KCmdLineArgs::parsedArgs();
        checkCommandLine(args, lst);
//        args->clear();
    }

    if (lst.count() > 0)
        startThemes(lst);
    else {
        if (m_themesDialog)
            m_themesDialog->show();
    }

    if (!KWindowSystem::compositingActive()) {
        KMessageBox::information(0,
                i18n("Please enable the desktop effects to get "
                    "full transparency support in SuperKaramba."),
                QString(),
                "noTransparencyMessage");
    }

    return 0;
}

void KarambaApplication::buildToolTip()
{
    if (!m_sysTrayIcon || !m_themesDialog)
        return;

    QStringList list = m_themesDialog->runningThemes();
    if (list.isEmpty()) {
        setToolTip();
        return;
    }

    QString toolTip("<b><center>" + i18n("SuperKaramba") + "</center></b>");
    toolTip += "<table width=300>";

    bool firstRun = true;
    for (QStringList::Iterator it = list.begin(); it != list.end(); ++it) {
        if (firstRun) {
            toolTip += "<tr><td align=right>";
            toolTip += i18np("1 Running Theme:", "%1 Running Themes:", list.count());
            toolTip += "</td><td align=left>" + (*it) + "</td></tr>";
            firstRun = false;
        } else {
            toolTip += "<tr><td></td><td align=left>" + (*it) + "</td></tr>";
        }
    }
    toolTip += "</table>";

    setToolTip(toolTip);
}

void KarambaApplication::setToolTip(const QString &tip)
{
    m_sysTrayIcon->setToolTip(tip);
}

void KarambaApplication::toggleSystemTray()
{
    //kDebug() ;
    if (m_sysTrayIcon->isVisible()) {
        KMessageBox::information(m_themesDialog, i18n("<qt>Hiding the system tray icon will keep SuperKaramba running "
                                                      "in background. To show it again use the theme menu.</qt>"),
                                 i18n("Hiding System Tray Icon"), "hideIcon");

        m_sysTrayIcon->hide();
        showKarambaMenuExtension();
    } else {
        showKarambaMenuExtension(false);

        m_sysTrayIcon->show();
    }

    SuperKarambaSettings::setShowSysTray(m_sysTrayIcon->isVisible());
    SuperKarambaSettings::self()->writeConfig();
}

void KarambaApplication::showKarambaMenuExtension(bool show)
{
    foreach(Karamba *k, KarambaManager::self()->getKarambas()) {
        if (show && !k->hasMenuExtension()) {
            KMenu *menu = new KMenu();
            menu->setTitle("SuperKaramba");

            menu->addAction(KIcon("superkaramba"),
                            i18n("Show System Tray Icon"), this,
                            SLOT(toggleSystemTray()),
                            Qt::CTRL + Qt::Key_S);

            menu->addAction(KIcon("get-hot-new-stuff"),
                            i18n("&Manage Themes..."), this,
                            SLOT(showThemesDialog()), Qt::CTRL + Qt::Key_M);

            menu->addAction(KIcon("application-exit"),
                            i18n("&Quit SuperKaramba"), this,
                            SLOT(quitSuperKaramba()), Qt::CTRL + Qt::Key_Q);
            k->setMenuExtension(menu);
        } else if (!show) {
            k->removeMenuExtension();
        }
    }
}

void KarambaApplication::showThemesDialog(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Context)
        m_sysTrayIcon->show();
    else {
      if (m_themesDialog->isVisible()) {
        m_themesDialog->hide();
      } else {
        m_themesDialog->show();
      }
    }
}

void KarambaApplication::setupSysTray(KAboutData* about)
{
    KAction* action;

    //Create theme list window.
    //This will function as the main window for the tray icon
    m_themesDialog = new ThemesDlg();

    //Set up systray icon
    m_sysTrayIcon = new KSystemTrayIcon();

    QMenu *menu = m_sysTrayIcon->contextMenu();
    menu->addAction(KIcon("superkaramba"),
                    i18n("Hide System Tray Icon"), this,
                    SLOT(toggleSystemTray()));

    menu->addSeparator();

    m_helpMenu = new KHelpMenu(m_themesDialog, about);
    action = KStandardAction::help(m_helpMenu,
                                   SLOT(appHelpActivated()),
                                   m_sysTrayIcon->actionCollection());

    menu->addAction(action);

    action = KStandardAction::aboutApp(m_helpMenu, SLOT(aboutApplication()),
                                       m_sysTrayIcon->actionCollection());
    menu->addAction(action);

    action = KStandardAction::aboutKDE(m_helpMenu, SLOT(aboutKDE()),
                                       m_sysTrayIcon->actionCollection());
    menu->addAction(action);

    m_sysTrayIcon->setIcon(m_sysTrayIcon->loadIcon("superkaramba"));

    setToolTip();

    if (SuperKarambaSettings::showSysTray())
        m_sysTrayIcon->show();
    else
        m_sysTrayIcon->hide();

    connect(m_sysTrayIcon, SIGNAL(quitSelected()),
            this, SLOT(quitSuperKaramba()));
    connect(m_sysTrayIcon,
            SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(showThemesDialog(QSystemTrayIcon::ActivationReason)));
}

void KarambaApplication::sendDataToTheme(const QString &themeName, const QString &data, bool notify)
{
    if (notify) {
        themeNotify(themeName, data);
    } else {
        setIncomingData(themeName, data);
    }
}

//
// D-Bus methods
//
bool KarambaApplication::closeTheme(const QString &themeName)
{
    foreach(Karamba *k, KarambaManager::self()->getKarambas()) {
        if (k->prettyName() == themeName) {
            k->closeWidget();
            return true;
        }
    }

    return false;
}


void KarambaApplication::hideSystemTray(bool hide)
{
    if (hide && m_sysTrayIcon->isVisible()) {
        toggleSystemTray();
    } else if (!hide && !m_sysTrayIcon->isVisible()) {
        toggleSystemTray();
    }
}

void KarambaApplication::openNamedTheme(const QString &file, const QString &themeName, bool subTheme)
{
    Karamba *k = new Karamba(KUrl(file), 0, -1, subTheme);
    k->setPrettyName(themeName);
}

void KarambaApplication::openTheme(const QString &file)
{
    new Karamba(KUrl(file));
}

void KarambaApplication::quitSuperKaramba()
{
    delete KarambaManager::self();

    if (m_themesDialog) {
        m_themesDialog->saveUserAddedThemes();
    }

    quit();
}

bool KarambaApplication::setIncomingData(const QString &prettyThemeName, const QString &data)
{
    Karamba *k = KarambaManager::self()->getKarambaByName(prettyThemeName);
    if (k == 0) {
        return false;
    }

    k->setIncomingData(data);

    return true;
}

void KarambaApplication::showThemeDialog()
{
    showThemesDialog();
}

bool KarambaApplication::themeNotify(const QString &prettyThemeName, const QString &data)
{
    Karamba *k = KarambaManager::self()->getKarambaByName(prettyThemeName);
    if (k == 0) {
        return false;
    }

    k->notifyTheme(prettyThemeName, data);

    return true;
}

