/****************************************************************************
*  widget_python.h  -  Functions for widget python api
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
#include <qobject.h>
#include "../karamba.h"
#include "meters/meter.h"
#include "python/meter.h"
#include "python/widget.h"

PyObject* py_get_widget_position(PyObject *, PyObject *args)
{
    long widget;
    if (!PyArg_ParseTuple(args, (char*)"l:getWidgetPosition", &widget))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;

    QPoint pos = ((Karamba*)widget)->getPosition();
    return Py_BuildValue((char*)"(i,i)", pos.x(), pos.y());
}

// Keep compatibility
PyObject* py_create_widget_mask(PyObject *, PyObject *args)
{
    long widget;
    char *text;
    if (!PyArg_ParseTuple(args, (char*)"ls:createWidgetMask", &widget, &text))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;
    return Py_BuildValue((char*)"l", 1);
}

// Keep compatibility
PyObject* py_redraw_widget_background(PyObject *, PyObject *args)
{
    long widget;
    if (!PyArg_ParseTuple(args, (char*)"l:redrawWidgetBackground", &widget))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;
    return Py_BuildValue((char*)"l", 1);
}

/* now a method we need to expose to Python */
long redrawWidget(long widget)
{
    Karamba* currTheme = (Karamba*)widget;
    foreach(QGraphicsItem *item, ((QGraphicsItemGroup*)currTheme)->children())
    item->update();

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
    Karamba* currTheme = (Karamba*)widget;
    //currTheme->test = true;
    currTheme->resizeTo((int)x, (int)y);
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
    Karamba* currTheme = (Karamba*)widget;
    currTheme->moveToPos(QPoint((int)x, (int)y));
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
long widgetSetOnTop(long widget, bool b)
{
    Karamba* currTheme = (Karamba*)widget;

    if (currTheme != 0)
    {
      currTheme->setOnTop(b);
    }

    return 1;
}

PyObject* py_set_widget_on_top(PyObject *, PyObject *args)
{
    long widget;
    long b;
    if (!PyArg_ParseTuple(args, (char*)"ll:setWidgetOnTop", &widget, &b))
        return NULL;
    return Py_BuildValue((char*)"l", widgetSetOnTop(widget, b));
}

// Keep compatibility
PyObject* py_toggle_widget_redraw(PyObject *, PyObject *args)
{
    long widget, b;

    if (!PyArg_ParseTuple(args, (char*)"ll:toggleWidgetRedraw", &widget, &b))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;
    return Py_BuildValue((char*)"l", 0);
}
