/*
*  misc_python.h  -  Misc Functions for python api
*
*  Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>
*  Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>
*  Copyright (C) 2004 Petri Damst� <damu@iki.fi> 
*  Copyright (C) 2004,2005 Luke Kenneth Casson Leighton <lkcl@lkcl.net>
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
*   an array of command options. The array is simply a list that contains the
*   command as the first entry, and each option as a seperate list entry.
*   Make sure to catch the returning value from executeInteractive(); it will
*   be the identifier (process number) for the command.
*
*   Output from the command triggers commandOutput callback. Among other items
*   commandOutput gives the process number of the command that gave the output.
*   This allows you to identify the output when running more than one command
*   at a time.
*
*   Example: 
*   To run the command "ls -la *.zip"
*
*      myCommand = ["ls", "-la", "*.zip"]
*      procID = karamba.executeInteractive(widget, myCommand)
*
*   To catch the output, screen commandOutput output for procID.
*
* ARGUMENTS
*   * long widget -- karamba
*   * list command -- command to execute
* RETURN VALUE
*   identification number of the executed command process
*/
PyObject* py_execute_command_interactive(PyObject* self, PyObject* args);

/** Misc/run
*
*  SYNOPSIS
*    long run(name, command, icon, list_of_args)
*  DESCRIPTION
*    This command simply executes a program or command on the system.
*    The difference between run and execute is that run takes arguments,
*    and the name of the icon to be displayed.
*  ARGUMENTS
*    * string name -- name to be displayed
*    * string command -- command to execute
*    * string icon -- name of icon to be displayed
*    * string list_of_args -- arguments to be passed to the command
*  RETURN VALUE
*    1 if successful
*/
PyObject* py_run_command(PyObject* self, PyObject* args);

/** Misc/attachClickArea
* 
*   SYNOPSIS
*     long attachClickArea(widget, meter, lB, mB, rB)
*   DESCRIPTION
*     It is possible to attach a clickarea to a meter (image or text field),
*     which is moved and resized correctly if the meter is moved or resized.
*
*     There is also a callback meterClicked(widget, meter, button) which is
*     called whenever a meter is clicked (if something is attached to it).
*     Given an Image or a TextLabel, this call makes it clickable. When a mouse
*     click is detected, the callback meterClicked is called.
*
*     lB, mB, and rB are strings that specify what command is executed when
*     this meter is clicked with the left mouse button, middle mouse button,
*     and right mouse button respectively. If given, the appropriate command is
*     executed when the mouse click is received.
*
*     The keyword arguments are all optional. If command is an empty string
*     nothing is executed.
*
*     For now the command given to RightButton has obviosly no effect (because
*     that brings up the SuperKaramba menu).
*
*   ARGUMENTS
*     * long widget -- karamba
*     * long meter -- pointer to meter
*     * string lB -- command to left mouse button
*     * string mB -- command to middle mouse button
*     * string rB -- command to right mouse button
*
*   RETURN VALUE
*     1 if successful
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
*   from. If you are running the theme from (compressed) skz (zip) file, the
*   return value will be the path of that file, including the file name.
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
*   Returns a string containing default language of a translation file.
*   Please, see userLanguage to obtain the preferred KDE interface language.
* ARGUMENTS
*   * long widget -- karamba
* RETURN VALUE
*   default language or empty string if no translation files found.
*/
PyObject* py_language(PyObject *self, PyObject *args);

/** Misc/userLanguage
*
* SYNOPSIS
*   string userLanguage(widget)
* DESCRIPTION
*   Returns a string containing the language name abbreviation for the
*   language user chose for the KDE session in Region & Language settings.
*   Implemented in version 0.40. Parse ~/.kde/share/config/kdeglobals
*   for 'Language' directly for earlier clients.
* ARGUMENTS
*   * long widget -- karamba
* RETURN VALUE
*   user language or empty string.
*/
PyObject* py_userLanguage(PyObject *self, PyObject *args);


/** Misc/userLanguages
*
* SYNOPSIS
*   string userLanguages(widget)
* DESCRIPTION
*   Returns a list (array) containing the language name abbreviations for the
*   preferred interface languages user chose for KDE session in Region & 
*   Language settings.
*   Having the whole array of preferred languages available is important for
*   cases when you cannot provide interface translation for the 1st preferred
*   language, but can for consecutive languages.
*   (Implemented in version 0.42.)
* ARGUMENTS
*   * long widget -- karamba
* RETURN VALUE
*   list (array) with user languages in the order of preference.
*/
PyObject* py_userLanguages(PyObject *self, PyObject *args);


