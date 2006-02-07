/*
 * Copyright (C) 2005 Petri Damst� <petri.damsten@iki.fi>
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
#include "karambawidget.h"
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
#include <qdir.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <kio/job.h>
#include <kprotocolinfo.h>
#include <kglobal.h>

ThemesDlg::ThemesDlg(QWidget *parent, const char *name)
        : QDialog(parent, name),
        Ui::ThemesLayout()
{
    setupUi(this);
    populateListbox();
    mNewStuff = 0;
}

ThemesDlg::~ThemesDlg()
{
    //kDebug() << k_funcinfo << endl;
    saveUserAddedThemes();

    if(mNewStuff)
    {
        delete mNewStuff;
    }
}

void ThemesDlg::saveUserAddedThemes()
{
    KStandardDirs ksd;
    QStringList t = themes();
    QStringList dirs = ksd.findDirs("data", QString(kapp->name()) + "/themes");
    QStringList::Iterator it = t.begin();
    bool remove
        ;

    while(it != t.end())
    {
        remove
            = false;
        QStringList::Iterator jtend( dirs.end() );
        for(QStringList::Iterator jt = dirs.begin(); jt != jtend; ++jt)
        {
            if(QFileInfo(*it).dir().path() + "/" == *jt)
            {
                remove
                    = true;
                break;
            }
        }
        if(remove
          )
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
    item->micon->setPixmap(KGlobal::iconLoader()->loadIcon("knewstuff",
                           KIcon::NoGroup, KIcon::SizeHuge));
    item->setHeaderText(i18n("Get New Stuff"));
    item->setDescriptionText(i18n("Download new themes."));

    item->buttonGo->setText(i18n("New Stuff..."));
    item->buttonGo->setEnabled(true);
    connect(item->buttonGo, SIGNAL(clicked()),
            this, SLOT(getNewStuff()));
    tableThemes->insertItem(item);

    item = new ThemeWidget;
    item->micon->setPixmap(KGlobal::iconLoader()->loadIcon("ksysguard",
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
            (new KarambaWidget(tf->file(), false))->show();
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
            (new KarambaWidget(*it, false))->show();
    }
}

void ThemesDlg::getNewStuff()
{
    KConfig* config = KGlobal::config();
    config->setGroup("KNewStuff");
    config->writeEntry("ProvidersUrl",
                       "http://download.kde.org/khotnewstuff/karamba-providers.xml");
    config->sync();
    m_newStuffStatus = config->entryMap("KNewStuffStatus").keys();

    if ( !mNewStuff )
    {
        mNewStuff = new SKNewStuff(this);
    }
    mNewStuff->download();
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

void ThemesDlg::newSkzTheme(const QString &file)
{
    addThemeToList(file);

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
    }
}

int ThemesDlg::addThemeToList(const QString &file)
{
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

    QString searchText = dlg->editSearch->text().toLower();
    if(searchText.isEmpty())
    {
        return true;
    }
    else
    {
        if(w->themeName->text().toLower().contains(searchText))
            return true;
        if(w->description->text().toLower().contains(searchText))
            return true;
    }
    return false;
}

void ThemesDlg::uninstall()
{
    ThemeWidget* w = static_cast<ThemeWidget*>(tableThemes->selectedItem());
    ThemeFile* tf = w->themeFile();
    KUrl trash("trash:/");
    KUrl theme(tf->file());

    karambaApp->dcopIface()->closeTheme(tf->name());
    KIO::move(theme, trash);
    tableThemes->removeItem(w);

    // Remove theme from KNewStuffStatus
    KConfig* config = KGlobal::config();

    config->setGroup("KNewStuffNames");
    QString name = config->readEntry(theme.path(),QString());
    kDebug() << theme.path() << name << endl;
    if(!name.isEmpty())
    {
        KGlobal::config()->deleteEntry(theme.path());
        config->setGroup("KNewStuffStatus");
        KGlobal::config()->deleteEntry(name);
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
