#include "skpackage.h"

#include "../src/themefile.h"

#include <kdesktopfile.h>
#include <kconfiggroup.h>

K_EXPORT_PLASMA_PACKAGESTRUCTURE(superkaramba, SkPackage)

SkPackage::SkPackage(QObject *parent, const QVariantList &args)
    : Plasma::PackageStructure(parent)
{
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

    const QString desktopfile = QFileInfo(path, QString("plasma-%1.desktop").arg(name)).absoluteFilePath();
    KDesktopFile desktop(desktopfile);
    KConfigGroup group = desktop.desktopGroup();
    group.writeEntry("Name", theme.name());
    group.writeEntry("Type", "Service");
    //group.writeEntry("Icon", "");
    group.writeEntry("ServiceTypes", "Plasma/Applet");
    group.writeEntry("X-KDE-Library", "plasma_applet_skapplet");
    group.writeEntry("X-KDE-PluginInfo-Version", theme.version());
    group.writeEntry("X-KDE-PluginInfo-Author", theme.author());
    group.writeEntry("X-KDE-PluginInfo-Email", theme.authorEmail());
    group.writeEntry("X-KDE-PluginInfo-Website", theme.homepage());
    group.writeEntry("X-KDE-PluginInfo-License", theme.license());
    group.writeEntry("X-KDE-PluginInfo-Category", "SuperKaramba");
    group.writeEntry("X-KDE-PluginInfo-EnabledByDefault", "false");

    setPath(path);
    return true;
}

#include "skpackage.moc"
