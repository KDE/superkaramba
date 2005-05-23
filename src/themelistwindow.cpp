/*
 * Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>
 * Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>
 * Copyright (c) 2005 Petri Damsten <damu@iki.fi>
 *
 * This file is part of Superkaramba.
 *
 *  Superkaramba is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Superkaramba is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Superkaramba; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ****************************************************************************/
#include "karambaapp.h"
#include "dcopinterface_stub.h"
#include "karamba.h"
#include "karambalistboxitem.h"
#include "themelistwindow.h"
#include "karambainterface.h"

void ThemeListWindow::openTheme()
{
  QStringList fileNames;
  fileNames = KFileDialog::getOpenFileNames(QString::null,
                                            "*.theme *.ctheme *.skz|Themes",
                                            0,
                                            "Open Themes");
  for ( QStringList::Iterator it = fileNames.begin(); it != fileNames.end(); ++it )
  {
    QFileInfo file( *it );
    if( file.exists() )
    {
      (new karamba( *it, false ))->show();
    }
  }
}

void ThemeListWindow::closeTheme()
{
  if (lbxThemes->currentItem() != -1)
  {
    KarambaIface* iface = ((KarambaApplication*)qApp)->dcopIface();
    QString appId = ((KarambaListBoxItem*)(lbxThemes->selectedItem()))->appId;
    dcopIface_stub dcop(appId.ascii(), iface->objId());

    dcop.closeTheme(lbxThemes->selectedItem()->text());
  }
}

void ThemeListWindow::themeSelected()
{
}

void ThemeListWindow::quitSuperKaramba()
{
  KarambaApplication* app = (KarambaApplication*)qApp;
  QStringList apps = app->getKarambas();
  QStringList::Iterator it;

  for (it = apps.begin(); it != apps.end(); ++it)
  {
    dcopIface_stub dcop((*it).ascii(), app->dcopIface()->objId());
    dcop.quit();
  }
}

void ThemeListWindow::addTheme(QString appId, QString name)
{
  KarambaListBoxItem* newItem = new KarambaListBoxItem(lbxThemes, name);
  newItem->appId = appId;
}

void ThemeListWindow::removeTheme(QString, QString name)
{
  QListBox *lb = lbxThemes;

  for ( int i = 0; i < (int)lb->count(); ++i )
  {
    QString appId = ((KarambaListBoxItem*)(lb->item( i )))->appId;
    if (appId == appId && lb->item(i)->text() == name)
    {
      lb->removeItem(i);
      break;
    }
  }
}
