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

#ifndef WIDGET_PYTHON_H
#define WIDGET_PYTHON_H

/** Widget/getWidgetPosition
*
* SYNOPSIS
*   tuple getWidgetPosition(widget)
* DESCRIPTION
*   Returns a Python Tuple containing the x and y position of you widget.
*   widget is a reference to the current widget.
* ARGUMENTS
*   * long widget -- karamba
* RETURN VALUE
*   position of the widget
*/
PyObject* py_get_widget_position(PyObject *self, PyObject *args);

/** Widget/createWidgetMask
*
* SYNOPSIS
*   long createWidgetMask(widget, mask)
* DESCRIPTION
*   This function doesn't work currently due to a bug in KDE. Please use
*   MASK= in your .theme file for the time being.
* ARGUMENTS
*   * long widget -- karamba
*   * string mask --  The name of the widget mask file.
* RETURN VALUE
*   1 if successful
*/
PyObject* py_create_widget_mask(PyObject *self, PyObject *args);

/** Widget/redrawWidgetBackground
*
* SYNOPSIS
*   long redrawWidgetBackground(widget)
* DESCRIPTION
*   Redraws widget background.
* ARGUMENTS
*   * long widget -- karamba
* RETURN VALUE
*   1 if successful
*/
PyObject* py_redraw_widget_background(PyObject *self, PyObject *args);

/** Widget/redrawWidget
*
* SYNOPSIS
*   long redrawWidget(widget)
* DESCRIPTION
*   This is THE most important function. After you do a bunch of other calls
*   (moving images, adding images or text, etc), you call this to update the
*   widget display area. You will not see your changes until you call this.
*   Redraws widget background.
* ARGUMENTS
*   * long widget -- karamba
* RETURN VALUE
*   1 if successful
*/
PyObject* py_redraw_widget(PyObject *self, PyObject *args);

/** Widget/resizeWidget
*
* SYNOPSIS
*   long resizeWidget(widget, w, h)
* DESCRIPTION
*   Resizes your karamba widget to width=w, height=h
* ARGUMENTS
*   * long widget -- karamba
*   * long w -- width
*   * long h -- height
* RETURN VALUE
*   1 if successful
*/
PyObject* py_resize_widget(PyObject *self, PyObject *args);

/** Widget/moveWidget
*
* SYNOPSIS
*   long moveWidget(widget, x, y)
* DESCRIPTION
*   Moves your karamba widget to a new screen location
* ARGUMENTS
*   * long widget -- karamba
*   * long x -- x coordinate
*   * long y -- y coordinate
* RETURN VALUE
*   1 if successful
*/
PyObject* py_move_widget(PyObject *self, PyObject *args);

/** Widget/toggleWidgetRedraw
*
* SYNOPSIS
*   long toggleWidgetRedraw(widget, b)
* DESCRIPTION
*   Toggles widget redraw.
* ARGUMENTS
*   * long widget -- karamba
*   * long b -- 1 = widget is drawn
* RETURN VALUE
*   1 if successful
*/
PyObject* py_toggle_widget_redraw(PyObject *self, PyObject *args);
PyObject* py_set_widget_on_top(PyObject *self, PyObject *args);

#endif // WIDGET_PYTHON_H
