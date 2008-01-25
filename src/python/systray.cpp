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

#if defined(_XOPEN_SOURCE) && !defined(__SUNPRO_CC)
#undef _XOPEN_SOURCE
#endif

#include <Python.h>
#include <QObject>
#include "../karamba.h"
#include "../meters/meter.h"
#include "python/meter.h"
#include "python/systray.h"

#include <KDebug>

#if 0
long moveSystray(long widget, long x, long y, long w, long h)
{
    Karamba* currTheme = (Karamba*)widget;

    if (currTheme->systemTray() != 0) {
        currTheme->systemTray()->move((int)x, (int)y);
        currTheme->systemTray()->setMinimumSize((int)w, (int)h);
        currTheme->systemTray()->layoutsystemTray();
        currTheme->systemTray()->show();
    }
    return 1;
}
#endif

PyObject* py_move_systray(PyObject *, PyObject *args)
{
    Q_UNUSED(args);

    static bool firstMessage = false;

    if (!firstMessage) {
        kWarning() << "Call to \"moveSystray\" not available in this version of SuperKaramba" ;
        firstMessage = true;
    }

    return 0;

#if 0
    long widget, x, y, w, h;
    if (!PyArg_ParseTuple(args, (char*)"lllll:moveSystray", &widget, &x, &y, &w, &h))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;
    return Py_BuildValue((char*)"l", moveSystray(widget, x, y, w, h));
#endif
}

/* now a method we need to expose to Python */
#if 0
long showSystray(long widget)
{
    Karamba* currTheme = (Karamba*)widget;

    if (currTheme->systemTray() != 0) {
        currTheme->systemTray()->show();
    }
    return 1;
}
#endif

PyObject* py_show_systray(PyObject *, PyObject *args)
{
    Q_UNUSED(args);

    static bool firstMessage = false;

    if (!firstMessage) {
        kWarning() << "Call to \"moveSystray\" not available in this version of SuperKaramba" ;
        firstMessage = true;
    }

    return 0;

#if 0
    long widget;
    if (!PyArg_ParseTuple(args, (char*)"l:showSystray", &widget))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;
    return Py_BuildValue((char*)"l", showSystray(widget));
#endif
}

/* now a method we need to expose to Python */
#if 0
long hideSystray(long widget)
{
    Karamba* currTheme = (Karamba*)widget;

    if (currTheme->systemTray() != 0) {
        currTheme->systemTray()->hide();
    }
    return 1;
}
#endif

PyObject* py_hide_systray(PyObject *, PyObject *args)
{
    Q_UNUSED(args);

    static bool firstMessage = false;

    if (!firstMessage) {
        kWarning() << "Call to \"moveSystray\" not available in this version of SuperKaramba" ;
        firstMessage = true;
    }

    return 0;

#if 0
    long widget;
    if (!PyArg_ParseTuple(args, (char*)"l:hideSystray", &widget))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;
    return Py_BuildValue((char*)"l", hideSystray(widget));
#endif
}

/* now a method we need to expose to Python */
#if 0
long createSystray(long widget, long x, long y, long w, long h)
{
    Karamba* currTheme = (Karamba*)widget;

    //Don't create more than one systray
    if (currTheme->systemTray() == 0) {
        currTheme->systemTray() = new Systemtray(currTheme);
        currTheme->systemTray()->move((int)x, (int)y);
        currTheme->systemTray()->setMinimumSize((int)w, (int)h);
        currTheme->systemTray()->initsystemTray();
        QObject::connect(currTheme->systemTray(), SIGNAL(updated()),
                         currTheme, SLOT(systrayUpdated()));
        currTheme->systemTray()->show();
    }

    return 1;
}
#endif

PyObject* py_create_systray(PyObject *, PyObject *args)
{
    Q_UNUSED(args);

    static bool firstMessage = false;

    if (!firstMessage) {
        kWarning() << "Call to \"moveSystray\" not available in this version of SuperKaramba" ;
        firstMessage = true;
    }

    return 0;

#if 0
    long widget, x, y, w, h;
    if (!PyArg_ParseTuple(args, (char*)"lllll:createSystray", &widget, &x, &y, &w, &h))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;
    return Py_BuildValue((char*)"l", createSystray(widget, x, y, w, h));
#endif
}

/* now a method we need to expose to Python */
#if 0
long getCurrentWindowCount(long widget)
{
    Karamba* currTheme = (Karamba*)widget;
    int num;

    num = 0;

    if (currTheme->systemTray() != 0) {
        num = currTheme->systemTray()->getCurrentWindowCount();
    }
    return num;
}
#endif

PyObject* py_get_current_window_count(PyObject *, PyObject *args)
{
    Q_UNUSED(args);

    static bool firstMessage = false;

    if (!firstMessage) {
        kWarning() << "Call to \"moveSystray\" not available in this version of SuperKaramba" ;
        firstMessage = true;
    }

    return 0;

#if 0
    long widget;
    if (!PyArg_ParseTuple(args, (char*)"l:getCurrentWindowCount", &widget))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;
    return Py_BuildValue((char*)"l", getCurrentWindowCount(widget));
#endif
}

/* now a method we need to expose to Python */
#if 0
long updateSystrayLayout(long widget)
{
    Karamba* currTheme = (Karamba*)widget;

    if (currTheme->systemTray() != 0) {
        currTheme->systemTray()->layoutsystemTray();
    }
    return 1;
}
#endif

PyObject* py_update_systray_layout(PyObject *, PyObject *args)
{
    Q_UNUSED(args);

    static bool firstMessage = false;

    if (!firstMessage) {
        kWarning() << "Call to \"moveSystray\" not available in this version of SuperKaramba" ;
        firstMessage = true;
    }

    return 0;

#if 0
    long widget;
    if (!PyArg_ParseTuple(args, (char*)"l:updateSystrayLayout", &widget))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;
    return Py_BuildValue((char*)"l", updateSystrayLayout(widget));
#endif
}

/* get the systray size from python */
#if 0
int getSystraySize(long widget)
{
    Karamba* currTheme = (Karamba*)widget;
    if (currTheme->systemTray() == 0) {
        return 0;
    } else {
        return currTheme->systemTray()->getTraySize();
    }
}
#endif

// Returns the size of the systray
PyObject* py_get_systray_size(PyObject*, PyObject* args)
{
    Q_UNUSED(args);

    static bool firstMessage = false;

    if (!firstMessage) {
        kWarning() << "Call to \"moveSystray\" not available in this version of SuperKaramba" ;
        firstMessage = true;
    }

    return 0;

#if 0
    long widget;

    if (!PyArg_ParseTuple(args, "l:getSystraySize", &widget))
        return NULL;

    return Py_BuildValue("l", getSystraySize(widget));
#endif
}

