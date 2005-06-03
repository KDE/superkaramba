/*
 * Copyright (C) 2005 Petri Damstén <petri.damsten@iki.fi>
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
#include "karambainterface.h"
#include "themesdlg.h"
#include "themewidget.h"
#include "kwidgetlistbox.h"
#include "karamba.h"
#include "superkarambasettings.h"
#include <kdebug.h>
#include <kfiledialog.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>
#include <kapplication.h>
#include <kiconloader.h>
#include <klocale.h>
#include <qtable.h>
#include <qdir.h>
#include <qlabel.h>
#include <qcombobox.h>

ThemesDlg::ThemesDlg(QWidget *parent, const char *name)
 : ThemesLayout(parent, name)
{
  populateListbox();
}

ThemesDlg::~ThemesDlg()
{
  saveUserAddedThemes();
}

void ThemesDlg::saveUserAddedThemes()
{
  KStandardDirs ksd;
  QStringList t = themes();
  QStringList dirs = ksd.findDirs("data", QString(kapp->name()) + "/themes");
  QStringList::Iterator it = t.begin();
  bool remove;

  while(it != t.end())
  {
    remove = false;
    for(QStringList::Iterator jt = dirs.begin(); jt != dirs.end(); ++jt)
    {
      if((*it).startsWith((*jt)))
      {
        remove = true;
        break;
      }
    }
    if(remove)
      it = t.remove(it);
    else
      ++it;
  }
  SuperkarambaSettings::setUserAddedThemes(t);
  SuperkarambaSettings::writeConfig();
}

QStringList ThemesDlg::themes()
{
  QStringList result;
  ThemeWidget* w;

  for(uint i = 2; i < tableThemes->count(); ++i)
  {
    w = static_cast<ThemeWidget*>(tableThemes->item(i));

    result.append(w->themeFile()->file());
  }
  return result;
}

void ThemesDlg::populateListbox()
{
  ThemeWidget* item;
  QDir dir;
  QStringList dirs;
  QStringList t;
  KStandardDirs ksd;

  tableThemes->clear();

  item = new ThemeWidget;
  item->icon->setPixmap(KGlobal::iconLoader()->loadIcon("knewstuff",
                        KIcon::NoGroup, KIcon::SizeHuge));
  item->themeName->setText(i18n("Get new stuff"));
  item->description->setText(i18n("Download new themes."));
  // TODO: Get new stuff
  item->buttonGo->setEnabled(false);
  item->buttonGo->setText(i18n("New Stuff..."));
  connect(item->buttonGo, SIGNAL(clicked()),
          this, SLOT(getNewStuff()));
  tableThemes->insertItem(item);

  item = new ThemeWidget;
  item->icon->setPixmap(KGlobal::iconLoader()->loadIcon("ksysguard",
                        KIcon::NoGroup, KIcon::SizeHuge));
  item->themeName->setText(i18n("Open local theme"));
  item->description->setText(i18n("Add local theme to the list."));
  item->buttonGo->setProperty("stdItem", 18);
  connect(item->buttonGo, SIGNAL(clicked()),
          this, SLOT(openLocalTheme()));
  tableThemes->insertItem(item);

  dirs = ksd.findDirs("data", QString(kapp->name()) + "/themes");
  // Get custom dirs from config here?
  for(QStringList::Iterator it = dirs.begin(); it != dirs.end(); ++it )
  {
    dir.setPath(*it);
    t = dir.entryList("*.skz; *.theme");
    for(QStringList::Iterator it = t.begin(); it != t.end(); ++it )
    {
      tableThemes->insertItem(
          new ThemeWidget(new ThemeFile(dir.filePath(*it))));
    }
  }
  t = SuperkarambaSettings::userAddedThemes();
  for(QStringList::Iterator it = t.begin(); it != t.end(); ++it )
  {
    ThemeFile* file = new ThemeFile(*it);

    if(file->isValid())
      tableThemes->insertItem(new ThemeWidget(file));
    else
      delete file;
  }
  tableThemes->setSelected(0);
}

void ThemesDlg::addToDesktop()
{
  ThemeWidget* w = static_cast<ThemeWidget*>(tableThemes->selectedItem());
  if(w)
  {
    ThemeFile* tf = w->themeFile();
    if(tf)
    {
      (new karamba(tf->file(), false))->show();
    }
  }
}

void ThemesDlg::openLocalTheme()
{
  QStringList fileNames;
  fileNames = KFileDialog::getOpenFileNames(":<themes>",
                  i18n("*.theme *.skz|Themes"),
                  this, i18n("Open Themes"));
  for(QStringList::Iterator it = fileNames.begin(); it != fileNames.end(); ++it)
  {
    ThemeFile file(*it);
    if(file.isValid())
      (new karamba(*it, false))->show();
  }
}

void ThemesDlg::getNewStuff()
{
  // TODO: Get new stuff here
}

void ThemesDlg::selectionChanged(int index)
{
  buttonAddToDesktop->setEnabled(index > 1);
}

void ThemesDlg::quitSuperKaramba()
{
  KarambaApplication* app = karambaApp;
  QStringList apps = app->getKarambas();
  QStringList::Iterator it;

  for (it = apps.begin(); it != apps.end(); ++it)
  {
    dcopIface_stub dcop((*it).ascii(), app->dcopIface()->objId());
    dcop.quit();
  }
}

QString ThemesDlg::realFile(QString file)
{
  // Get absolute path with NO symlinks
  QFileInfo fi(file);
  return QDir(fi.dir().canonicalPath()).filePath(fi.fileName());
}

int ThemesDlg::themeIndex(QString file)
{
  ThemeWidget* w;
  file = realFile(file);

  for(uint i = 2; i < tableThemes->count(); ++i)
  {
    w = static_cast<ThemeWidget*>(tableThemes->item(i));

    if(w->themeFile()->file() == file)
      return i;
  }
  return -1;
}

void ThemesDlg::addTheme(QString , QString file)
{
  int i = themeIndex(file);
  if(i < 0)
    tableThemes->insertItem(new ThemeWidget(new ThemeFile(file)));
}

void ThemesDlg::removeTheme(QString, QString)
{
}

#include "themesdlg.moc"
