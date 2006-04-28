/****************************************************************************
*  input_python.cpp  -  Functions for input box python api
*
*  Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>
*  Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>
*  Copyright (c) 2004 Petri Damstén <damu@iki.fi>
*  Copyright (c) 2005 Alexander Wiedenbruch <mail@wiedenbruch.de>
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
#include "input_python.h"

PyObject* py_createInputBox(PyObject *, PyObject *args)
{
  long widget, x, y, w, h;
  PyObject *text;
  if (!PyArg_ParseTuple(args, (char*)"lllllO:createInputBox", &widget, &x, &y, &w, &h, &text))
    return NULL;

  if (!checkKaramba(widget))
    return NULL;

  Input *tmp = new Input((karamba*)widget, (int)x, (int)y, (int)w, (int)h);
  tmp->setValue(PyString2QString(text));
  tmp->setTextProps(((karamba*)widget)->getDefaultTextProps());
  ((karamba*)widget)->meterList->append(tmp);
  tmp->show();

  ((karamba*)widget)->makeActive();

  return (Py_BuildValue((char*)"l", (long)tmp));
}

PyObject* py_deleteInputBox(PyObject *, PyObject *args)
{
  long widget, meter;
  if (!PyArg_ParseTuple(args, (char*)"ll:deleteInputBox", &widget, &meter))
    return NULL;

  if (!checkKarambaAndMeter(widget, meter, "Input"))
    return NULL;

  bool result = ((karamba*)widget)->meterList->removeRef((Meter*)meter);

  ((karamba*)widget)->makePassive();

  return Py_BuildValue((char*)"l", result);
}

PyObject* py_getThemeInputBox(PyObject *self, PyObject *args)
{
  return py_getThemeMeter(self, args, "Input");
}

PyObject* py_getInputBoxValue(PyObject *self, PyObject *args)
{
  return py_getStringValue(self, args, "Input");
}

PyObject* py_setInputBoxValue(PyObject *self, PyObject *args)
{
  return py_setStringValue(self, args, "Input");
}

PyObject* py_hideInputBox(PyObject *self, PyObject *args)
{
  return py_hide(self, args, "Input");
}

PyObject* py_showInputBox(PyObject *self, PyObject *args)
{
  return py_show(self, args, "Input");
}

PyObject* py_getInputBoxPos(PyObject *self, PyObject *args)
{
  return py_getPos(self, args, "Input");
}

PyObject* py_moveInputBox(PyObject *self, PyObject *args)
{
  return py_move(self, args, "Input");
}

PyObject* py_getInputBoxSize(PyObject *self, PyObject *args)
{
  return py_getSize(self, args, "Input");
}

PyObject* py_resizeInputBox(PyObject *self, PyObject *args)
{
  return py_resize(self, args, "Input");
}

PyObject* py_setInputBoxFont(PyObject *, PyObject *args)
{
  long widget, inputBox;
  char* text;
  if (!PyArg_ParseTuple(args, (char*)"lls:changeInputBoxFont",
                        &widget, &inputBox, &text))
    return NULL;

  if (!checkKarambaAndMeter(widget, inputBox, "Input"))
    return NULL;

  ((Input*)inputBox)->setFont(text);
  return Py_BuildValue((char*)"l", 1);
}

PyObject* py_getInputBoxFont(PyObject *, PyObject *args)
{
  long widget, inputBox;
  if (!PyArg_ParseTuple(args, (char*)"ll:getInputBoxFont", &widget, &inputBox))
    return NULL;

  if (!checkKarambaAndMeter(widget, inputBox, "Input"))
    return NULL;

  return Py_BuildValue((char*)"s", ((Input*)inputBox)->getFont().ascii());
}

PyObject* py_setInputBoxFontColor(PyObject *, PyObject *args)
{
  long widget, inputBox;
  long r, g, b;
  if (!PyArg_ParseTuple(args, (char*)"lllll:changeInputBoxFontColor", &widget, &inputBox, &r, &g, &b))
    return NULL;

  if (!checkKarambaAndMeter(widget, inputBox, "Input"))
    return NULL;

  ((Input*)inputBox)->setFontColor(QColor(r, g, b));
  return Py_BuildValue((char*)"l", 1);
}

PyObject* py_getInputBoxFontColor(PyObject *, PyObject *args)
{
  long widget, inputBox;
  if (!PyArg_ParseTuple(args, (char*)"ll:changeInputBoxFontColor", &widget, &inputBox))
    return NULL;

  if (!checkKarambaAndMeter(widget, inputBox, "Input"))
    return NULL;

  QColor color = ((Input*)inputBox)->getFontColor();
  return Py_BuildValue((char*)"(i,i,i)", color.red(), color.green(), color.blue());
}

PyObject* py_setInputBoxSelectionColor(PyObject *, PyObject *args)
{
  long widget, inputBox;
  long r, g, b;
  if (!PyArg_ParseTuple(args, (char*)"lllll:changeInputBoxSelectionColor", &widget, &inputBox, &r, &g, &b))
    return NULL;

  if (!checkKarambaAndMeter(widget, inputBox, "Input"))
    return NULL;

  ((Input*)inputBox)->setSelectionColor(QColor(r, g, b));
  return Py_BuildValue((char*)"l", 1);
}

PyObject* py_getInputBoxSelectionColor(PyObject *, PyObject *args)
{
  long widget, inputBox;
  if (!PyArg_ParseTuple(args, (char*)"ll:changeInputBoxSelectionColor", &widget, &inputBox))
    return NULL;

  if (!checkKarambaAndMeter(widget, inputBox, "Input"))
    return NULL;

  QColor color = ((Input*)inputBox)->getSelectionColor();
  return Py_BuildValue((char*)"(i,i,i)", color.red(), color.green(), color.blue());
}

PyObject* py_setInputBoxBGColor(PyObject *, PyObject *args)
{
  long widget, inputBox;
  long r, g, b;
  if (!PyArg_ParseTuple(args, (char*)"lllll:changeInputBoxBackgroundColor", &widget, &inputBox, &r, &g, &b))
    return NULL;

  if (!checkKarambaAndMeter(widget, inputBox, "Input"))
    return NULL;

  ((Input*)inputBox)->setBGColor(QColor(r, g, b));
  return Py_BuildValue((char*)"l", 1);
}

PyObject* py_getInputBoxBGColor(PyObject *, PyObject *args)
{
  long widget, inputBox;
if (!PyArg_ParseTuple(args, (char*)"ll:getInputBoxBackgroundColor", &widget, &inputBox))
  return NULL;

  if (!checkKarambaAndMeter(widget, inputBox, "Input"))
    return NULL;

  QColor color = ((Input*)inputBox)->getBGColor();
  return Py_BuildValue((char*)"(i,i,i)", color.red(), color.green(), color.blue());
}

PyObject* py_setInputBoxFrameColor(PyObject *, PyObject *args)
{
  long widget, inputBox;
  long r, g, b;
if (!PyArg_ParseTuple(args, (char*)"lllll:changeInputBoxFrameColor", &widget, &inputBox, &r, &g, &b))
  return NULL;

  if (!checkKarambaAndMeter(widget, inputBox, "Input"))
    return NULL;

  ((Input*)inputBox)->setColor(QColor(r, g, b));
  return Py_BuildValue((char*)"l", 1);
}

PyObject* py_getInputBoxFrameColor(PyObject *, PyObject *args)
{
  long widget, inputBox;
if (!PyArg_ParseTuple(args, (char*)"ll:getInputBoxFrameColor", &widget, &inputBox))
  return NULL;

  if (!checkKarambaAndMeter(widget, inputBox, "Input"))
    return NULL;

  QColor color = ((Input*)inputBox)->getColor();
  return Py_BuildValue((char*)"(i,i,i)", color.red(), color.green(), color.blue());
}

PyObject* py_setInputBoxSelectedTextColor(PyObject *, PyObject *args)
{
  long widget, inputBox;
  long r, g, b;
if (!PyArg_ParseTuple(args, (char*)"lllll:changeInputBoxSelectedTextColor", &widget, &inputBox, &r, &g, &b))
  return NULL;

  if (!checkKarambaAndMeter(widget, inputBox, "Input"))
    return NULL;

  ((Input*)inputBox)->setSelectedTextColor(QColor(r, g, b));
  return Py_BuildValue((char*)"l", 1);
}

PyObject* py_getInputBoxSelectedTextColor(PyObject *, PyObject *args)
{
  long widget, inputBox;
if (!PyArg_ParseTuple(args, (char*)"ll:getInputBoxSelectedTextColor", &widget, &inputBox))
  return NULL;

  if (!checkKarambaAndMeter(widget, inputBox, "Input"))
    return NULL;

  QColor color = ((Input*)inputBox)->getSelectedTextColor();
  return Py_BuildValue((char*)"(i,i,i)", color.red(), color.green(), color.blue());
}

PyObject* py_setInputBoxFontSize(PyObject *, PyObject *args)
{
  long widget, inputBox;
  long size;
  if (!PyArg_ParseTuple(args, (char*)"lll:changeInputBoxFontSize",
                      &widget, &inputBox, &size))
    return NULL;

  if (!checkKarambaAndMeter(widget, inputBox, "Input"))
    return NULL;

  ((Input*)inputBox)->setFontSize( size );
  return Py_BuildValue((char*)"l", 1);
}

PyObject* py_getInputBoxFontSize(PyObject *, PyObject *args)
{
  long widget, inputBox;
  if (!PyArg_ParseTuple(args, (char*)"ll:getInputBoxFontSize", &widget, &inputBox))
    return NULL;

  if (!checkKarambaAndMeter(widget, inputBox, "Input"))
    return NULL;

  return Py_BuildValue((char*)"l", ((Input*)inputBox)->getFontSize());
}

PyObject* py_setInputFocus(PyObject *, PyObject *args)
{
  long widget, inputBox;
  if (!PyArg_ParseTuple(args, (char*)"ll:setInputFocus", &widget, &inputBox))
    return NULL;

  if (!checkKarambaAndMeter(widget, inputBox, "Input"))
    return NULL;

  //((karamba*)widget)->setActiveWindow();

  ((Input*)inputBox)->setInputFocus();
  return Py_BuildValue((char*)"l", 1);
}

PyObject* py_clearInputFocus(PyObject *, PyObject *args)
{
  long widget, inputBox;
  if (!PyArg_ParseTuple(args, (char*)"ll:clearInputFocus", &widget, &inputBox))
    return NULL;

  if (!checkKarambaAndMeter(widget, inputBox, "Input"))
    return NULL;

  ((Input*)inputBox)->clearInputFocus();
  return Py_BuildValue((char*)"l", 1);
}

PyObject* py_getInputFocus(PyObject *, PyObject *args)
{
  long widget;
  if (!PyArg_ParseTuple(args, (char*)"l:getInputFocus", &widget))
    return NULL;

  if (!checkKaramba(widget))
    return NULL;
  
  //
  // FocusWidget() returns the currently focused line edit,
  // but unfortunately we need an 'Input' object here.
  //
  QWidget *obj = ((karamba*)widget)->focusWidget();
  
  if(obj->isA("QLineEdit")) // SKLineEdit is no Q_Object, but QLineEdit can only be here as a SKLineEdit
    return Py_BuildValue((char*)"l", ((SKLineEdit*)obj)->getInput());
  
  return Py_BuildValue((char*)"l", 0);
}
