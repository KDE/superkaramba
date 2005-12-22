/****************************************************************************
*  richtextlabel_python.cpp  -  Functions for richtext python api
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
#include "richtextlabel.h"
#include "meter_python.h"
#include "richtextlabel_python.h"

PyObject* py_createRichText(PyObject *, PyObject *args)
{
  long widget, underline = 0;
  PyObject *text;
  if (!PyArg_ParseTuple(args, (char*)"lO|l:createRichText",
      &widget, &text, &underline))
    return NULL;
  if (!checkKaramba(widget))
    return NULL;
  RichTextLabel *tmp = new RichTextLabel((karamba*)widget);
  tmp->setText(PyString2QString(text), underline);
  tmp->setTextProps(((karamba*)widget)->getDefaultTextProps());
  ((karamba*)widget)->meterList->append(tmp);
  ((karamba*)widget)->clickList->append(tmp);
  return (Py_BuildValue((char*)"l", (long)tmp));
}

PyObject* py_deleteRichText(PyObject *, PyObject *args)
{
  long widget, meter;
  if (!PyArg_ParseTuple(args, (char*)"ll:deleteRichText", &widget, &meter))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, "RichTextLabel"))
    return NULL;

  ((karamba*)widget)->deleteMeterFromSensors((Meter*)meter);
  ((karamba*)widget)->clickList->removeRef((Meter*)meter);
  return Py_BuildValue((char*)"l",
      ((karamba*)widget)->meterList->removeRef((Meter*)meter));
}

PyObject* py_getThemeRichText(PyObject *self, PyObject *args)
{
  return py_getThemeMeter(self, args, "RichTextLabel");
}

PyObject* py_getRichTextSize(PyObject *self, PyObject *args)
{
  return py_getSize(self, args, "RichTextLabel");
}

PyObject* py_resizeRichText(PyObject *self, PyObject *args)
{
  return py_resize(self, args, "RichTextLabel");
}

PyObject* py_getRichTextPos(PyObject *self, PyObject *args)
{
  return py_getPos(self, args, "RichTextLabel");
}

PyObject* py_moveRichText(PyObject *self, PyObject *args)
{
  return py_move(self, args, "RichTextLabel");
}

PyObject* py_hideRichText(PyObject *self, PyObject *args)
{
  return py_hide(self, args, "RichTextLabel");
}

PyObject* py_showRichText(PyObject *self, PyObject *args)
{
  return py_show(self, args, "RichTextLabel");
}

PyObject* py_getRichTextValue(PyObject *self, PyObject *args)
{
  return py_getStringValue(self, args, "RichTextLabel");
}

PyObject* py_setRichTextValue(PyObject *self, PyObject *args)
{
  return py_setStringValue(self, args, "RichTextLabel");
}

PyObject* py_getRichTextSensor(PyObject *self, PyObject *args)
{
  return py_getSensor(self, args, "RichTextLabel");
}

PyObject* py_setRichTextSensor(PyObject *self, PyObject *args)
{
  return py_setSensor(self, args, "RichTextLabel");
}

PyObject* py_setRichTextFontSize(PyObject *, PyObject *args)
{
  long widget, textSensor;
  long size;
  if (!PyArg_ParseTuple(args, (char*)"lll:changeRichTextSize",
      &widget, &textSensor, &size))
    return NULL;
  if (!checkKarambaAndMeter(widget, textSensor, "RichTextLabel"))
    return NULL;
  ((RichTextLabel*)textSensor)->setFontSize( size );
  return Py_BuildValue((char*)"l", 1);
}

PyObject* py_getRichTextFontSize(PyObject *, PyObject *args)
{
  long widget, textSensor;
  if (!PyArg_ParseTuple(args, (char*)"ll:getRichTextSize", &widget, &textSensor))
    return NULL;
  if (!checkKarambaAndMeter(widget, textSensor, "RichTextLabel"))
    return NULL;
  return Py_BuildValue((char*)"l", ((RichTextLabel*)textSensor)->getFontSize());
}

PyObject* py_setRichTextFont(PyObject *, PyObject *args)
{
  long widget, textSensor;
  char* text;
  if (!PyArg_ParseTuple(args, (char*)"lls:changeRichTextFont",
      &widget, &textSensor, &text))
    return NULL;
  if (!checkKarambaAndMeter(widget, textSensor, "RichTextLabel"))
    return NULL;
  ((RichTextLabel*)textSensor)->setFont( text );
  return Py_BuildValue((char*)"l", 1);
}

PyObject* py_getRichTextFont(PyObject *, PyObject *args)
{
  long widget, textSensor;
  if (!PyArg_ParseTuple(args, (char*)"ll:getRichTextFont", &widget, &textSensor))
    return NULL;
  if (!checkKarambaAndMeter(widget, textSensor, "RichTextLabel"))
    return NULL;
  return Py_BuildValue((char*)"s", ((RichTextLabel*)textSensor)->getFont().ascii());
}

// Set the width of a Rich Text Label
PyObject* py_set_rich_text_width(PyObject*, PyObject* args)
{
  long widget, text, size;
  if (!PyArg_ParseTuple(args, (char*)"lll:setRichTextWidth", &widget, &text, &size))
      return NULL;
  if (!checkKarambaAndMeter(widget, text, "RichTextLabel"))
    return NULL;

  RichTextLabel* richText = (RichTextLabel*) text;

  richText -> setWidth(size);
  return Py_BuildValue((char*)"l", 1);
}

