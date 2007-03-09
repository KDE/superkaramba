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

#include <QDesktopWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMenu>

#include <kurl.h>
#include <kaction.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kdeversion.h>
#include <kcmdlineargs.h>
#include <ksystemtrayicon.h>
#include <khelpmenu.h>
#include <kstandardaction.h>
#include <kactioncollection.h>

#include "meters/textfield.h"

#include "superkarambasettings.h"
#include "mainwidget.h"
#include "themesdlg.h"
#include "karamba.h"
#include "karambaapp.h"
#include "karambaapp.moc"

KarambaApplication::KarambaApplication(Display *display, Qt::HANDLE visual, Qt::HANDLE colormap)
        :   KUniqueApplication(display, visual, colormap),
        m_scene(0),
        m_view(0),
        m_themesDialog(0)
{
    bool globalView = false;

    if (globalView) {
        m_scene = new QGraphicsScene;
        m_view = new MainWidget(m_scene);
        m_view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

        m_view->show();
    }
}

KarambaApplication::~KarambaApplication()
{
    while (m_karambas.count()) {
        Karamba *k = m_karambas.takeFirst();
        k->closeWidget();
        removeKaramba(k);
    }

    delete m_view;
    delete m_scene;

    delete m_themesDialog;
}

void KarambaApplication::checkCommandLine(KCmdLineArgs *args, QList<KUrl> &lst)
{
    /******
    *     Not a saved session - check for themes given on command line
    **/

    if (args->count() > 0) {
        for (int i = 0; i < (args->count()); i++) {
            if (args->arg(i) && *args->arg(i)) {
                KUrl url = args->url(i);
                lst.append(url);
            }
        }
    }
}

void KarambaApplication::startThemes(QList<KUrl> &lst)
{
    foreach(KUrl url, lst) {
        Karamba *k = new Karamba(url, m_view, m_scene);
        m_karambas.append(k);
    }

    buildToolTip();
}

void KarambaApplication::removeKaramba(Karamba *k)
{
    m_karambas.removeAll(k);

    if (m_scene)
        m_scene->removeItem(k);

    m_themesDialog->removeTheme(k->prettyName(), k->theme().file(), k->instance());

    delete k;
    buildToolTip();
}

void KarambaApplication::closeTheme(QString themeName)
{
    foreach(Karamba *k, m_karambas) {
        if (k->objectName() == themeName) {
            k->closeWidget();
            removeKaramba(k);
        }
    }
}

void KarambaApplication::addKaramba(Karamba *newK)
{
    m_karambas.append(newK);
    int instance = m_themesDialog->addTheme(newK->prettyName(), newK->theme().file());
    newK->setInstance(instance);

    buildToolTip();
}

bool KarambaApplication::themeExists(QString prettyName)
{
    foreach(Karamba *k, m_karambas) {
        if (k->prettyName() == prettyName)
            return true;
    }

    return false;
}

void KarambaApplication::checkPreviousSession(QList<KUrl> &lst)
{
    if (isSessionRestored()) {
        KConfig* config = sessionConfig();
        QList<QString> themePaths = config->group("General Options").readEntry("OpenThemes", QList<QString>());
        foreach(QString path, themePaths)
        lst.append(KUrl(path));
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

    return 0;
}

void KarambaApplication::globalQuitSuperKaramba()
{
    foreach(Karamba *k, m_karambas) {
        k->closeWidget();
        removeKaramba(k);
    }

    if (m_themesDialog)
        m_themesDialog->saveUserAddedThemes();

    quit();
}

bool KarambaApplication::hasKaramba(const Karamba *k) const
{
    return m_karambas.contains(const_cast<Karamba*>(k));
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

void KarambaApplication::globalHideSysTray(bool hide)
{
    SuperKarambaSettings::setShowSysTray(!hide);
    SuperKarambaSettings::writeConfig();

    hideSysTray(hide);
}

void KarambaApplication::hideSysTray(bool hide)
{
    //kdDebug() << k_funcinfo << endl;
    if (hide) {
        if (m_sysTrayIcon) {
            KMessageBox::information(m_themesDialog, i18n("<qt>Hiding the system tray icon will keep SuperKaramba running "
                                     "in background. To show it again use the theme menu.</qt>"),
                                     i18n("Hiding System Tray Icon"), "hideIcon");

            m_sysTrayIcon->hide();
        }
        showKarambaMenuExtension();
    } else {
        showKarambaMenuExtension(false);

        if (m_sysTrayIcon)
            m_sysTrayIcon->show();
    }
}

void KarambaApplication::showKarambaMenuExtension(bool show)
{
    foreach(Karamba *k, m_karambas) {
        if (show)
            k->showMenuExtension();
        else
            k->hideMenuExtension();
    }
}

void KarambaApplication::globalShowThemeDialog(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Context)
        m_sysTrayIcon->show();
    else
        m_themesDialog->show();
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
                    SLOT(globalHideSysTray()));

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
            this, SLOT(globalQuitSuperKaramba()));
    connect(m_sysTrayIcon,
            SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(globalShowThemeDialog(QSystemTrayIcon::ActivationReason)));
}
