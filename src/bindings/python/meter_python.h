/***************************************************************************
 * Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>                *
 * Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef METER_PYTHON_H
#define METER_PYTHON_H

// Python uses char* where it should use const char*
#define PY_PARSE(a, b, c, d) (PyArg_ParseTuple(a, (char*)b, c, d))
#define PY_BUILD(a, b)       (Py_BuildValue((char*)a, b))

bool checkKaramba(long widget);
bool checkMeter(long widget, long meter, const char* type);
bool checkKarambaAndMeter(long widget, long meter, const char* type);

QString PyString2QString(PyObject* text);
PyObject* QString2PyString(QString string);

PyObject* py_getThemeMeter(PyObject *self, PyObject *args, QString type);
PyObject* py_getSize(PyObject *self, PyObject *args, QString type);
PyObject* py_resize(PyObject *self, PyObject *args, QString type);
PyObject* py_getPos(PyObject *self, PyObject *args, QString type);
PyObject* py_move(PyObject *self, PyObject *args, QString type);
PyObject* py_hide(PyObject *self, PyObject *args, QString type);
PyObject* py_show(PyObject *self, PyObject *args, QString type);
PyObject* py_getValue(PyObject *self, PyObject *args, QString type);
PyObject* py_setValue(PyObject *self, PyObject *args, QString type);
PyObject* py_getStringValue(PyObject *self, PyObject *args, QString type);
PyObject* py_setStringValue(PyObject *self, PyObject *args, QString type);
PyObject* py_getMinMax(PyObject *self, PyObject *args, QString type);
PyObject* py_setMinMax(PyObject *self, PyObject *args, QString type);
PyObject* py_getSensor(PyObject *self, PyObject *args, QString type);
PyObject* py_setSensor(PyObject *self, PyObject *args, QString type);
PyObject* py_getColor(PyObject *self, PyObject *args, QString type);
PyObject* py_setColor(PyObject *self, PyObject *args, QString type);

#endif // METER_PYTHON_H

