/*
 * languageList from klocale.cpp
 *  Copyright (c) 1997,2001 Stephan Kulow <coolo@kde.org>
 *  Copyright (c) 1999 Preston Brown <pbrown@kde.org>
 *  Copyright (c) 1999-2002 Hans Petter Bieker <bieker@kde.org>
 *  Copyright (c) 2002 Lukas Tinkl <lukas@kde.org>
 *
 * libintl.cpp -- gettext related functions from glibc-2.0.5
 *  Copyright (C) 1995 Software Foundation, Inc.
 *
 * This file is part of SuperKaramba.
 *  Copyright (c) 2005 Petri Damsten <damu@iki.fi>
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
#include <config.h>

#include "themelocale.h"
#include "themefile.h"
#include <kdebug.h>
#include <kconfig.h>
#include <kglobal.h>
#include <klocale.h>
#include <qbuffer.h>
#include <qglobal.h>
#include <qiodevice.h>
#include <stdlib.h>

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifndef W
# define W(flag, data) ((flag) ? SWAP (data) : (data))
#endif

typedef Q_UINT32 nls_uint32;

struct loaded_domain
{
  const char *data;
  int must_swap;
  nls_uint32 nstrings;
  struct string_desc *orig_tab;
  struct string_desc *trans_tab;
  nls_uint32 hash_size;
  nls_uint32 *hash_tab;
};

static inline nls_uint32 SWAP (nls_uint32  i)
{
  return (i << 24) | ((i & 0xff00) << 8) | ((i >> 8) & 0xff00) | (i >> 24);
}

/* @@ end of prolog @@ */

/* The magic number of the GNU message catalog format.  */
#define _MAGIC 0x950412de
#define _MAGIC_SWAPPED 0xde120495

/* Revision number of the currently used .mo (binary) file format.  */
#define MO_REVISION_NUMBER 0


/* Defines the so called `hashpjw' function by P.J. Weinberger
   [see Aho/Sethi/Ullman, COMPILERS: Principles, Techniques and Tools,
   1986, 1987 Bell Telephone Laboratories, Inc.]  */
static inline unsigned long hash_string  (const char *__str_param);

/* @@ end of prolog @@ */

/* Header for binary .mo file format.  */
struct mo_file_header
{
  /* The magic number.  */
  nls_uint32 magic;
  /* The revision number of the file format.  */
  nls_uint32 revision;
  /* The number of strings pairs.  */
  nls_uint32 nstrings;
  /* Offset of table with start offsets of original strings.  */
  nls_uint32 orig_tab_offset;
  /* Offset of table with start offsets of translation strings.  */
  nls_uint32 trans_tab_offset;
  /* Size of hashing table.  */
  nls_uint32 hash_tab_size;
  /* Offset of first hashing entry.  */
  nls_uint32 hash_tab_offset;
};

struct string_desc
{
  /* Length of addressed string.  */
  nls_uint32 length;
  /* Offset of string in file.  */
  nls_uint32 offset;
};

void tl_nl_load_domain(QIODevice* device, int size,
                       struct sk_kde_loaded_l10nfile *domain_file);
char* tl_nl_find_msg(const struct sk_kde_loaded_l10nfile *domain_file,
                     const char *msgid);
void tl_nl_unload_domain(struct loaded_domain *domain);

ThemeLocale::ThemeLocale(ThemeFile* theme)
  : m_theme(theme)
{
  setLanguage(languageList());
}

ThemeLocale::~ThemeLocale()
{
  unload();
}

void ThemeLocale::unload()
{
  if(m_domain.data)
  {
    tl_nl_unload_domain((struct loaded_domain *)m_domain.data);
    m_domain.data = 0;
  }
}

QString ThemeLocale::translate(QString text) const
{
  if(text == 0)
    return QString::null;
  if(m_domain.data)
  {
    QString result = QString::fromUtf8(tl_nl_find_msg(&m_domain, text.ascii()));
    if(result.isEmpty())
      return text;
    else
      return result;
  }
  return text;
}

