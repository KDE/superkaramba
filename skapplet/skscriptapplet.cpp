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

#include "skscriptapplet.h"
//#include "skappletadaptor.h"

#include "../src/karamba.h"
#include "../src/karambamanager.h"

#include <QTimer>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <KToggleAction>

K_EXPORT_PLASMA_APPLETSCRIPTENGINE(superkaramba, SkScriptApplet)

class SkScriptApplet::Private
{
    public:
        QString themeFile;
        QPointer<Karamba> theme;

        Private() : theme(0) {}
        ~Private() { delete theme; }
};

SkScriptApplet::SkScriptApplet(QObject *parent, const QVariantList &args)
    : Plasma::AppletScript(parent)
    , d(new Private)
{
    Q_UNUSED(args);

    //setHasConfigurationInterface(true);
    //setDrawStandardBackground(true);
    //setAspectRatioMode(Qt::IgnoreAspectRatio);
}

SkScriptApplet::~SkScriptApplet()
{
    kDebug();
    delete d;
}

bool SkScriptApplet::init()
{
    Q_ASSERT( applet() );
    applet()->setContentSize(100, 50);

    QString name = QDir(package()->path()).dirName();
    if( name.toLower().startsWith("sk_") )
        name = name.mid(3);

    Q_ASSERT( package() );
    QFileInfo fi(package()->path(), QString("%1.theme").arg(name));
    if( ! fi.exists() )
        return false;

    d->themeFile = fi.absoluteFilePath();
    QTimer::singleShot(100, this, SLOT(loadKaramba()));
    return true;
}

void SkScriptApplet::loadKaramba()
{
    QGraphicsScene *scene = applet()->scene();
    Q_ASSERT( scene );
    Q_ASSERT( scene->views().count() > 0 );
    QGraphicsView* view = scene->views()[0];

    connect(KarambaManager::self(), SIGNAL(karambaStarted(QGraphicsItemGroup*)), this, SLOT(karambaStarted(QGraphicsItemGroup*)));
    connect(KarambaManager::self(), SIGNAL(karambaClosed(QGraphicsItemGroup*)), this, SLOT(karambaClosed(QGraphicsItemGroup*)));

    Q_ASSERT( ! d->theme );
    d->theme = new Karamba(d->themeFile, view);

    QPointF origPos = d->theme->pos();
    d->theme->setParentItem(applet());
    d->theme->moveToPos(origPos.toPoint());

    scene->installEventFilter(applet());
    view->installEventFilter(applet());
    view->viewport()->installEventFilter(applet());

    if( KToggleAction* lockedAction = d->theme->findChild<KToggleAction*>("lockedAction") ) {
        // disable locked action since Plasma will handle it for us.
        if( ! lockedAction->isChecked() )
            lockedAction->setChecked(true);
        lockedAction->setVisible(false);
    }
}

QSizeF SkScriptApplet::contentSizeHint() const
{
    return d->theme ? d->theme->boundingRect().size() : Plasma::AppletScript::contentSizeHint();
}

void SkScriptApplet::paintInterface(QPainter *painter, const QStyleOptionGraphicsItem *option, const QRect &contentsRect)
{
    //if( d->appletadaptor ) d->appletadaptor->paintInterface(painter, option, contentsRect);
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(contentsRect);
}

void SkScriptApplet::karambaStarted(QGraphicsItemGroup* group)
{
    if( d->theme && d->theme == group ) {
        kDebug()<<">>>>>>>>>>>> SkScriptApplet::karambaStarted";
        applet()->setContentSize(d->theme->boundingRect().size());
        applet()->updateConstraints(Plasma::SizeConstraint);
    }
}

void SkScriptApplet::karambaClosed(QGraphicsItemGroup* group)
{
    if( d->theme && d->theme == group ) {
        kDebug()<<">>>>>>>>>>>> SkScriptApplet::karambaClosed";
        //d->themeFile = QString();
        d->theme = 0;
        Q_ASSERT( applet() );
        Q_ASSERT( applet()->scene() );
        //applet()->scene()->removeItem(applet());
        applet()->destroy();
        deleteLater();
    }
}

#include "skscriptapplet.moc"
