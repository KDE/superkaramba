/****************************************************************************
*  graph_python.h  -  Functions for graph python api
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

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include <Python.h>
#include <qobject.h>
#include "karamba.h"
#include "meter.h"
#include "meter_python.h"
#include "graph_python.h"

PyObject* py_createGraph(PyObject *, PyObject *args)
{
  long widget, x, y, w, h, points;

  if (!PyArg_ParseTuple(args, (char*)"llllll", &widget, &x, &y, &w, &h, &points))
    return NULL;
  if (!checkKaramba(widget))
    return NULL;

  Graph *tmp =
      new Graph((karamba*)widget, (int)x, (int)y, (int)w, (int)h, (int)points);
  ((karamba*)widget)->meterList->append(tmp);
  return (Py_BuildValue((char*)"l", (long)tmp));
}

PyObject* py_deleteGraph(PyObject *, PyObject *args)
{
  long widget, meter;
  if (!PyArg_ParseTuple(args, (char*)"ll", &widget, &meter))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, "Graph"))
    return NULL;

  ((karamba*)widget)->deleteMeterFromSensors((Meter*)meter);
  return Py_BuildValue((char*)"l",
      ((karamba*)widget)->meterList->removeRef((Meter*)meter));
}

PyObject* py_getThemeGraph(PyObject *self, PyObject *args)
{
  return py_getThemeMeter(self, args, "Graph");
}

PyObject* py_getGraphSize(PyObject *self, PyObject *args)
{
  return py_getSize(self, args, "Graph");
}

PyObject* py_resizeGraph(PyObject *self, PyObject *args)
{
  return py_resize(self, args, "Graph");
}

PyObject* py_getGraphPos(PyObject *self, PyObject *args)
{
  return py_getPos(self, args, "Graph");
}

PyObject* py_moveGraph(PyObject *self, PyObject *args)
{
  return py_move(self, args, "Graph");
}

PyObject* py_hideGraph(PyObject *self, PyObject *args)
{
  return py_hide(self, args, "Graph");
}

PyObject* py_showGraph(PyObject *self, PyObject *args)
{
  return py_show(self, args, "Graph");
}

PyObject* py_getGraphMinMax(PyObject *self, PyObject *args)
{
  return py_getMinMax(self, args, "Graph");
}

PyObject* py_setGraphMinMax(PyObject *self, PyObject *args)
{
  return py_setMinMax(self, args, "Graph");
}

PyObject* py_getGraphValue(PyObject *self, PyObject *args)
{
  return py_getValue(self, args, "Graph");
}

PyObject* py_setGraphValue(PyObject *self, PyObject *args)
{
  return py_setValue(self, args, "Graph");
}

PyObject* py_getGraphSensor(PyObject *self, PyObject *args)
{
  return py_getSensor(self, args, "Graph");
}

PyObject* py_setGraphSensor(PyObject *self, PyObject *args)
{
  return py_setSensor(self, args, "Graph");
}

PyObject* py_getGraphColor(PyObject *self, PyObject *args)
{
  return py_getColor(self, args, "Graph");
}

PyObject* py_setGraphColor(PyObject *self, PyObject *args)
{
  return py_setColor(self, args, "Graph");
}


