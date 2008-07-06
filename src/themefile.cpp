/****************************************************************************
*  themefile.cpp - Theme file handling
*
*  Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>
*  Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>
*  Copyright (c) 2004 Petri Damst� <damu@iki.fi>
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
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
****************************************************************************/
#include "themefile.h"
#include "lineparser.h"
#include "themelocale.h"

#include <KZip>
#include <KTempDir>
#include <KApplication>
#include <KMessageBox>
#include <KStandardDirs>
#include <KLocale>
#include <kio/job.h>
#include <kio/copyjob.h>
#include <KIO/NetAccess>
#include <kross/core/manager.h>
#include <kross/core/interpreter.h>

#include <QDomNode>

class ZipFile
{
public:
    ZipFile() :
            m_zip(0), m_file(0), m_tempDir(0)
    {}
    void setFile(const QString& filename)
    {
        m_filename = filename;
        if (filename.isEmpty())
            return;

        const KArchiveEntry* entry;

        entry = m_dir->entry(filename);
        if (entry == 0 || !entry->isFile()) {
            m_file = 0;
            return;
        }
        m_file = static_cast<const KArchiveFile*>(entry);
    }
    void setZip(const QString& zipfile)
    {
        closeZip();

        m_zip = new KZip(zipfile);

        if (!m_zip->open(QIODevice::ReadOnly)) {
            qDebug("Unable to open '%s' for reading.", zipfile.toAscii().constData());
            return;
        }
        m_dir = m_zip->directory();
        if (m_dir == 0) {
            qDebug("Error reading directory contents of file %s", zipfile.toAscii().constData());
            return;
        }
    }

    virtual ~ZipFile()
    {
        closeZip();
    }

    void closeZip()
    {
        if (m_zip) {
            m_zip->close();
            delete m_zip;
            delete m_tempDir;
        }
    }

    QByteArray data()
    {
        if (m_file)
            return m_file->data();
        else {
            if (!m_filename.isEmpty())
                qDebug("Error reading file %s from zip", m_filename.toAscii().constData());
            return QByteArray();
        }
    }

    bool exists()
    {
        return (m_file != 0);
    }

    QString extractArchive()
    {
        QString tmpPath = KStandardDirs::locateLocal("tmp", "runningThemes/");

        m_tempDir = new KTempDir(tmpPath);
        m_tempDir->setAutoRemove(true);

        m_dir->copyTo(m_tempDir->name());

        return m_tempDir->name();
    }

    bool extractArchiveTo(const QString& path)
    {
        m_dir->copyTo(path);
        return true;
    }

    bool extractArchiveFileTo(const QString& file, const QString& path)
    {
        const KArchiveEntry* entry = m_dir ? m_dir->entry(file) : 0;
        const KArchiveFile* f = (entry && entry->isFile()) ? static_cast<const KArchiveFile*>(entry) : 0;
        if (! f)
            return false;
        f->copyTo(path);
        return true;
    }

private:
    KZip* m_zip;
    const KArchiveFile* m_file;
    QString m_filename;
    const KArchiveDirectory* m_dir;
    KTempDir* m_tempDir;
};

class ThemeFile::Private
{
public:
    QString path;
    bool zipTheme;
    QString file;
    QString id;
    QString mo;
    QString name;
    QString theme;
    QString script;
    QString icon;
    QString version;
    QString license;
    QTextStream* stream;
    QByteArray ba;
    QFile fl;
    QString description;
    QString author;
    QString authorEmail;
    QString homepage;
    ThemeLocale* locale;
    ZipFile* zip;
    KUrl UrlPath;

    Private()
        : zipTheme(false), stream(0), locale(0), zip(0)
    {
    }

    ~Private()
    {
        delete stream;
        delete locale;
        delete zip;
    }

};

ThemeFile::ThemeFile(const KUrl& url)
    : d(new Private)
{
    if (url.isValid())
        set(url);
}

ThemeFile::~ThemeFile()
{
    delete d;
}

bool ThemeFile::open()
{
    bool result = false;

    close();

    if (d->zipTheme) {
        d->zip->setFile(d->theme);
        d->ba = d->zip->data();
        if (d->ba.size() > 0) {
            d->stream = new QTextStream(d->ba, QIODevice::ReadOnly);
            result = true;
        }
    } else {
        d->fl.setFileName(d->file);

        if (d->fl.open(QIODevice::ReadOnly | QIODevice::Text)) {
            d->stream = new QTextStream(&d->fl);        // use a text stream
            result = true;
        }
    }
    return result;
}

