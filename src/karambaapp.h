/***************************************************************************
 *   Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>              *
 *   Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>                 *
 *   Copyright (C) 2007 Alexander Wiedenbruch <mail@wiedenbruch.de>        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef KARAMBAAPP_H
#define KARAMBAAPP_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QSystemTrayIcon>

#include <kuniqueapplication.h>
#include <kdeversion.h>
#include <kcmdlineargs.h>
#include <ksystemtrayicon.h>
#include <khelpmenu.h>
#include <kstandardaction.h>

#include "mainwidget.h"
#include "karamba.h"
#include "themesdlg.h"

#define karambaApp ((KarambaApplication*)qApp)

class Karamba;

class KarambaApplication : public KUniqueApplication
{
  Q_OBJECT

  public:
    KarambaApplication(Display *display, Qt::HANDLE visual, Qt::HANDLE colormap);
    ~KarambaApplication();

    int newInstance();

    void removeKaramba(Karamba *k);

    bool hasKaramba(Karamba *k);
    
    bool themeExists(QString prettyName);

    void addKaramba(Karamba *newK);

    void buildToolTip();

    void setUpSysTray(KAboutData* about);

    void closeTheme(QString themeName);

  private:
    QGraphicsScene *m_scene;
    MainWidget *m_view;

    QList<Karamba*> m_karambas;

    KSystemTrayIcon *m_sysTrayIcon;
    KHelpMenu *m_helpMenu;

    ThemesDlg *m_themesDialog;

    void checkCommandLine(KCmdLineArgs *args, QList<KUrl> &lst);
    void startThemes(QList<KUrl> &lst);
    void checkPreviousSession(QList<KUrl> &lst);
    void setToolTip(const QString &tip = QString());
    void hideSysTray(bool hide = true);
    void showKarambaMenuExtension(bool show = true);
     
  public Q_SLOTS:
    void globalHideSysTray(bool hide = true);

    void globalQuitSuperKaramba();

    void globalShowThemeDialog(QSystemTrayIcon::ActivationReason reason = QSystemTrayIcon::Unknown);
};

#endif // 
