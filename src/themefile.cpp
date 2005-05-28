/****************************************************************************
*  themefile.cpp - Theme file handling
*
*  Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>
*  Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>
*  Copyright (c) 2004 Petri Damstén <damu@iki.fi>
*
*  This file is part of Superkaramba.
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
#include "themefile.h"
#include "lineparser.h"
#include <kurl.h>
#include <kzip.h>
#include <qtextstream.h>
#include <qfileinfo.h>
#include <qdom.h>

ThemeFile::ThemeFile()
  : m_stream(0)
{
}

ThemeFile::~ThemeFile()
{
  delete m_stream;
}

bool ThemeFile::open()
{
  bool result = false;

  if(m_zipTheme)
  {
    m_ba = readZipFile(m_theme);
    if(m_ba.size() > 0)
    {
      m_stream = new QTextStream(m_ba, IO_ReadOnly);
      result = true;
    }
  }
  else
  {
    m_fl.setName(m_file);

    if(m_fl.open(IO_ReadOnly|IO_Translate))
    {
      m_stream = new QTextStream(&m_fl);        // use a text stream
      result = true;
    }
  }
  return result;
}

bool ThemeFile::nextLine(LineParser& parser)
{
  parser.set("");

  if(m_stream)
  {
    QString result = m_stream->readLine();

    if(result.isNull())
      return false;
    parser.set(result);
    return true;
  }
  return false;
}

bool ThemeFile::close()
{
  if(m_stream)
  {
    delete m_stream;
    m_stream = 0;
    m_fl.close();
    m_ba.resize(0);
    return true;
  }
  return false;
}

bool ThemeFile::exists() const
{
  QFileInfo file(m_file);
  return file.exists();
}

bool ThemeFile::set(const KURL &url)
{
  if(!url.isLocalFile())
  {
    // TODO: download file
    return false;
  }
  else
  {
    m_file = url.path();
    if(!exists())
      return false;
  }

  QFileInfo fi(m_file);

  m_name = fi.baseName();
  m_theme = m_name + ".theme";
  m_python = m_name;

  if(isZipFile(m_file))
  {
    m_path = m_file;
    m_zipTheme = true;
    parseXml();
  }
  else
  {
    m_path = fi.dirPath(true) + "/";
    m_zipTheme = false;
  }
  return true;
}

void ThemeFile::parseXml()
{
  QDomDocument doc("superkaramba_theme");
  doc.setContent(readThemeFile("maindata.xml"));
  QDomElement element = doc.documentElement();

  QDomNode n = element.firstChild();
  while(!n.isNull())
  {
    QDomElement e = n.toElement();
    if(!e.isNull())
    {
      if(e.tagName() == "name")
        m_name = e.text();
      if(e.tagName() == "themefile")
        m_file = e.text();
      if(e.tagName() == "python_module")
        m_python = e.text();
      if(e.tagName() == "description")
        m_description = e.text();
      if(e.tagName() == "author")
        m_author = e.text();
      if(e.tagName() == "author_email")
        m_authorEmail = e.text();
      if(e.tagName() == "homepage")
        m_homepage = e.text();
    }
    n = n.nextSibling();
  }
}

bool ThemeFile::isThemeFile(const QString& filename) const
{
  QFileInfo fileInfo(filename);

  // TODO: check if file exists
  return fileInfo.isRelative();
}

QByteArray ThemeFile::readThemeFile(const QString& filename) const
{
  QByteArray ba;

  if(isZipTheme())
  {
    ba = readZipFile(filename);
  }
  else
  {
    QFile file(path() + "/" + filename);

    if(file.open(IO_ReadOnly))
    {
      ba = file.readAll();
      file.close();
    }
  }
  return ba;
}

QByteArray ThemeFile::readZipFile(const QString& filename) const
{
  QByteArray result;
  KZip zip(m_file);
  const KArchiveDirectory* dir;
  const KArchiveEntry* entry;
  const KArchiveFile* file;

  if(!zip.open(IO_ReadOnly))
  {
    qDebug("Unable to open '%s' for reading.", m_file.ascii());
    return result;
  }
  dir = zip.directory();
  if(dir == 0)
  {
    qDebug("Error reading directory contents of file %s", m_file.ascii());
    return result;
  }

  entry = dir->entry(filename);
  if(entry == 0 || !entry->isFile())
  {
    qDebug("Error reading %s file from %s",
           filename.ascii(), m_file.ascii());
    return false;
  }

  file = static_cast<const KArchiveFile*>(entry);
  result = file->data();
  zip.close();
  return result;
}

bool ThemeFile::isZipFile(const QString& filename)
{
  QFile file(filename);

  if(file.open(IO_ReadOnly))
  {
    unsigned char buf[5];

    if(file.readBlock((char*)buf, 4) == 4)
    {
      if(buf[0] == 'P' && buf[1] == 'K' && buf[2] == 3 && buf[3] == 4)
        return true;
    }
  }
  return false;
}

