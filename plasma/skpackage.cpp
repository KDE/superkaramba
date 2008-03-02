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

#include "skpackage.h"

#include "../src/themefile.h"

#include <kdesktopfile.h>
#include <kconfiggroup.h>
#include <kstandarddirs.h>

K_EXPORT_PLASMA_PACKAGESTRUCTURE(superkaramba, SkPackage)

SkPackage::SkPackage(QObject *parent, const QVariantList &args)
    : Plasma::PackageStructure(parent)
{
    Q_UNUSED(args);
    setDefaultMimetypes( QStringList() << "application/zip" << "application/x-superkaramba" );
}

SkPackage::~SkPackage()
{
}

bool SkPackage::installPackage(const QString &archivePath, const QString &packageRoot)
{
    kDebug()<<"SkPackage::installPackage archivePath="<<archivePath<<"packageRoot="<<packageRoot;

    ThemeFile theme(archivePath);

    QDir root(packageRoot);
    const QString name = QString("sk_%1").arg(theme.id());
    if( !root.cd(name) && ( !root.mkdir(name) || !root.cd(name) ) ) {
        return false;
    }

    const QString path = root.path();
    if( ! theme.extractArchiveTo(path) ) {
        return false;
    }

    const QString desktopfile = KStandardDirs::locateLocal("services", QString("plasma-scriptengine-%1.desktop").arg(name));
    KDesktopFile desktop(desktopfile);
    KConfigGroup group = desktop.desktopGroup();

    group.writeEntry("Name", theme.name());
    group.writeEntry("Comment", i18n("SuperKaramba Theme"));

    const QString iconfile = theme.iconName().isEmpty() ? "superkaramba" : QFileInfo(path, theme.iconName()).absoluteFilePath();
    group.writeEntry("Icon", iconfile);
    group.writeEntry("Type", "Service");
    group.writeEntry("ServiceTypes", "Plasma/Applet");

    group.writeEntry("X-KDE-PluginInfo-Name", name);
    group.writeEntry("X-KDE-PluginInfo-Version", theme.version());
    group.writeEntry("X-KDE-PluginInfo-Author", theme.author());
    group.writeEntry("X-KDE-PluginInfo-Email", theme.authorEmail());
    group.writeEntry("X-KDE-PluginInfo-Website", theme.homepage());
    group.writeEntry("X-KDE-PluginInfo-License", theme.license());
    group.writeEntry("X-KDE-PluginInfo-Category", "SuperKaramba");
    group.writeEntry("X-KDE-PluginInfo-EnabledByDefault", "true");

    group.writeEntry("X-Plasma-Language", "superkaramba");
    group.writeEntry("X-Plasma-ComponentTypes", "Applet");
    group.writeEntry("X-Plasma-PackageFormat", "superkaramba");

    setPath(path);
    return true;
}

#include "skpackage.moc"
