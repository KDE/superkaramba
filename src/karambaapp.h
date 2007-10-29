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
#include <QGraphicsItemGroup>

#include <KUrl>
#include <KUniqueApplication>

#define karambaApp ((KarambaApplication*)qApp)

class KCmdLineArgs;
class KSystemTrayIcon;
class KHelpMenu;
class ThemesDlg;
class Karamba;

class KarambaApplication : public KUniqueApplication
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.SuperKaramba")
public:
    KarambaApplication(Display *display, Qt::HANDLE visual, Qt::HANDLE colormap);
    ~KarambaApplication();

    int newInstance();

    void setupSysTray(KAboutData* about);

public Q_SLOTS:
    void showThemesDialog(QSystemTrayIcon::ActivationReason reason = QSystemTrayIcon::Unknown);

    void karambaStarted(QGraphicsItemGroup*);
    void karambaClosed(QGraphicsItemGroup*);

    Q_SCRIPTABLE bool closeTheme(const QString &prettyThemeName);
    Q_SCRIPTABLE void hideSystemTray(bool hide);
    Q_SCRIPTABLE void openNamedTheme(const QString &file, const QString &themeName, bool subTheme);
    Q_SCRIPTABLE void openTheme(const QString &file);
    Q_SCRIPTABLE void quitSuperKaramba();
    Q_SCRIPTABLE bool setIncomingData(const QString &prettyThemeName, const QString &data);
    Q_SCRIPTABLE void showThemeDialog();
    Q_SCRIPTABLE bool themeNotify(const QString &prettyThemeName, const QString &data);

private:
    void checkCommandLine(KCmdLineArgs *args, QList<KUrl> &lst);
    void startThemes(const QList<KUrl> &lst);
    void checkPreviousSession(QList<KUrl> &lst);
    void setToolTip(const QString &tip = QString());
    void showKarambaMenuExtension(bool show = true);
    void buildToolTip();

private Q_SLOTS:
    void toggleSystemTray();
    void sendDataToTheme(const QString &themeName, const QString &data, bool notify);

private:
    KSystemTrayIcon *m_sysTrayIcon;
    KHelpMenu *m_helpMenu;

    ThemesDlg *m_themesDialog;
};

#endif
