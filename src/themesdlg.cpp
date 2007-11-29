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
#include "themesdlg.h"

#include "karambaapp.h"
#include "themewidget.h"
#include "kwidgetlistbox.h"
#include "karamba.h"
#include "superkarambasettings.h"

#include <KArchive>
#include <KFileDialog>
#include <KStandardDirs>
#include <KIconLoader>
#include <KIO/CopyJob>
#include <KDirWatch>
#include <KTar>
#include <KIO/NetAccess>
#include <KRun>

ThemesDlg::ThemesDlg(QWidget *parent, const char *name)
        : QDialog(parent),
        m_newStuffInitialized(false)
{
    setupUi(this);
    setObjectName(name);

    populateListbox();

    connect(buttonAddToDesktop, SIGNAL(clicked()), this, SLOT(addToDesktop()));
    connect(tableThemes, SIGNAL(selected(int)), this, SLOT(selectionChanged(int)));
    connect(tableThemes, SIGNAL(itemDropped(QPoint, ThemeWidget*)), this, SLOT(addToDesktop(QPoint, ThemeWidget*)));
    connect(editSearch, SIGNAL(textChanged(QString)), this, SLOT(search(QString)));
    connect(comboShow, SIGNAL(activated(QString)), this, SLOT(search(QString)));
}

ThemesDlg::~ThemesDlg()
{
    //kDebug() ;
    saveUserAddedThemes();
}

void ThemesDlg::saveUserAddedThemes()
{
    KStandardDirs ksd;
    QStringList t = themes();
    QStringList dirs = ksd.findDirs("data", QString(kapp->objectName()) + "/themes");
    QStringList::Iterator it = t.begin();
    bool remove;

    while (it != t.end()) {
        remove = false;
        QStringList::Iterator jtend(dirs.end());
        for (QStringList::Iterator jt = dirs.begin(); jt != jtend; ++jt) {
            if (QFileInfo(*it).dir().path() + '/' == *jt) {
                remove = true;
                break;
            }
        }
        if (remove)
            it = t.erase(it);
        else
            ++it;
    }
    SuperKarambaSettings::setUserAddedThemes(t);
    SuperKarambaSettings::self()->writeConfig();
}

