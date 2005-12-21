/*
*  misc_python.h  -  Misc Functions for python api
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

#ifndef MISC_PYTHON_H
#define MISC_PYTHON_H

/** @file
*
* These are global functions that are used to interpret
* certain Python calls.
*/

/** Misc/acceptDrops
*
* SYNOPSIS
*   long acceptDrops(widget)
* DESCRIPTION
*   Calling this enables your widget to receive Drop events. In other words,
*   the user will be able to drag icons from her desktop and drop them on
*   your widget. The "itemDropped" callback is called as a result with the
*   data about the icon that was dropped on your widget. This allows, for
*   example, icon bars where items are added to the icon bar by Drag and
*   Drop.
* ARGUMENTS
*   * long widget -- karamba
* RETURN VALUE
*   1 if successful
*/
PyObject* py_accept_drops(PyObject *self, PyObject *args);

/** Misc/execute
*
* SYNOPSIS
*   long execute(command)
* DESCRIPTION
*   This command simply executes a program or command on the system. This is
*   just for convience (IE you could accomplish this directly through python,
*   but sometimes threading problems crop up that way). The only option is a
*   string containing the command to execute.
* ARGUMENTS
*   * string command -- command to execute
* RETURN VALUE
*   1 if successful
*/
PyObject* py_execute_command(PyObject* self, PyObject* args);

/** Misc/executeInteractive
*
* SYNOPSIS
*   long executeInteractive(widget, command)
* DESCRIPTION
*   This command executes a program or command on the system. But it allows
*   you to get any text that the program outputs. Futhermore, it won't freeze
*   up your widget while the command executes.
*
*   To use it, call executeInteractive with the reference to your widget and
*   a list of command options. The array is simply a list that contains the
*   command as the first entry, and each option as a seperate list entry.
*   Output from the command is returned via the commandOutput callback.
*
*   The command returns the process number of the command. This is useful if
*   you want to run more than one program at a time. The number will allow
*   you to figure out which program is outputting in the commandOutput
*   callback.
*
*   Example: Run the command "ls -la *.zip"
*
*   myCommand = ["ls", "-la", "*.zip"]
*   karamba.executeInteractive(widget, myCommand)
* ARGUMENTS
*   * long widget -- karamba
*   * list command -- command to execute
* RETURN VALUE
*   1 if successful
*/
PyObject* py_execute_command_interactive(PyObject* self, PyObject* args);

/**
*   It is possible to attach a clickarea to a meter (image or text field),
*   which is moved and resized correctly if the meter is moved or resized.
*
*   This method corresponds to the Python call
     @code
     long attachClickArea(widget, meter, lB, mB, rB)
     @endcode
*
*   There is also a callback meterClicked(widget, meter, button) which is
*   called whenever a meter is clicked (if something is attached to it).
*   Given an Image or a TextLabel, this call makes it clickable. When a mouse
*   click is detected, the callback meterClicked is called.
*
*   lB, mB, and rB are strings that specify what command is executed when
*   this meter is clicked with the left mouse button, middle mouse button,
*   and right mouse button respectively. If given, the appropriate command is
*   executed when the mouse click is received.
*
*   The keyword arguments are all optional. If command is an empty string
*   nothing is executed.
*
*   For now the command given to RightButton has obviosly no effect (because
*   that brings up the SuperKaramba menu).
*
*   The Python arguments are as follows:
*   - long widget -- karamba
*   - long meter -- pointer to meter
*   - string lB -- command to left mouse button
*   - string mB -- command to middle mouse button
*   - string rB -- command to right mouse button
*
* @return  1 if successful
*/
PyObject* py_attach_clickArea(PyObject* self, PyObject* args, PyObject* dict);

/** Misc/toggleShowDesktop
*
* SYNOPSIS
*   long toggleShowDesktop(widget)
* DESCRIPTION
*   This shows/hides the current desktop just like the Show Desktop button on
*   kicker. Basically, it minimizes all the windows on the current desktop.
*   Call it once to show the desktop and again to hide it.
* ARGUMENTS
*   * long widget -- karamba
* RETURN VALUE
*   1 if successful
*/
PyObject* py_toggle_show_desktop(PyObject *self, PyObject *args);

