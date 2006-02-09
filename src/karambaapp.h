/***************************************************************************
 *   Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>              *
 *   Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef KARAMBAAPP_H
#define KARAMBAAPP_H

#include "kapplication.h"
#include <kdeversion.h>
#include <ksystemtray.h>

#undef KDE_3_2
#undef KDE_3_3
#if defined(KDE_MAKE_VERSION)
#if KDE_VERSION >= KDE_MAKE_VERSION(3,2,0)
#define KDE_3_2
#endif
#if KDE_VERSION >= KDE_MAKE_VERSION(3,3,0)
#define KDE_3_3
#endif
#endif

#define karambaApp ((KarambaApplication*)qApp)

class karamba;
class KarambaIface;
class KCmdLineArgs;
class ThemesDlg;
class dcopIface_stub;
class KHelpMenu;
class KAboutData;

class KarambaApplication : public KApplication
{
    Q_OBJECT

    friend class KarambaIface;

  private:
    static int fd;
    KHelpMenu* m_helpMenu;

    void showKarambaMenuExtension(bool show = true);
    void setToolTip(const QString &tip = QString::null);

  protected:
    KarambaIface* iface;
    ThemesDlg* themeListWindow;
    dcopIface_stub* dcopIfaceStub;
    QObjectList *karambaList;
    KSystemTray* sysTrayIcon;

  public:
    KarambaApplication();
    ~KarambaApplication();

    QString getMainKaramba();
    QStringList getKarambas();
    bool themeExists(QString pretty_name);
    void initDcopStub(QCString app = "");
    void setUpSysTray(KAboutData* about);
    void checkPreviousSession(KApplication &app, QStringList &lst);
    void checkCommandLine(KCmdLineArgs *args, QStringList &lst);
    bool startThemes(QStringList &lst);
    KarambaIface* dcopIface() { return iface; };
    dcopIface_stub* dcopStub() { return dcopIfaceStub; };
    QWidget* parentWindow() { return (QWidget*)themeListWindow; };

    void addKaramba(karamba* k, bool reloading = false);
    void deleteKaramba(karamba* k, bool reloading = false);
    bool hasKaramba(karamba* k);

    static bool lockKaramba();
    static void unlockKaramba();
    static void checkSuperKarambaDir();

  public slots:
    void buildToolTip();
    void globalQuitSuperKaramba();
    void globalShowThemeDialog();
    void globalHideSysTray(bool hide = true);

  protected slots:
    void quitSuperKaramba();
    void showThemeDialog();
    void hideSysTray(bool hide = true);
};

#endif // KARAMBAAPP_H