bool ThemeFile::nextLine(LineParser& parser)
{
    parser.set("");

    if (d->stream) {
        QString result = d->stream->readLine();
        while ( result.endsWith("\\") ) {
            result.chop(1); // remove continuation directive
            result += d->stream->readLine();
        }

        if (result.isNull())
            return false;
        parser.set(result);
        return true;
    }
    return false;
}

bool ThemeFile::close()
{
    if (d->stream) {
        delete d->stream;
        d->stream = 0;
        d->fl.close();
        d->ba.resize(0);
        return true;
    }
    return false;
}

bool ThemeFile::isValid() const
{
    return (exists() && !d->name.isEmpty() && !d->theme.isEmpty());
}

bool ThemeFile::exists() const
{
    QFileInfo file(d->file);
    return file.exists();
}

QPixmap ThemeFile::icon() const
{
    QPixmap icon;
    icon.loadFromData(readThemeFile(d->icon));
    return icon;
}

QString ThemeFile::iconName() const
{
    return d->icon;
}

bool ThemeFile::set(const KUrl &url)
{
    if (!url.isLocalFile() && !url.protocol().isEmpty()) {
        if (KMessageBox::warningContinueCancel(qApp->activeWindow(),
                                               i18n("You are about to install and run %1 SuperKaramba theme. Since "
                                                    "themes can contain executable code you should only install themes "
                                                    "from sources that you trust. Continue?",
                                                    url.prettyUrl()),
                                               i18n("Executable Code Warning"),
                                               KGuiItem(i18n("Install")))
                //i18n("Install").arg(url.prettyUrl()))
                == KMessageBox::Cancel) {
            return false;
        }

        QDir themeDir(KStandardDirs::locateLocal("appdata", "themes/", true));
        QFileInfo localFile = themeDir.filePath(url.fileName());

        if (localFile.exists()) {
            if (KMessageBox::warningContinueCancel(qApp->activeWindow(),
                                                   i18n("%1 already exists. Do you want to overwrite it?", localFile.filePath()),
                                                   i18n("File Exists"),
                                                   KGuiItem(i18n("Overwrite"))
                                                  )
                    == KMessageBox::Cancel) {
                return false;
            }
        }

        KIO::Job *job = KIO::file_copy(url, localFile.filePath(), -1, KIO::Overwrite);

        if (!KIO::NetAccess::synchronousRun(job, qApp->activeWindow())) {
            return false;
        }

        d->file = localFile.filePath();
    } else {
        if (url.directory().isEmpty() || url.directory() == "/")
            d->file = canonicalFile(QDir::current().filePath(url.fileName()));
        else
            d->file = canonicalFile(url.path());
        if (!exists())
            return false;
    }

    d->UrlPath = url;

    QFileInfo fi(d->file);

    d->name = fi.completeBaseName();
    d->theme = d->name + ".theme";
    d->script = d->name;

    bool fileExtensionFound = false;
    const QStringList availInterp = Kross::Manager::self().interpreters();
    foreach (const QString &interpreter, availInterp) {
        QString fileExtension = Kross::Manager::self().interpreterInfo(interpreter)->wildcard();
        fileExtension.remove(0, 1);

        if (fileExists(fi.path() + '/' + d->script + fileExtension)) {
            d->script += fileExtension;
            fileExtensionFound = true;
            break;
        }
    }
    if (!fileExtensionFound) {
        d->script += ".py";
    }

    d->id = d->name;

    if (isZipFile(d->file)) {
        d->path = d->file;
        d->zipTheme = true;
        d->zip = new ZipFile();
        d->zip->setZip(d->file);
    } else {
        d->path = fi.absoluteDir().absolutePath() + '/';
        d->zipTheme = false;
    }
    parseXml();

    QFileInfo fimo(d->script);
    if (d->script.isEmpty())
        fimo.setFile(d->theme);
    else
        fimo.setFile(d->script);
    d->mo = fimo.completeBaseName();

    d->locale = new ThemeLocale(this);
    return isValid();
}

KUrl ThemeFile::getUrlPath()
{
    return d->UrlPath;
}

