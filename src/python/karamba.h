/****************************************************************************
*  karamba_python.h  -  Functions for calling python scripts
*
*  Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>
*  Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>
*  Copyright (c) 2004 Petri Damst� <damu@iki.fi>
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

#ifndef KARAMBA_PYTHON_H
#define KARAMBA_PYTHON_H

class QString;
class KMenu;
class Task;
class Startup;
class Karamba;
class Meter;
struct _object;
typedef struct _object PyObject;
struct _ts;
typedef struct _ts PyThreadState;
class ThemeFile;

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
    bool callObject(const char* func, PyObject* pArgs, bool lock = true);

public:
    KarambaPython(const ThemeFile& theme, bool reloading);
    ~KarambaPython();

    static void initPython();
    static void shutdownPython();

    bool isExtensionLoaded()
    {
        return pythonThemeExtensionLoaded;
    }
    bool initWidget(Karamba* k);
    bool widgetUpdated(Karamba* k);
    bool widgetClosed(Karamba* k);
    bool menuOptionChanged(Karamba* k, QString key, bool value);
    bool meterClicked(Karamba* k, Meter* meter, int button);
    bool meterClicked(Karamba* k, QString anchor, int button);
    bool widgetClicked(Karamba* k, int x, int y, int button);
    bool keyPressed(Karamba* k, const Meter* meter, const QString& text);
    bool widgetMouseMoved(Karamba* k, int x, int y, int button);
    bool menuItemClicked(Karamba* k, KMenu* menu, long id);
    bool activeTaskChanged(Karamba* k, Task* t);
    bool taskAdded(Karamba* k, Task* t);
    bool taskRemoved(Karamba* k, Task* t);
    bool startupAdded(Karamba* k, Startup* t);
    bool startupRemoved(Karamba* k, Startup* t);
    bool commandOutput(Karamba* k, int pid, char *buffer);
    bool commandFinished(Karamba* k, int pid);
    bool itemDropped(Karamba* k, QString text, int x, int y);
    bool themeNotify(Karamba* k, const char *from, const char *txt);
    bool systrayUpdated(Karamba* k);
    bool desktopChanged(Karamba* k, int desktop);
    bool wallpaperChanged(Karamba* k, int desktop);
};

PyObject* py_testInterface(PyObject *, PyObject *args);

#endif // KARAMBA_PYTHON_H
