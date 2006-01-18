/***************************************************************************
 *   Copyright (C) 2004 by Petri Damsten                                   *
 *   petri.damsten@iki.fi                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef DCOPINTERFACE_H
#define DCOPINTERFACE_H

#include <dcopobject.h>

class dcopIface : virtual public DCOPObject
{
  K_DCOP
public:

k_dcop:
    virtual ASYNC openTheme(QString file) = 0;
    virtual ASYNC openNamedTheme(QString file, QString name, bool is_sub_theme) = 0;
    virtual ASYNC closeTheme(QString name) = 0;
    virtual ASYNC quit() = 0;
    virtual ASYNC hideSystemTray(bool hide) = 0;
    virtual ASYNC showThemeDialog() = 0;

    virtual int themeAdded(QString appId, QString file) = 0;
    virtual ASYNC themeClosed(QString appId, QString file, int instance) = 0;
    virtual ASYNC themeNotify(QString name, QString text) = 0;
    virtual ASYNC setIncomingData(QString name, QString obj) = 0;
    virtual bool isMainKaramba() = 0;

};

#endif // DCOPINTERFACE_H
