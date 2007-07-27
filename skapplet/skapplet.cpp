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

//#include <QFile>
#include <QGraphicsScene>
//#include <QPaintEvent>
#include <QPainter>
//#include <QPixmap>
#include <QPointer>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsItemGroup>
#include <KDebug>
#include <KLocale>
#include <KDialog>
#include <KFileDialog>
#include <KToggleAction>

//#include <plasma/svg.h>

#include "../src/karamba.h"
#include "../src/karambamanager.h"

/// \internal d-pointer class.
class SuperKarambaApplet::Private
{
    public:
        SuperKarambaApplet* applet;
        QPointer<Karamba> themeItem;
        KUrl themePath;

        explicit Private(SuperKarambaApplet* a) : applet(a), themeItem(0) {}
        ~Private() { delete themeItem; }

        void initTheme()
        {
            Q_ASSERT(applet);
            QPointF origPos = themeItem->pos();
            themeItem->setParentItem(applet);
            themeItem->moveToPos( origPos.toPoint() );
            KToggleAction* moveAction = themeItem->findChild<KToggleAction*>("moveAction");
            if( moveAction ) {
                connect(moveAction, SIGNAL(toggled(bool)), applet, SLOT(moveActionToggled(bool)));
                applet->setLockApplet( moveAction->isChecked() );
            }
        }

        void setLockApplet(bool locked)
        {
            Q_ASSERT(themeItem);
            KToggleAction* moveAction = themeItem->findChild<KToggleAction*>("moveAction");
            if( moveAction )
                moveAction->setChecked(locked);
        }
};

SuperKarambaApplet::SuperKarambaApplet(QObject *parent, const QStringList &args)
    : Plasma::Applet(parent, args)
    , d(new Private(this))
{
    kDebug() << "========================> SuperKarambaApplet Ctor" << endl;
    setHasConfigurationInterface(true);

    KConfigGroup cg = config("SuperKaramba");
    d->themePath = cg.readEntry("theme", KUrl());

    if( ! d->themePath.isValid() ) {
        //d->themePath = KUrl("file:///home/kde4/svn/_src/KDE/kdeutils/superkaramba/examples/rubyclock/clock.theme");
        //d->themePath = KUrl("/home/kde4/svn/_src/KDE/kdeutils/superkaramba/examples/richtext/richtext.theme");
        //d->themePath = KUrl("/home/kde4/svn/_src/KDE/kdeutils/superkaramba/examples/pythonclock/pythonclock.theme");
        //d->themePath = KUrl("/home/kde4/svn/_src/KDE/kdeutils/superkaramba/examples/aeroai/aero_aio.skz");

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
            d->themePath = filedialog->selectedUrl();
    }

    if( d->themePath.isValid() )
        QTimer::singleShot(100, this, SLOT(loadKaramba()));
}

SuperKarambaApplet::~SuperKarambaApplet()
{
    kDebug() << "========================> SuperKarambaApplet Dtor" << endl;
    delete d;
}

void SuperKarambaApplet::loadKaramba()
{
    kDebug() << "SuperKarambaApplet::loadKaramba() Theme: " << d->themePath << endl;

    QGraphicsScene *gfxScene = scene();
    Q_ASSERT( gfxScene );
    Q_ASSERT( gfxScene->views().count() > 0 );

    d->themeItem = new Karamba(d->themePath, gfxScene->views()[0]);
    d->initTheme();

    connect(KarambaManager::self(), SIGNAL(karambaStarted(QGraphicsItemGroup*)), this, SLOT(karambaStarted(QGraphicsItemGroup*)));
    connect(KarambaManager::self(), SIGNAL(karambaClosed(QGraphicsItemGroup*)), this, SLOT(karambaClosed(QGraphicsItemGroup*)));
}

void SuperKarambaApplet::paintInterface(QPainter *painter, const QStyleOptionGraphicsItem *option, const QRect &rect)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(rect);
}

void SuperKarambaApplet::showConfigurationInterface()
{
    if( d->themeItem )
        d->themeItem->popupGlobalMenu();
}

void SuperKarambaApplet::configAccepted()
{
    kDebug() << "SuperKarambaApplet::configAccepted" << endl;

    KConfigGroup cg = config("SuperKaramba");
    cg.writeEntry("theme", d->themePath);
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
    return d->themeItem ? d->themeItem->boundingRect() : Applet::boundingRect();
}

void SuperKarambaApplet::karambaStarted(QGraphicsItemGroup* group)
{
    if( ! d->themeItem ) {
        kDebug()<<">>>>>>>>>>>> SuperKarambaApplet::karambaStarted"<<endl;
        d->themeItem = dynamic_cast< Karamba* >( group );
        Q_ASSERT(d->themeItem);
        d->initTheme();
        QGraphicsItem::update();
        constraintsUpdated();
    }
}

void SuperKarambaApplet::karambaClosed(QGraphicsItemGroup* group)
{
    if( d->themeItem == group ) {
        kDebug()<<">>>>>>>>>>>> SuperKarambaApplet::karambaClosed"<<endl;
        d->themeItem = 0;
    }
}

void SuperKarambaApplet::moveActionToggled(bool toggled)
{
    if( d->applet && d->applet->lockApplet() != toggled )
        d->applet->setLockApplet(toggled);
}

void SuperKarambaApplet::slotLockApplet(bool locked)
{
    Plasma::Applet::slotLockApplet(locked);
    d->setLockApplet(locked);
}

#include "skapplet.moc"
