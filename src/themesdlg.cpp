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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 ****************************************************************************/
#include "karambaapp.h"
#include "dcopinterface_stub.h"
#include "karambainterface.h"
#include "themesdlg.h"
#include "themewidget.h"
#include "kwidgetlistbox.h"
#include "karamba.h"

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#ifdef HAVE_KNEWSTUFF
  #include "sknewstuff.h"
#endif

#include "superkarambasettings.h"
#include <karchive.h>
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
#include <qptrlist.h>
#include <kio/job.h>
#include <kprotocolinfo.h>

ThemesDlg::ThemesDlg(QWidget *parent, const char *name)
 : ThemesLayout(parent, name)
{
  populateListbox();
#ifdef HAVE_KNEWSTUFF
  mNewStuff = 0;
#endif
}

ThemesDlg::~ThemesDlg()
{
  //kdDebug() << k_funcinfo << endl;
  saveUserAddedThemes();
#ifdef HAVE_KNEWSTUFF
  if(mNewStuff)
  {
    delete mNewStuff;
  }
#endif
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
    QStringList::Iterator jtend( dirs.end() );
    for(QStringList::Iterator jt = dirs.begin(); jt != jtend; ++jt)
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
  item->setHeaderText(i18n("Get New Stuff"));
  item->setDescriptionText(i18n("Download new themes."));

  item->buttonGo->setText(i18n("New Stuff..."));
#ifdef HAVE_KNEWSTUFF
  item->buttonGo->setEnabled(true);
  connect(item->buttonGo, SIGNAL(clicked()),
          this, SLOT(getNewStuff()));
#else
  item->buttonGo->setEnabled(false);
#endif
  tableThemes->insertItem(item);

  item = new ThemeWidget;
  item->icon->setPixmap(KGlobal::iconLoader()->loadIcon("ksysguard",
                        KIcon::NoGroup, KIcon::SizeHuge));
  item->setHeaderText(i18n("Open Local Theme"));
  item->setDescriptionText(i18n("Add local theme to the list."));
  item->buttonGo->setProperty("stdItem", 18);
  item->buttonGo->setText(i18n("Open..."));
  connect(item->buttonGo, SIGNAL(clicked()),
          this, SLOT(openLocalTheme()));
  tableThemes->insertItem(item);

  dirs = ksd.findDirs("data", QString(kapp->name()) + "/themes");
  // Get custom dirs from config here?
  QStringList::Iterator itend( dirs.end() );
  for(QStringList::Iterator it = dirs.begin(); it != itend; ++it )
  {
    dir.setPath(*it);
    t = dir.entryList("*.skz; *.theme");
    for(QStringList::Iterator it = t.begin(); it != t.end(); ++it )
    {
      item = new ThemeWidget(new ThemeFile(dir.filePath(*it)));
      tableThemes->insertItem(item);
      item->buttonGo->setText(i18n("Uninstall"));
      connect(item->buttonGo, SIGNAL(clicked()),
              this, SLOT(uninstall()));
    }
  }
  t = SuperKarambaSettings::userAddedThemes();
  for(QStringList::Iterator it = t.begin(); it != t.end(); ++it )
  {
    ThemeFile* file = new ThemeFile(*it);

    if(file->isValid())
    {
      item = new ThemeWidget(file);
      tableThemes->insertItem(item);
      item->buttonGo->setText(i18n("Uninstall"));
      connect(item->buttonGo, SIGNAL(clicked()),
              this, SLOT(uninstall()));
    }
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
      (new karamba(tf->file(), QString()))->show();
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
      (new karamba(*it, QString()))->show();
  }
}

void ThemesDlg::getNewStuff()
{
#ifdef HAVE_KNEWSTUFF
  KConfig* config = KGlobal::config();
  config->setGroup("KNewStuff");
  config->writePathEntry("ProvidersUrl",
      QString::fromLatin1("http://download.kde.org/khotnewstuff/karamba-providers.xml"));
  config->sync();
  m_newStuffStatus = config->entryMap("KNewStuffStatus").keys();
  //This check is b/c KNewStuff will download, throw an error, 
  //and still have the entry in the config that it was successful
  configSanityCheck();

  if ( !mNewStuff )
  {
    mNewStuff = new SKNewStuff(this);
  }
  mNewStuff->download();
#endif
}