void ThemeFile::parseXml()
{
    if (!fileExists("maindata.xml"))
        return;
    QByteArray ba = readThemeFile("maindata.xml");
    QDomDocument doc("superkaramba_theme");
    doc.setContent(ba);
    QDomElement element = doc.documentElement();

    QDomNode n = element.firstChild();
    while (!n.isNull()) {
        QDomElement e = n.toElement();
        if (!e.isNull()) {
            if (e.tagName() == "name") {
                d->name = e.text();
            } else if (e.tagName() == "themefile") {
                d->theme = e.text();
            } else if (e.tagName() == "python_module") {
                d->script = e.text();
            } else if (e.tagName() == "script_module") {
                d->script = e.text();
            } else if (e.tagName() == "description") {
                d->description = e.text();
            } else if (e.tagName() == "author") {
                d->author = e.text();
            } else if (e.tagName() == "author_email") {
                d->authorEmail = e.text();
            } else if (e.tagName() == "homepage") {
                 d->homepage = e.text();
            } else if (e.tagName() == "icon") {
                d->icon = e.text();
            } else if (e.tagName() == "version") {
                d->version = e.text();
            } else if (e.tagName() == "license") {
                d->license = e.text();
            }
        }
        n = n.nextSibling();
    }
}

bool ThemeFile::canUninstall() const
{
    QFileInfo fi(file());
    if (fi.permission(QFile::WriteUser) ||
            fi.permission(QFile::WriteGroup) ||
            fi.permission(QFile::WriteOther))
        return true;
    return false;
}

bool ThemeFile::isThemeFile(const QString& filename) const
{
    QFileInfo fileInfo(filename);

    return fileInfo.isRelative();
}

bool ThemeFile::fileExists(const QString& filename) const
{
    if (isThemeFile(filename)) {
        if (isZipTheme()) {
            d->zip->setFile(filename);
            return d->zip->exists();
        } else
            return QFileInfo(path() + '/' + filename).exists();
    } else
        return QFileInfo(filename).exists();
}

QByteArray ThemeFile::readThemeFile(const QString& filename) const
{
    //QTime time;
    //time.start();
    QByteArray ba;

    if (isZipTheme()) {
        d->zip->setFile(filename);
        ba = d->zip->data();
    } else {
        QFile file(path() + '/' + filename);

        if (file.open(QIODevice::ReadOnly)) {
            ba = file.readAll();
            file.close();
        }
    }
    //kDebug() << "Read theme file: " << filename << ", " << time.elapsed()
    //    << "ms" << endl;
    return ba;
}

bool ThemeFile::isZipFile(const QString& filename)
{
    QFile file(filename);

    if (file.open(QIODevice::ReadOnly)) {
        unsigned char buf[5];

        if (file.read((char*)buf, 4) == 4) {
            if (buf[0] == 'P' && buf[1] == 'K' && buf[2] == 3 && buf[3] == 4)
                return true;
        }
    }
    return false;
}

bool ThemeFile::scriptModuleExists() const
{
    if (d->script.isEmpty()) {
        return false;
    }

    if (fileExists(d->script)) {
        return true;
    }

    return false;
}

QString ThemeFile::canonicalFile(const QString& file)
{
    // Get absolute path with NO symlinks
    QFileInfo fi(file);
    return QDir(fi.dir().canonicalPath()).filePath(fi.fileName());
}

QString ThemeFile::extractArchive() const
{
    if (isZipTheme()) {
        return d->zip->extractArchive();
    }

    return QString();
}

bool ThemeFile::extractArchiveTo(const QString& path)
{
    return isZipTheme() ? d->zip->extractArchiveTo(path) : false;
}

bool ThemeFile::extractArchiveFileTo(const QString& file, const QString& path)
{
    return isZipTheme() ? d->zip->extractArchiveFileTo(file, path) : false;
}

bool ThemeFile::copyArchiveTo(const QString& path)
{
    if (! isZipTheme())
        return false;
    KUrl url = getUrlPath();
    if (! url.isValid())
        return false;
    KIO::CopyJob* job = KIO::copy(url, path, KIO::HideProgressInfo | KIO::Overwrite);
    bool ok = KIO::NetAccess::synchronousRun( job, 0 );
    return ok;
}

bool ThemeFile::isZipTheme() const
{
    return d->zipTheme;
}

const QString& ThemeFile::name() const
{
    return d->name;
}

const QString& ThemeFile::version() const
{
    return d->version;
}

const QString& ThemeFile::license() const
{
    return d->license;
}

const QString& ThemeFile::id() const
{
    return d->id;
}

const QString& ThemeFile::mo() const
{
    return d->mo;
}

const QString& ThemeFile::file() const
{
    return d->file;
}

const QString& ThemeFile::scriptModule() const
{
    return d->script;
}

const QString& ThemeFile::path() const
{
    return d->path;
}

const QString& ThemeFile::description() const
{
    return d->description;
}

const QString& ThemeFile::author() const
{
    return d->author;
}

const QString& ThemeFile::authorEmail() const
{
    return d->authorEmail;
}

const QString& ThemeFile::homepage() const
{
    return d->homepage;
}

const ThemeLocale* ThemeFile::locale() const
{
    return d->locale;
}

