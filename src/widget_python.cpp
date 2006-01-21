/****************************************************************************
*  widget_python.h  -  Functions for widget python api
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
#include "widget_python.h"

/* now a method we need to expose to Python */
int getWidgetXCoordinate(long widget)
{
  karamba* currTheme = (karamba*)widget;
  return currTheme->x();
}

/* now a method we need to expose to Python */
int getWidgetYCoordinate(long widget)
{
  karamba* currTheme = (karamba*)widget;
  return currTheme->y();
}

PyObject* py_get_widget_position(PyObject *, PyObject *args)
{
  long widget;
  if(!PyArg_ParseTuple(args, (char*)"l:getWidgetPosition", &widget))
    return NULL;
  if (!checkKaramba(widget))
    return NULL;
  return Py_BuildValue((char*)"(i,i)", getWidgetXCoordinate(widget),
                                getWidgetYCoordinate(widget));
}

/* now a method we need to expose to Python */
long createWidgetMask(long widget, char* path)
{
  karamba* currTheme = (karamba*)widget;
  QBitmap bm;
  QString maskpath;
  QString rootPath;
  rootPath.setAscii(currTheme->theme().path().ascii());

  currTheme->clearMask();

  maskpath.setAscii(path);
  rootPath.append(maskpath.ascii());

  if(currTheme->theme().isZipTheme())
  {
    QByteArray ba = currTheme->theme().readThemeFile(path);
    bm.loadFromData(ba);
  }
  else
  {
    bm.load(rootPath);
  }
  currTheme->setMask(bm);

  return (long)currTheme->widgetMask;
}

PyObject* py_create_widget_mask(PyObject *, PyObject *args)
{
  long widget;
  char *text;
  if (!PyArg_ParseTuple(args, (char*)"ls:createWidgetMask", &widget, &text))
    return NULL;
  if (!checkKaramba(widget))
    return NULL;
  return Py_BuildValue((char*)"l", createWidgetMask(widget, text));
}

/* now a method we need to expose to Python */
long redrawWidgetBackground(long widget)
{
  karamba* currTheme = (karamba*)widget;
  currTheme->kroot->repaint(true);
  return 1;
}

PyObject* py_redraw_widget_background(PyObject *, PyObject *args)
{
  long widget;
  if (!PyArg_ParseTuple(args, (char*)"l:redrawWidgetBackground", &widget))
    return NULL;
  if (!checkKaramba(widget))
    return NULL;
  return Py_BuildValue((char*)"l", redrawWidgetBackground(widget));
}

/* now a method we need to expose to Python */
long redrawWidget(long widget)
{
  karamba* currTheme = (karamba*)widget;
  currTheme->externalStep();
  return 1;
}

PyObject* py_redraw_widget(PyObject *, PyObject *args)
{
  long widget;
  if (!PyArg_ParseTuple(args, (char*)"l:redrawWidget", &widget))
    return NULL;
  if (!checkKaramba(widget))
    return NULL;
  return Py_BuildValue((char*)"l", redrawWidget(widget));
}

/* now a method we need to expose to Python */
long resizeWidget(long widget, long x, long y)
{
  karamba* currTheme = (karamba*)widget;
  //currTheme->test = true;
  currTheme->setFixedSize((int)x,(int)y);
  //currTheme->test = false;
  return 1;
}

PyObject* py_resize_widget(PyObject *, PyObject *args)
{
  long widget, x, y;
  if (!PyArg_ParseTuple(args, (char*)"lll:resizeWidget", &widget, &x, &y))
    return NULL;
  if (!checkKaramba(widget))
    return NULL;
  return Py_BuildValue((char*)"l", resizeWidget(widget, x, y));
}

/* now a method we need to expose to Python */
long moveWidget(long widget, long x, long y)
{
  karamba* currTheme = (karamba*)widget;
  currTheme->move((int)x, (int)y);
  return 1;
}

PyObject* py_move_widget(PyObject *, PyObject *args)
{
  long widget, x, y;
  if (!PyArg_ParseTuple(args, (char*)"lll:moveWidget", &widget, &x, &y))
    return NULL;
  if (!checkKaramba(widget))
    return NULL;
  return Py_BuildValue((char*)"l", moveWidget(widget, x, y));
}

/* now a method we need to expose to Python */
long widgetSetOnTop(long widget, bool b) {
  karamba* currTheme = (karamba*)widget;

  if (currTheme != 0)
  {
    currTheme->setAlwaysOnTop(b);
  }
  return 1;
}

PyObject* py_set_widget_on_top(PyObject *, PyObject *args)
{
  long widget;
  long b;
  if (!PyArg_ParseTuple(args, (char*)"ll:setWidgetOnTop", &widget, &b ))
    return NULL;
  return Py_BuildValue((char*)"l", widgetSetOnTop(widget, b));
}

/* now a method we need to expose to Python */
long toggleWidgetRedraw(long widget, bool b)
{
  karamba* currTheme = (karamba*)widget;
  if (currTheme != 0)
  {
    currTheme->toggleWidgetUpdate( b );
  }
  return 0;
}

PyObject* py_toggle_widget_redraw(PyObject *, PyObject *args)
{
  long widget, b;

  if (!PyArg_ParseTuple(args, (char*)"ll:toggleWidgetRedraw", &widget, &b ))
    return NULL;
  if (!checkKaramba(widget))
    return NULL;
  return Py_BuildValue((char*)"l", toggleWidgetRedraw(widget, b));
}
