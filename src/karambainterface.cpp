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
#include <kdebug.h>
#include "qwidgetlist.h"
#include "themesdlg.h"

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
      //if(k->prettyName == name)
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

ThemesDlg* KarambaIface::getThemeWnd()
{
  QWidgetList  *list = QApplication::allWidgets();
  QWidgetListIt it( *list );         // iterate over the widgets
  QWidget* w;
  ThemesDlg* result = 0;

  while ( (w=it.current()) != 0 ) // for each widget...
  {
    ++it;
    if (QString(w->name()) == "ThemesLayout")
    {
      result = (ThemesDlg*) w;
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
    (new karamba(filename, QString()))->show();
  }
}

void KarambaIface::openNamedTheme(QString filename, QString name, bool is_sub_theme)
{
	QFileInfo file(filename);
	if(file.exists())
	{
		(new karamba(filename, name, false, -1, is_sub_theme))->show();
	}
}

void KarambaIface::closeTheme(QString name)
{
  kdDebug() << "KarambaIface::closeTheme: " << name << endl;
  karamba* k;

  while((k = getKaramba(name)))
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

void KarambaIface::themeNotify(QString name, QString text)
{
  karamba* k = getKaramba(name);
  if(k)
  {
    k->themeNotify(name, text);
  }
}

void KarambaIface::setIncomingData(QString name, QString text)
{
  karamba* k = getKaramba(name);
  if(k)
  {
    k->_setIncomingData(text);
  }
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
