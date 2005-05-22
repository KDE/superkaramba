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
class ThemeListWindow;
class dcopIface_stub;

class KarambaApplication : public KApplication
{
private:
  static int fd;
protected:
  KarambaIface* iface;
  ThemeListWindow* themeListWindow;
  dcopIface_stub* dcopIfaceStub;
  QObjectList *karambaList;

public:
  KarambaApplication();
  ~KarambaApplication();

  QString getMainKaramba();
  QStringList getKarambas();
  void initDcopStub(QCString app = "");
  void setUpSysTray();
  void checkPreviousSession(KApplication &app, QStringList &lst);
  void checkCommandLine(KCmdLineArgs *args, QStringList &lst);
  void showWelcomeDialog(QStringList &lst);
  bool startThemes(QStringList &lst);
  KarambaIface* dcopIface() { return iface; };
  dcopIface_stub* dcopStub() { return dcopIfaceStub; };
  QWidget* parentWindow() { return (QWidget*)themeListWindow; };

  void addKaramba(karamba* k, QString &themeName);
  void deleteKaramba(karamba* k, QString &themeName);
  bool hasKaramba(karamba* k);

  static bool lockKaramba();
  static void unlockKaramba();
  static void checkSuperKarambaDir();
};

#endif // KARAMBAAPP_H
