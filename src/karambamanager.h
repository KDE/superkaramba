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

#ifndef KARAMBA_MANAGER
#define KARAMBA_MANAGER

#include <QObject>

#include "karamba.h"
#include "superkaramba_export.h"

class SUPERKARAMBA_EXPORT KarambaManager : public QObject
{
    Q_OBJECT

    public:
        static KarambaManager* self();
        ~KarambaManager();

        void addKaramba(Karamba *newKaramba);
        void removeKaramba(Karamba *karamba);
        Karamba* getKaramba(const QString &prettyThemeName) const;
        Karamba* getKarambaByName(const QString &name) const;
        QList<Karamba*> getKarambas() const;
        bool checkKaramba(const Karamba *karamba) const;

    Q_SIGNALS:
        void karambaStarted(QGraphicsItemGroup*);
        void karambaClosed(QGraphicsItemGroup*);

    private:
        KarambaManager();

        class Private;
        Private* const d;

        static KarambaManager *instance;
};

#endif