void ThemeLocale::setLanguage(const QStringList &languages)
{
  unload();
  for(QStringList::ConstIterator it = languages.begin();
      it != languages.end();
      ++it)
  {
    QString file =
        QString("locale/%1/LC_MESSAGES/%2.mo").arg(*it).arg(m_theme->mo());

    if(m_theme->fileExists(file))
    {
      QBuffer buffer(m_theme->readThemeFile(file));
      tl_nl_load_domain(&buffer, buffer.size(), &m_domain);
      m_language = *it;
      return;
    }
  }
}

QStringList ThemeLocale::languageList()
{
  KConfig* config = KGlobal::instance()->config();
  // Reset the list and add the new languages
  QStringList languageList;
  languageList +=
      QStringList::split(':', QFile::decodeName(::getenv("KDE_LANG")));

  languageList += config->readListEntry("Language", ':');

  // same order as setlocale use
  // HPB: Only run splitLocale on the environment variables..
  QStringList langs;

  langs << QFile::decodeName(::getenv("LC_ALL"));
  langs << QFile::decodeName(::getenv("LC_MESSAGES"));
  langs << QFile::decodeName(::getenv("LANG"));

  for(QStringList::Iterator it = langs.begin();
      it != langs.end();
      ++it )
  {
    QString ln, ct, chrset;
    KLocale::splitLocale(*it, ln, ct, chrset);
    /*
    We don't use these in zip themes...
    if (!ct.isEmpty())
    {
      langs.insert(it, ln + '_' + ct);
      if (!chrset.isEmpty())
        langs.insert(it, ln + '_' + ct + '.' + chrset);
    }
    */
    langs.insert(it, ln);
  }
  languageList += langs;
  // Remove empty strings
  QStringList::Iterator end( languageList.end() );
  for(QStringList::Iterator it=languageList.begin(); it!=end;)
  {
    if((*it).isEmpty())
      it = languageList.remove(it);
    else
      ++it;
  }
  return languageList;
}

char* tl_nl_find_msg (const struct sk_kde_loaded_l10nfile *domain_file,
                       const char *msgid)
{
  size_t top, act, bottom;
  struct loaded_domain *domain;

  if (domain_file->decided == 0)
    return NULL;

  if (domain_file->data == NULL)
    return NULL;

  domain = (struct loaded_domain *) domain_file->data;

  /* Locate the MSGID and its translation.  */
  if (domain->hash_size > 2 && domain->hash_tab != NULL)
  {
    /* Use the hashing table.  */
    nls_uint32 len = strlen (msgid);
    nls_uint32 hash_val = hash_string (msgid);
    nls_uint32 idx = hash_val % domain->hash_size;
    nls_uint32 incr = 1 + (hash_val % (domain->hash_size - 2));
    nls_uint32 nstr = W (domain->must_swap, domain->hash_tab[idx]);

    if (nstr == 0)
      /* Hash table entry is empty.  */
      return NULL;

    if (W (domain->must_swap, domain->orig_tab[nstr - 1].length) == len
        && strcmp (msgid,
                   domain->data + W (domain->must_swap,
                                     domain->orig_tab[nstr - 1].offset)) == 0)
      return (char *) domain->data + W (domain->must_swap,
    domain->trans_tab[nstr - 1].offset);

    while (1)
    {
      if (idx >= domain->hash_size - incr)
        idx -= domain->hash_size - incr;
      else
        idx += incr;

      nstr = W (domain->must_swap, domain->hash_tab[idx]);
      if (nstr == 0)
        /* Hash table entry is empty.  */
        return NULL;

      if (W (domain->must_swap, domain->orig_tab[nstr - 1].length) == len
          && strcmp (msgid,
                     domain->data + W (domain->must_swap,
                                       domain->orig_tab[nstr - 1].offset))
          == 0)
        return (char *) domain->data
            + W (domain->must_swap, domain->trans_tab[nstr - 1].offset);
    }
    /* NOTREACHED */
  }

  /* Now we try the default method:  binary search in the sorted
  array of messages.  */
  bottom = 0;
  top = domain->nstrings;
  act = top;
  while (bottom < top)
  {
    int cmp_val;

    act = (bottom + top) / 2;
    cmp_val = strcmp (msgid, domain->data
        + W (domain->must_swap,
             domain->orig_tab[act].offset));
    if (cmp_val < 0)
      top = act;
    else if (cmp_val > 0)
      bottom = act + 1;
    else
      break;
  }

  /* If an translation is found return this.  */
  return bottom >= top ? NULL : (char *) domain->data
      + W (domain->must_swap,
           domain->trans_tab[act].offset);
}

