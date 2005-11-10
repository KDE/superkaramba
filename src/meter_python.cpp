/***************************************************************************
 * Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>                *
 * Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include <Python.h>
#include <qobject.h>
#include "karamba.h"
#include "karambaapp.h"
#include "meter.h"
#include "meter_python.h"
#include "lineparser.h"

bool checkKaramba(long widget)
{
/*
  if (!karambaApp)
  {
    PyErr_SetString(PyExc_ValueError, "app pointer was 0.");
    return false;
  }
*/
  if (!widget)
  {
    PyErr_SetString(PyExc_ValueError, "widget pointer was 0.");
    return false;
  }
  if (!karambaApp->hasKaramba((karamba*)widget))
  {
    QString tmp;

    tmp.sprintf("no %x widget found.", (unsigned int)widget);
    PyErr_SetString(PyExc_ValueError, tmp.ascii());
    return false;
  }
  return true;
}

bool checkMeter(long widget, long meter, const char* type)
{
  if (!meter)
  {
    PyErr_SetString(PyExc_ValueError, "meter pointer was 0.");
    return false;
  }
  if (!((karamba*)widget)->hasMeter((Meter*)meter))
  {
    QString tmp;

    tmp.sprintf("widget does not have meter %x.", (unsigned int)meter);
    PyErr_SetString(PyExc_ValueError, tmp.ascii());
    return false;
  }
  if (!((QObject*)meter)->isA(type))
  {
    QString tmp;

    tmp.sprintf("meter is not type of %s.", type);
    PyErr_SetString(PyExc_TypeError, tmp.ascii());
    return false;
  }
  return true;
}

bool checkKarambaAndMeter(long widget, long meter, const char* type)
{
  return checkKaramba(widget) && checkMeter(widget, meter, type);
}

// This just throws awya extra bytes.
// I guess there is a better way to do this...
QString fromUcs4(Q_UINT32* ucs4)
{
  QString result = "";
  while(*ucs4 != 0)
  {
    result += QChar((Q_UINT16)*ucs4);
    ++ucs4;
  }
  return result;
}

// Converts a Python String to a QString with Unicode support
QString PyString2QString(PyObject* text)
{
    QString qtext;
    if (PyString_CheckExact(text))
    {
        char* t = PyString_AsString(text);
        qtext.setAscii(t);
    }
    else if (PyUnicode_CheckExact(text))
    {
        Py_UNICODE* t = PyUnicode_AsUnicode(text);
        if(sizeof(Py_UNICODE) == 4)
          qtext = fromUcs4((Q_UINT32*)t);
        else
          qtext = QString::fromUcs2((Q_UINT16*)t);
    }
    else
    {
        // Error raise execption ...
    }
    return qtext;
}

// Converts a QString to a Python String with Unicode support
PyObject* QString2PyString(QString string)
{
  PyObject *pyString;

  const unsigned short* tmp = string.ucs2();
  bool dofree = false;

  if(tmp)
  {
    #if Py_UNICODE_SIZE == 4

      Py_UNICODE* buf = new Py_UNICODE[string.length()];
      
      for(unsigned int i = 0; i < string.length(); i++)
      {
        buf[i] = tmp[i];
      }
      dofree = true;

    #else

      Py_UNICODE* buf = (Py_UNICODE*) tmp;

    #endif

    pyString = PyUnicode_FromUnicode(buf, string.length());

    if(dofree)
    {
      delete [] buf;
    }
  }

  else
    pyString = PyString_FromString("");

  return pyString;
}


long getMeter(long widget, char* name)
{
  karamba* theme = (karamba*)widget;
  QObjectListIt it( *theme->meterList ); // iterate over meters

  while ( it != 0 )
  {
    if (strcmp(((Meter*) *it)->name(), name) == 0)
      return (long)*it;
    ++it;
  }
  return 0;
}

PyObject* py_getThemeMeter(PyObject *, PyObject *args, QString type)
{
  long widget, meter;
  char* name;
  if (!PyArg_ParseTuple(args, (char*)"ls", &widget, &name))
    return NULL;
  if (!checkKaramba(widget))
    return NULL;
  if(!name)
    return NULL;
  meter = getMeter(widget, name);
  if (!checkMeter(widget, meter, type.ascii()))
    return NULL;
  return (Py_BuildValue((char*)"l", meter));
}

PyObject* py_getSize(PyObject *, PyObject *args, QString type)
{
  long widget;
  long meter;
  if (!PyArg_ParseTuple(args, (char*)"ll", &widget, &meter))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, type.ascii()))
    return NULL;
  return Py_BuildValue((char*)"(i,i)", ((Meter*)meter)->getWidth(),
                                ((Meter*)meter)->getHeight());
}

PyObject* py_resize(PyObject *, PyObject *args, QString type)
{
  long widget, meter, x, y;
  if (!PyArg_ParseTuple(args, (char*)"llll", &widget, &meter, &x, &y))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, type.ascii()))
    return NULL;
  ((Meter*)meter)->setSize(((Meter*)meter)->getX(), ((Meter*)meter)->getY(),
                           x, y);
  return Py_BuildValue((char*)"l", 1);
}

