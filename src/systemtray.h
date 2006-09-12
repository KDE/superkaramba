/*
 ***************************************************************************
    copyright (C) 2003      Adam Geitgey <adam@rootnode.org>
                  2003      Sven Leiber <s.leiber@web.de>
                  2000-2001 Matthias Ettrich <ettrich@kde.org>
                  2000-2001 Matthias Elter   <elter@kde.org>
                  2001      Carsten Pfeiffer <pfeiffer@kde.org>
                  2001      Martijn Klingens <mklingens@yahoo.com>
 ***************************************************************************

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
 */

#ifndef TESTCARDAPPLET_H
#define TESTCARDAPPLET_H

#include <qobject.h>
#include <qpixmap.h>
#include <q3ptrlist.h>
//#include <qxembed.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <QWidget>

class KWinModule;

typedef long unsigned int Atom;


class Systemtray : public QWidget
{
    Q_OBJECT
public:
    Systemtray(QWidget* parent);
    ~Systemtray();
    
    void updateBackgroundPixmap ( const QPixmap & );

    int getCurrentWindowCount();

    virtual void initSystray( void );

public slots:
    void updateTrayWindows();
    int getTraySize();
    void systemTrayWindowAdded( WId w );
    void systemTrayWindowRemoved( WId w );
    void layoutSystray();
    
signals:
    void updated();

private:
    KWinModule *kwin_module;
    Q3ValueList<WId> systemTrayWindows;

    //Q3PtrList<QXEmbed> m_Wins;	// KDE4

    Atom net_system_tray_selection;
    Atom net_system_tray_opcode;

    int no_of_systray_windows;
};

#endif
