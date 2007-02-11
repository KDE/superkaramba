/****************************************************************************
*  graph_python.cpp  -  Functions for graph python api
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

#ifndef GRAPH_PYTHON_H
#define GRAPH_PYTHON_H

/** Graph/createGraph
*
* SYNOPSIS
*   long createGraph(widget, x, y, w, h, points)
* DESCRIPTION
*   This creates a graph at x,y with width and height w,h.
* ARGUMENTS
*   * long widget -- karamba
*   * long x -- x coordinate
*   * long y -- y coordinate
*   * long w -- width
*   * long h -- height
*   * long points -- Number of points in graph
* RETURN VALUE
*   Pointer to new graph meter
*/
PyObject* py_createGraph(PyObject *self, PyObject *args);

/** Graph/deleteGraph
*
* SYNOPSIS
*   long deleteGraph(widget, graph)
* DESCRIPTION
*   This deletes graph.
* ARGUMENTS
*   * long widget -- karamba
*   * long widget -- graph
* RETURN VALUE
*   1 if successful
*/
PyObject* py_deleteGraph(PyObject *self, PyObject *args);

/** Graph/getThemeGraph
*
* SYNOPSIS
*   long getThemeGraph(widget, name)
* DESCRIPTION
*   You can reference graph in your python code that was created in the
*   theme file. Basically, you just add a NAME= value to the GRAPH line in
*   the .theme file. Then if you want to use that object, instead of calling
*   createGraph, you can call this function.
*
*   The name you pass to the function is the same one that you gave it for
*   the NAME= parameter in the .theme file.
* ARGUMENTS
*   * long widget -- karamba
*   * string name -- name of the graph to get
* RETURN VALUE
*   Pointer to graph
*/
PyObject* py_getThemeGraph(PyObject *self, PyObject *args);

/** Graph/getGraphSize
*
* SYNOPSIS
*   tuple getGraphSize(widget, graph)
* DESCRIPTION
*   Given a reference to a graph object, this will return a tuple
*   containing the height and width of a graph object.
* ARGUMENTS
*   * long widget -- karamba
*   * long graph -- pointer to graph
* RETURN VALUE
*   size
*/
PyObject* py_getGraphSize(PyObject *self, PyObject *args);

/** Graph/resizeGraph
*
* SYNOPSIS
*   long resizeGraph(widget, graph, w, h)
* DESCRIPTION
*   This will resize graph to new height and width.
* ARGUMENTS
*   * long widget -- karamba
*   * long graph -- pointer to graph
*   * long w -- new width
*   * long h -- new height
* RETURN VALUE
*   1 if successful
*/
PyObject* py_resizeGraph(PyObject *self, PyObject *args);

/** Graph/getGraphPos
*
* SYNOPSIS
*   tuple getGraphPos(widget, graph)
* DESCRIPTION
*   Given a reference to a graph object, this will return a tuple
*   containing the x and y coordinate of a graph object.
* ARGUMENTS
*   * long widget -- karamba
*   * long graph -- pointer to graph
* RETURN VALUE
*   pos
*/
PyObject* py_getGraphPos(PyObject *self, PyObject *args);

/** Graph/moveGraph
*
* SYNOPSIS
*   long moveGraph(widget, graph, x, y)
* DESCRIPTION
*   This will move graph to new x and y coordinates.
* ARGUMENTS
*   * long widget -- karamba
*   * long graph -- pointer to graph
*   * long x -- x coordinate
*   * long y -- y coordinate
* RETURN VALUE
*   1 if successful
*/
PyObject* py_moveGraph(PyObject *self, PyObject *args);

/** Graph/hideGraph
*
* SYNOPSIS
*   long hideGraph(widget, graph)
* DESCRIPTION
*   This hides an graph. In other words, during subsequent calls to
*   widgetUpdate(), this graph will not be drawn.
* ARGUMENTS
*   * long widget -- karamba
*   * long graph -- pointer to graph
* RETURN VALUE
*   1 if successful
*/
PyObject* py_hideGraph(PyObject *self, PyObject *args);

