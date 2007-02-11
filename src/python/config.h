/****************************************************************************
*  config_python.h  -  Functions for config python api
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

#ifndef CONFIG_PYTHON_H
#define CONFIG_PYTHON_H

/** Config/addMenuConfigOption
*
* SYNOPSIS
*   long addMenuConfigOption(widget, key, name)
* DESCRIPTION
*   SuperKaramba supports a simplistic configuration pop-up menu. This menu
*   appears when you right-click on a widget and choose Configure Theme.
*   Basically, it allows you to have check-able entrys in the menu to allow
*   the user to enable or disable features in your theme.
*
*   Before you use any configuration menu stuff, you NEED to add a new
*   callback to your script:
*
*   def menuOptionChanged(widget, key, value):
*
*   This will get called whenever a config menu option is changed. Now you
*   can add items to the config menu:
*
*   addMenuConfigOption(widget, String key, String name)
*
*   Key is the name of a key value where the value will be saved
*   automatically into the widget's config file. Name is the actual text that
*   will show up in the config menu.
*
*   For example, I could allow the user to enable or disable a clock showing
*   up in my theme:
*
*   karamba.addMenuConfigOption(widget, "showclock", "Display a clock")
* ARGUMENTS
*   * long widget -- karamba
*   * string key -- key for menu item
*   * string name -- name of the graph to get
* RETURN VALUE
*   1 if successful
*/
PyObject* py_add_menu_config_option(PyObject *self, PyObject *args);

/** Config/setMenuConfigOption
*
* SYNOPSIS
*   long setMenuConfigOption(widget, key, value)
* DESCRIPTION
*   This sets whether or not the given option is checked in the theme's
*   Configure Theme menu. Value should be 0 if key should not be checked and
*   1 if key should be checked.
*
*   See addMenuConfigOption for a more detailed explanation.
* ARGUMENTS
*   * long widget -- karamba
*   * string key -- key for menu item
*   * int value -- 1 if checked
* RETURN VALUE
*   1 if successful
*/
PyObject* py_set_menu_config_option(PyObject *self, PyObject *args);

/** Config/readMenuConfigOption
*
* SYNOPSIS
*   long readMenuConfigOption(widget, key)
* DESCRIPTION
*   This returns whether or not the given option is checked in the theme's
*   Configure Theme menu.
*
*   See addMenuConfigOption for a more detailed explanation.
* ARGUMENTS
*   * long widget -- karamba
*   * string key -- key for menu item
* RETURN VALUE
*   0 is returned if it is not checked and 1 is returned if it is.
*/
PyObject* py_read_menu_config_option(PyObject *self, PyObject *args);

/** Config/writeConfigEntry
*
* SYNOPSIS
*   long writeConfigEntry(widget, key, value)
* DESCRIPTION
*   SuperKaramba automatically supports configuration files for each theme.
*   These files will be saved in /your/home/dir/.superkaramba/ and will be
*   named themenamerc where themename is the name of the theme.
*
*   This function writes an entry into the config file with the given key and
*   value.
*
*   For example, to save my favorite color, I would do
*   karamba.writeConfigEntry(widget, "FavColor", "Red")
* ARGUMENTS
*   * long widget -- karamba
*   * string key -- key for config item
*   * string value -- config value
* RETURN VALUE
*   1 if successful
*/
PyObject* py_write_config_entry(PyObject *self, PyObject *args);

/** Config/readConfigEntry
*
* SYNOPSIS
*   string|long readConfigEntry(widget, key, value)
* DESCRIPTION
*   This function reads an entry from the config file with the given key.
* ARGUMENTS
*   * long widget -- karamba
*   * string key -- key for config item
* RETURN VALUE
*   config value for key
*/
PyObject* py_read_config_entry(PyObject *self, PyObject *args);

#endif // CONFIG_PYTHON_H
