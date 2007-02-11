/***************************************************************************
 * Copyright (C) 2004 Luke Kenneth Casson Leighton <lkcl@lkcl.net>         * 
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef SVC_GRPS_PYTHON_H
#define SVC_GRPS_PYTHON_H

#include <Python.h>

//****p* Misc/getServiceGroups
//
// SYNOPSIS
//   list getServiceGroups(widget, path)
// DESCRIPTION
//  This function returns a list of services and service groups
//  that are in the user's KDE Menu.  It is not a recursive
//  function, so if there are submenus (service groups) in the
//  returned results, you must call getServiceGroups with the
//  path of the submenu in order to obtain the information in
//  that submenu.
//  The return result is complex: it's a list of tuples.
//  The tuple contains two elements - a 1 if the second element
//  is a service, and a 0 if it's a service group.
//  The second element is a dictionary, with keys (if they exist)
//  of caption, comment, icon, and relpath if it's a service group,
//  and keys (if they exist) of exec, menuid, name, path, icon,
//  library, comment, type and genericname.
//  To fully understand the return results of this function,
//  it is thoroughly recommended //  that you look up the
//  KDE documentation on KServiceGroup and KService.
// ARGUMENTS
//   * long widget -- karamba
//   * string path -- path to the Service Group you wish to retrieve
// RETURN VALUE
//   List of Dictionaries of services and service groups
//***
PyObject* py_get_service_groups(PyObject *self, PyObject *args);

#endif // SVC_GRPS_PYTHON_H
