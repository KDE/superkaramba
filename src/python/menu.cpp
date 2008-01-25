/****************************************************************************
*  menu_python.h  -  Functions for menu python api
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

#include <kmenu.h>

#include "../karamba.h"
#include "meters/meter.h"
#include "python/meter.h"
#include "python/menu.h"

long createMenu(long widget)
{
    Karamba* currTheme = (Karamba*)widget;

    //KMenu* tmp = new KMenu();
    KMenu *tmp = currTheme->addPopupMenu();
    /*
      currTheme->connect(tmp, SIGNAL(triggered(QAction*)), currTheme,
                         SLOT(passMenuItemClicked(QAction*)));
    */
    return (long)tmp;
}

PyObject* py_create_menu(PyObject *, PyObject *args)
{
    long widget;
    if (!PyArg_ParseTuple(args, (char*)"l:createMenu", &widget))
        return NULL;
    return Py_BuildValue((char*)"l", createMenu(widget));
}

bool menuExists(Karamba* currTheme, KMenu* menu)
{
    return currTheme->popupMenuExisting(menu);
}

long deleteMenu(long widget, long menu)
{
    Karamba* currTheme = (Karamba*)widget;
    KMenu* tmp = (KMenu*)menu;

    //currTheme->removePopupMenu(tmp);
    if (menuExists(currTheme, tmp))
        currTheme->deletePopupMenu(tmp);

    return 1;
}

PyObject* py_delete_menu(PyObject *, PyObject *args)
{
    long widget, menu;
    if (!PyArg_ParseTuple(args, (char*)"ll:deleteMenu", &widget, &menu))
        return NULL;
    return Py_BuildValue((char*)"l", deleteMenu(widget, menu));
}

QAction *addMenuItem(long widget, long menu, QString text, QString icon)
{
    Karamba* currTheme = (Karamba*)widget;
    KMenu* tmp = (KMenu*)menu;

    QAction *action = 0;
    if (menuExists(currTheme, tmp)) {
        //action = tmp->addAction(KIcon(icon), text);
        action = currTheme->addMenuItem(tmp, text, icon);
    }
    return action;
}

PyObject* py_add_menu_item(PyObject *, PyObject *args)
{
    long widget, menu;
    char* i;
    PyObject* t;
    if (!PyArg_ParseTuple(args, (char*)"llOs:addMenuItem", &widget, &menu, &t, &i))
        return NULL;
    QString icon;
    QString text;
    icon = QString::fromAscii(i);
    text = PyString2QString(t);
    return Py_BuildValue((char*)"l", addMenuItem(widget, menu, text, icon));
}

QAction* addMenuSeparator(long widget, long menu)
{
    Karamba* currTheme = (Karamba*)widget;
    KMenu* tmp = (KMenu*)menu;

    QAction *sep = 0;
    if (menuExists(currTheme, tmp)) {
        sep = tmp->addSeparator();
    }

    return sep;
}

PyObject* py_add_menu_separator(PyObject *, PyObject *args)
{
    long widget, menu;

    if (!PyArg_ParseTuple(args, (char*)"ll:addMenuSeparator", &widget, &menu))
        return NULL;

    return Py_BuildValue((char*)"l", addMenuSeparator(widget, menu));
}

long removeMenuItem(long widget, long menu, QAction *action)
{
    Karamba* currTheme = (Karamba*)widget;
    KMenu* tmp = (KMenu*)menu;

    if (menuExists(currTheme, tmp)) {
        //tmp->removeAction(action);
        currTheme->deleteMenuItem(action);
        return 1;
    } else {
        return 0;
    }
}

PyObject* py_remove_menu_item(PyObject *, PyObject *args)
{
    long widget, menu, id;
    if (!PyArg_ParseTuple(args, (char*)"lll:removeMenuItem", &widget, &menu, &id))
        return NULL;
    return Py_BuildValue((char*)"l", removeMenuItem(widget, menu, (QAction*)id));
}

long popupMenu(long widget, long menu, long x, long y)
{
    Karamba* currTheme = (Karamba*)widget;
    KMenu* tmp = (KMenu*)menu;

    if (menuExists(currTheme, tmp)) {
        ///tmp->popup(tmp->mapToGlobal( QPoint(x,y) ));
        currTheme->popupMenu(tmp, QPoint(x, y));
        return 1;
    } else {
        return 0;
    }
}

PyObject* py_popup_menu(PyObject *, PyObject *args)
{
    long widget, menu, x, y;
    if (!PyArg_ParseTuple(args, (char*)"llll:popupMenu", &widget, &menu, &x, &y))
        return NULL;
    return Py_BuildValue((char*)"l", popupMenu(widget, menu, x, y));
}