/** Misc/readThemeFile
*
* SYNOPSIS
*   string readThemeFile(widget, file)
* DESCRIPTION
*   Returns a string containing the contents of the file.
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
*    to the clickable icon while hovering over this area.
* ARGUMENTS
*   * long widget -- karamba
*   * long x -- x coordinate
*   * long y -- y coordinate
*   * long w -- width
*   * long h -- height
*   * string cmd_to_run -- command to be run
* RETURN VALUE
*   a handle to the click area
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
*   * string theme -- path to new theme
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
*   Moves all widgets within a theme in a particular direction relative to
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

/** Misc/changeInterval
* 
*  SYNOPSIS
*    long changeInterval(widget, interval)
*  DESCRIPTION
*    Calling this changes your widget's refresh rate (ms)
*  ARGUMENTS
*    * long widget -- karamba
*    * int interval -- interval, in ms
*  RETURN VALUE
*    1 if successful
*/
PyObject* py_change_interval(PyObject *self, PyObject *args);

/** Misc/createServiceClickArea
*
*  SYNOPSIS
*    long createServiceClickArea(widget, x, y, w, h, name_of_command, cmd_to_run, icon_to_display)
*  DESCRIPTION
*     This creates a clickable area at x,y with width and height w,h. When
*     this area is clicked, cmd_to_run will be executed. The mouse will change
*     to the clickable icon when over this area.  For more information on
*     the difference between createClickArea and createServiceClickArea,
*     see the KDE documentation about KService, and the difference
*     between KRun::run and KRun::runCommand.
*  ARGUMENTS
*    * long widget -- karamba
*    * long x -- x coordinate
*    * long y -- y coordinate
*    * long w -- width
*    * long h -- height
*    * string name_of_command -- name to be displayed
*    * string cmd_to_run -- command to be run
*    * string icon_to_display -- name of icon to be displayed
*  RETURN VALUE
*    a handle to the click area
*/
PyObject* py_create_service_click_area(PyObject *self, PyObject *args);

/** Misc/removeClickArea
*
*  SYNOPSIS
*    long removeClickArea(widget, clickarea)
*  DESCRIPTION
*     This function deletes a clickable area.
*  ARGUMENTS
*    * long widget -- karamba
*    * long clickarea -- handle to the click area
*  RETURN VALUE
*    1 if successful
*/
PyObject* py_remove_click_area(PyObject *self, PyObject *args);


/** Misc/getPrettyThemeName
*
*  SYNOPSIS
*    string getPrettyThemeName(theme)
*  DESCRIPTION
*    When a theme is created with openNamedTheme, there is an
*    option to give the theme an alternative name.
*    This is useful if you open several widgets from the same theme:
*    you need to give them unique names in order to contact them
*    (for example, with callTheme or with setIncomingData)
*  ARGUMENTS
*    * string theme -- path to the theme
*  RETURN VALUE
*    the pretty name of the theme
*/
PyObject* py_get_pretty_name(PyObject *self, PyObject *args);

/** Misc/openNamedTheme
*
*  SYNOPSIS
*    long openNamedTheme(theme, pretty_name, is_sub_theme)
*  DESCRIPTION
*    Opens a new theme, giving it a pretty (alternative and by your
*    own choice _unique_) name.
*    If you do not want the theme to be loaded when superkaramba is
*    first started up (but instead want it to only be opened by
*    this function call) then set is_sub_theme to 1.
*    Themes opened with openNamedTheme will be unique.  If a theme
*    with the same pretty name already exists, openNamedTheme will
*    have no effect.  If you want duplicate themes (and a bit of a
*    mess), use openTheme, instead.
*  ARGUMENTS
*    * string theme -- path to new theme
*    * string pretty_name -- the name to be associated with the new widget
*    * long is_sub_theme -- sets persistence (save state) of the theme
*  RETURN VALUE
*    a handle to the widget created
*/
PyObject* py_open_named_theme(PyObject *self, PyObject *args);

/** Misc/callTheme
*
*  SYNOPSIS
*    long callTheme(widget, theme, info)
*  DESCRIPTION
*  Sends a string to the theme identified by the pretty name.
*  If you need to pass complex arguments (dictionaries, lists), use python
*  "repr" and "eval" functions to marshall and unmarshall the data structure.
*  (themeNotify Will be called in the target theme when message is received)
*
*  Note: As a bug/feature of SuperKaramba version 0.40, multiple instances of
*  the same theme share global variables. If you want to communicate to other
*  instances of the same theme, just communicate through global variables.
*  ARGUMENTS
*    * long widget -- karamba
*    * string theme -- pretty name of the theme to be called
*    * string info -- a string containing the info to be passed to the theme
*  RETURN VALUE
*    1 if successful
*/
PyObject* py_call_theme(PyObject *self, PyObject *args);

