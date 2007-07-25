/*
 * Copyright (C) 2007 Sebastian Sauer <mail@dipe.org>
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

#include "skapplet.h"

//#include <math.h>

//#include <QApplication>
#include <QFile>
//#include <QBitmap>
#include <QGraphicsScene>
//#include <QMatrix>
#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>
#include <QStyleOptionGraphicsItem>

#include <KDebug>
#include <KLocale>
//#include <KIcon>
//#include <KSharedConfig>
#include <KDialog>
#include <KFileDialog>

#include <plasma/svg.h>

extern "C" {
    /// The SuperKaramba library exports this function.
    typedef QGraphicsItemGroup* (*startKaramba)(const KUrl &theme, QGraphicsView *view);
}

SuperKarambaApplet::SuperKarambaApplet(QObject *parent, const QStringList &args)
    : Plasma::Applet(parent, args)
    , m_themeItem(0)
{
    kDebug() << "========================> SuperKarambaApplet Ctor" << endl;
    setHasConfigurationInterface(true);

    KConfigGroup cg = config("SuperKaramba");
    m_themePath = cg.readEntry("theme", KUrl());

    if( ! m_themePath.isValid() ) {
        //m_themePath = KUrl("file:///home/kde4/svn/_src/KDE/kdeutils/superkaramba/examples/rubyclock/clock.theme");
        //m_themePath = KUrl("/home/kde4/svn/_src/KDE/kdeutils/superkaramba/examples/richtext/richtext.theme");
        //m_themePath = KUrl("/home/kde4/svn/_src/KDE/kdeutils/superkaramba/examples/pythonclock/pythonclock.theme");
        //m_themePath = KUrl("/home/kde4/svn/_src/KDE/kdeutils/superkaramba/examples/aeroai/aero_aio.skz");


        KFileDialog* filedialog = new KFileDialog(
            KUrl("kfiledialog:///SuperKarambaPlasmaApplet"), // startdir
            i18n("*.skz *.theme|Theme Files\n*|All Files"), // filter
            0, // custom widget
            0 // parent
        );
        filedialog->setCaption( i18n("SuperKaramba Theme") );
        filedialog->setOperationMode( KFileDialog::Opening );
        filedialog->setMode( KFile::File | KFile::ExistingOnly | KFile::LocalOnly );
        if( filedialog->exec() )
            m_themePath = filedialog->selectedUrl();
    }

    QTimer::singleShot(0, this, SLOT(loadKaramba()));
}

SuperKarambaApplet::~SuperKarambaApplet()
{
    kDebug() << "========================> SuperKarambaApplet Dtor" << endl;
    delete m_themeItem;
}

void SuperKarambaApplet::loadKaramba()
{
    kDebug() << "SuperKarambaApplet::loadKaramba() Theme: " << m_themePath << endl;

    QGraphicsScene *gfxScene = scene();
    Q_ASSERT( gfxScene );

    QString karambaLib = QFile::encodeName(KLibLoader::self()->findLibrary(QLatin1String("libsuperkaramba")));
    KLibrary *lib = KLibLoader::self()->library(karambaLib);
    if (lib) {
        startKaramba createKaramba = 0;
        createKaramba = (startKaramba)lib->resolveFunction("startKaramba");
        if (createKaramba) {
            m_themeItem = createKaramba(m_themePath, gfxScene->views()[0]);
            m_themeItem->setParentItem(this);
            m_themeItem->setPos(0, 0);

            QObject* item = dynamic_cast<QObject*>(m_themeItem);
            if (item) {
                connect(this, SIGNAL(showKarambaMenu()), item, SLOT(popupGlobalMenu()));
            }
        }
    } else {
        kWarning() << "Could not load " << karambaLib << endl;
    }
}

void SuperKarambaApplet::showConfigurationInterface()
{
    emit showKarambaMenu();
}

void SuperKarambaApplet::configAccepted()
{
    kDebug() << "SuperKarambaApplet::configAccepted" << endl;

    KConfigGroup cg = config("SuperKaramba");
    cg.writeEntry("theme", m_themePath);
    /*
    m_showTimeString = ui.showTimeStringCheckBox->checkState() == Qt::Checked;
    m_showSecondHand = ui.showSecondHandCheckBox->checkState() == Qt::Checked;
    cg.writeEntry("showTimeString", m_showTimeString);
    cg.writeEntry("showSecondHand", m_showSecondHand);
    dataEngine("time")->setProperty("reportSeconds", m_showSecondHand);
    */
    QGraphicsItem::update();
    /*
    cg.writeEntry("size", ui.spinSize->value());
    m_theme->resize(ui.spinSize->value(), ui.spinSize->value());
    QStringList tzs = ui.timeZones->selection();
    dataEngine("time")->connectSource(m_timezone, this);
    */
    constraintsUpdated();
    cg.config()->sync();
}

void SuperKarambaApplet::constraintsUpdated()
{
    kDebug() << "SuperKarambaApplet::constraintsUpdated" << endl;
    Plasma::Applet::constraintsUpdated();
}

QRectF SuperKarambaApplet::boundingRect() const
{
    if (m_themeItem) {
        return m_themeItem->boundingRect();
    } else {
        return Applet::boundingRect();
    }
}

#include "skapplet.moc"
