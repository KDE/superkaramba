/*
 * Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>
 * Copyright (c) 2005 Ryan Nickell <p0z3r@earthlink.net>
 *
 * This file is part of SuperKaramba.
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

#ifndef __showdesktop_h__
#define __showdesktop_h__

class KWinModule;

/**
 * Singleton class that handles desktop access (minimizing all windows)
 */
class ShowDesktop : public QObject
{
    Q_OBJECT

public:
    static ShowDesktop* the();
    bool desktopShowing() { return showingDesktop; }

public slots:
    void showDesktop( bool show );
    void toggle() { showDesktop( !desktopShowing() ); }

signals:
    void desktopShown( bool shown );

private slots:
    void slotCurrentDesktopChanged(int);
    void slotWindowChanged(WId w, unsigned int dirty);

private:
    ShowDesktop();

    bool              showingDesktop;
    KWinModule*       kWinModule;
    QValueList<WId>   iconifiedList;

};

#endif
