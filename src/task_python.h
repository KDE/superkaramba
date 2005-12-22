/****************************************************************************
*  task_python.h  -  Functions for task python api
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

#ifndef TASK_PYTHON_H
#define TASK_PYTHON_H

/** Task/performTaskAction
*
* SYNOPSIS
*   long performTaskAction(widget, task, action)
* DESCRIPTION
*   This peforms the given action on a task object. widget is a reference to
*   the current widget. task is a reference to a task object you got from
*   getTaskList(). Action is a number from 1 to 10. See the list below.
*
*   Possible actions:
*   * 1 = Maximize the window
*   * 2 = Restore the window (use on iconified windows)
*   * 3 = Iconify the window (minimize it)
*   * 4 = Close the window
*   * 5 = Activate (give focus to) the window
*   * 6 = Raise the window
*   * 7 = Lower the window
*   * 8 = Smart Focus/Minimize - This will what the KDE taskbar does when you
*         click on a window. If it is iconified, raise it. If it has focus,
*         iconify it.
*   * 9 = Toggle whether this window is always on top
*   * 10 = Toggle wheter this window is shaded (rolled up)
* ARGUMENTS
*   * long widget -- karamba
*   * long task -- pointer to task
*   * long action -- action number
* RETURN VALUE
*   1 if successful
*/
PyObject* py_perform_task_action(PyObject *self, PyObject *args);

/** Task/getTaskInfo
*
* SYNOPSIS
*   list getTaskInfo(widget, task)
* DESCRIPTION
*   This returns all of the info about a certain task in the form of a Python
*   List. widget is a reference to the current widget. task is a reference to
*   the window you want info about which you obtain by calling getTaskList().
* ARGUMENTS
*   * long widget -- karamba
*   * long task -- pointer to task
* RETURN VALUE
*   Here is the format of the returned list by index value:
*   * 0 = Task name (The full name of the window)
*   * 1 = Icon name
*   * 2 = Class name - This is for grouping tasks. All tasks with the same
*         name can be grouped together because they are instances of the same
*         program.
*   * 3 = Desktop number - The desktop number this window is on
*   * 4 = Is this window maximized? 0=no, 1=yes
*   * 5 = Is this window iconified (minimized)? 0=no, 1=yes
*   * 6 = Is this window shaded (rolled up)? 0=no, 1=yes
*   * 7 = Is this window focused? 0=no, 1=yes
*   * 8 = A reference back to the task you got info on
*/
PyObject* py_get_task_info(PyObject *self, PyObject *args);

/** Task/getStartupInfo
*
* SYNOPSIS
*   list getStartupInfo(widget, task)
* DESCRIPTION
*   This returns all of the info about a certain starting task in the form of
*   a Python List. widget is a reference to the current widget. task is a
*   reference to the window you want info about which you obtain by calling
*   getStartupList().
* ARGUMENTS
*   * long widget -- karamba
*   * long task -- pointer to task
* RETURN VALUE
*   Here is the format of the returned list by index value:
*   * 0 = Task name (The full name of the window)
*   * 1 = Icon name
*   * 2 = Executable name
*   * 3 = A reference back to the task you got info on
*/
PyObject* py_get_startup_info(PyObject* self, PyObject* args);

/** Task/getTaskNames
*
* SYNOPSIS
*   list getTaskNames(widget)
* DESCRIPTION
*   This returns a Python List containing the String names of all open
*   windows on the system. This is for convience if you want to list open
*   windows or see if a window by a certain name exists. Anything else
*   requires the reference to the window you would obtain from getTaskList()
* ARGUMENTS
*   * long widget -- karamba
* RETURN VALUE
*   Task list
*/
PyObject* py_get_task_names(PyObject *self, PyObject *args);

/** Task/getTaskList
*
* SYNOPSIS
*   list getTaskList(widget)
* DESCRIPTION
*   This returns a Python List object with references to all the current
*   windows open on this system. You can then call performTaskAction() or
*   getTaskInfo() on any of the entries in the list.
* ARGUMENTS
*   * long widget -- karamba
* RETURN VALUE
*   Task list
*/
PyObject* py_get_task_list(PyObject *self, PyObject *args);

/** Task/getStartupList
*
* SYNOPSIS
*   list getTaskList(widget)
* DESCRIPTION
*   This returns a Python List object with references to all the current
*   windows that are in the process of loading on this system. You can then
*   call getStartupInfo() on any of the entries in the list.
* ARGUMENTS
*   * long widget -- karamba
* RETURN VALUE
*   startup list
*/
PyObject* py_get_startup_list(PyObject *self, PyObject *args);

#endif // TASK_PYTHON_H
