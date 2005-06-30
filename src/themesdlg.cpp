/*
 * Copyright (C) 2005 Petri Damstén <petri.damsten@iki.fi>
 *
 * This file is part of SuperKaramba.
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
#include "karambaapp.h"
#include "dcopinterface_stub.h"
#include "karambainterface.h"
#include "themesdlg.h"
#include "themewidget.h"
#include "kwidgetlistbox.h"
#include "karamba.h"
#include "sknewstuff.h"
#include "superkarambasettings.h"
#include <kdebug.h>
#include <kfiledialog.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>
#include <kapplication.h>
#include <kiconloader.h>
#include <klocale.h>
#include <qlineedit.h>
#include <qtable.h>
#include <qdir.h>
#include <qlabel.h>
#include <qcombobox.h>

ThemesDlg::ThemesDlg(QWidget *parent, const char *name)
 : ThemesLayout(parent, name)
{
  populateListbox();
  mNewStuff = 0;
}

ThemesDlg::~ThemesDlg()
{
  kdDebug() << k_funcinfo << endl;
  saveUserAddedThemes();
  delete mNewStuff;
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
      if(QFileInfo(*it).dir().path() + "/" == *jt)
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
  SuperKarambaSettings::setUserAddedThemes(t);
  SuperKarambaSettings::writeConfig();
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
  item->themeName->setText(i18n("Get New Stuff"));
  item->description->setText(i18n("Download new themes."));

  item->buttonGo->setEnabled(true);
  item->buttonGo->setText(i18n("New Stuff..."));
  connect(item->buttonGo, SIGNAL(clicked()),
          this, SLOT(getNewStuff()));
  tableThemes->insertItem(item);

  item = new ThemeWidget;
  item->icon->setPixmap(KGlobal::iconLoader()->loadIcon("ksysguard",
                        KIcon::NoGroup, KIcon::SizeHuge));
  item->themeName->setText(i18n("Open Local Theme"));
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
  t = SuperKarambaSettings::userAddedThemes();
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
  KConfig* config = KGlobal::config();
  config->setGroup("KNewStuff");
  config->writeEntry( "ProvidersUrl", "http://download.kde.org/khotnewstuff/karamba-providers.xml" );
  config->sync();

  if ( !mNewStuff )
  {
    mNewStuff = new SKNewStuff(this);
  }
  mNewStuff->download();
}

void ThemesDlg::selectionChanged(int index)
{
  buttonAddToDesktop->setEnabled(index > 1);
}

void ThemesDlg::quitSuperKaramba()
{
  kdDebug() << k_funcinfo << endl;
  KarambaApplication* app = karambaApp;
  QStringList apps = app->getKarambas();
  QStringList::Iterator it;

  for (it = apps.begin(); it != apps.end(); ++it)
  {
    dcopIface_stub dcop((*it).ascii(), app->dcopIface()->objId());
    dcop.quit();
  }
}

int ThemesDlg::themeIndex(QString file)
{
  ThemeWidget* w;
  file = ThemeFile::canonicalFile(file);

  for(uint i = 2; i < tableThemes->count(); ++i)
  {
    w = static_cast<ThemeWidget*>(tableThemes->item(i));

    if(w->themeFile()->file() == file)
      return i;
  }
  return -1;
}

int ThemesDlg::addThemeToList(const QString &file)
{
  int i = themeIndex(file);
  if(i < 0)
    i = tableThemes->insertItem(new ThemeWidget(new ThemeFile(file)));
  tableThemes->setSelected(i);
  return i;
}

int ThemesDlg::addTheme(const QString& , const QString &file)
{
  int i = addThemeToList(file);
  ThemeWidget* w = static_cast<ThemeWidget*>(tableThemes->item(i));
  if(w)
    return w->addInstance();
  return -1;
}

void ThemesDlg::removeTheme(const QString&, const QString& file, int instance)
{
  int i = themeIndex(file);
  ThemeWidget* w = static_cast<ThemeWidget*>(tableThemes->item(i));
  if(w)
    return w->removeInstance(instance);
}

void ThemesDlg::search(const QString&)
{
  tableThemes->showItems(&filter, this);
}

bool ThemesDlg::filter(int index, QWidget* widget, void* data)
{
  if(index < 2)
    return true;

  ThemesDlg* dlg = static_cast<ThemesDlg*>(data);
  ThemeWidget* w = static_cast<ThemeWidget*>(widget);

  if(dlg->comboShow->currentItem() == 1) // Running themes
    if(w->instances() == 0)
      return false;

  QString searchText = dlg->editSearch->text().lower();
  if(searchText.isEmpty())
  {
    return true;
  }
  else
  {
    if(w->themeName->text().lower().contains(searchText))
      return true;
    if(w->description->text().lower().contains(searchText))
      return true;
  }
  return false;
}

#include "themesdlg.moc"
