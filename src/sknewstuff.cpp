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
#include <krun.h>
#include <kstandarddirs.h>
#include <ktar.h>
#include <kurl.h>
#include <qdir.h>
#include <qfileinfo.h>

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "karambaapp.h"
#include "themesdlg.h"
#ifdef HAVE_KNEWSTUFF
#include "sknewstuff.h"

SKNewStuff::SKNewStuff( ThemesDlg *dlg ) :
  KNewStuff( "superkaramba/themes", dlg ),
  mDlg( dlg )
{
}

bool SKNewStuff::install( const QString &fileName )
{
  kdDebug() << "SKNewStuff::install(): " << fileName << endl;

  KMimeType::Ptr result = KMimeType::findByURL(fileName);
  KStandardDirs myStdDir;
  QFileInfo fi(fileName);
  QString base = fi.baseName();
  QString baseDestDir =myStdDir.saveLocation("data", kapp->instanceName() + "/themes/", true);
  const QString destDir = baseDestDir + base + "/";
  KStandardDirs::makeDir( destDir );

  kdDebug() << "SKNewStuff::install() mimetype: " << result->name() << endl;

  if( result->name() == "application/x-gzip" ||
      result->name() == "application/x-tgz" ||
      result->name() == "application/x-bzip" ||
      result->name() == "application/x-bzip2" ||
      result->name() == "application/x-tbz" ||
      result->name() == "application/x-tbz2" ||
      result->name() == "application/x-tar" ||
      result->name() == "application/x-tarz")
  {
    kdDebug() << "SKNewStuff::install() gzip/bzip2 mimetype encountered" <<
        endl;
    KTar archive( fileName );
    if ( !archive.open( IO_ReadOnly ) )
      return false;
    const KArchiveDirectory *archiveDir = archive.directory();
    archiveDir->copyTo(destDir);
    //Add the theme to the Theme Dialog
    mDlg->addThemeToDialog(archiveDir, destDir);
    archive.close();
  }
  else if(result->name() == "application/x-zip" ||
          result->name() == "application/x-superkaramba")
  {
    kdDebug() << "SKNewStuff::install() zip mimetype encountered" << endl;
    //TODO: write a routine to check if this is a valid .skz file
    //otherwise we need to unpack it like it is an old theme that was packaged
    //as a .zip instead of .bz2 or .tar.gz
    KURL sourceFile(fileName);
    KURL destFile( destDir + sourceFile.fileName() );
    if(!KIO::NetAccess::file_copy( sourceFile, destFile ))
    {
      return false;
    }
    KIO::NetAccess::removeTempFile( sourceFile.url() );
    //Add the skz theme to the Theme Dialog
    mDlg->addSkzThemeToDialog(destFile.path());
  }
  else if(result->name() == "plain/text")
  {
    kdDebug() << "SKNewStuff::install() plain text" << endl;
  }
  else if(result->name() == "text/html")
  {
    kdDebug() << "SKNewStuff::install() text/html" << endl;
    KRun::runURL( m_sourceLink, "text/html");
  }
  else
  {
    kdDebug() << "SKNewStuff::install() Error no compatible mimetype encountered to install"
              << endl;
    return false;
  }
  return true;
}

bool SKNewStuff::createUploadFile( const QString &fileName )
{
  kdDebug() << "SKNewStuff::createUploadFile(): " << fileName << endl;
  return true;
}

QString SKNewStuff::downloadDestination( KNS::Entry *entry )
{
  KURL source = entry->payload();
  m_sourceLink = source;

  kdDebug() << "SKNewStuff::downloadDestination() url: "
    << source.url() <<  " fileName: " << source.fileName() << endl;
  QString file(source.fileName());
  if ( file.isEmpty() )
  {
    kdDebug() << "The file was empty. " << source.url() << 
      " must be a URL link." << endl;
    KRun::runURL( source, "text/html");
    return file;
  }
  return KGlobal::dirs()->saveLocation( "tmp" ) + source.fileName();
}
#endif //HAVE_KNEWSTUFF
