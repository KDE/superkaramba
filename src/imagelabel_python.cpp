/****************************************************************************
*  imagelabel_python.cpp  -  Imagelabel functions for python api
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
#include "imagelabel.h"
#include "meter_python.h"
#include "imagelabel_python.h"
#include "lineparser.h"

ImageLabel* createImageLabel(karamba *theme, long x, long y,
                             char* path, bool bg)
{
  QString file;
  //QString fakefile;

  /*tmp->setThemePath(theme->themePath);*/
  //FIXME: This is an ugly hack to ensure a unique reference
  //to add to the meterList.  It is a workaround for when a clickarea
  //is attached to an image, the image is deleted, and a new image is
  //created. A correct solution would be have dictionaries with a key/value
  //pair of ints->refs.
  ImageLabel *tmp2 = new ImageLabel(theme, x, y, 0, 0);
  ImageLabel *tmp = new ImageLabel(theme, x, y, 0, 0);
  delete tmp2;
  
  if(path)
  {
    file.setAscii(path);
    tmp->setValue(file);
    //tmp->parseImages(file, fakefile, x,y, 0, 0);
  }
  tmp->setBackground(bg);
  theme->setSensor(LineParser(file), tmp);
  theme->meterList->append (tmp);
  theme->imageList->append (tmp);
  if(bg)
    theme->kroot->repaint(true);
  return tmp;
}

PyObject* py_createImage(PyObject *, PyObject *args)
{
  long widget, x, y;
  char *text;
  if (!PyArg_ParseTuple(args, (char*)"llls:createImage", &widget, &x, &y, &text))
    return NULL;
  if (!checkKaramba(widget))
    return NULL;

  ImageLabel *tmp = createImageLabel((karamba*)widget, x, y, text, 0);
  return (Py_BuildValue((char*)"l", (long)tmp));
}

PyObject* py_createBackgroundImage(PyObject *, PyObject *args)
{
  long widget, x, y;
  char *text;
  if (!PyArg_ParseTuple(args, (char*)"llls:createBackgroundImage", &widget, &x, &y,
                        &text))
    return NULL;
  if (!checkKaramba(widget))
    return NULL;
  ImageLabel *tmp = createImageLabel((karamba*)widget, x, y, text, 1);
  return (Py_BuildValue((char*)"l", (long)tmp));
}

//Matthew Kay: new function for creating icons for tasks
/**
 * creates the icon for the specified task as a karamba image
 * @param ctask         the window id of the task to create the icon for
 */
PyObject* py_createTaskIcon(PyObject *, PyObject *args)
{
  long widget, x, y;
  long ctask;
  if (!PyArg_ParseTuple(args, (char*)"llll:createTaskIcon", &widget, &x, &y, &ctask))
    return NULL;
  if (!checkKaramba(widget))
    return NULL;

  //get the specified task and insure it exists
  TaskList taskList = ((karamba*)widget)->taskManager.tasks();
  Task* task = 0;
  Task* currTask = 0;
  for (task = taskList.first(); task; task = taskList.next())
  {
    if ((long)task == (long)ctask)
    {
      //task found
      currTask = task;
      break;
    }
  }
  if (currTask == 0)
  {
    //no task was found
    qWarning("Task not found.");
    return (long)NULL ;
  }
  //retrieve the QPixmap that represents this image
  QPixmap iconPixmap = KWin::icon(currTask->window());

  ImageLabel *tmp = createImageLabel((karamba*)widget, x, y, 0, 0);
  tmp->setValue(iconPixmap);
  return (Py_BuildValue((char*)"l", (long)tmp));
}

PyObject* py_deleteImage(PyObject *, PyObject *args)
{
  long widget, meter;
  if (!PyArg_ParseTuple(args, (char*)"ll:deleteImage", &widget, &meter))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, "ImageLabel"))
    return NULL;

  ((karamba*)widget)->deleteMeterFromSensors((Meter*)meter);
  ((karamba*)widget)->clickList->removeRef((Meter*)meter);
  ((karamba*)widget)->imageList->removeRef((Meter*)meter);
  return Py_BuildValue((char*)"l",
      ((karamba*)widget)->meterList->removeRef((Meter*)meter));
}

PyObject* py_getThemeImage(PyObject *self, PyObject *args)
{
  return py_getThemeMeter(self, args, "ImageLabel");
}

PyObject* py_getImagePos(PyObject *self, PyObject *args)
{
  return py_getPos(self, args, "ImageLabel");
}

PyObject* py_getImageSize(PyObject *self, PyObject *args)
{
  return py_getSize(self, args, "ImageLabel");
}

PyObject* py_moveImage(PyObject *self, PyObject *args)
{
  return py_move(self, args, "ImageLabel");
}

PyObject* py_hideImage(PyObject *self, PyObject *args)
{
  return py_hide(self, args, "ImageLabel");
}

PyObject* py_showImage(PyObject *self, PyObject *args)
{
  return py_show(self, args, "ImageLabel");
}

PyObject* py_getImageValue(PyObject *self, PyObject *args)
{
  return py_getStringValue(self, args, "ImageLabel");
}