/* @@ begin of epilog @@ */
/* We assume to have `unsigned long int' value with at least 32 bits.  */
#define HASHWORDBITS 32

static inline unsigned long
hash_string (const char *str_param)
{
  unsigned long int hval, g;
  const char *str = str_param;

  /* Compute the hash value for the given string.  */
  hval = 0;
  while (*str != '\0')
  {
    hval <<= 4;
    hval += (unsigned long) *str++;
    g = hval & ((unsigned long) 0xf << (HASHWORDBITS - 4));
    if (g != 0)
    {
      hval ^= g >> (HASHWORDBITS - 8);
      hval ^= g;
    }
  }
  return hval;
}

/* Load the message catalogs specified by device.  If it is no valid
   message catalog do nothing.  */
void tl_nl_load_domain (QIODevice* device, int size,
                         struct sk_kde_loaded_l10nfile *domain_file)
{
  struct mo_file_header *data = (struct mo_file_header *) -1;
  struct loaded_domain *domain;

  domain_file->decided = 1;
  domain_file->data = NULL;

  /* If the record does not represent a valid locale the FILENAME
  might be NULL.  This can happen when according to the given
  specification the locale file name is different for XPG and CEN
  syntax.  */
  if (device == NULL)
    return;

  /* Try to open the addressed file.  */
  if (device->open(IO_ReadOnly) == false)
    return;

  /* We must know about the size of the file.  */
  if (size < (off_t) sizeof (struct mo_file_header))
  {
    /* Something went wrong.  */
    device->close();
    return;
  }

  /* If the data is not yet available (i.e. mmap'ed) we try to load
  it manually.  */
  if (data == (struct mo_file_header *) -1)
  {
    off_t to_read;
    char *read_ptr;

    data = (struct mo_file_header *) malloc (size);
    if (data == NULL)
      return;

    to_read = size;
    read_ptr = (char *) data;
    do
    {
      long int nb = (long int) device->readBlock (read_ptr, to_read);
      if (nb == -1)
      {
        device->close();
        return;
      }

      read_ptr += nb;
      to_read -= nb;
    }
    while (to_read > 0);

    device->close();
  }

  /* Using the magic number we can test whether it really is a message
  catalog file.  */
  if (data->magic != _MAGIC && data->magic != _MAGIC_SWAPPED)
  {
    /* The magic number is wrong: not a message catalog file.  */
    free (data);
    return;
  }

  domain_file->data
        = (struct loaded_domain *) malloc (sizeof (struct loaded_domain));
  if (domain_file->data == NULL)
    return;

  domain = (struct loaded_domain *) domain_file->data;
  domain->data = (char *) data;
  domain->must_swap = data->magic != _MAGIC;

  /* Fill in the information about the available tables.  */
  switch (W (domain->must_swap, data->revision))
  {
    case 0:
      domain->nstrings = W (domain->must_swap, data->nstrings);
      domain->orig_tab = (struct string_desc *)
            ((char *) data + W (domain->must_swap,
              data->orig_tab_offset));
      domain->trans_tab = (struct string_desc *)
            ((char *) data + W (domain->must_swap,
              data->trans_tab_offset));
      domain->hash_size = W (domain->must_swap, data->hash_tab_size);
      domain->hash_tab = (nls_uint32 *)
            ((char *) data + W (domain->must_swap,
              data->hash_tab_offset));
      break;
    default:
      /* This is an illegal revision.  */
      free (data);
      free (domain);
      domain_file->data = NULL;
      return;
  }
}

void tl_nl_unload_domain (struct loaded_domain *domain)
{
  free ((void *) domain->data);
  free (domain);
}
