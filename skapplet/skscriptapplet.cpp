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

#include <QGraphicsScene>

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

    //setDrawStandardBackground(false);
}

SkScriptApplet::~SkScriptApplet()
{
    kDebug();
    delete d;
}

bool SkScriptApplet::init()
{
    QString name = QDir(package()->path()).dirName();
    if( name.toLower().startsWith("sk_") )
        name = name.mid(3);

    Q_ASSERT( package() );
    QFileInfo fi(package()->path(), QString("%1.theme").arg(name));
    if( ! fi.exists() )
        return false;

    d->themeFile = fi.absoluteFilePath();

    Q_ASSERT( applet() );
    QGraphicsScene *gfxScene = applet()->scene();
    Q_ASSERT( gfxScene );
    Q_ASSERT( gfxScene->views().count() > 0 );

    Q_ASSERT( ! d->theme );
    d->theme = new Karamba(d->themeFile, gfxScene->views()[0]);
    //d->initTheme();

    QPointF origPos = d->theme->pos();
    d->theme->setParentItem(applet());
    d->theme->moveToPos(origPos.toPoint());

    //d->appletadaptor = new Skip::AppletAdaptor(d->theme, applet());

    return true;
}

QSizeF SkScriptApplet::contentSizeHint() const
{
    return d->theme ? d->theme->boundingRect().size() : Plasma::AppletScript::contentSizeHint();
}

void SkScriptApplet::paintInterface(QPainter *p, const QStyleOptionGraphicsItem *option, const QRect &contentsRect)
{
    //if( d->appletadaptor ) d->appletadaptor->paintInterface(p, option, contentsRect);
}

#include "skscriptapplet.moc"