/** Misc/setIncomingData
*
*  SYNOPSIS
*    long setIncomingData(widget, theme, info)
*  DESCRIPTION
*  Contacts a theme - identified by the pretty name - and stores a string
*  to be associated with the remote theme.  The difference between
*  setIncomingData and callTheme is that the theme is not notified
*  by setIncomingData that the data has arrived.  Previous information,
*  if any, is overwritten.  Use getIncomingData to retrieve the last
*  stored information.
*  setIncomingData is not very sophisticated, and could benefit from
*  having info passed to it put into a queue, instead of being overwritten.
*  ARGUMENTS
*    * long widget -- karamba
*    * string theme -- pretty name of theme the information is passed to.
*    * string info -- a string containing the info to be passed to the theme
*  RETURN VALUE
*    1 if successful
*/
PyObject* py_set_incoming_data(PyObject *self, PyObject *args);

/** Misc/getIncomingData
*
*  SYNOPSIS
*    long getIncomingData(widget)
*  DESCRIPTION
*  Obtains the last data received by any other theme that set the
*  "incoming data" of this theme.  This isn't particularly sophisticated
*  and could benefit from the data being placed in an FIFO queue instead.
*  ARGUMENTS
*    * long widget -- karamba
*  RETURN VALUE
*    string containing last information received from setIncomingData
*/
PyObject* py_get_incoming_data(PyObject *self, PyObject *args);

/** Misc/getUpdateTime
*
*  SYNOPSIS
*    long getUpdateTime(widget)
*  DESCRIPTION
*    returns the last stored update time. Intended for use 
*    so that the next refresh interval can work out how long ago
*    the mouse was last moved over the widget.
*  ARGUMENTS
*    * long widget -- karamba
*  RETURN VALUE
*    last stored update time (from setUpdateTime)
*/
PyObject* py_get_update_time(PyObject *self, PyObject *args);

/** Misc/setWantRightButton
*
*  SYNOPSIS
*    long setWantRightButton(widget, want_receive_right_button)
*  DESCRIPTION
*    There's a management menu for SuperKaramba themes which
*    allows themes to be loaded, closed, moved to other
*    screens.  Not all themes will want this management
*    interface.  call karamba.wantRightButton(widget, 1)
*    if you want to receive MouseUpdate button notifications.
*  ARGUMENTS
*    * long widget -- karamba
*    * long want_receive_right_button -- whether the widget will receive right clicks
*  RETURN VALUE
*    1 if successful
*/
PyObject* py_want_right_button(PyObject *self, PyObject *args);

/** Misc/setWantMeterWheelEvent
*
*  SYNOPSIS
*    long setWantMeterWheelEvent(widget, want_receive_wheel_event)
*  DESCRIPTION
*    Enabling this allows themes to receive a wheel event when
*    the wheel is turned over a meter.
*    This function is available after version 0.42.
*    This behaviour is default in SuperKaramba 0.50 and later,
*    so this function will be not available after the 0.50 version.
*  ARGUMENTS
*    * long widget -- karamba
*    * long want_receive_wheel_event -- whether the theme will receive mouse wheel events
*  RETURN VALUE
*    1 if successful
*/
PyObject* py_want_wheel_event(PyObject *, PyObject *args);

/** Misc/managementPopup
*
*  SYNOPSIS
*    long managementPopup(widget)
*  DESCRIPTION
*    There's a management menu for SuperKaramba themes which
*    allows themes to be loaded, closed, moved to other
*    screens.  If you want this popup menu to appear, call
*    this function.
*  ARGUMENTS
*    * long widget -- karamba
*  RETURN VALUE
*    1 if successful
*/
PyObject* py_management_popup(PyObject *self, PyObject *args);

/** Misc/setUpdateTime
*
*  SYNOPSIS
*    long getUpdateTime(widget, updated_time)
*  DESCRIPTION
*    returns the last stored update time.  intended for use 
*    so that the next refresh interval can work out how long ago
*    the mouse was last moved over the widget.
*  ARGUMENTS
*    * long widget -- karamba
*    * long updated_time -- the update time to be associated with the widget
*  RETURN VALUE
*    1 if successful
*/
PyObject* py_set_update_time(PyObject *self, PyObject *args);

#endif /*  MISC_PYTHON_H */

