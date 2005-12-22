/****************************************************************************
*  menu_python.h  -  Functions for menu python api
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

#ifndef MENU_PYTHON_H
#define MENU_PYTHON_H

/** Menu/createMenu
*
* SYNOPSIS
*   long createMenu(widget)
* DESCRIPTION
*   This creates an empty popup menu and returns a pointer to the menu.
* ARGUMENTS
*   * long widget -- karamba
* RETURN VALUE
*   pointer to menu
*/
PyObject* py_create_menu(PyObject *self, PyObject *args);

/** Menu/deleteMenu
*
* SYNOPSIS
*   long deleteMenu(widget, menu)
* DESCRIPTION
*   This deletes the referenced menu if that menu exists.
* ARGUMENTS
*   * long widget -- karamba
*   * long menu -- pointer to menu
* RETURN VALUE
*   1 if menu existed and was deleted, returns 0 otherwise.
*/
PyObject* py_delete_menu(PyObject *self, PyObject *args);

/** Menu/addMenuItem
*
* SYNOPSIS
*   long addMenuItem(widget, menu, text, icon)
* DESCRIPTION
*   This adds an entry to the given menu with label text and with given icon.
*   icon can be just an application name in which case the user's current
*   icon set is used, or can be a path to a 16x16 png file.
*
*   The function returns the id of the menu item, which identifies that popup
*   menu item uniquely among popupmenu items application-wide or returns 0
*   if the given menu doesn't exist.
* ARGUMENTS
*   * long widget -- karamba
*   * long menu -- pointer to menu
*   * string text -- text for menu item
*   * string icon -- icon
* RETURN VALUE
*   menu item id
*/
PyObject* py_add_menu_item(PyObject *self, PyObject *args);

/** Menu/addMenuSeparator
*
* SYNOPSIS
*   long addMenuSeparator(widget, menu)
* DESCRIPTION
*   This adds an menu separator to the given menu.
* ARGUMENTS
*   * long widget -- karamba
*   * long menu -- pointer to menu
* RETURN VALUE
*   menu item id
*/
PyObject* py_add_menu_separator(PyObject *self, PyObject *args);

/** Menu/removeMenuItem
*
* SYNOPSIS
*   long removeMenuItem(widget, menu, id)
* DESCRIPTION
*   This removes the item with given id from given menu if that menu exists.
* ARGUMENTS
*   * long widget -- karamba
*   * long menu -- pointer to menu
*   * long id -- menu item id
* RETURN VALUE
*   1 if the menu item existed and was removed or returns zero otherwise.
*/
PyObject* py_remove_menu_item(PyObject *self, PyObject *args);

/** Menu/popupMenu
*
* SYNOPSIS
*   long popupMenu(widget, menu, x, y)
* DESCRIPTION
*   This pops up the given menu at the given co-ordinates. The co-ordinates
*   are relative to the widget, not the screen. You can use negative
*   co-ordinates to make a menu appear to the right of or above your theme.
* ARGUMENTS
*   * long widget -- karamba
*   * long menu -- pointer to menu
*   * long x -- x coordinate
*   * long y -- y coordinate
* RETURN VALUE
*   1 if the menu existed and was popped up, returns 0 otherwise.
*/
PyObject* py_popup_menu(PyObject *self, PyObject *args);

#endif // MENU_PYTHON_H