/** Misc/getThemePath
*
* SYNOPSIS
*   string getThemePath(widget)
* DESCRIPTION
*   Returns a string containing the directory where your theme was loaded
*   from.
* ARGUMENTS
*   * long widget -- karamba
* RETURN VALUE
*   path to theme
*/
PyObject* py_get_theme_path(PyObject *self, PyObject *args);

/** Misc/language
*
* SYNOPSIS
*   string language(widget)
* DESCRIPTION
*   Returns a string containing default language.
* ARGUMENTS
*   * long widget -- karamba
* RETURN VALUE
*   default language or empty string if no translation files found.
*/
PyObject* py_language(PyObject *self, PyObject *args);

/** Misc/readThemeFile
*
* SYNOPSIS
*   string readThemeFile(widget, file)
* DESCRIPTION
*   Returns a string containing the file
* ARGUMENTS
*   * long widget -- karamba
*   * string file -- name of the file to read
* RETURN VALUE
*   file contents
*/
PyObject* py_read_theme_file(PyObject *self, PyObject *args);

/** Misc/createClickArea
*
* SYNOPSIS
*   long createClickArea(widget, x, y, w, h, cmd_to_run)
* DESCRIPTION
*    This creates a clickable area at x,y with width and height w,h. When
*    this area is clicked, cmd_to_run will be executed. The mouse will change
*    to the clickable icon when over this area.
* ARGUMENTS
*   * long widget -- karamba
*   * long x -- x coordinate
*   * long y -- y coordinate
*   * long w -- width
*   * long h -- height
*   * string cmd_to_run -- command to be run
* RETURN VALUE
*   1 if successful
*/
PyObject* py_create_click_area(PyObject *self, PyObject *args);

/** Misc/openTheme
*
* SYNOPSIS
*   long openTheme(theme)
* DESCRIPTION
*   Opens new theme.
* ARGUMENTS
*   * string theme -- path to new theme
* RETURN VALUE
*   1 if successful
*/
PyObject* py_open_theme(PyObject *self, PyObject *args);

/** Misc/reloadTheme
*
* SYNOPSIS
*   long reloadTheme(theme)
* DESCRIPTION
*   Reloads current theme.
* ARGUMENTS
*   * long widget -- karamba
* RETURN VALUE
*   1 if successful
*/
PyObject* py_reload_theme(PyObject *self, PyObject *args);

/** Misc/getNumberOfDesktop
*
* SYNOPSIS
*   long getNumberOfDesktop(widget)
* DESCRIPTION
*   Returns number fo desktops
* ARGUMENTS
*   * long widget -- karamba
* RETURN VALUE
*   number of desktops
*/
PyObject* py_get_number_of_desktops(PyObject *self, PyObject *args);

/** Misc/translateAll
*
* SYNOPSIS
*   long translateAll(widget, relative_x, relative_y)
* DESCRIPTION
*   Moves all widgets within a theme in a particular direction relative from
*   the previous spot without moving the parent theme widget.
* ARGUMENTS
*   * long widget -- karamba
*   * long translate_x -- move horizontally
*   * long translate_y -- move vertically
* RETURN VALUE
*   1 if successful
*/
PyObject* py_translate_all(PyObject *self, PyObject *args);

/** Misc/show
*
* SYNOPSIS
*   string show(widget)
* DESCRIPTION
*   show theme
* ARGUMENTS
*   * long widget -- karamba
* RETURN VALUE
*   1 if successful
*/
PyObject* py_show(PyObject *self, PyObject *args);

/** Misc/hide
*
* SYNOPSIS
*   string hide(widget)
* DESCRIPTION
*   hide theme
* ARGUMENTS
*   * long widget -- karamba
* RETURN VALUE
*   1 if successful
*/
PyObject* py_hide(PyObject *self, PyObject *args);

/** Misc/getIp
*
* SYNOPSIS
*   string getIp(widget, interface_name)
* DESCRIPTION
*   get current IP address of the interface_name interface.
* ARGUMENTS
*   * long widget -- karamba
*   * string interface_name -- name of the interface to get ip
* RETURN VALUE
*   ip address
*/
PyObject* py_get_ip(PyObject *self, PyObject *args);

#endif // MISC_PYTHON_H
