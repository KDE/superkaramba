/****************************************************************************
*  bar_python.cpp  -  Functions for bar python api
*
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

#ifndef BAR_PYTHON_H
#define BAR_PYTHON_H

/** Bar/createBar
*
* SYNOPSIS
*   long createBar(widget, x, y, w, h, image)
* DESCRIPTION
*   This creates a bar at x,y with width and height w,h.
* ARGUMENTS
*   * long widget -- karamba
*   * long x -- x coordinate
*   * long y -- y coordinate
*   * long w -- width
*   * long h -- height
*   * string image -- Path to image
* RETURN VALUE
*   Pointer to new bar meter
*/
PyObject* py_createBar(PyObject *self, PyObject *args);

/** Bar/deleteBar
*
* SYNOPSIS
*   long deleteBar(widget, bar)
* DESCRIPTION
*   This deletes bar.
* ARGUMENTS
*   * long widget -- karamba
*   * long widget -- bar
* RETURN VALUE
*   1 if successful
*/
PyObject* py_deleteBar(PyObject *self, PyObject *args);

/** Bar/getThemeBar
*
* SYNOPSIS
*   long getThemeBar(widget, name)
* DESCRIPTION
*   You can reference bar in your python code that was created in the
*   theme file. Basically, you just add a NAME= value to the BAR line in
*   the .theme file. Then if you want to use that object, instead of calling
*   createBar, you can call this function.
*
*   The name you pass to the function is the same one that you gave it for
*   the NAME= parameter in the .theme file.
* ARGUMENTS
*   * long widget -- karamba
*   * string name -- name of the bar to get
* RETURN VALUE
*   Pointer to bar
*/
PyObject* py_getThemeBar(PyObject *self, PyObject *args);

/** Bar/getBarSize
*
* SYNOPSIS
*   tuple getBarSize(widget, bar)
* DESCRIPTION
*   Given a reference to a bar object, this will return a tuple
*   containing the height and width of a bar object.
* ARGUMENTS
*   * long widget -- karamba
*   * long bar -- pointer to bar
* RETURN VALUE
*   size
*/
PyObject* py_getBarSize(PyObject *self, PyObject *args);

/** Bar/resizeBar
*
* SYNOPSIS
*   long resizeBar(widget, bar, w, h)
* DESCRIPTION
*   This will resize bar to new height and width.
* ARGUMENTS
*   * long widget -- karamba
*   * long bar -- pointer to bar
*   * long w -- new width
*   * long h -- new height
* RETURN VALUE
*   1 if successful
*/
PyObject* py_resizeBar(PyObject *self, PyObject *args);

/** Bar/getBarPos
*
* SYNOPSIS
*   tuple getBarPos(widget, bar)
* DESCRIPTION
*   Given a reference to a bar object, this will return a tuple
*   containing the x and y coordinate of a bar object.
* ARGUMENTS
*   * long widget -- karamba
*   * long bar -- pointer to bar
* RETURN VALUE
*   pos
*/
PyObject* py_getBarPos(PyObject *self, PyObject *args);

/** Bar/moveBar
*
* SYNOPSIS
*   long moveBar(widget, bar, x, y)
* DESCRIPTION
*   This will move bar to new x and y coordinates.
* ARGUMENTS
*   * long widget -- karamba
*   * long bar -- pointer to bar
*   * long x -- x coordinate
*   * long y -- y coordinate
* RETURN VALUE
*   1 if successful
*/
PyObject* py_moveBar(PyObject *self, PyObject *args);

/** Bar/hideBar
*
* SYNOPSIS
*   long hideBar(widget, bar)
* DESCRIPTION
*   This hides an bar. In other words, during subsequent calls to
*   widgetUpdate(), this bar will not be drawn.
* ARGUMENTS
*   * long widget -- karamba
*   * long bar -- pointer to bar
* RETURN VALUE
*   1 if successful
*/
PyObject* py_hideBar(PyObject *self, PyObject *args);

/** Bar/showBar
*
* SYNOPSIS
*   long showBar(widget, bar)
* DESCRIPTION
*   This shows an bar. In other words, during subsequent calls to
*   widgetUpdate(), this bar will be drawn.
* ARGUMENTS
*   * long widget -- karamba
*   * long bar -- pointer to bar
* RETURN VALUE
*   1 if successful
*/
PyObject* py_showBar(PyObject *self, PyObject *args);

