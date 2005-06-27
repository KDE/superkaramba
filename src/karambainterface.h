/***************************************************************************
 *   Copyright (C) 2004 by Petri Damsten                                   *
 *   petri.damsten@iki.fi                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef KARAMBAINTERFACE_H
#define KARAMBAINTERFACE_H

#include "dcopinterface.h"

class karamba;
class ThemeListWindow;

class KarambaIface: virtual public dcopIface
{
public:
  KarambaIface();
  ~KarambaIface();
  karamba* getKaramba(QString name);
  ThemesDlg* getThemeWnd();

public slots:
  virtual void openTheme(QString filename);
  virtual void closeTheme(QString name);
  virtual void quit();

  virtual int themeAdded(QString appId, QString file);
  virtual void themeClosed(QString appId, QString file, int instance);
  virtual bool isMainKaramba();
  
  virtual void hideSystemTray(bool show);
};

#endif // KARAMBAINTERFACE_H
