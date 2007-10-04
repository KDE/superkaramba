/****************************************************************************
*  karambasessionmanaged.cpp  -  Karamba session management
*
*  Copyright (C) 2004 -
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
#include "karambasessionmanaged.h"

#include "karamba.h"

#include <KApplication>
#include <KConfig>
#include <KConfigGroup>
#include <KSessionManager>
#include "karambamanager.h"

bool KarambaSessionManaged::saveState(QSessionManager&)
{
    QList<QString> openThemes;

    QList<Karamba*> themes = KarambaManager::self()->getKarambas();
    foreach(Karamba *k, themes) {
        if (k->isSubTheme()) {
            continue;
        }

        QString path = QFileInfo(k->theme().file()).absoluteFilePath();
        k->writeConfigData();
        openThemes.append(path);
    }

    KSharedConfigPtr config = KGlobal::config();
    KConfigGroup group(config, "Session");
    group.writeEntry("OpenThemes", openThemes);

    /*
    KConfig *config = kapp->sessionConfig();
    KConfigGroup group(config, "Session");
    group.writeEntry("OpenThemes", openThemes);
    */
    return true;
}

bool KarambaSessionManaged::commitData(QSessionManager&)
{
    return true;
}
