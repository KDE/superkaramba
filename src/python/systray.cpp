/****************************************************************************
*  systray_python.h  -  Functions for systray python api
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
#include "systray_python.h"

long moveSystray(long widget, long x, long y, long w, long h)
{
  karamba* currTheme = (karamba*)widget;

  if (currTheme->systray != 0) {
    currTheme->systray->move((int)x,(int)y);
    currTheme->systray->setMinimumSize((int)w,(int)h);
    currTheme->systray->layoutSystray();
    currTheme->systray->show();
  }
  return 1;
}

PyObject* py_move_systray(PyObject *, PyObject *args)
{
  long widget, x, y, w, h;
  if (!PyArg_ParseTuple(args, (char*)"lllll:moveSystray", &widget, &x, &y, &w, &h))
    return NULL;
  if (!checkKaramba(widget))
    return NULL;
  return Py_BuildValue((char*)"l", moveSystray(widget, x, y, w, h));
}

/* now a method we need to expose to Python */
long showSystray(long widget)
{
  karamba* currTheme = (karamba*)widget;

  if (currTheme->systray != 0)
  {
    currTheme->systray->show();
  }
  return 1;
}

PyObject* py_show_systray(PyObject *, PyObject *args)
{
  long widget;
  if (!PyArg_ParseTuple(args, (char*)"l:showSystray", &widget))
    return NULL;
  if (!checkKaramba(widget))
    return NULL;
  return Py_BuildValue((char*)"l", showSystray(widget));
}

/* now a method we need to expose to Python */
long hideSystray(long widget)
{
  karamba* currTheme = (karamba*)widget;

  if (currTheme->systray != 0)
  {
    currTheme->systray->hide();
  }
  return 1;
}

PyObject* py_hide_systray(PyObject *, PyObject *args)
{
  long widget;
  if (!PyArg_ParseTuple(args, (char*)"l:hideSystray", &widget))
    return NULL;
  if (!checkKaramba(widget))
    return NULL;
  return Py_BuildValue((char*)"l", hideSystray(widget));
}

/* now a method we need to expose to Python */
long createSystray(long widget, long x, long y, long w, long h)
{
  karamba* currTheme = (karamba*)widget;

  //Don't create more than one systray
  if (currTheme->systray == 0) {
    currTheme->systray = new Systemtray(currTheme);
    currTheme->systray->move((int)x,(int)y);
    currTheme->systray->setMinimumSize((int)w,(int)h);
    currTheme->systray->initSystray();
    QObject::connect(currTheme->systray,SIGNAL(updated()),
                     currTheme,SLOT(systrayUpdated()));
    currTheme->systray->show();
  }

  return 1;
}

PyObject* py_create_systray(PyObject *, PyObject *args)
{
  long widget, x, y, w, h;
  if (!PyArg_ParseTuple(args, (char*)"lllll:createSystray", &widget, &x, &y, &w, &h))
    return NULL;
  if (!checkKaramba(widget))
    return NULL;
  return Py_BuildValue((char*)"l", createSystray(widget, x, y, w, h));
}

/* now a method we need to expose to Python */
long getCurrentWindowCount(long widget)
{
  karamba* currTheme = (karamba*)widget;
  int num;

  num = 0;

  if (currTheme->systray != 0)
  {
    num = currTheme->systray->getCurrentWindowCount();
  }
  return num;
}

PyObject* py_get_current_window_count(PyObject *, PyObject *args)
{
  long widget;
  if (!PyArg_ParseTuple(args, (char*)"l:getCurrentWindowCount", &widget ))
    return NULL;
  if (!checkKaramba(widget))
    return NULL;
  return Py_BuildValue((char*)"l", getCurrentWindowCount(widget));
}

/* now a method we need to expose to Python */
long updateSystrayLayout(long widget)
{
  karamba* currTheme = (karamba*)widget;

  if (currTheme->systray != 0)
  {
    currTheme->systray->layoutSystray();
  }
  return 1;
}

PyObject* py_update_systray_layout(PyObject *, PyObject *args)
{
  long widget;
  if (!PyArg_ParseTuple(args, (char*)"l:updateSystrayLayout", &widget ))
    return NULL;
  if (!checkKaramba(widget))
    return NULL;
  return Py_BuildValue((char*)"l", updateSystrayLayout(widget));
}

/* get the systray size from python */
int getSystraySize(long widget) {
	karamba* currTheme = (karamba*)widget;
	if(currTheme->systray == 0) {
		return 0;
	} else {
		return currTheme->systray->getTraySize();
	}
}

// Returns the size of the systray
PyObject* py_get_systray_size(PyObject*, PyObject* args)
{
    long widget;

    if (!PyArg_ParseTuple(args, "l:getSystraySize", &widget))
        return NULL;

    return Py_BuildValue("l", getSystraySize(widget));
}

