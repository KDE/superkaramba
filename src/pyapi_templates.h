/*
 * Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>
 * Copyright (c) 2005 Ryan Nickell <p0z3r@earthlink.net>
 *
 * This file is part of Superkaramba.
 *
 *  Superkaramba is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Superkaramba is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Superkaramba; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ****************************************************************************/
#define PY_API_RETURN(__value) \
  return (toPyObject(__value));\
}

#define PY_API_GET_METER(__name, __type) \
static PyObject* py_getTheme ## __name(PyObject *self, PyObject *args)\
{\
  long widget, meter;\
  char* name;\
  if (!PyArg_ParseTuple(args, (char*)"ls", &widget, &name))\
    return NULL;\
  meter = getMeter(widget, name);\
  if (meter)\
    if (!((QObject*)meter)->isA(#__type))\
      return NULL;\
  return (toPyObject(meter));\
}

#define PY_API_METER_CREATE(__func, __type) \
PyObject* __func(karamba* theme, int x, int y, int w, int h, QString s);\
static PyObject* py_ ## __func(PyObject *self, PyObject *args)\
{\
  long widget, x, y, w, h;\
  char *text;\
  if (!PyArg_ParseTuple(args, (char*)"lllll|s", &widget, &x, &y, &w, &h, &text))\
    return NULL;\
  if (!widget)\
    return NULL;\
  if (!((QObject*)widget)->isA("karamba"))\
    return NULL;\
  return __func((karamba*)widget, x, y, w, h, QString(text));\
}\
PyObject* __func(karamba* theme, int x, int y, int w, int h, QString s)\
{


#define PY_API_METER_0(__func, __type) \
PyObject* __func(karamba* theme, __type* meter);\
static PyObject* py_ ## __func(PyObject *self, PyObject *args)\
{\
  long widget, meter;\
  if (!PyArg_ParseTuple(args, (char*)"ll", &widget, &meter))\
    return NULL;\
  if (!widget || !meter)\
    return NULL;\
  if (!((QObject*)widget)->isA("karamba") || !((QObject*)meter)->isA(#__type))\
    return NULL;\
  return __func((karamba*)widget, (__type*)meter);\
}\
PyObject* __func(karamba* theme, __type* meter)\
{

#define PY_API_METER_P(__func, __type) \
PyObject* __func(karamba* theme, __type* meter, QPoint point);\
static PyObject* py_ ## __func(PyObject *self, PyObject *args)\
{\
  long widget, meter, x, y;\
  if (!PyArg_ParseTuple(args, (char*)"llll", &widget, &meter, &x, &y))\
    return NULL;\
  if (!widget || !meter)\
    return NULL;\
  if (!((QObject*)widget)->isA("karamba") || !((QObject*)meter)->isA(#__type))\
    return NULL;\
  return __func((karamba*)widget, (__type*)meter, QPoint(x,y));\
}\
PyObject* __func(karamba* theme, __type* meter, QPoint point)\
{

#define PY_API_METER_S(__func, __type) \
PyObject* __func(karamba* theme, __type* meter, QString s);\
static PyObject* py_ ## __func(PyObject *self, PyObject *args)\
{\
  long widget, meter;\
  char* s;\
  if (!PyArg_ParseTuple(args, (char*)"lls", &widget, &meter, &s))\
    return NULL;\
  if (!widget || !meter)\
    return NULL;\
  if (!((QObject*)widget)->isA("karamba") || !((QObject*)meter)->isA(#__type))\
    return NULL;\
  return __func((karamba*)widget, (__type*)meter, QString(s));\
}\
PyObject* __func(karamba* theme, __type* meter, QString s)\
{

#define PY_API_METER_L(__func, __type) \
PyObject* __func(karamba* theme, __type* meter, long l);\
static PyObject* py_ ## __func(PyObject *self, PyObject *args)\
{\
  long widget, meter, l;\
  char* s;\
  if (!PyArg_ParseTuple(args, (char*)"lll", &widget, &meter, &l))\
    return NULL;\
  if (!widget || !meter)\
    return NULL;\
  if (!((QObject*)widget)->isA("karamba") || !((QObject*)meter)->isA(#__type))\
    return NULL;\
  return __func((karamba*)widget, (__type*)meter, l);\
}\
PyObject* __func(karamba* theme, __type* meter, long l)\
{

#define PY_API_BASIC(__name, __type) \
PY_API_GET_METER(__name, __type) \
PY_API_METER_0(get ## __name ## Size, __type)\
  QPoint size(meter->getWidth(), meter->getHeight());\
PY_API_RETURN(size)\
\
PY_API_METER_P(resize ## __name, __type)\
  meter->setSize(meter->getX(), meter->getY(), point.x(), point.y());\
PY_API_RETURN(1)\
\
PY_API_METER_0(get ## __name ## Pos, __type)\
  QPoint pos(meter->getX(), meter->getY());\
PY_API_RETURN(pos)\
\
PY_API_METER_P(move ## __name, __type)\
  meter->setX(point.x());\
  meter->setY(point.y());\
PY_API_RETURN(1)\
\
PY_API_METER_0(hide ## __name, __type)\
  meter->hide();\
PY_API_RETURN(1)\
\
PY_API_METER_0(show ## __name, __type)\
  meter->show();\
PY_API_RETURN(1)\
\
PY_API_METER_0(get ## __name ## Sensor, __type)\
  QString s;\
  Sensor* sensor = theme->findSensorFromList(meter);\
  if (sensor)\
    s = theme->findSensorFromMap(sensor);\
PY_API_RETURN(s)\
\
PY_API_METER_S(set ## __name ## Sensor, __type)\
  theme->deleteMeterFromSensors(meter);\
  theme->setSensor(s, meter);\
PY_API_RETURN(1)

#define PY_API_DECLARE_BASIC(__name)\
  {"create" #__name, py_create ## __name, METH_VARARGS, "Create new ## __name ##."},\
  {"delete" #__name, py_delete ## __name, METH_VARARGS, "Delete ## __name ##."},\
  {"getTheme" #__name, py_getTheme ## __name, METH_VARARGS, "Get ## __name ## from .theme using it's name."},\
  {"get" # __name "Size", py_get ## __name ## Size, METH_VARARGS, "Get ## __name ## size."},\
  {"resize" # __name, py_resize ## __name, METH_VARARGS, "Resize ## __name ##."},\
  {"get" # __name "Pos", py_get ## __name ## Pos, METH_VARARGS, "Get ## __name ## position."},\
  {"move" # __name, py_move ## __name, METH_VARARGS, "Move ## __name ##."},\
  {"hide" # __name, py_hide ## __name, METH_VARARGS, "Hide ## __name ##."},\
  {"show" # __name, py_show ## __name, METH_VARARGS, "Show ## __name ##."},\
  {"get" # __name "Sensor", py_get ## __name ## Sensor, METH_VARARGS, "Get ## __name ## sensor."},\
  {"set" # __name "Sensor", py_set ## __name ## Sensor, METH_VARARGS, "Set ## __name ## sensor."},
