/*
 * Copyright (C) 2005 Ryan Nickell <p0z3r @ earthlink . net>
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

#include <KIO/NetAccess>
#include <KMimeType>
#include <KRun>
#include <KStandardDirs>
#include <KTar>
#include <KUrl>

#include <QDir>
#include <QFileInfo>

#include "karambaapp.h"
#include "themesdlg.h"
#include "sknewstuff.h"

SKNewStuff::SKNewStuff(ThemesDlg *dlg) :
        KNewStuff("superkaramba/themes", dlg),
        mDlg(dlg)
{}

bool SKNewStuff::install(const QString &fileName)
{
    kDebug() << "SKNewStuff::install(): " << fileName ;

    KMimeType::Ptr result = KMimeType::findByUrl(fileName);
    KStandardDirs myStdDir;
    QFileInfo fi(fileName);
    QString base = fi.baseName();
    QString baseDestDir = myStdDir.saveLocation("data", KGlobal::mainComponent().componentName() + "/themes/", true);
    const QString destDir = baseDestDir + base + '/';
    KStandardDirs::makeDir(destDir);

    kDebug() << "SKNewStuff::install() mimetype: " << result->name() ;

    if (result->name() == "application/x-gzip" ||
            result->name() == "application/x-compressed-tar" ||
            result->name() == "application/x-bzip" ||
            result->name() == "application/x-bzip" ||
            result->name() == "application/x-bzip-compressed-tar" ||
            result->name() == "application/x-bzip-compressed-tar2" ||
            result->name() == "application/x-tar" ||
            result->name() == "application/x-tarz") {
        kDebug() << "SKNewStuff::install() gzip/bzip2 mimetype encountered" <<
        endl;
        KTar archive(fileName);
        if (!archive.open(QIODevice::ReadOnly))
            return false;
        const KArchiveDirectory *archiveDir = archive.directory();
        archiveDir->copyTo(destDir);
        //Add the theme to the Theme Dialog
        mDlg->addThemeToDialog(archiveDir, destDir);
        archive.close();
    } else if (result->name() == "application/zip" ||
               result->name() == "application/x-superkaramba") {
        kDebug() << "SKNewStuff::install() zip mimetype encountered" ;
        //TODO: write a routine to check if this is a valid .skz file
        //otherwise we need to unpack it like it is an old theme that was packaged
        //as a .zip instead of .bz2 or .tar.gz
        KUrl sourceFile(fileName);
        KUrl destFile(destDir + sourceFile.fileName());
        if (!KIO::NetAccess::file_copy(sourceFile, destFile)) {
            return false;
        }
        KIO::NetAccess::removeTempFile(sourceFile.url());
        //Add the skz theme to the Theme Dialog
        mDlg->addSkzThemeToDialog(destFile.path());
    } else if (result->name() == "plain/text") {
        kDebug() << "SKNewStuff::install() plain text" ;
    } else if (result->name() == "text/html") {
        kDebug() << "SKNewStuff::install() text/html" ;
        KRun::runUrl(m_sourceLink, "text/html", 0);
    } else {
        kDebug() << "SKNewStuff::install() Error no compatible mimetype encountered to install"
        << endl;
        return false;
    }
    return true;
}

bool SKNewStuff::createUploadFile(const QString &fileName)
{
    kDebug() << "SKNewStuff::createUploadFile(): " << fileName ;
    return true;
}

QString SKNewStuff::downloadDestination(KNS::Entry *entry)
{
    KUrl source = entry->payload();
    m_sourceLink = source;

    kDebug() << "SKNewStuff::downloadDestination() url: "
    << source.url() <<  " fileName: " << source.fileName() << endl;
    QString file(source.fileName());
    if (file.isEmpty()) {
        kDebug() << "The file was empty. " << source.url() <<
        " must be a URL link." << endl;
        KRun::runUrl(source, "text/html", 0);
        return file;
    }
    return KGlobal::dirs()->saveLocation("tmp") + source.fileName();
}
