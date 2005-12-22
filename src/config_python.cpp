/****************************************************************************
*  config_python.cpp  -  Functions for config python api
*
*  Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>
*  Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>
*  Copyright (c) 2004 Petri Damstén <damu@iki.fi>
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

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include <Python.h>
#include <qobject.h>
#include "karamba.h"
#include "meter.h"
#include "meter_python.h"
#include "config_python.h"

// API-Function addMenuConfigOption
long addMenuConfigOption(long widget, QString key, QString name)
{
  karamba* currTheme = (karamba*)widget;

  currTheme -> addMenuConfigOption(key, name);

  return 1;
}

PyObject* py_add_menu_config_option(PyObject *, PyObject *args)
{
  long widget;
  char* key;
  PyObject* name;

  if (!PyArg_ParseTuple(args, (char*)"lsO:addMenuConfigOption", &widget, &key, &name))
    return NULL;
  if (!checkKaramba(widget))
    return NULL;

  QString k, n;
  k.setAscii(key);
  n = PyString2QString(name);

  return Py_BuildValue((char*)"l", addMenuConfigOption(widget, k, n));
}

long setMenuConfigOption(long widget, QString key, bool value)
{
  karamba* currTheme = (karamba*)widget;

  return currTheme -> setMenuConfigOption(key, value);
}

PyObject* py_set_menu_config_option(PyObject *, PyObject *args)
{
  long widget;
  char* key;
  int value;

  if (!PyArg_ParseTuple(args, (char*)"lsi:setMenuConfigOption", &widget, &key, &value))
    return NULL;
  if (!checkKaramba(widget))
    return NULL;

  QString k;
  k.setAscii(key);

  return Py_BuildValue((char*)"l", setMenuConfigOption(widget, k, (bool)value));
}

long readMenuConfigOption(long widget, QString key)
{
  karamba* currTheme = (karamba*)widget;

  return currTheme -> readMenuConfigOption(key);
}

PyObject* py_read_menu_config_option(PyObject *, PyObject *args)
{
  long widget;
  char* key;

  if (!PyArg_ParseTuple(args, (char*)"ls:readMenuConfigOption", &widget, &key))
    return NULL;
  if (!checkKaramba(widget))
    return NULL;

  QString k;
  k.setAscii(key);

  return Py_BuildValue((char*)"l", readMenuConfigOption(widget, k));
}

// API-Function writeConfigEntry
long writeConfigEntry(long widget, QString key, QString value)
{
  karamba* currTheme = (karamba*)widget;

  currTheme -> config -> setGroup("theme");
  currTheme -> config -> writeEntry(key, value);

  return 1;
}

PyObject* py_write_config_entry(PyObject *, PyObject *args)
{
  long widget;
  char* key;
  char* value;

  if (!PyArg_ParseTuple(args, (char*)"lss:writeConfigEntry", &widget, &key, &value))
    return NULL;
  if (!checkKaramba(widget))
    return NULL;
  QString k, v;
  k.setAscii(key);
  v.setAscii(value);

  return Py_BuildValue((char*)"l", writeConfigEntry(widget, k, value));
}

// API-Function readConfigEntry
QVariant readConfigEntry(long widget, QString key)
{
  karamba* currTheme = (karamba*)widget;

  currTheme -> config -> setGroup("theme");
  return currTheme -> config -> readEntry(key);
}

PyObject* py_read_config_entry(PyObject *, PyObject *args)
{
  long widget;
  char* key;
  if (!PyArg_ParseTuple(args, (char*)"ls:readConfigEntry", &widget, &key))
    return NULL;
  if (!checkKaramba(widget))
    return NULL;
  QString k;
  k.setAscii(key);

  QVariant entry = readConfigEntry(widget, k);
  QString type;
  type.setAscii(entry.typeName());

  if (type == "Bool")
  {
    return Py_BuildValue((char*)"l", (int)entry.toBool());
  }

  bool isint = false;
  int i = entry.toInt(&isint);
  if (isint)
  {
    return Py_BuildValue((char*)"l", i);
  }

  if (type == "QString")
  {
    return Py_BuildValue((char*)"s", entry.toString().ascii());
  }
  // Add more types if needed
  return NULL;
}

