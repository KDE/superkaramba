/****************************************************************************
*  textlabel_python.cpp  -  Functions for textlabel python api
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

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include <Python.h>
#include <qobject.h>
#include "karamba.h"
#include "textlabel.h"
#include "meter_python.h"
#include "textlabel_python.h"

PyObject* py_createText(PyObject *, PyObject *args)
{
  long widget, x, y, w, h;
  PyObject *text;
  if (!PyArg_ParseTuple(args, (char*)"lllllO:createText", &widget, &x, &y, &w, &h, &text))
    return NULL;
  if (!checkKaramba(widget))
    return NULL;
  TextLabel *tmp =
      new TextLabel((karamba*)widget, (int)x, (int)y, (int)w, (int)h);
  tmp->setValue(PyString2QString(text));
  tmp->setTextProps(((karamba*)widget)->getDefaultTextProps());
  ((karamba*)widget)->meterList->append(tmp);
  return (Py_BuildValue((char*)"l", (long)tmp));
}

PyObject* py_deleteText(PyObject *, PyObject *args)
{
  long widget, meter;
  if (!PyArg_ParseTuple(args, (char*)"ll:deleteText", &widget, &meter))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, "TextLabel"))
    return NULL;

  ((karamba*)widget)->deleteMeterFromSensors((Meter*)meter);
  ((karamba*)widget)->clickList->removeRef((Meter*)meter);
  return Py_BuildValue((char*)"l",
      ((karamba*)widget)->meterList->removeRef((Meter*)meter));
}

PyObject* py_getThemeText(PyObject *self, PyObject *args)
{
  return py_getThemeMeter(self, args, "TextLabel");
}

PyObject* py_getTextSize(PyObject *self, PyObject *args)
{
  return py_getSize(self, args, "TextLabel");
}

PyObject* py_resizeText(PyObject *self, PyObject *args)
{
  return py_resize(self, args, "TextLabel");
}

PyObject* py_getTextPos(PyObject *self, PyObject *args)
{
  return py_getPos(self, args, "TextLabel");
}

PyObject* py_moveText(PyObject *self, PyObject *args)
{
  return py_move(self, args, "TextLabel");
}

PyObject* py_hideText(PyObject *self, PyObject *args)
{
  return py_hide(self, args, "TextLabel");
}

PyObject* py_showText(PyObject *self, PyObject *args)
{
  return py_show(self, args, "TextLabel");
}

PyObject* py_getTextValue(PyObject *self, PyObject *args)
{
  return py_getStringValue(self, args, "TextLabel");
}

PyObject* py_setTextValue(PyObject *self, PyObject *args)
{
  return py_setStringValue(self, args, "TextLabel");
}

PyObject* py_getTextSensor(PyObject *self, PyObject *args)
{
  return py_getSensor(self, args, "TextLabel");
}

PyObject* py_setTextSensor(PyObject *self, PyObject *args)
{
  return py_setSensor(self, args, "TextLabel");
}

PyObject* py_getTextColor(PyObject *self, PyObject *args)
{
  return py_getColor(self, args, "TextLabel");
}

PyObject* py_setTextColor(PyObject *self, PyObject *args)
{
  return py_setColor(self, args, "TextLabel");
}

PyObject* py_setTextShadow(PyObject *, PyObject *args)
{
  long widget, textSensor;
  long shadow;
  if (!PyArg_ParseTuple(args, (char*)"lll:changeTextShadow",
      &widget, &textSensor, &shadow))
    return NULL;
  if (!checkKarambaAndMeter(widget, textSensor, "TextLabel"))
    return NULL;
  ((TextLabel*)textSensor)->setShadow( shadow );
  return Py_BuildValue((char*)"l", 1);
}

PyObject* py_getTextShadow(PyObject *, PyObject *args)
{
  long widget, textSensor;
  if (!PyArg_ParseTuple(args, (char*)"ll:getTextShadow", &widget, &textSensor))
    return NULL;
  if (!checkKarambaAndMeter(widget, textSensor, "TextLabel"))
    return NULL;
  return Py_BuildValue((char*)"l", ((TextLabel*)textSensor)->getShadow());
}

PyObject* py_setTextFontSize(PyObject *, PyObject *args)
{
  long widget, textSensor;
  long size;
  if (!PyArg_ParseTuple(args, (char*)"lll:changeTextSize",
      &widget, &textSensor, &size))
    return NULL;
  if (!checkKarambaAndMeter(widget, textSensor, "TextLabel"))
    return NULL;
  ((TextLabel*)textSensor)->setFontSize( size );
  return Py_BuildValue((char*)"l", 1);
}

PyObject* py_getTextFontSize(PyObject *, PyObject *args)
{
  long widget, textSensor;
  if (!PyArg_ParseTuple(args, (char*)"ll:getTextSize", &widget, &textSensor))
    return NULL;
  if (!checkKarambaAndMeter(widget, textSensor, "TextLabel"))
    return NULL;
  return Py_BuildValue((char*)"l", ((TextLabel*)textSensor)->getFontSize());
}

PyObject* py_setTextFont(PyObject *, PyObject *args)
{
  long widget, textSensor;
  char* text;
  if (!PyArg_ParseTuple(args, (char*)"lls:changeTextFont",
      &widget, &textSensor, &text))
    return NULL;
  if (!checkKarambaAndMeter(widget, textSensor, "TextLabel"))
    return NULL;
  ((TextLabel*)textSensor)->setFont( text );
  return Py_BuildValue((char*)"l", 1);
}

PyObject* py_getTextFont(PyObject *, PyObject *args)
{
  long widget, textSensor;
  if (!PyArg_ParseTuple(args, (char*)"ll:getTextFont", &widget, &textSensor))
    return NULL;
  if (!checkKarambaAndMeter(widget, textSensor, "TextLabel"))
    return NULL;
  return Py_BuildValue((char*)"s", ((TextLabel*)textSensor)->getFont().ascii());
}

PyObject* py_setTextAlign(PyObject *, PyObject *args)
{
  long widget, textSensor;
  char* text;
  if (!PyArg_ParseTuple(args, (char*)"lls:changeTextFont",
      &widget, &textSensor, &text))
    return NULL;
  if (!checkKarambaAndMeter(widget, textSensor, "TextLabel"))
    return NULL;
  ((TextLabel*)textSensor)->setAlignment( text );
  return Py_BuildValue((char*)"l", 1);
}

PyObject* py_getTextAlign(PyObject *, PyObject *args)
{
  long widget, textSensor;
  if (!PyArg_ParseTuple(args, (char*)"ll:getTextFont", &widget, &textSensor))
    return NULL;
  if (!checkKarambaAndMeter(widget, textSensor, "TextLabel"))
    return NULL;
  return Py_BuildValue((char*)"s", ((TextLabel*)textSensor)->getAlignment().ascii());
}

PyObject* py_setTextScroll(PyObject *, PyObject *args)
{
  long widget, textSensor;
  char* type;
  int x=0, y=0, pause=0, gap=0;
  if (!PyArg_ParseTuple(args, (char*)"lls|llll:setScroll",
      &widget, &textSensor, &type, &x, &y, &gap, &pause))
    return NULL;
  if (!checkKarambaAndMeter(widget, textSensor, "TextLabel"))
    return NULL;
  ((TextLabel*)textSensor)->setScroll(type, QPoint(x,y), gap, pause);
  return Py_BuildValue((char*)"l", 1);
}
