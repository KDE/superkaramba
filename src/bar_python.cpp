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

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include <Python.h>
#include <qobject.h>
#include "karamba.h"
#include "meter.h"
#include "meter_python.h"
#include "bar_python.h"

PyObject* py_createBar(PyObject *, PyObject *args)
{
  long widget, x, y, w, h;
  char *text;
  if (!PyArg_ParseTuple(args, (char*)"lllll|s", &widget, &x, &y, &w, &h, &text))
    return NULL;
  if (!checkKaramba(widget))
    return NULL;

  Bar *tmp = new Bar((karamba*)widget, x,y,w,h);
  if (text && text[0] != '\0')
    tmp->setImage(text);
  ((karamba*)widget)->meterList->append(tmp);
  return (Py_BuildValue((char*)"l", (long)tmp));
}

PyObject* py_deleteBar(PyObject *, PyObject *args)
{
  long widget, meter;
  if (!PyArg_ParseTuple(args, (char*)"ll", &widget, &meter))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, "Bar"))
    return NULL;

  ((karamba*)widget)->deleteMeterFromSensors((Meter*)meter);
  return Py_BuildValue((char*)"l",
      ((karamba*)widget)->meterList->removeRef((Meter*)meter));
}

PyObject* py_getThemeBar(PyObject *self, PyObject *args)
{
  return py_getThemeMeter(self, args, "Bar");
}

PyObject* py_getBarSize(PyObject *self, PyObject *args)
{
  return py_getSize(self, args, "Bar");
}

PyObject* py_resizeBar(PyObject *self, PyObject *args)
{
  return py_resize(self, args, "Bar");
}

PyObject* py_getBarPos(PyObject *self, PyObject *args)
{
  return py_getPos(self, args, "Bar");
}

PyObject* py_moveBar(PyObject *self, PyObject *args)
{
  return py_move(self, args, "Bar");
}

PyObject* py_hideBar(PyObject *self, PyObject *args)
{
  return py_hide(self, args, "Bar");
}

PyObject* py_showBar(PyObject *self, PyObject *args)
{
  return py_show(self, args, "Bar");
}

PyObject* py_getBarMinMax(PyObject *self, PyObject *args)
{
  return py_getMinMax(self, args, "Bar");
}

PyObject* py_setBarMinMax(PyObject *self, PyObject *args)
{
  return py_setMinMax(self, args, "Bar");
}

PyObject* py_getBarValue(PyObject *self, PyObject *args)
{
  return py_getValue(self, args, "Bar");
}

PyObject* py_setBarValue(PyObject *self, PyObject *args)
{
  return py_setValue(self, args, "Bar");
}

PyObject* py_getBarSensor(PyObject *self, PyObject *args)
{
  return py_getSensor(self, args, "Bar");
}

PyObject* py_setBarSensor(PyObject *self, PyObject *args)
{
  return py_setSensor(self, args, "Bar");
}

PyObject* py_getBarImage(PyObject *, PyObject *args)
{
  long widget, meter;
  if (!PyArg_ParseTuple(args, (char*)"ll", &widget, &meter))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, "Bar"))
    return NULL;
  return Py_BuildValue((char*)"s", ((Bar*)meter)->getImage().ascii());
}

PyObject* py_setBarImage(PyObject *, PyObject *args)
{
  long widget, meter;
  char* s;
  if (!PyArg_ParseTuple(args, (char*)"lls", &widget, &meter, &s))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, "Bar"))
    return NULL;
  return Py_BuildValue((char*)"l", ((Bar*)meter)->setImage(s));
}

PyObject* py_getBarVertical(PyObject *, PyObject *args)
{
  long widget, meter;
  if (!PyArg_ParseTuple(args, (char*)"ll", &widget, &meter))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, "Bar"))
    return NULL;
  return Py_BuildValue((char*)"l", ((Bar*)meter)->getVertical());
}

PyObject* py_setBarVertical(PyObject *, PyObject *args)
{
  long widget, meter, l;
  if (!PyArg_ParseTuple(args, (char*)"lll", &widget, &meter, &l))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, "Bar"))
    return NULL;
  ((Bar*)meter)->setVertical(l);
  return Py_BuildValue((char*)"l", 1);
}