PyObject* py_setImageValue(PyObject *self, PyObject *args)
{
  return py_setStringValue(self, args, "ImageLabel");
}

PyObject* py_getImageSensor(PyObject *self, PyObject *args)
{
  return py_getSensor(self, args, "ImageLabel");
}

PyObject* py_setImageSensor(PyObject *self, PyObject *args)
{
  return py_setSensor(self, args, "ImageLabel");
}

PyObject* py_removeImageEffects(PyObject *, PyObject *args)
{
  long widget, meter;
  if (!PyArg_ParseTuple(args, (char*)"ll:removeImageEffects", &widget, &meter))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, "ImageLabel"))
    return NULL;
  ((ImageLabel*)meter)->removeEffects();
  return Py_BuildValue((char*)"l", 1);
}

PyObject* py_changeImageIntensity(PyObject *, PyObject *args)
{
  long widget, meter;
  long millisec = 0;
  float ratio;
  if (!PyArg_ParseTuple(args, (char*)"llf|l:changeImageIntensity", &widget, &meter,
                        &ratio, &millisec))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, "ImageLabel"))
    return NULL;
  ((ImageLabel*)meter)->intensity(ratio, millisec);
  return Py_BuildValue((char*)"l", 1);
}

PyObject* py_changeImageChannelIntensity(PyObject *, PyObject *args)
{
  long widget, meter;
  float ratio;
  char* channel;
  long millisec = 0;
  if (!PyArg_ParseTuple(args, (char*)"llfs|l:changeImageChannelIntensity", &widget,
                        &meter, &ratio, &channel, &millisec))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, "ImageLabel"))
    return NULL;
  ((ImageLabel*)meter)->channelIntensity(ratio, channel, millisec);
  return Py_BuildValue((char*)"l", 1);
}

PyObject* py_changeImageToGray(PyObject *, PyObject *args)
{
  long widget, meter;
  long millisec = 0;
  if (!PyArg_ParseTuple(args, (char*)"ll|l:changeImageToGray", &widget, &meter,
                        &millisec))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, "ImageLabel"))
    return NULL;
  ((ImageLabel*)meter)->toGray(millisec);
  return Py_BuildValue((char*)"l", 1);
}

PyObject* py_removeImageTransformations(PyObject *, PyObject *args)
{
  long widget, meter;
  if (!PyArg_ParseTuple(args, (char*)"ll:removeImageTransformations", &widget, &meter))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, "ImageLabel"))
    return NULL;
  ((ImageLabel*)meter)->removeImageTransformations();
  return Py_BuildValue((char*)"l", 1);
}

PyObject* py_rotateImage(PyObject *, PyObject *args)
{
  long widget, meter;
  long deg;
  if (!PyArg_ParseTuple(args, (char*)"lll:rotateImage", &widget, &meter, &deg))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, "ImageLabel"))
    return NULL;
  ((ImageLabel*)meter)->rotate((int)deg);
  return Py_BuildValue((char*)"l", 1);
}

PyObject* py_getImageHeight(PyObject *, PyObject *args)
{
  long widget, meter;
  if (!PyArg_ParseTuple(args, (char*)"ll:getImageHeight", &widget, &meter))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, "ImageLabel"))
    return NULL;
  return Py_BuildValue((char*)"l", ((ImageLabel*)meter)->getHeight());
}

PyObject* py_getImageWidth(PyObject *, PyObject *args)
{
  long widget, meter;
  if (!PyArg_ParseTuple(args, (char*)"ll:getImageWidth", &widget, &meter))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, "ImageLabel"))
    return NULL;
  return Py_BuildValue((char*)"l", ((ImageLabel*)meter)->getWidth());
}

PyObject* py_resizeImageSmooth(PyObject *, PyObject *args)
{
  long widget, meter;
  long w, h;
  if (!PyArg_ParseTuple(args, (char*)"llll:resizeImageSmooth", &widget, &meter,
                        &w, &h))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, "ImageLabel"))
    return NULL;
  ((ImageLabel*)meter)->smoothScale((int)w, (int)h);
  return Py_BuildValue((char*)"l", 1);
}

PyObject* py_resizeImage(PyObject *, PyObject *args)
{
  long widget, meter, w, h;
  if (!PyArg_ParseTuple(args, (char*)"llll:resizeImage", &widget, &meter,
                        &w, &h))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, "ImageLabel"))
    return NULL;
  ((ImageLabel*)meter)->scale((int)w, (int)h);
  return Py_BuildValue((char*)"l", 1);
}

PyObject* py_addImageTooltip(PyObject *, PyObject *args)
{
  long widget, meter;
  PyObject* t;
  if (!PyArg_ParseTuple(args, (char*)"llO:addImageTooltip", &widget, &meter, &t))
    return NULL;
  if (!checkKarambaAndMeter(widget, meter, "ImageLabel"))
    return NULL;
  ((ImageLabel*)meter)->setTooltip(PyString2QString(t));
  return Py_BuildValue((char*)"l", 1);
}