/** Bar/getBarValue
*
* SYNOPSIS
*   long getBarValue(widget, bar)
* DESCRIPTION
*   Returns current bar value.
* ARGUMENTS
*   * long widget -- karamba
*   * long bar -- pointer to bar
* RETURN VALUE
*   value
*/
PyObject* py_getBarValue(PyObject *self, PyObject *args);

/** Bar/setBarValue
*
* SYNOPSIS
*   long setBarValue(widget, bar, value)
* DESCRIPTION
*   Sets current bar value.
* ARGUMENTS
*   * long widget -- karamba
*   * long bar -- pointer to bar
*   * long value -- new value
* RETURN VALUE
*   1 if successful
*/
PyObject* py_setBarValue(PyObject *self, PyObject *args);

/** Bar/getBarMinMax
*
* SYNOPSIS
*   tuple getBarMinMax(widget, bar)
* DESCRIPTION
*   Returns current bar value.
* ARGUMENTS
*   * long widget -- karamba
*   * long bar -- pointer to bar
* RETURN VALUE
*   min & max
*/
PyObject* py_getBarMinMax(PyObject *self, PyObject *args);

/** Bar/setBarMinMax
*
* SYNOPSIS
*   long setBarMinMax(widget, bar, min, max)
* DESCRIPTION
*   Returns current bar value.
* ARGUMENTS
*   * long widget -- karamba
*   * long bar -- pointer to bar
*   * long min -- min value
*   * long max -- max value
* RETURN VALUE
*   1 if successful
*/
PyObject* py_setBarMinMax(PyObject *self, PyObject *args);

/** Bar/getBarSensor
*
* SYNOPSIS
*   string getBarSensor(widget, bar)
* DESCRIPTION
*   Get current sensor string
* ARGUMENTS
*   * long widget -- karamba
*   * long bar -- pointer to bar
* RETURN VALUE
*   sensor string
*/
PyObject* py_getBarSensor(PyObject *self, PyObject *args);

/** Bar/setBarSensor
*
* SYNOPSIS
*   long setBarSensor(widget, bar, sensor)
* DESCRIPTION
*   Get current sensor string
* ARGUMENTS
*   * long widget -- karamba
*   * long bar -- pointer to bar
*   * string sensor -- new sensor as in theme files
* RETURN VALUE
*   1 if successful
*/
PyObject* py_setBarSensor(PyObject *self, PyObject *args);

/** Bar/getBarImage
*
* SYNOPSIS
*   string getBarImage(widget, bar)
* DESCRIPTION
*   Get bar image
* ARGUMENTS
*   * long widget -- karamba
*   * long bar -- pointer to bar
* RETURN VALUE
*   path to bar image
*/
PyObject* py_getBarImage(PyObject *self, PyObject *args);

/** Bar/setBarImage
*
* SYNOPSIS
*   long setBarImage(widget, bar, image)
* DESCRIPTION
*   Get bar image
* ARGUMENTS
*   * long widget -- karamba
*   * long bar -- pointer to bar
*   * string image -- new image
* RETURN VALUE
*   1 if successful
*/
PyObject* py_setBarImage(PyObject *self, PyObject *args);

/** Bar/getBarVertical
*
* SYNOPSIS
*   string getBarVertical(widget, bar)
* DESCRIPTION
*   Check if bar is vertical bar
* ARGUMENTS
*   * long widget -- karamba
*   * long bar -- pointer to bar
* RETURN VALUE
*   1 if vertical
*/
PyObject* py_getBarVertical(PyObject *self, PyObject *args);

/** Bar/setBarVertical
*
* SYNOPSIS
*   long setBarVertical(widget, bar)
* DESCRIPTION
*   Set bar vertical
* ARGUMENTS
*   * long widget -- karamba
*   * long bar -- pointer to bar
*   * long vertical -- 1 if vertical
* RETURN VALUE
*   1 if successful
*/
PyObject* py_setBarVertical(PyObject *self, PyObject *args);

#endif // BAR_PYTHON_H