void ThemesDlg::selectionChanged(int index)
{
  buttonAddToDesktop->setEnabled(index > 1);

  for(uint i=2; i < tableThemes->count(); ++i)
  {
    ThemeWidget* w = static_cast<ThemeWidget*>(tableThemes->item(i));
    w->showButton(false);
  }
  ThemeWidget* w = static_cast<ThemeWidget*>(tableThemes->item(index));
  ThemeFile* themeFile = w->themeFile();
  if(themeFile && themeFile->canUninstall())
      w->showButton(true);
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

void ThemesDlg::addSkzThemeToDialog(const QString &file)
{
  kdDebug() << "addSkzThemeToDialog(): file = " << file << endl;
  addThemeToList(file);
  writeNewStuffConfig(file);
}

void ThemesDlg::addThemeToDialog(const KArchiveDirectory *archiveDir,
                           const QString& destDir)
{
  kdDebug() << "addThemeToDialog(): destDir = " << destDir << endl;
  QStringList entries = archiveDir->entries();

  QStringList::Iterator end( entries.end() );
  for(QStringList::Iterator it = entries.begin(); it != end; ++it)
  {
    if(archiveDir->entry(*it)->isDirectory())
    {
      addThemeToDialog(static_cast<const KArchiveDirectory*>(archiveDir->entry(*it)),
                destDir + *it + "/");
    }
    else
    {
      QFileInfo fi(*it);
      if(fi.extension( FALSE ) == "theme")
      {
        addThemeToList(destDir + *it);
        writeNewStuffConfig(destDir);
      }
    }
  }
}

void ThemesDlg::writeNewStuffConfig(const QString &file)
{
#ifdef HAVE_KNEWSTUFF
  KConfig* config = KGlobal::config();
  QStringList keys = config->entryMap("KNewStuffStatus").keys();

  for(QStringList::Iterator it = m_newStuffStatus.begin();
      it != m_newStuffStatus.end(); ++it)
  {
    keys.remove(*it);
  }
  if(!keys.isEmpty())
  {
    config->setGroup("KNewStuffNames");
    config->writeEntry(file, keys[0]);
    config->sync();
  }
#endif
}

void ThemesDlg::configSanityCheck()
{
#ifdef HAVE_KNEWSTUFF
  KConfig* config = KGlobal::config();
  QStringList statusKeys = config->entryMap("KNewStuffStatus").keys();
  QStringList nameKeys = config->entryMap("KNewStuffNames").keys();
  QStringList removeList;

  for(QStringList::Iterator it = statusKeys.begin();
      it != statusKeys.end(); ++it)
  {
    QString keyName(*it);
    bool removeKey = true;
    config->setGroup("KNewStuffNames");
    for(QStringList::Iterator it2 = nameKeys.begin();
        it2 != nameKeys.end(); ++it2)
    {
      QString tempName(config->readEntry(*it2));
      if( tempName.compare(keyName) == 0)
      {
        removeKey = false;
      }
      
    }
    if( removeKey )
    {
      kdDebug() << "sanityCheck() deleting entry " << keyName << endl;
      config->setGroup("KNewStuffStatus");
      config->deleteEntry( keyName );
    }
  }
  config->sync();
#endif
}

int ThemesDlg::addThemeToList(const QString &file)
{
  kdDebug() << "addThemeToList() file: " << file << endl;
  int i = themeIndex(file);
  if(i < 0)
  {
    ThemeWidget* item = new ThemeWidget(new ThemeFile(file));

    i = tableThemes->insertItem(item);
    item->buttonGo->setText(i18n("Uninstall"));
    connect(item->buttonGo, SIGNAL(clicked()),
            this, SLOT(uninstall()));
  }
  tableThemes->setSelected(i);
  return i;
}

int ThemesDlg::addTheme(const QString& , const QString &file)
{
  int i = addThemeToList(file);
  int result = -1;

  ThemeWidget* w = static_cast<ThemeWidget*>(tableThemes->item(i));
  if(w)
    result = w->addInstance();
  karambaApp->buildToolTip();
  return result;
}

void ThemesDlg::removeTheme(const QString&, const QString& file, int instance)
{
  int i = themeIndex(file);

  ThemeWidget* w = static_cast<ThemeWidget*>(tableThemes->item(i));
  if(w)
    w->removeInstance(instance);
  karambaApp->buildToolTip();
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

bool ThemesDlg::isDownloaded( const QString& path )
{
  kdDebug() << "isDownloaded path: " << path << endl;
  KConfig* config = KGlobal::config();
  config->setGroup("KNewStuffNames");
  return !config->readEntry(path).isEmpty();
}

void ThemesDlg::uninstall()
{
  ThemeWidget* w = static_cast<ThemeWidget*>(tableThemes->selectedItem());
  ThemeFile* tf = w->themeFile();
  KURL trash("trash:/");
  KURL theme(tf->file());
  QString tempPath(tf->path());

  karambaApp->dcopIface()->closeTheme(tf->name());
  if(!KProtocolInfo::isKnownProtocol(trash))
    trash = KGlobalSettings::trashPath();

  if(!tf->isZipTheme())
  {
    kdDebug() << "encountered unpacked theme" << endl;
    //Don't move it to the trash if it is a local theme
    if(isDownloaded(tempPath))
    {
      QFileInfo remPath(tf->path());
      QDir remDir(remPath.dir());
      remDir.cdUp();
      kdDebug() << "moving " << remDir.path() << " to the trash" << endl;
      KIO::move(remDir.path(), trash);
    }
    tableThemes->removeItem(w);
    
    //some themes have multiple .theme files
    //find all .themes that could be listed in the dialog for the directory removed
    QPtrList<ThemeWidget> list;
    for(uint i = 2; i < tableThemes->count(); ++i)
    {
      ThemeWidget* tempW = static_cast<ThemeWidget*>(tableThemes->item(i));
      ThemeFile* tempTf = tempW->themeFile();
      if( tempTf->path().compare( tempPath ) == 0 )
      {
        list.append( tempW );
      }
    }
    ThemeWidget *twPtr;
    for ( twPtr = list.first(); twPtr; twPtr = list.next() )
    {
      karambaApp->dcopIface()->closeTheme(twPtr->themeFile()->name());
      tableThemes->removeItem( twPtr );
    }
#ifdef HAVE_KNEWSTUFF
    // Remove theme from KNewStuffStatus
    KConfig* config = KGlobal::config();
    config->setGroup("KNewStuffNames");
    QString name = config->readEntry(tempPath);
    if(!name.isEmpty())
    {
      kdDebug() << "removing " << tempPath << " under KNewStuffNames from superkarambarc" 
                << endl;
      kapp->config()->deleteEntry(tempPath);
      config->setGroup("KNewStuffStatus");
      kdDebug() << "removing " << name << " under KNewStuffStatus from superkarambarc" 
                << endl;
      kapp->config()->deleteEntry(name);
      kapp->config()->sync();
    }
#endif
    
  }
  else
  {
    kdDebug() << "encountered skz theme" << endl;
    if(isDownloaded(theme.path()))
    {
      QFileInfo remPath(theme.path());
      QDir remDir(remPath.dir());
      kdDebug() << "moving " << remDir.path() << " to the trash" << endl;
      KIO::move(remDir.path(), trash);
    }
    tableThemes->removeItem(w);
#ifdef HAVE_KNEWSTUFF
    // Remove theme from KNewStuffStatus
    KConfig* config = KGlobal::config();
    config->setGroup("KNewStuffNames");
    QString name = config->readEntry(theme.path());
    if(!name.isEmpty())
    {
      kdDebug() << "removing " << theme.path() << " from superkarambarc" << endl;
      kapp->config()->deleteEntry(theme.path());
      config->setGroup("KNewStuffStatus");
      kdDebug() << "removing " << name << " from superkarambarc" << endl;
      kapp->config()->deleteEntry(name);
      kapp->config()->sync();
    }
#endif
  }
  selectionChanged(tableThemes->selected());
}

QStringList ThemesDlg::runningThemes()
{
  QStringList list;
  ThemeWidget* w;

  for(uint i = 2; i < tableThemes->count(); ++i)
  {
    w = static_cast<ThemeWidget*>(tableThemes->item(i));

    if(w->instances() > 0)
      list.append(w->themeFile()->name());
  }
  return list;
}

#include "themesdlg.moc"
