/****************************************************************************
*  karamba_python.h  -  Functions for calling python scripts
*
*  Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>
*  Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>
*  Copyright (c) 2004 Petri Damst�n <damu@iki.fi>
*
*  This file is part of Superkaramba.
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

#ifndef KARAMBA_PYTHON_H
#define KARAMBA_PYTHON_H

class QString;
class KPopupMenu;
class Task;
class Startup;
class karamba;
class Meter;
struct _object;
typedef struct _object PyObject;
struct _ts;
typedef struct _ts PyThreadState;

class KarambaPython
{
protected:
  bool pythonThemeExtensionLoaded;
  PyObject *pName, *pModule;
  PyObject *pDict;
  static PyThreadState* mainThreadState;

  void getLock(PyThreadState** myThreadState);
  PyObject* getFunc(const char* function);
  void releaseLock(PyThreadState* myThreadState);
  bool callObject(const char* func, PyObject* pArgs);

public:
  KarambaPython(QString themePath, QString themeName, bool reloading);
  ~KarambaPython();

  static void initPython();
  static void shutdownPython();

  bool isExtensionLoaded() { return pythonThemeExtensionLoaded; };
  bool initWidget(karamba* k);
  bool widgetUpdated(karamba* k);
  bool widgetClosed(karamba* k);
  bool menuOptionChanged(karamba* k, QString key, bool value);
  bool meterClicked(karamba* k, Meter* meter, int button);
  bool meterClicked(karamba* k, QString anchor, int button);
  bool widgetClicked(karamba* k, int x, int y, int button);
  bool keyPressed(karamba* k, QString text);
  bool widgetMouseMoved(karamba* k, int x, int y, int button);
  bool menuItemClicked(karamba* k, KPopupMenu* menu, long id);
  bool activeTaskChanged(karamba* k, Task* t);
  bool taskAdded(karamba* k, Task* t);
  bool taskRemoved(karamba* k, Task* t);
  bool startupAdded(karamba* k, Startup* t);
  bool startupRemoved(karamba* k, Startup* t);
  bool commandOutput(karamba* k, int pid, char *buffer);
  bool itemDropped(karamba* k, QString text);
  bool systrayUpdated(karamba* k);
  bool desktopChanged(karamba* k, int desktop);
  bool wallpaperChanged(karamba* k, int desktop);
};

#endif // KARAMBA_PYTHON_H
