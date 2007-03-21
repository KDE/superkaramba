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

    bool hasKaramba(const Karamba *k) const;
    Karamba* getKaramba(const QString &prettyName) const;
    bool themeExists(const QString &prettyName) const;
    void closeTheme(const QString &themeName);

    void setupSysTray(KAboutData* about);

public Q_SLOTS:
    void quitSuperKaramba();
    void showThemeDialog(QSystemTrayIcon::ActivationReason reason = QSystemTrayIcon::Unknown);

    void karambaStarted(Karamba*, bool);
    void karambaClosed(Karamba*);

private:
    void checkCommandLine(KCmdLineArgs *args, QList<KUrl> &lst);
    void startThemes(const QList<KUrl> &lst);
    void checkPreviousSession(QList<KUrl> &lst);
    void setToolTip(const QString &tip = QString());
    void showKarambaMenuExtension(bool show = true);
    void buildToolTip();

private Q_SLOTS:
    void toggleSystemTray();

private:
    QList<Karamba*> m_karambas;

    KSystemTrayIcon *m_sysTrayIcon;
    KHelpMenu *m_helpMenu;

    ThemesDlg *m_themesDialog;
};

#endif