PyObject* py_getPos(PyObject *, PyObject *args, QString type)
{
  long widget, meter;
  if (!PyArg_ParseTuple(args, (char*)"ll", &widget, &meter))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, type.ascii()))
    return NULL;
  return Py_BuildValue((char*)"(i,i)", ((Meter*)meter)->getX(),
                                ((Meter*)meter)->getY());
}

PyObject* py_move(PyObject *, PyObject *args, QString type)
{
  long widget, meter, x, y;
  if (!PyArg_ParseTuple(args, (char*)"llll", &widget, &meter, &x, &y))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, type.ascii()))
    return NULL;
  ((Meter*)meter)->setSize(x, y,
                           ((Meter*)meter)->getWidth(),
                           ((Meter*)meter)->getHeight());
  return Py_BuildValue((char*)"l", 1);
}

PyObject* py_hide(PyObject *, PyObject *args, QString type)
{
  long widget, meter;
  if (!PyArg_ParseTuple(args, (char*)"ll", &widget, &meter))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, type.ascii()))
    return NULL;
  ((Meter*)meter)->hide();
  return Py_BuildValue((char*)"l", 1);
}

PyObject* py_show(PyObject *, PyObject *args, QString type)
{
  long widget, meter;
  if (!PyArg_ParseTuple(args, (char*)"ll", &widget, &meter))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, type.ascii()))
    return NULL;
  ((Meter*)meter)->show();
  return Py_BuildValue((char*)"l", 1);
}

PyObject* py_getValue(PyObject *, PyObject *args, QString type)
{
  long widget, meter;
  if (!PyArg_ParseTuple(args, (char*)"ll", &widget, &meter))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, type.ascii()))
    return NULL;
  return Py_BuildValue((char*)"l", ((Meter*)meter)->getValue());
}

PyObject* py_setValue(PyObject *, PyObject *args, QString type)
{
  long widget, meter, l;
  if (!PyArg_ParseTuple(args, (char*)"lll", &widget, &meter, &l))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, type.ascii()))
    return NULL;
  ((Meter*)meter)->setValue(l);
  return Py_BuildValue((char*)"l", ((long)meter));
}

PyObject* py_getStringValue(PyObject *, PyObject *args, QString type)
{
  long widget, meter;
  if (!PyArg_ParseTuple(args, (char*)"ll", &widget, &meter))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, type.ascii()))
    return NULL;
  return Py_BuildValue((char*)"O",
      QString2PyString(((Meter*)meter)->getStringValue()));
}

PyObject* py_setStringValue(PyObject *, PyObject *args, QString type)
{
  long widget, meter;
  PyObject* s;
  if (!PyArg_ParseTuple(args, (char*)"llO", &widget, &meter, &s))
    return NULL;
  if (!s)
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, type.ascii()))
    return NULL;
  ((Meter*)meter)->setValue(PyString2QString(s));
  return Py_BuildValue((char*)"l", ((long)meter));
}

PyObject* py_getMinMax(PyObject *, PyObject *args, QString type)
{
  long widget, meter;
  if (!PyArg_ParseTuple(args, (char*)"ll", &widget, &meter))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, type.ascii()))
    return NULL;
  return Py_BuildValue((char*)"(i,i)", ((Meter*)meter)->getMin(),
                                ((Meter*)meter)->getMax());
}

PyObject* py_setMinMax(PyObject *, PyObject *args, QString type)
{
  long widget, meter, x, y;
  if (!PyArg_ParseTuple(args, (char*)"llll", &widget, &meter, &x, &y))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, type.ascii()))
    return NULL;
  ((Meter*)meter)->setMin(x);
  ((Meter*)meter)->setMax(y);
  return Py_BuildValue((char*)"l", 1);
}

PyObject* py_getSensor(PyObject *, PyObject *args, QString type)
{
  long widget, meter;
  QString s;
  if (!PyArg_ParseTuple(args, (char*)"ll", &widget, &meter))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, type.ascii()))
    return NULL;
  return Py_BuildValue((char*)"s",
                       ((karamba*)widget)->getSensor((Meter*)meter).ascii());
}

PyObject* py_setSensor(PyObject *, PyObject *args, QString type)
{
  long widget, meter;
  char* s;

  if (!PyArg_ParseTuple(args, (char*)"lls", &widget, &meter, &s))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, type.ascii()))
    return NULL;
  ((karamba*)widget)->setSensor(LineParser(s), (Meter*)meter);
  return Py_BuildValue((char*)"l", 1);
}

PyObject* py_setColor(PyObject *, PyObject *args, QString type)
{
  long widget, meter;
  long r, g, b;
  if (!PyArg_ParseTuple(args, (char*)"lllll", &widget, &meter, &r, &g, &b))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, type.ascii()))
    return NULL;
  ((Meter*)meter)->setColor(QColor(r, g, b));
  return Py_BuildValue((char*)"l", 1);
}

PyObject* py_getColor(PyObject *, PyObject *args, QString type)
{
  long widget, meter;
  if (!PyArg_ParseTuple(args, (char*)"ll", &widget, &meter))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, type.ascii()))
    return NULL;
  QColor color = ((Meter*)meter)->getColor();
  return Py_BuildValue((char*)"(i,i,i)", color.red(), color.green(), color.blue());
}

