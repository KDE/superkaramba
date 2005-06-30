/****************************************************************************
*  karambasessionmanaged.cpp  -  Karamba session management
*
*  Copyright (C) 2004 -
*
*  This file is part of SuperKaramba.
*
*  SuperKaramba is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  SuperKaramba is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with SuperKaramba; if not, write to the Free Software
*  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
****************************************************************************/

#include <kapplication.h>
#include <kconfig.h>
#include "karambasessionmanaged.h"
#include "karamba.h"
#include "qwidgetlist.h"

bool KSessionManaged::saveState(QSessionManager&)
{
  KConfig* config = kapp->sessionConfig();

  config->setGroup("General Options");

  QString openThemes="";

  QWidgetList  *list = QApplication::allWidgets();
  QWidgetListIt it( *list );         // iterate over the widgets
  QWidget * w;
  while ( (w=it.current()) != 0 ) // for each widget...
  {
    ++it;
    if (QString(w->name()).startsWith("karamba"))
    {
      karamba* k = (karamba*) w;
      openThemes += QFileInfo(k->theme().file()).absFilePath();
      k->writeConfigData();
      openThemes += ";";
    }
  }
  delete list;                      // delete the list, not the widgets

  qDebug("Open themes %s", openThemes.ascii());
  config->writeEntry("OpenThemes", openThemes);
  
  config->writeEntry("ShowSysTray", karambaApp->sysTrayIconShown());

  return true;
}

bool KSessionManaged::commitData(QSessionManager&)
{
  return true;
}
