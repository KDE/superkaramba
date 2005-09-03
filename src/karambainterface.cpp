/***************************************************************************
 *   Copyright (C) 2004 by Petri Damsten                                   *
 *   petri.damsten@iki.fi                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "karamba.h"
#include "karambainterface.h"
#include "themesdlg.h"

KarambaIface::KarambaIface(): DCOPObject("KarambaIface")
{
}

KarambaIface::~KarambaIface()
{
}

karamba* KarambaIface::getKaramba(QString name)
{
  karamba* result = 0;

  foreach (QWidget *w, QApplication::allWidgets())
  {
    if (QString(w->name()).startsWith("karamba"))
    {
      karamba* k = (karamba*) w;
      if(k->theme().name() == name)
      {
        result = k;
        break;
      }
    }
  }
  return result;
}

ThemesDlg* KarambaIface::getThemeWnd()
{
  ThemesDlg* result = 0;

  foreach (QWidget *w, QApplication::allWidgets())
  {
    if (QString(w->name()) == "ThemesLayout")
    {
      result = (ThemesDlg*) w;
      break;
    }
  }
  return result;
}

void KarambaIface::openTheme(QString filename)
{
  QFileInfo file(filename);
  if(file.exists())
  {
    (new karamba(filename, false))->show();
  }
}

void KarambaIface::closeTheme(QString name)
{
  karamba* k;

  while(k = getKaramba(name))
  {
    k->writeConfigData();
    k->close(true);
  }
}

int KarambaIface::themeAdded(QString appId, QString file)
{
  ThemesDlg* tw = getThemeWnd();
  if(tw)
    return tw->addTheme(appId, file);
  return -1;
}

void KarambaIface::themeClosed(QString appId, QString file, int instance)
{
  ThemesDlg* tw = getThemeWnd();
  if(tw)
    tw->removeTheme(appId, file, instance);
}

bool KarambaIface::isMainKaramba()
{
  if(getThemeWnd())
    return true;
  return false;
}

void KarambaIface::quit()
{
  karambaApp->quitSuperKaramba();
}

void KarambaIface::hideSystemTray(bool hide)
{
  karambaApp->hideSysTray(hide);
}

void KarambaIface::showThemeDialog()
{
  karambaApp->showThemeDialog();
}
