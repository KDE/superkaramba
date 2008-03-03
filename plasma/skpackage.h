/*
 * Copyright (C) 2008 Sebastian Sauer <mail@dipe.org>
 *
 * This file is part of SuperKaramba.
 *
 * SuperKaramba is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * SuperKaramba is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SuperKaramba; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef SKPACKAGE_H
#define SKPACKAGE_H

#include <plasma/package.h>
#include <plasma/packagestructure.h>
#include <plasma/packagemetadata.h>

/**
 * The SkPackage class implements a \a Plasma::PackageStructure to
 * deal with SuperKaramba Theme Files - those handy *.skz files
 * packaged as zip.
 */
class SkPackage : public Plasma::PackageStructure
{
    Q_OBJECT
public:

    /**
     * Constructor.
     *
     * \param parent The parent QObject this QObject will be
     * child of.
     * \param args The optional list of arguments.
     */
    SkPackage(QObject *parent, const QVariantList &args);

    /**
     * Destructor.
     */
    virtual ~SkPackage();

    /**
     * Installs a package matching this package structure.
     *
     * @param archivePath path to the package archive file
     * @param packageRoot path to the directory where the package should be
     *                    installed to
     * @return true on successful installation, false otherwise
     **/
    virtual bool installPackage(const QString &archivePath, const QString &packageRoot);
};

#endif
