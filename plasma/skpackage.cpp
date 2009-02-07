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

#include "themefile.h"

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
    kDebug()<<"archivePath="<<archivePath<<"packageRoot="<<packageRoot;

    ThemeFile theme(archivePath);

    QDir root(packageRoot);
    const QString name = QString("sk_%1").arg(theme.id());
    if( !root.cd(name) && ( !root.mkdir(name) || !root.cd(name) ) ) {
        return false;
    }

    const QString path = root.path();
    //if( ! theme.extractArchiveTo(path) ) {
    if( ! theme.copyArchiveTo(path) ) {
        kWarning()<<"Failed to copy archive="<<archivePath<<" to path="<<path;
        return false;
    }

    setPath(path);

    Plasma::PackageMetadata data;
    data.setPluginName(name);
    data.setImplementationApi("superkaramba");
    data.setCategory("SuperKaramba");
    //data.setServiceType(const QString &);
    data.setName(theme.name());
    data.setDescription(i18n("SuperKaramba Theme"));
    data.setAuthor(theme.author());
    data.setEmail(theme.authorEmail());
    data.setVersion(theme.version());
    data.setWebsite(theme.homepage());
    data.setLicense(theme.license());
    //data.setApplication("superkaramba");
    //data.setRequiredVersion(const QString &);

    QString iconfile = "superkaramba";
    if( ! theme.iconName().isEmpty() ) {
        theme.extractArchiveFileTo(theme.iconName(), path);
        QFileInfo fi(path, theme.iconName());
        if( fi.exists() )
            iconfile = fi.absoluteFilePath();
    }

    Plasma::Package::registerPackage(data, iconfile);

#if 0
    // write a desktop-file that is used by Plasma to know about our new applet.
    const QString desktopfile = KStandardDirs::locateLocal("services", QString("plasma-scriptengine-%1.desktop").arg(name));
    KDesktopFile desktop(desktopfile);
    KConfigGroup group = desktop.desktopGroup();

    group.writeEntry("Name", theme.name());
    group.writeEntry("Comment", i18n("SuperKaramba Theme"));
    group.writeEntry("Type", "Service");
    group.writeEntry("ServiceTypes", "Plasma/Applet");

    QString iconfile = "superkaramba";
    if( ! theme.iconName().isEmpty() ) {
        QFileInfo fi(path, theme.iconName());
        if( fi.exists() )
            iconfile = fi.absoluteFilePath();
    }
    group.writeEntry("Icon", iconfile);

    group.writeEntry("X-KDE-PluginInfo-Name", name);
    group.writeEntry("X-KDE-PluginInfo-Version", theme.version());
    group.writeEntry("X-KDE-PluginInfo-Author", theme.author());
    group.writeEntry("X-KDE-PluginInfo-Email", theme.authorEmail());
    group.writeEntry("X-KDE-PluginInfo-Website", theme.homepage());
    group.writeEntry("X-KDE-PluginInfo-License", theme.license());
    group.writeEntry("X-KDE-PluginInfo-Category", "SuperKaramba");
    group.writeEntry("X-KDE-PluginInfo-EnabledByDefault", "true");

    // the SkAppletScript ScriptEngine implementation is responsible for our new applet.
    group.writeEntry("X-Plasma-API", "superkaramba");
    // it is an applet.
    group.writeEntry("X-Plasma-ComponentTypes", "Applet");
    // the SkPackage PackageStructure implementation is responsible for the applets package format.
    group.writeEntry("X-Plasma-PackageFormat", "superkaramba");

    group.sync();

    // rebuild the cache to let e.g. Plasma know about our new desktop-file.
    //FIXME would be better if we just let plasma know about the new desktop-file and let it add on the fly without using the sycoca
    QDBusMessage msg = QDBusMessage::createSignal("/", "org.kde.KSycoca", "notifyDatabaseChanged" );
    msg << QStringList();
    if( QDBusConnection::sessionBus().isConnected() )
        QDBusConnection::sessionBus().send(msg);
#endif
    return true;
}

#include "skpackage.moc"
