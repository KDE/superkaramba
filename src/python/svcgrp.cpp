/***************************************************************************
 *                                                                         *
 * Copyright (C) 2004 Luke Kenneth Casson Leighton <lkcl@lkcl.net>         *
 *                                                                         *
 *  contains code from kickermenu:                                         *
 *                                                                         *
 *   Copyright (C) 2004 by Tommy Brander                                   *
 *   tbr00001@student.mdh.se                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#if defined(_XOPEN_SOURCE) && !defined(__SUNPRO_CC)
#undef _XOPEN_SOURCE
#endif

#include <Python.h>
#include "python/task.h"
#include <qobject.h>
#include <kservice.h>
#include <kservicegroup.h>
#include "../karamba.h"
#include "python/svcgrp.h"

static PyObject *get_svc_grp(KServiceGroup::Ptr const& g)
{
    //Avoid adding empty groups.
    KServiceGroup::Ptr subMenuRoot = KServiceGroup::group(g->relPath());
    if (subMenuRoot->childCount() == 0)
        return NULL;
    // Ignore dotfiles.
    if ((g->name().at(0) == '.'))
        return NULL;

    PyObject *tuple = PyTuple_New(2);
    PyObject *dict = PyDict_New();

    PyDict_SetItem(dict, PyString_FromString("caption"),
                   PyString_FromString(g->caption().toAscii().data()));
    if (g->comment() != NULL)
        PyDict_SetItem(dict, PyString_FromString("comment"),
                       PyString_FromString(g->comment().toAscii().data()));
    if (g->icon() != NULL)
        PyDict_SetItem(dict, PyString_FromString("icon"),
                       PyString_FromString(g->icon().toAscii().data()));
    PyDict_SetItem(dict, PyString_FromString("relpath"),
                   PyString_FromString(g->relPath().toAscii().data()));

    PyTuple_SET_ITEM(tuple, 0, Py_BuildValue((char*)"l", 0));
    PyTuple_SET_ITEM(tuple, 1, dict);

    return tuple;
}


static PyObject *get_svc(KService::Ptr const& g)
{
    PyObject *tuple = PyTuple_New(2);
    PyObject *dict = PyDict_New();

    if (!g->exec().isNull())
        PyDict_SetItem(dict, PyString_FromString("exec"),
                       PyString_FromString(g->exec().toAscii().data()));

    if (!g->menuId().isNull())
        PyDict_SetItem(dict, PyString_FromString("menuid"),
                       PyString_FromString(g->menuId().toAscii().data()));

    if (!g->name().isNull())
        PyDict_SetItem(dict, PyString_FromString("name"),
                       PyString_FromString(g->name().toAscii().data()));

    if (!g->path().isNull())
        PyDict_SetItem(dict, PyString_FromString("path"),
                       PyString_FromString(g->path().toAscii().data()));

    if (!g->icon().isNull())
        PyDict_SetItem(dict, PyString_FromString("icon"),
                       PyString_FromString(g->icon().toAscii().data()));

    if (!g->library().isNull())
        PyDict_SetItem(dict, PyString_FromString("library"),
                       PyString_FromString(g->library().toAscii().data()));
    if (!g->comment().isNull())
        PyDict_SetItem(dict, PyString_FromString("comment"),
                       PyString_FromString(g->comment().toAscii().data()));
    QString type = g->isApplication() ? "Application" : "Service";
    PyDict_SetItem(dict, PyString_FromString("type"),
                       PyString_FromString(type.toAscii().data()));

    if (!g->genericName().isNull())
        PyDict_SetItem(dict, PyString_FromString("genericname"),
                       PyString_FromString(g->genericName().toAscii().data()));
    /*
    PyDict_SetItem(dict, PyString_FromString("terminal"),
          Py_BuildValue("l", g->terminal()));
    PyDict_SetItem(dict, PyString_FromString("type"),
                   PyString_FromString(g->type().toAscii().data()));
    PyDict_SetItem(dict, PyString_FromString("username"),
                   PyString_FromString(g->username().toAscii().data()));
    PyDict_SetItem(dict, PyString_FromString("substuid"),
          Py_BuildValue("l", g->substituteUid()));
    PyDict_SetItem(dict, PyString_FromString("path"),
                   PyString_FromString(g->path().toAscii().data()));
          */

    PyTuple_SET_ITEM(tuple, 0, Py_BuildValue((char*)"l", 1));
    PyTuple_SET_ITEM(tuple, 1, dict);

    return tuple;
}

static PyObject *getServiceGroups(const char *rel_path)
{
    PyObject *list = PyList_New(0);

    // We ask KSycoca to give us all services (sorted).
    KServiceGroup::Ptr root = KServiceGroup::group(rel_path);

    if (!root || !root->isValid())
        return list;

    bool excludeNoDisplay_ = true;
    bool detailed_ = false;
    bool detailedNamesFirst_ = false;

    KServiceGroup::List sl = root->entries(true, excludeNoDisplay_, true,
                                           detailed_ && !detailedNamesFirst_);

    QStringList suppressGenericNames = root->suppressGenericNames();

    KServiceGroup::List::ConstIterator it = sl.constBegin();
    for (; it != sl.constEnd(); ++it) {
        KSycocaEntry *e = (KSycocaEntry*)(*it).data();

        PyObject *tuple = NULL;
        if (e->isType(KST_KServiceGroup)) {
            KServiceGroup::Ptr g(static_cast<KServiceGroup *>(e));
            tuple = get_svc_grp(g);
        } else if (e->isType(KST_KService)) {
            KService::Ptr g(static_cast<KService *>(e));
            tuple = get_svc(g);
        }

        if (tuple != NULL)
            PyList_Append(list, tuple);
    }

    return list;
}

PyObject* py_get_service_groups(PyObject *, PyObject *args)
{
    char *rel_path;
    if (!PyArg_ParseTuple(args, (char*)"s:getServiceGroup", &rel_path))
        return NULL;
    return getServiceGroups(rel_path);
}