/** Graph/showGraph
*
* SYNOPSIS
*   long showGraph(widget, graph)
* DESCRIPTION
*   This shows an graph. In other words, during subsequent calls to
*   widgetUpdate(), this graph will be drawn.
* ARGUMENTS
*   * long widget -- karamba
*   * long graph -- pointer to graph
* RETURN VALUE
*   1 if successful
*/
PyObject* py_showGraph(PyObject *self, PyObject *args);

/** Graph/getGraphValue
*
* SYNOPSIS
*   long getGraphValue(widget, graph)
* DESCRIPTION
*   Returns current graph value.
* ARGUMENTS
*   * long widget -- karamba
*   * long graph -- pointer to graph
* RETURN VALUE
*   value
*/
PyObject* py_getGraphValue(PyObject *self, PyObject *args);

/** Graph/setGraphValue
*
* SYNOPSIS
*   long setGraphValue(widget, graph, value)
* DESCRIPTION
*   Sets current graph value.
* ARGUMENTS
*   * long widget -- karamba
*   * long graph -- pointer to graph
*   * long value -- new value
* RETURN VALUE
*   1 if successful
*/
PyObject* py_setGraphValue(PyObject *self, PyObject *args);

/** Graph/getGraphMinMax
*
* SYNOPSIS
*   tuple getGraphMinMax(widget, graph)
* DESCRIPTION
*   Returns current graph value.
* ARGUMENTS
*   * long widget -- karamba
*   * long graph -- pointer to graph
* RETURN VALUE
*   min & max
*/
PyObject* py_getGraphMinMax(PyObject *self, PyObject *args);

/** Graph/setGraphMinMax
*
* SYNOPSIS
*   long setGraphMinMax(widget, graph, min, max)
* DESCRIPTION
*   Returns current graph value.
* ARGUMENTS
*   * long widget -- karamba
*   * long graph -- pointer to graph
*   * long min -- min value
*   * long max -- max value
* RETURN VALUE
*   1 if successful
*/
PyObject* py_setGraphMinMax(PyObject *self, PyObject *args);

/** Graph/getGraphSensor
*
* SYNOPSIS
*   string getGraphSensor(widget, graph)
* DESCRIPTION
*   Get current sensor string
* ARGUMENTS
*   * long widget -- karamba
*   * long graph -- pointer to graph
* RETURN VALUE
*   sensor string
*/
PyObject* py_getGraphSensor(PyObject *self, PyObject *args);

/** Graph/setGraphSensor
*
* SYNOPSIS
*   long setGraphSensor(widget, graph, sensor)
* DESCRIPTION
*   Get current sensor string
* ARGUMENTS
*   * long widget -- karamba
*   * long graph -- pointer to graph
*   * string sensor -- new sensor as in theme files
* RETURN VALUE
*   1 if successful
*/
PyObject* py_setGraphSensor(PyObject *self, PyObject *args);

/** Graph/getGraphColor
*
* SYNOPSIS
*   tuple getGraphColor(widget, graph)
* DESCRIPTION
*   Get current graph color
* ARGUMENTS
*   * long widget -- karamba
*   * long graph -- pointer to graph
* RETURN VALUE
*   (red, green, blue)
*/
PyObject* py_getGraphColor(PyObject *self, PyObject *args);

/** Graph/setGraphColor
*
* SYNOPSIS
*   tuple setGraphColor(widget, graph, red, green, blue)
* DESCRIPTION
*   Set current graph color
* ARGUMENTS
*   * long widget -- karamba
*   * long graph -- pointer to graph
*   * long red -- red component of color
*   * long green -- green component of color
*   * long blue -- blue component of color
* RETURN VALUE
*   1 if successful
*/
PyObject* py_setGraphColor(PyObject *self, PyObject *args);

#endif // GRAPH_PYTHON_H