QStringList ThemesDlg::themes()
{
    QStringList result;
    ThemeWidget* w;

    for (uint i = 2; i < tableThemes->count(); ++i) {
        w = (ThemeWidget*)(tableThemes->item(i));

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

    item->iconLabel->setPixmap(KIconLoader::global()->loadIcon("get-hot-new-stuff",
                               KIconLoader::NoGroup, KIconLoader::SizeHuge));
    item->setHeaderText(i18n("Get New Stuff"));
    item->setDescriptionText(i18n("Download new themes."));

    item->buttonGo->setText(i18n("New Stuff..."));
    item->buttonGo->setEnabled(true);
    QObject::connect(item->buttonGo, SIGNAL(clicked()),
                     this, SLOT(getNewStuff()));

    tableThemes->insertItem((QWidget*)(item));

    item = new ThemeWidget;
    item->iconLabel->setPixmap(KIconLoader::global()->loadIcon("document-open",
                               KIconLoader::NoGroup, KIconLoader::SizeHuge));
    item->setHeaderText(i18n("Open Local Theme"));
    item->setDescriptionText(i18n("Add local theme to the list."));
    item->buttonGo->setProperty("stdItem", 18);
    item->buttonGo->setText(i18nc("Open theme button", "Open..."));
    QObject::connect((QObject*)(item->buttonGo), SIGNAL(clicked()),
                     (QObject*)(this), SLOT(openLocalTheme()));
    tableThemes->insertItem((QWidget*)item);

    dirs = ksd.findDirs("data", QString(kapp->objectName()) + "/themes");
    // Get custom dirs from config here?
    QStringList::Iterator itend(dirs.end());
    for (QStringList::Iterator it = dirs.begin(); it != itend; ++it) {
        QStringList types;
        types << "*.skz" << "*.theme";

        dir.setPath(*it);

        t = dir.entryList(types);
        for (QStringList::Iterator it = t.begin(); it != t.end(); ++it) {
            item = new ThemeWidget(new ThemeFile(dir.filePath(*it)));
            tableThemes->insertItem((QWidget*)item);
            item->buttonGo->setText(i18n("Uninstall"));
            QObject::connect((QObject*)item->buttonGo, SIGNAL(clicked()),
                             (QObject*)this, SLOT(uninstall()));
        }
    }
    t = SuperKarambaSettings::userAddedThemes();
    for (QStringList::Iterator it = t.begin(); it != t.end(); ++it) {
        ThemeFile* file = new ThemeFile(*it);

        if (file->isValid()) {
            item = new ThemeWidget(file);
            tableThemes->insertItem((QWidget*)item);
            item->buttonGo->setText(i18n("Uninstall"));
            QObject::connect((QObject*)item->buttonGo, SIGNAL(clicked()),
                             (QObject*)this, SLOT(uninstall()));
        } else
            delete file;
    }
    tableThemes->setSelected(0);
}

void ThemesDlg::addToDesktop()
{
    ThemeWidget* w = (ThemeWidget*)(tableThemes->selectedItem());
    if (w) {
        ThemeFile* tf = w->themeFile();
        if (tf) {
            new Karamba(tf->file());
        }
    }
}

void ThemesDlg::addToDesktop(QPoint pos, ThemeWidget* w)
{
    if (w) {
        ThemeFile* tf = w->themeFile();
        if (tf) {
            new Karamba(tf->file(), 0, -1, false, pos);
        }
    }
}

void ThemesDlg::openLocalTheme()
{
    QStringList fileNames;
    fileNames = KFileDialog::getOpenFileNames(KUrl(),
                i18n("*.theme *.skz|Themes"),
                (QWidget*)this, i18n("Open Themes"));

    for (QStringList::Iterator it = fileNames.begin(); it != fileNames.end(); ++it) {
        ThemeFile file(*it);
        if (file.isValid()) {
            new Karamba(*it);
        }
    }
}

void ThemesDlg::getNewStuff()
{
    if (!m_newStuffInitialized) {
        KDirWatch *dirWatch = KDirWatch::self();
        connect(dirWatch, SIGNAL(created(const QString &)),
            SLOT(installNewTheme(const QString &)));

        QString destDir = KStandardDirs::locateLocal("appdata", "themes/");
        dirWatch->addDir(destDir, KDirWatch::WatchFiles);

        m_newStuffInitialized = true;
    }

    KNS::Engine::download();
}

void ThemesDlg::installNewTheme(const QString &newTheme)
{
    // The created signal is already emitted before the file is
    // completely written, so wait until the new theme exists
    KMimeType::Ptr result;
    do {
        result = KMimeType::findByUrl(newTheme);
    } while (result->name() == "application/x-zerosize");

    if (result->name() == "application/x-gzip" ||
        result->name() == "application/x-compressed-tar" ||
        result->name() == "application/x-bzip" ||
        result->name() == "application/x-bzip" ||
        result->name() == "application/x-bzip-compressed-tar" ||
        result->name() == "application/x-bzip-compressed-tar2" ||
        result->name() == "application/x-tar" ||
        result->name() == "application/x-tarz") {

        kDebug() << "SKNewStuff::install() gzip/bzip2 mimetype encountered" << endl;

        KTar archive(newTheme);
        if (!archive.open(QIODevice::ReadOnly)) {
            return;
        }

        QString destDir = KStandardDirs::locateLocal("appdata", "themes/");

        const KArchiveDirectory *archiveDir = archive.directory();
        archiveDir->copyTo(destDir);
        //Add the theme to the Theme Dialog
        addThemeToDialog(archiveDir, destDir);
        archive.close();
    } else if (result->name() == "application/zip" ||
               result->name() == "application/x-superkaramba") {

        kDebug() << "SKNewStuff::install() zip mimetype encountered" << endl;
        //TODO: write a routine to check if this is a valid .skz file
        //otherwise we need to unpack it like it is an old theme that was packaged
        //as a .zip instead of .bz2 or .tar.gz

        //Add the skz theme to the Theme Dialog
        addSkzThemeToDialog(newTheme);
    } else if (result->name() == "plain/text") {
        kDebug() << "SKNewStuff::install() plain text" << endl;
    } else if (result->name() == "text/html" ||
        result->name() == "application/x-php") {

        kDebug() << "SKNewStuff::install() text/html" << endl;
        KRun::runUrl(newTheme, "text/html", 0);
    } else {
        kDebug() << "SKNewStuff::install() Error no compatible mimetype encountered to install" << endl;
        return;
    }
}

void ThemesDlg::selectionChanged(int index)
{
    buttonAddToDesktop->setEnabled(index > 1);

    for (uint i = 2; i < tableThemes->count(); ++i) {
        ThemeWidget* w = (ThemeWidget*)(tableThemes->item(i));
        w->showButton(false);
    }
    ThemeWidget* w = (ThemeWidget*)(tableThemes->item(index));
    ThemeFile* themeFile = w->themeFile();
    if (themeFile && themeFile->canUninstall())
        w->showButton(true);
}

int ThemesDlg::themeIndex(const QString &file)
{
    ThemeWidget* w;
    QString canonicalFile = ThemeFile::canonicalFile(file);

    for (uint i = 2; i < tableThemes->count(); ++i) {
        w = (ThemeWidget*)(tableThemes->item(i));

        if (w->themeFile()->file() == canonicalFile)
            return i;
    }
    return -1;
}

void ThemesDlg::addSkzThemeToDialog(const QString &file)
{
    //kDebug() << "addSkzThemeToDialog(): file = " << file ;
    addThemeToList(file);
}

void ThemesDlg::addThemeToDialog(const KArchiveDirectory *archiveDir,
                                 const QString& destDir)
{
    kDebug() << "addThemeToDialog(): destDir = " << destDir ;
    QStringList entries = archiveDir->entries();

    QStringList::Iterator end(entries.end());
    for (QStringList::Iterator it = entries.begin(); it != end; ++it) {
        if (archiveDir->entry(*it)->isDirectory()) {
            addThemeToDialog(static_cast<const KArchiveDirectory*>(archiveDir->entry(*it)),
                             destDir + *it + '/');
        } else {
            QFileInfo fi(*it);
            if (fi.suffix() == "theme") {
                addThemeToList(destDir + *it);
            }
        }
    }
}

int ThemesDlg::addThemeToList(const QString &file)
{
    kDebug() << "addThemeToList() file: " << file ;
    int i = themeIndex(file);
    if (i < 0) {
        ThemeWidget* item = new ThemeWidget(new ThemeFile(file));

        i = tableThemes->insertItem((QWidget*)item);
        item->buttonGo->setText(i18n("Uninstall"));
        QObject::connect((QObject*)item->buttonGo, SIGNAL(clicked()),
                         (QObject*)this, SLOT(uninstall()));
    }
    tableThemes->setSelected(i);
    return i;
}

int ThemesDlg::addTheme(const QString& , const QString &file)
{
    int i = addThemeToList(file);
    int result = -1;

    ThemeWidget* w = (ThemeWidget*)(tableThemes->item(i));
    if (w)
        result = w->addInstance();

    return result;
}

void ThemesDlg::removeTheme(const QString&, const QString& file, int instance)
{
    int i = themeIndex(file);

    ThemeWidget* w = (ThemeWidget*)(tableThemes->item(i));
    if (w) {
        w->removeInstance(instance);
    }

}

void ThemesDlg::search(const QString&)
{
    tableThemes->showItems(&filter, this);
}

bool ThemesDlg::filter(int index, QWidget* widget, void* data)
{
    if (index < 2)
        return true;

    ThemesDlg* dlg = (ThemesDlg*)(data);
    ThemeWidget* w = (ThemeWidget*)(widget);

    if (dlg->comboShow->currentIndex() == 1) // Running themes
        if (w->instances() == 0)
            return false;

    QString searchText = dlg->editSearch->text().toLower();
    if (searchText.isEmpty()) {
        return true;
    } else {
        if (w->themeName->text().toLower().contains(searchText))
            return true;
        if (w->description->text().toLower().contains(searchText))
            return true;
    }
    return false;
}

bool ThemesDlg::isDownloaded(const QString& path)
{
    kDebug() << "isDownloaded path: " << path ;
    KSharedConfigPtr config = KGlobal::config();
    return !config->group("KNewStuffNames").readEntry(path, QString()).isEmpty();
}

void ThemesDlg::uninstall()
{
    ThemeWidget* w = (ThemeWidget*)(tableThemes->selectedItem());
    ThemeFile* tf = w->themeFile();
    KUrl theme(tf->file());
    QString tempPath(tf->path());

    karambaApp->closeTheme(tf->name());

    if (!tf->isZipTheme()) {
        kDebug() << "encountered unpacked theme" ;
        //Don't move it to the trash if it is a local theme
        if (isDownloaded(tempPath)) {
            QFileInfo remPath(tf->path());
            QDir remDir(remPath.dir());
            remDir.cdUp();
            kDebug() << "moving " << remDir.path() << " to the trash" ;
            KIO::trash(remDir.path());
        }
        tableThemes->removeItem((QWidget*)w);

        //some themes have multiple .theme files
        //find all .themes that could be listed in the dialog for the directory removed
        QList<ThemeWidget*> list;
        for (uint i = 2; i < tableThemes->count(); ++i) {
            ThemeWidget* tempW = (ThemeWidget*)(tableThemes->item(i));
            ThemeFile* tempTf = tempW->themeFile();
            if (tempTf->path().compare(tempPath) == 0) {
                list.append(tempW);
            }
        }
        foreach (ThemeWidget *twPtr, list) {
            karambaApp->closeTheme(twPtr->themeFile()->name());
            tableThemes->removeItem((QWidget*)twPtr);
        }
        // Remove theme from KNewStuffStatus
        KSharedConfigPtr config = KGlobal::config();
        QString name = config->group("KNewStuffNames").readEntry(tempPath, QString());
        if (!name.isEmpty()) {
            // kapp-config() -> KGlobal::config()
            kDebug() << "removing " << tempPath << " under KNewStuffNames from superkarambarc"
            << endl;
            KGlobal::config()->group("KNewStuffNames").deleteEntry(tempPath);
            kDebug() << "removing " << name << " under KNewStuffStatus from superkarambarc"
            << endl;
            KGlobal::config()->group("KNewStuffStatus").deleteEntry(name);
            KGlobal::config()->sync();
        }
    } else {
        kDebug() << "encountered skz theme" ;
        if (isDownloaded(theme.path())) {
            QFileInfo remPath(theme.path());
            QDir remDir(remPath.dir());
            kDebug() << "moving " << remDir.path() << " to the trash" ;
            KIO::trash(remDir.path());
        }
        tableThemes->removeItem((QWidget*)w);
        // Remove theme from KNewStuffStatus
        KSharedConfigPtr config = KGlobal::config();
        QString name = config->group("KNewStuffNames").readEntry(theme.path(), QString());
        if (!name.isEmpty()) {
            kDebug() << "removing " << theme.path() << " from superkarambarc" ;
            KGlobal::config()->group("KNewStuffNames").deleteEntry(theme.path());
            kDebug() << "removing " << name << " from superkarambarc" ;
            KGlobal::config()->group("KNewStuffStatus").deleteEntry(name);
            KGlobal::config()->sync();
        }
    }
    selectionChanged(tableThemes->selected());
}

QStringList ThemesDlg::runningThemes()
{
    QStringList list;
    ThemeWidget* w;

    for (uint i = 2; i < tableThemes->count(); ++i) {
        w = (ThemeWidget*)(tableThemes->item(i));

        if (w->instances() > 0)
            list.append(w->themeFile()->name());
    }
    return list;
}

#include "themesdlg.moc"
