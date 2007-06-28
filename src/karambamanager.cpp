/*
 * Copyright (c) 2007 Alexander Wiedenbruch <mail@wiedenbruch.de>
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

#include "karambamanager.h"

#include <KGlobal>

KarambaManager* KarambaManager::instance = 0;

class KarambaManager::Private
{
    public:
        QList<Karamba*> karambaList;
};

KarambaManager::KarambaManager()
    : d(new Private)
{
}

KarambaManager::~KarambaManager()
{
    while (d->karambaList.count() > 0) {
        d->karambaList[0]->closeWidget();
    }

    delete d;
}

KarambaManager* KarambaManager::self()
{
    if (instance == 0) {
        instance = new KarambaManager;
    }

    return instance;
}

void KarambaManager::addKaramba(Karamba *newKaramba)
{
    d->karambaList.append(newKaramba);
    emit karambaStarted(newKaramba);
}

void KarambaManager::removeKaramba(Karamba *karamba)
{
    d->karambaList.removeAll(karamba);
    emit karambaClosed(karamba);
    karamba->deleteLater();
}

Karamba* KarambaManager::getKaramba(const QString &prettyThemeName) const
{
    foreach (Karamba *k, d->karambaList) {
        if (prettyThemeName == k->prettyName()) {
            return k;
        }
    }

    return 0;
}

Karamba* KarambaManager::getKarambaByName(const QString &name) const
{
    foreach (Karamba *k, d->karambaList) {
        if (name == k->theme().name()) {
            return k;
        }
    }

    return 0;
}

QList<Karamba*> KarambaManager::getKarambas() const
{
    return d->karambaList;
}

bool KarambaManager::checkKaramba(const Karamba *karamba) const
{
    return d->karambaList.contains(const_cast<Karamba*>(karamba));
}


#include "karambamanager.moc"

