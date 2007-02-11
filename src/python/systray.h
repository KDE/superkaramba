/****************************************************************************
*  systray_python.h  -  Functions for systray python api
*
*  Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>
*  Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>
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

#ifndef SYSTRAY_PYTHON_H
#define SYSTRAY_PYTHON_H

struct _object;
typedef _object PyObject;

/** Systray/moveSystray
*
* SYNOPSIS
*   long moveSystray(widget, x, y, w, h)
* DESCRIPTION
*   ??
* ARGUMENTS
*   * long widget -- karamba
*   * long x -- x coordinate
*   * long y -- y coordinate
*   * long w -- width
*   * long h -- height
* RETURN VALUE
*   1 if successful
*/
PyObject* py_move_systray(PyObject *self, PyObject *args);

/** Systray/showSystray
*
* SYNOPSIS
*   long showSystray(widget)
* DESCRIPTION
*   ??
* ARGUMENTS
*   * long widget -- karamba
* RETURN VALUE
*   1 if successful
*/
PyObject* py_show_systray(PyObject *self, PyObject *args);

/** Systray/hideSystray
*
* SYNOPSIS
*   long hideSystray(widget)
* DESCRIPTION
*   ??
* ARGUMENTS
*   * long widget -- karamba
* RETURN VALUE
*   1 if successful
*/
PyObject* py_hide_systray(PyObject *self, PyObject *args);

/** Systray/createSystray
*
* SYNOPSIS
*   long createSystray(widget, x, y, w, h)
* DESCRIPTION
*   ??
* ARGUMENTS
*   * long widget -- karamba
*   * long x -- x coordinate
*   * long y -- y coordinate
*   * long w -- width
*   * long h -- height
* RETURN VALUE
*   1 if successful
*/
PyObject* py_create_systray(PyObject *self, PyObject *args);

/** Systray/getCurrentWindowCount
*
* SYNOPSIS
*   long getCurrentWindowCount(widget)
* DESCRIPTION
*   ??
* ARGUMENTS
*   * long widget -- karamba
* RETURN VALUE
*   window count
*/
PyObject* py_get_current_window_count(PyObject *self, PyObject *args);

/** Systray/updateSystrayLayout
*
* SYNOPSIS
*   long getCurrentWindowCount(widget)
* DESCRIPTION
*   ??
* ARGUMENTS
*   * long widget -- karamba
* RETURN VALUE
*   1 if successful
*/
PyObject* py_update_systray_layout(PyObject *self, PyObject *args);
PyObject* py_get_systray_size(PyObject *self, PyObject *args);

#endif // SYSTRAY_PYTHON_H
