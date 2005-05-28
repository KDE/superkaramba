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
#include "qwidgetlist.h"
#include "themelistwindow.h"

KarambaIface::KarambaIface(): DCOPObject("KarambaIface")
{
}

KarambaIface::~KarambaIface()
{
}

karamba* KarambaIface::getKaramba(QString name)
{
  QWidgetList *list = QApplication::allWidgets();
  QWidgetListIt it(*list);  // iterate over the widgets
  QWidget* w;
  karamba* result = 0;

  while ( (w=it.current()) != 0 )  // for each widget...
  {
    ++it;
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
  delete list;
  return result;
}

ThemeListWindow* KarambaIface::getThemeWnd()
{
  QWidgetList  *list = QApplication::allWidgets();
  QWidgetListIt it( *list );         // iterate over the widgets
  QWidget* w;
  ThemeListWindow* result = 0;

  while ( (w=it.current()) != 0 ) // for each widget...
  {
    ++it;
    if (QString(w->name()) == "ThemeListWindowLayout")
    {
      result = (ThemeListWindow*) w;
      break;
    }
  }
  delete list;                      // delete the list, not the widgets
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
  karamba* k = getKaramba(name);
  if(k)
  {
    k->writeConfigData();
    k->close();
  }
}

void KarambaIface::quit()
{
  qApp->closeAllWindows();
  qApp->quit();
}

void KarambaIface::themeAdded(QString appId, QString name)
{
  ThemeListWindow* tw = getThemeWnd();
  if(tw)
    tw->addTheme(appId, name);
}

void KarambaIface::themeClosed(QString appId, QString name)
{
  ThemeListWindow* tw = getThemeWnd();
  if(tw)
    tw->removeTheme(appId, name);
}

bool KarambaIface::isMainKaramba()
{
  if(getThemeWnd())
    return true;
  return false;
}



