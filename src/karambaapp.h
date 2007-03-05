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

#ifndef KARAMBAAPP_H
#define KARAMBAAPP_H

#include <QSystemTrayIcon>

#include <kuniqueapplication.h>
#define karambaApp ((KarambaApplication*)qApp)

class QGraphicsView;
class QGraphicsScene;
class KCmdLineArgs;
class KSystemTrayIcon;
class KHelpMenu;
class ThemesDlg;
class MainWidget;
class Karamba;
class KarambaApplication : public KUniqueApplication
{
    Q_OBJECT
public:
    KarambaApplication(Display *display, Qt::HANDLE visual, Qt::HANDLE colormap);
    ~KarambaApplication();

    int newInstance();

    void addKaramba(Karamba *newK);
    void removeKaramba(Karamba *k);

    bool hasKaramba(const Karamba *k) const;
    bool themeExists(QString prettyName);
    void closeTheme(QString themeName);

    void buildToolTip();
    void setupSysTray(KAboutData* about);

public Q_SLOTS:
    void globalHideSysTray(bool hide = true);
    void globalQuitSuperKaramba();
    void globalShowThemeDialog(QSystemTrayIcon::ActivationReason reason = QSystemTrayIcon::Unknown);

private:
    void checkCommandLine(KCmdLineArgs *args, QList<KUrl> &lst);
    void startThemes(QList<KUrl> &lst);
    void checkPreviousSession(QList<KUrl> &lst);
    void setToolTip(const QString &tip = QString());
    void hideSysTray(bool hide = true);
    void showKarambaMenuExtension(bool show = true);

    QGraphicsScene *m_scene;
    MainWidget *m_view;

    QList<Karamba*> m_karambas;

    KSystemTrayIcon *m_sysTrayIcon;
    KHelpMenu *m_helpMenu;

    ThemesDlg *m_themesDialog;

};

#endif
