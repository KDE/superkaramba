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

private:
    KZip* m_zip;
    const KArchiveFile* m_file;
    QString m_filename;
    const KArchiveDirectory* m_dir;
    KTempDir* m_tempDir;
};

ThemeFile::ThemeFile(const KUrl& url)
        : m_zipTheme(false), m_stream(0), m_locale(0), m_zip(0)
{
    if (url.isValid())
        set(url);
}

ThemeFile::~ThemeFile()
{
    delete m_stream;
    delete m_locale;
    delete m_zip;
}

bool ThemeFile::open()
{
    bool result = false;

    close();

    if (m_zipTheme) {
        m_zip->setFile(m_theme);
        m_ba = m_zip->data();
        if (m_ba.size() > 0) {
            m_stream = new QTextStream(m_ba, QIODevice::ReadOnly);
            result = true;
        }
    } else {
        m_fl.setFileName(m_file);

        if (m_fl.open(QIODevice::ReadOnly | QIODevice::Text)) {
            m_stream = new QTextStream(&m_fl);        // use a text stream
            result = true;
        }
    }
    return result;
}

bool ThemeFile::nextLine(LineParser& parser)
{
    parser.set("");

    if (m_stream) {
        QString result = m_stream->readLine();

        if (result.isNull())
            return false;
        parser.set(result);
        return true;
    }
    return false;
}

bool ThemeFile::close()
{
    if (m_stream) {
        delete m_stream;
        m_stream = 0;
        m_fl.close();
        m_ba.resize(0);
        return true;
    }
    return false;
}

bool ThemeFile::isValid() const
{
    return (exists() && !m_name.isEmpty() && !m_theme.isEmpty());
}

bool ThemeFile::exists() const
{
    QFileInfo file(m_file);
    return file.exists();
}

QPixmap ThemeFile::icon() const
{
    QPixmap icon;
    icon.loadFromData(readThemeFile(m_icon));
    return icon;
}

bool ThemeFile::set(const KUrl &url)
{
    if (!url.isLocalFile() && !url.protocol().isEmpty()) {
        if (KMessageBox::warningContinueCancel(kapp->activeWindow(),
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
            if (KMessageBox::warningContinueCancel(kapp->activeWindow(),
                                                   i18n("%1 already exists. Do you want to overwrite it?", localFile.filePath()),
                                                   i18n("File Exists"),
                                                   KGuiItem(i18n("Overwrite"))
                                                  )
                    == KMessageBox::Cancel) {
                return false;
            }
        }
        if (!KIO::NetAccess::file_copy(url, localFile.filePath(), -1, true,
                                       false, kapp->activeWindow())) {
            return false;
        }
        m_file = localFile.filePath();
    } else {
        if (url.directory().isEmpty() || url.directory() == "/")
            m_file = canonicalFile(QDir::current().filePath(url.fileName()));
        else
            m_file = canonicalFile(url.path());
        if (!exists())
            return false;
    }

    m_UrlPath = url;

    QFileInfo fi(m_file);

    m_name = fi.completeBaseName();
    m_theme = m_name + ".theme";
    m_script = m_name;

    bool fileExtensionFound = false;
    QStringList availInterp = Kross::Manager::self().interpreters();
    foreach (QString interpreter, availInterp) {
        QString fileExtension = Kross::Manager::self().interpreterInfo(interpreter)->wildcard();
        fileExtension.remove(0, 1);

        if (fileExists(fi.path() + '/' + m_script + fileExtension)) {
            m_script += fileExtension;
            fileExtensionFound = true;
            break;
        }
    }
    if (!fileExtensionFound) {
        m_script += ".py";
    }

    m_id = m_name;

    if (isZipFile(m_file)) {
        m_path = m_file;
        m_zipTheme = true;
        m_zip = new ZipFile();
        m_zip->setZip(m_file);
    } else {
        m_path = fi.absoluteDir().absolutePath() + '/';
        m_zipTheme = false;
    }
    parseXml();

    QFileInfo fimo(m_script);
    if (m_script.isEmpty())
        fimo.setFile(m_theme);
    else
        fimo.setFile(m_script);
    m_mo = fimo.completeBaseName();

    m_locale = new ThemeLocale(this);
    return isValid();
}

KUrl ThemeFile::getUrlPath()
{
    return m_UrlPath;
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
                m_name = e.text();
            } else if (e.tagName() == "themefile") {
                m_theme = e.text();
            } else if (e.tagName() == "python_module") {
                m_script = e.text();
            } else if (e.tagName() == "script_module") {
                m_script = e.text();
            } else if (e.tagName() == "description") {
                m_description = e.text();
            } else if (e.tagName() == "author") {
                m_author = e.text();
            } else if (e.tagName() == "author_email") {
                m_authorEmail = e.text();
            } else if (e.tagName() == "homepage") {
                 m_homepage = e.text();
            } else if (e.tagName() == "icon") {
                m_icon = e.text();
            } else if (e.tagName() == "version") {
                m_version = e.text();
            } else if (e.tagName() == "license") {
                m_license = e.text();
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
            m_zip->setFile(filename);
            return m_zip->exists();
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
        m_zip->setFile(filename);
        ba = m_zip->data();
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
    if (m_script.isEmpty()) {
        return false;
    }

    if (fileExists(m_script)) {
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
        return m_zip->extractArchive();
    }

    return QString();
}
