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

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include <Python.h>
#include <qobject.h>
#include "karamba.h"
#include "meter.h"
#include "meter_python.h"
#include "menu_python.h"

long createMenu(long widget)
{
  karamba* currTheme = (karamba*)widget;

  KMenu* tmp = new KMenu(currTheme);
  currTheme->menuList->append (tmp );

  currTheme->connect(tmp, SIGNAL(activated(int)), currTheme,
                     SLOT(passMenuItemClicked(int)));

  return (long)tmp;
}

PyObject* py_create_menu(PyObject *, PyObject *args)
{
  long widget;
  if (!PyArg_ParseTuple(args, (char*)"l:createMenu", &widget))
    return NULL;
  return Py_BuildValue((char*)"l", createMenu(widget));
}

bool menuExists(karamba* currTheme, KMenu* menu)
{
  bool foundMenu = false;

  /*
  KDE4 NEEDS TO BE TESTED
  for(int i = 0; i < (int)currTheme->menuList->count(); i++)
  {
    if(i==0)
    {
      tmp = (KMenu*) currTheme->menuList->first();
    }
    else
    {
      tmp = (KMenu*) currTheme->menuList->next();
    }
    if(tmp != 0)
    {
      if(tmp == menu)
      {
        foundMenu = true;
        break;
      }
    }
  }
  return foundMenu;
  */
  
  QObject *tmp;
  foreach(tmp, *currTheme->menuList)
  {
    if(tmp != 0)
    {
      if(qobject_cast<KMenu*>(tmp) == menu)
      {
        foundMenu = true;
        break;
      }
    }
  }
  
  return foundMenu;
}

long deleteMenu(long widget, long menu)
{
  karamba* currTheme = (karamba*)widget;
  KMenu* tmp = (KMenu*)menu;

  currTheme->menuList->removeAll(tmp);

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
  karamba* currTheme = (karamba*)widget;
  KMenu* tmp = (KMenu*)menu;

  QAction *action = 0;
  if(menuExists(currTheme, tmp))
  {
    action = tmp->addAction(SmallIconSet(icon), text);
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
  karamba* currTheme = (karamba*)widget;
  KMenu* tmp = (KMenu*)menu;

  QAction *sep = 0;
  if(menuExists(currTheme, tmp))
  {
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
  karamba* currTheme = (karamba*)widget;
  KMenu* tmp = (KMenu*)menu;

  if(menuExists(currTheme,tmp))
  {
    tmp->removeAction(action);
    return 1;
  }
  else
  {
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
  karamba* currTheme = (karamba*)widget;
  KMenu* tmp = (KMenu*)menu;

  if(menuExists(currTheme,tmp))
  {
    tmp->popup(currTheme->mapToGlobal( QPoint(x,y) ));
    return 1;
  }
  else
  {
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

