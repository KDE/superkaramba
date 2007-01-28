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

#include <kapplication.h>
#include <kdebug.h>
#include <kfilemetainfo.h>
#include <kio/netaccess.h>
#include <kmimetype.h>
#include <kstandarddirs.h>
#include <ktar.h>
#include <kurl.h>
#include <QDir>
#include <qfileinfo.h>

#include "karambaapp.h"
#include "themesdlg.h"
#include "sknewstuff.h"

SKNewStuff::SKNewStuff( ThemesDlg *dlg ) :
        KNewStuff( "superkaramba/themes", dlg ),
        mDlg( dlg )
{}

void SKNewStuff::addThemes(const KArchiveDirectory *archiveDir,
                           const QString& destDir)
{
    QStringList entries = archiveDir->entries();

    QStringList::Iterator end( entries.end() );
    for(QStringList::Iterator it = entries.begin(); it != end; ++it)
    {
        if(archiveDir->entry(*it)->isDirectory())
        {
            addThemes(static_cast<const KArchiveDirectory*>(archiveDir->entry(*it)),
                      destDir + *it + "/");
        }
        else
        {
            QFileInfo fi(*it);
            if(fi.extension() == "theme")
            {
                mDlg->addThemeToList(destDir + *it);
            }
        }
    }
}

bool SKNewStuff::install( const QString &fileName )
{
    kDebug() << "SKNewStuff::install(): " << fileName << endl;

    KMimeType::Ptr result = KMimeType::findByURL(fileName);
    KStandardDirs myStdDir;
    const QString destDir =myStdDir.saveLocation("data", KGlobal::mainComponent().componentName() + "/themes/", true);
    KStandardDirs::makeDir( destDir );

    kDebug() << "SKNewStuff::install() mimetype: " << result->name() << endl;

    if( result->name() == "application/x-gzip" ||
            result->name() == "application/x-tgz" ||
            result->name() == "application/x-bzip" ||
            result->name() == "application/x-bzip2" ||
            result->name() == "application/x-tbz" ||
            result->name() == "application/x-tbz2" ||
            result->name() == "application/x-tar" ||
            result->name() == "application/x-tarz")
    {
        kDebug() << "SKNewStuff::install() gzip/bzip2 mimetype encountered" <<
        endl;
        KTar archive( fileName );
        if ( !archive.open( QIODevice::ReadOnly ) )
            return false;
        const KArchiveDirectory *archiveDir = archive.directory();
        archiveDir->copyTo(destDir);
        addThemes(archiveDir, destDir);
        archive.close();
    }
    else if(result->name() == "application/x-zip" ||
            result->name() == "application/x-superkaramba")
    {
        kDebug() << "SKNewStuff::install() zip mimetype encountered" << endl;
        //TODO: write a routine to check if this is a valid .skz file
        //otherwise we need to unpack it like it is an old theme that was packaged
        //as a .zip instead of .bz2 or .tar.gz
        KUrl sourceFile(fileName);
        KUrl destFile( destDir + sourceFile.fileName() );
        if(!KIO::NetAccess::file_copy( sourceFile, destFile ))
        {
            return false;
        }
        KIO::NetAccess::removeTempFile( sourceFile.url() );
        mDlg->newSkzTheme(destFile.path());
    }
    else if(result->name() == "plaint/text")
    {
        kDebug() << "SKNewStuff::install() plain text" << endl;
    }
    else
    {
        kDebug() << "SKNewStuff::install() Error no compatible mimetype encountered to install"
        << endl;
        return false;
    }
    return true;
}

bool SKNewStuff::createUploadFile( const QString &fileName )
{
    kDebug() << "SKNewStuff::createUploadFile(): " << fileName << endl;
    return true;
}

QString SKNewStuff::downloadDestination( KNS::Entry *entry )
{
    KUrl source = entry->payload();
    kDebug() << "SKNewStuff::downloadDestination() fileName: "
    << source.fileName() << endl;
    return KGlobal::dirs()->saveLocation( "tmp" ) + source.fileName();
}
