/****************************************************************************
*  config_python.cpp  -  Functions for config python api
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

#if defined(_XOPEN_SOURCE) && !defined(__SUNPRO_CC)
#undef _XOPEN_SOURCE
#endif

#include <Python.h>

#include <QObject>

#include <kconfig.h>

#include "meters/meter.h"

#include "python/meter.h"
#include "python/config.h"

#include <kconfiggroup.h>

#include "../karamba.h"

// API-Function addMenuConfigOption
long addMenuConfigOption(long widget, QString key, QString name)
{
    Karamba* currTheme = (Karamba*)widget;

    currTheme->addMenuConfigOption(key, name);

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
    k = QString::fromAscii(key);
    n = PyString2QString(name);

    return Py_BuildValue((char*)"l", addMenuConfigOption(widget, k, n));
}

long setMenuConfigOption(long widget, QString key, bool value)
{
    Karamba* currTheme = (Karamba*)widget;

    return currTheme->setMenuConfigOption(key, value);
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
    k = QString::fromAscii(key);

    return Py_BuildValue((char*)"l", setMenuConfigOption(widget, k, (bool)value));
}

long readMenuConfigOption(long widget, QString key)
{
    Karamba* currTheme = (Karamba*)widget;

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
    k = QString::fromAscii(key);

    return Py_BuildValue((char*)"l", readMenuConfigOption(widget, k));
}

// API-Function writeConfigEntry
long writeConfigEntry(long widget, QString key, QString value)
{
    Karamba* currTheme = (Karamba*)widget;

    currTheme->getConfig()->group("theme").writeEntry(key, value);

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
    k = QString::fromAscii(key);
    v = QString::fromAscii(value);

    return Py_BuildValue((char*)"l", writeConfigEntry(widget, k, value));
}

// API-Function readConfigEntry
QString readConfigEntry(long widget, QString key)
{
    Karamba* currTheme = (Karamba*)widget;

    return currTheme->getConfig()->group("theme").readEntry(key, QString());
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
    k = QString::fromAscii(key);

    QString entry = readConfigEntry(widget, k);

    if (entry.isEmpty())
        return Py_BuildValue((char*)"");

    if (entry.startsWith("false", Qt::CaseInsensitive))
        return Py_BuildValue((char*)"l", 0);

    if (entry.startsWith("true", Qt::CaseInsensitive))
        return Py_BuildValue((char*)"l", 0);

    bool ok;
    if (entry.toInt(&ok))
        return Py_BuildValue((char*)"l", entry.toInt());

    return Py_BuildValue((char*)"s",
                         entry.toAscii().constData());

    return NULL;
}

