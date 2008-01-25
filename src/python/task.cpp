/****************************************************************************
*  task_python.cpp  -  Functions for task python api
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

#if defined(_XOPEN_SOURCE) && !defined(__SUNPRO_CC)
#undef _XOPEN_SOURCE
#endif

#include <Python.h>
#include <qobject.h>
#include "../karamba.h"
#include "taskmanager.h"
#include "meters/meter.h"
#include "python/meter.h"
#include "python/task.h"

// This does something with a task, such as minimize or close it
int performTaskAction(long widget, long ctask, long action)
{
    Q_UNUSED(widget);

    Task::TaskPtr currTask;
    Task::TaskPtr task;

    QList<Task::TaskPtr> taskList = TaskManager::self()->tasks().values();

    foreach(task, taskList) {
        if ((long)task.data() == (long)ctask) {
            currTask = task;
        }
    }

    if (!currTask.isNull()) {
        switch (action) {
        case 1:
            currTask->setMaximized(true);
            break;

        case 2:
            currTask->restore();
            break;

        case 3:
            currTask->setIconified(true);
            break;

        case 4:
            currTask->close();
            break;

        case 5:
            currTask->activate();
            break;

        case 6:
            currTask->raise();
            break;

        case 7:
            currTask->lower();
            break;

        case 8:
            currTask->activateRaiseOrIconify();
            break;

        case 9:
            currTask->toggleAlwaysOnTop();
            break;

        case 10:
            currTask->toggleShaded();
            break;

        default:
            printf("You are trying to perform an invalid action in \
                   performTaskAction\n");
        }
        return 1;
    } else {
        return 0;
    }
}

PyObject* py_perform_task_action(PyObject *, PyObject *args)
{
    long widget, task, action;
    if (!PyArg_ParseTuple(args, (char*)"lll:performTaskAction",
                          &widget, &task, &action))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;
    return Py_BuildValue((char*)"l", performTaskAction(widget, task, action));
}

// This returns all the info about a certain task
// Return type is a Python List
PyObject* getTaskInfo(long widget, long ctask)
{
    Q_UNUSED(widget);

    Task::TaskPtr currTask;
    Task::TaskPtr task;

    QList<Task::TaskPtr> taskList = TaskManager::self()->tasks().values();

    foreach(task, taskList) {
        if ((long)task.data() == (long)ctask) {
            currTask = task;
        }

    }

    if (!currTask.isNull()) {
        PyObject* pList = PyList_New(0);

        //Task Name
        if (currTask->name() != NULL) {
            PyList_Append(pList, PyString_FromString(currTask->name().toLatin1().constData()));
        } else {
            PyList_Append(pList, PyString_FromString(""));
        }

        //Icon Name
        if (currTask->info().iconName() != NULL) {
            PyList_Append(pList, PyString_FromString(
                              currTask->info().iconName().toLatin1().constData()));
        } else {
            PyList_Append(pList, PyString_FromString(""));
        }

        //Class Name
        if (currTask->className() != NULL) {
            PyList_Append(pList, PyString_FromString(currTask->className().toLatin1().constData()));
        } else {
            PyList_Append(pList, PyString_FromString(""));
        }

        // Desktop this task is on
        PyList_Append(pList, PyInt_FromLong(currTask->desktop()));

        // is it maximized?
        PyList_Append(pList, PyInt_FromLong(currTask->isMaximized()));

        // is it iconified?
        PyList_Append(pList, PyInt_FromLong(currTask->isIconified()));

        // is it shaded?
        PyList_Append(pList, PyInt_FromLong(currTask->isShaded()));

        // is it focused?
        PyList_Append(pList, PyInt_FromLong(currTask->isActive()));

        // a reference back to itself
        PyList_Append(pList, PyInt_FromLong((long)currTask));

        return pList;

    } else {
        qWarning("Task not found.");
        return NULL;
    }
}

PyObject* py_get_task_info(PyObject *, PyObject *args)
{
    long widget, task;
    if (!PyArg_ParseTuple(args, (char*)"ll:getTaskInfo", &widget, &task))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;
    return getTaskInfo(widget, task);
}

// This returns all the info about a certain startup
// Return type is a Python List
PyObject* getStartupInfo(long widget, long cstartup)
{
    Q_UNUSED(widget);

    Startup::StartupPtr currentStartup;
    Startup::StartupPtr startup;

    Startup::List startupList = TaskManager::self()->startups();
    foreach(currentStartup, startupList) {
        if ((long)currentStartup.data() == (long)cstartup) {
            startup = currentStartup;
            break;
        }
    }

    if (!startup.isNull()) {
        PyObject* pList = PyList_New(0);

        //Startup Name
        if (startup -> text() != NULL) {
            PyList_Append(pList, PyString_FromString(startup -> text().toLatin1().constData()));
        } else {
            PyList_Append(pList, PyString_FromString(""));
        }

        //Icon Name
        if (startup -> icon() != NULL) {
            PyList_Append(pList, PyString_FromString(startup -> icon().toLatin1().constData()));
        } else {
            PyList_Append(pList, PyString_FromString(""));
        }

        //Executable Name
        if (startup -> bin() != NULL) {
            PyList_Append(pList, PyString_FromString(startup -> bin().toLatin1().constData()));
        } else {
            PyList_Append(pList, PyString_FromString(""));
        }

        // a reference back to itself
        PyList_Append(pList, PyInt_FromLong((long) startup));

        return pList;

    } else {
        return NULL;
    }
}

PyObject* py_get_startup_info(PyObject*, PyObject* args)
{
    long widget, startup;
    if (!PyArg_ParseTuple(args, (char*)"ll:getStartupInfo", &widget, &startup))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;
    return getStartupInfo(widget, startup);
}

// This gets a system task list
// It returns a String List of task names
PyObject* getTaskNames(long widget)
{
    Q_UNUSED(widget);

    PyObject* pList = PyList_New(0);
    PyObject* pString;

    QList<Task::TaskPtr> taskList = TaskManager::self()->tasks().values();

    Task::TaskPtr task;
    foreach(task, taskList) {
        const char* tmp = task.data()->name().toLatin1().constData();
        if (tmp == 0)
            continue;

        pString = PyString_FromString(tmp);
        if (pString)
            PyList_Append(pList, pString);
    }
    return pList;
}

PyObject* py_get_task_names(PyObject *, PyObject *args)
{
    long widget;
    if (!PyArg_ParseTuple(args, (char*)"l:getTaskNames", &widget))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;
    return getTaskNames(widget);
}

// This gets a system task list
PyObject* getTaskList(long widget)
{
    Q_UNUSED(widget);

    PyObject* pList = PyList_New(0);
    PyObject* pString;

    QList<Task::TaskPtr> taskList = TaskManager::self()->tasks().values();

    Task::TaskPtr task;
    foreach(task, taskList) {
        pString = PyInt_FromLong((long)task.data());
        PyList_Append(pList, pString);
    }
    return pList;
}

PyObject* py_get_task_list(PyObject *, PyObject *args)
{
    long widget;
    if (!PyArg_ParseTuple(args, (char*)"l:getTaskList", &widget))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;
    return getTaskList(widget);
}

// This gets a system startup list
PyObject* getStartupList(long widget)
{
    Q_UNUSED(widget);

    PyObject* pList = PyList_New(0);
    PyObject* pString;

    Startup::List startupList = TaskManager::self()->startups();

    Startup::StartupPtr startup;
    foreach(startup, startupList) {
        pString = PyInt_FromLong((long) startup.data());
        PyList_Append(pList, pString);
    }
    return pList;
}

PyObject* py_get_startup_list(PyObject *, PyObject *args)
{
    long widget;
    if (!PyArg_ParseTuple(args, (char*)"l:getStartupList", &widget))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;
    return getStartupList(widget);
}
