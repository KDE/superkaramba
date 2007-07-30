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
#include "skappletadaptor.h"

#include <QGraphicsScene>
#include <QPainter>
#include <QTimer>
#include <QPointer>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsItemGroup>
#include <QGraphicsView>
//#include <QPaintEvent>
//#include <QImage>
//#include <QLabel>
//#include <QPixmap>
//#include <QX11Info>
//#include <QColor>
//#include <QPixmap>
//#include <QFile>
#include <KDebug>
#include <KLocale>
#include <KDialog>
#include <KFileDialog>
#include <KToggleAction>

//#include <plasma/svg.h>

#include "../src/karamba.h"
#include "../src/karambamanager.h"

K_EXPORT_PLASMA_APPLET(skapplet, SuperKarambaApplet)

/// \internal d-pointer class.
class SuperKarambaApplet::Private : public QObject
{
    public:
        SuperKarambaApplet* applet;
        Skip::AppletAdaptor* appletadaptor;
        QPointer<Karamba> themeItem;
        KUrl themePath;
        bool locked;

        explicit Private(SuperKarambaApplet* a) : applet(a), appletadaptor(0), themeItem(0), locked(true) {}
        ~Private() { delete appletadaptor; delete themeItem; }

        void initTheme()
        {
            Q_ASSERT(applet);
            Q_ASSERT(themeItem);
            QPointF origPos = themeItem->pos();
            themeItem->setParentItem(applet);
            themeItem->moveToPos( origPos.toPoint() );

            //testcases
            applet->scene()->installEventFilter(this);
            QGraphicsView* view = applet->scene()->views()[0];
            view->installEventFilter(this);
            view->viewport()->installEventFilter(this);

            if( KToggleAction* moveAction = themeItem->findChild<KToggleAction*>("moveAction") ) {
                moveAction->setChecked(locked);
                connect(moveAction, SIGNAL(toggled(bool)), applet, SLOT(lockedActionToggled(bool)));
            }

            delete appletadaptor;
            appletadaptor = new Skip::AppletAdaptor(themeItem, applet);
        }

    private:
        bool eventFilter(QObject* watched, QEvent* event)
        {
            switch( event->type() ) {
                case QEvent::ContextMenu:
                    kDebug() << "eventFilter type=ContextMenu watched=" << (watched ? QString("%1 [%2]").arg(watched->objectName()).arg(watched->metaObject()->className()) : "NULL") << endl;
                    //static_cast<QContextMenuEvent*>(event);
                    //return true;
                    break;
                case QEvent::GraphicsSceneContextMenu:
                    kDebug() << "eventFilter type=GraphicsSceneContextMenu watched=" << (watched ? QString("%1 [%2]").arg(watched->objectName()).arg(watched->metaObject()->className()) : "NULL") << endl;
                    break;
                case QEvent::KeyPress:
                    kDebug() << "eventFilter type=KeyPress watched=" << (watched ? QString("%1 [%2]").arg(watched->objectName()).arg(watched->metaObject()->className()) : "NULL") << endl;
                    //QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
                    //qDebug() << "Ate key press" << keyEvent->key();
                    break;
                default:
                    break;
            }
            return false;
        }
};

SuperKarambaApplet::SuperKarambaApplet(QObject *parent, const QStringList &args)
    : Plasma::Applet(parent, args)
    , d(new Private(this))
{
    //watchForFocus(d,true);

    kDebug() << "========================> SuperKarambaApplet Ctor" << endl;
    setHasConfigurationInterface(true);

    KConfigGroup cg = config();
    d->themePath = cg.readEntry("theme", KUrl());
    d->locked = cg.readEntry("locked", true);

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
    //watchForFocus(d, false);
    delete d;
}

Karamba* SuperKarambaApplet::karamba() const
{
    return d->themeItem;
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
    if( d->appletadaptor )
        d->appletadaptor->paintInterface(painter, option, rect);
}

void SuperKarambaApplet::showConfigurationInterface()
{
    if( d->themeItem )
        d->themeItem->popupGlobalMenu();
}

void SuperKarambaApplet::configAccepted()
{
    kDebug() << "SuperKarambaApplet::configAccepted" << endl;
    KConfigGroup cg = config();
    cg.writeEntry("theme", d->themePath);
    cg.writeEntry("locked", d->locked);
    QGraphicsItem::update();
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

void SuperKarambaApplet::lockedActionToggled(bool toggled)
{
    d->locked = toggled;
    configAccepted();
}

#include "skapplet.moc"