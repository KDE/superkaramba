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

#include "skappletscript.h"
#include "skappletadaptor.h"

#include "karamba.h"
#include "karambamanager.h"

#include <QTimer>
#include <QLabel>
#include <QComboBox>
#include <QGridLayout>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <KToggleAction>
#include <KColorScheme>
#include <KGlobalSettings>

K_EXPORT_PLASMA_APPLETSCRIPTENGINE(superkaramba, SkAppletScript)

class SkAppletScript::Private
{
    public:
        QString themeFile;
        QPointer<Karamba> theme;
        SkAppletAdaptor* appletadaptor;
        QList<QAction*> actions;
        QStringList errors;

        Private() : theme(0), appletadaptor(0) {}
        ~Private() { delete theme; }
};

SkAppletScript::SkAppletScript(QObject *parent, const QVariantList &args)
    : Plasma::AppletScript(parent)
    , d(new Private)
{
    Q_UNUSED(args);
}

SkAppletScript::~SkAppletScript()
{
    kDebug();
    delete d;
}

bool SkAppletScript::init()
{
    Q_ASSERT( applet() );
    Q_ASSERT( package() );

    if( applet()->isContainment() ) { // Plasma::Containment
        Plasma::Containment *c = dynamic_cast<Plasma::Containment *>(applet());

        //c->setContainmentType(Plasma::Containment::PanelContainment);
        c->setZValue(150);
        c->setFormFactor(Plasma::Horizontal);
        c->setLocation(Plasma::TopEdge);
        //c->setScreen(0);
        //c->addApplet("clock");
        //c->flushUpdatedConstraints();
        //if (c->layout()) c->layout()->invalidate();
        //c->updateConstraints(Plasma::AllConstraints);
        //package()->metadata()->;
    }
    else { // Plasma::Applet
        //applet()->setHasConfigurationInterface(false);//true);
//applet()->setDrawStandardBackground(false);
        applet()->setAspectRatioMode(Plasma::IgnoreAspectRatio);
    }

    QDir dir(package()->path());
    QString name = dir.dirName();
    if( name.toLower().startsWith("sk_") )
        name = name.mid(3);
    QFileInfo fi(dir, QString("%1.theme").arg(name));
    if( ! fi.exists() ) {
        QFileInfoList files = dir.entryInfoList(QStringList() << "*.skz" << "*.theme", QDir::Files);
        if(files.count() < 1) {
            kWarning() << "Failed to locate the themefile in path=" << package()->path();
            return false;
        }
        fi = files[0];
    }

    d->themeFile = fi.absoluteFilePath();
    QTimer::singleShot(50, this, SLOT(loadKaramba()));
    return true;
}

void SkAppletScript::loadKaramba()
{
    d->errors.clear();

    Q_ASSERT( applet() );
    Q_ASSERT( applet()->scene() );
    Q_ASSERT( applet()->scene()->views().count() > 0 );
    QGraphicsScene *scene = applet()->scene();
    QGraphicsView* view = scene->views()[0];

    connect(KarambaManager::self(), SIGNAL(karambaStarted(QGraphicsItemGroup*)), this, SLOT(karambaStarted(QGraphicsItemGroup*)));
    connect(KarambaManager::self(), SIGNAL(karambaClosed(QGraphicsItemGroup*)), this, SLOT(karambaClosed(QGraphicsItemGroup*)));

    Q_ASSERT( ! d->theme );
    d->theme = new Karamba(d->themeFile, view, -1, false, QPoint(), false, false);
    d->theme->setParentItem(applet());
    const QRectF geometry = applet()->geometry();
    d->theme->moveToPos(QPoint(int(geometry.x()), int(geometry.y())));

    //view->viewport()->installEventFilter(this);

    if( applet()->isContainment() ) { // Plasma::Containment
        Plasma::Containment *c = dynamic_cast<Plasma::Containment *>(applet());
        Q_ASSERT(c);
        d->appletadaptor = new SkContainmentAdaptor(d->theme, c);

        // While Plasma::Applet does provide such a functionality, Plasma::Containment
        // does not. So, let's add it manualy...
        //QAction* configure = new QAction(i18n("Panel Settings"), this);
        //configure->setIcon(KIcon("configure"));
        //connect(configure, SIGNAL(triggered()), this, SLOT(showConfigurationInterface()));
        //d->actions << configure;
    }
    else { // Plasma::Applet
        d->appletadaptor = new SkAppletAdaptor(d->theme, applet());
    }

    if( KToggleAction* lockedAction = d->theme->findChild<KToggleAction*>("lockedAction") ) {
        // disable locked action since Plasma will handle it for us.
        if( ! lockedAction->isChecked() )
            lockedAction->setChecked(true);
        lockedAction->setVisible(false);
    }

    if( QAction* configAction = d->theme->findChild<QAction*>("configureTheme") ) {
        d->actions << configAction;
    }

    connect(d->theme, SIGNAL(positionChanged()), this, SLOT(positionChanged()));
    connect(d->theme, SIGNAL(sizeChanged()), this, SLOT(sizeChanged()));
    connect(d->theme, SIGNAL(error(QString)), this, SLOT(scriptError(QString)));

    // hack to prevent the applet's background from being drawn
    //applet()->setOpacity(0.0);

    d->theme->startKaramba();
}

void SkAppletScript::positionChanged()
{
    QPointF p = applet()->pos();
    //QRectF r = applet()->geometry();

//QSizeF s = r.size();
//s = QSizeF(s.width()+p.x(),s.height()+p.y());
//applet()->setContentSize(s);


    //FIXME WTF, sebsauer, 2008-03-07; somehow this doesn't seem to work correct if we are a panel :-(

    //Q_ASSERT( ! managingLayout() );

    //applet()->setPos(p);
    //d->theme->getView()->move(p.x(),p.y());

/*
QPointF p2;// = r.topLeft();
p = QPointF(p.x()+p2.x(),p.y()+p2.y());

    r.moveTo(p);
    applet()->setGeometry(r);
*/

applet()->moveBy(p.x(),p.y());
//applet()->parentItem()->moveBy(p.x(),p.y());


    //applet()->update(r);
    //d->theme->getView()->update();
    //applet()->updateConstraints();//Plasma::SizeConstraint
}

void SkAppletScript::sizeChanged()
{
    QRectF r = d->theme->boundingRect();
    QSizeF s = r.size();
    applet()->resize(s);
    applet()->setMinimumSize(s);//hack else a Panel may lose its size :-/
    //applet()->updateConstraints(Plasma::SizeConstraint);
}

void SkAppletScript::scriptError(const QString& err)
{
    d->errors << err;
}

#if 0
QSizeF SkAppletScript::contentSizeHint() const
{
    if( ! d->theme )
        return Plasma::AppletScript::contentSizeHint();
    QRectF r = d->theme->boundingRect();
    QSizeF s = r.size();
    return s;
}
#endif

void SkAppletScript::paintInterface(QPainter *painter, const QStyleOptionGraphicsItem *option, const QRect &contentsRect)
{
    if( d->errors.count() > 0 ) {
        QColor fontcolor = KColorScheme(QPalette::Active, KColorScheme::View, Plasma::Theme::defaultTheme()->colorScheme()).foreground().color();
        painter->setPen(QPen(fontcolor));

        painter->setRenderHint(QPainter::SmoothPixmapTransform);
        painter->setRenderHint(QPainter::Antialiasing);

        const QString title = i18n("Failed to launch SuperKaramba Theme");

        QFont titlefont;
        titlefont.setBold( true );
        painter->setFont(titlefont);

        QRect titlerect = painter->boundingRect(contentsRect, 0, title);
        painter->drawText(titlerect, title, QTextOption());

        QRect textrect = contentsRect;
        textrect.setY( titlerect.y() + titlerect.height() + 4 );

        const QString text = d->errors.join("\n");

        QFont textfont;
        textfont.setPointSize( KGlobalSettings::smallestReadableFont().pointSize() );
        painter->setFont(textfont);

        QTextOption textopts;
        textopts.setWrapMode(QTextOption::WrapAnywhere);
        painter->drawText(textrect, text, textopts);
    }
    else {
        //painter->save();
        //painter->setCompositionMode(QPainter::CompositionMode_Source);
        //painter->fillRect(contentsRect,Qt::transparent);
        //painter->restore();

        if( d->appletadaptor ) {
            d->appletadaptor->paintInterface(painter, option, contentsRect);
        }
    }
}

QList<QAction*> SkAppletScript::contextualActions()
{
    return d->actions;
}

void SkAppletScript::constraintsEvent(Plasma::Constraints constraints)
{
    if( constraints & Plasma::FormFactorConstraint ) {
#if 0
        if( ! applet()->isContainment() ) {
            applet()->setDrawStandardBackground(false);
        }
#endif
    }

    if( constraints & Plasma::SizeConstraint ) {
        if( d->theme ) {
            const QRectF r = d->theme->boundingRect();
            const QSizeF s = applet()->geometry().size();
            const qreal x = s.width() / r.width();
            const qreal y = s.height() / r.height();

            QTransform t = d->theme->transform();
            t.reset();
            d->theme->setTransform(t.scale(x, y));
        }
    }

#if 0
    if( constraints & Plasma::ImmutableConstraint ) {
        Q_ASSERT( applet() );
        //applet()->setDrawStandardBackground();
        //applet()->setHandlesChildEvents();
        //if( d->theme ) d->theme->update();
        //applet()->update();

        if( applet()->drawStandardBackground() && d->theme ) {
            //QRectF geometry = applet()->geometry();
            //geometry.setSize( );
            applet()->resize( d->theme->boundingRect().size() );
        }
    }
#endif

}

void SkAppletScript::showConfigurationInterface()
{
    //TODO
}

void SkAppletScript::karambaStarted(QGraphicsItemGroup* group)
{
    if( d->theme && d->theme == group ) {
        kDebug()<<">>>>>>>>>>>> SkAppletScript::karambaStarted theme-name="<<d->theme->theme().name();

        if( applet()->isContainment() ) {
            Plasma::Containment *c = dynamic_cast<Plasma::Containment *>(applet());
            Q_ASSERT(c);
            foreach(Plasma::Applet* a, c->applets()) {
                a->raise();
            }
        }

        applet()->resize(d->theme->boundingRect().size());
        applet()->updateConstraints(Plasma::SizeConstraint);
    }
}

void SkAppletScript::karambaClosed(QGraphicsItemGroup* group)
{
    if( d->theme && d->theme == group ) {
        kDebug()<<">>>>>>>>>>>> SkAppletScript::karambaClosed theme-name="<<d->theme->theme().name();
        //d->themeFile = QString();
        d->theme = 0;
        Q_ASSERT( applet() );
        Q_ASSERT( applet()->scene() );
        //applet()->scene()->removeItem(applet());
        applet()->destroy();
        deleteLater();
    }
}

bool SkAppletScript::eventFilter(QObject* watched, QEvent* event)
{
    switch( event->type() ) {
        case QEvent::ContextMenu: {
            kDebug() << "eventFilter type=ContextMenu watched=" << (watched ? QString("%1 [%2]").arg(watched->objectName()).arg(watched->metaObject()->className()) : "NULL") ;
            //static_cast<QContextMenuEvent*>(event);
        } break;
        case QEvent::GraphicsSceneContextMenu:
            kDebug() << "eventFilter type=GraphicsSceneContextMenu watched=" << (watched ? QString("%1 [%2]").arg(watched->objectName()).arg(watched->metaObject()->className()) : "NULL") ;
            //return true;
            break;
        case QEvent::KeyPress:
            kDebug() << "eventFilter type=KeyPress watched=" << (watched ? QString("%1 [%2]").arg(watched->objectName()).arg(watched->metaObject()->className()) : "NULL") ;
            //QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            //kDebug() << "Ate key press" << keyEvent->key();
            break;
        default:
            break;
    }
    return Plasma::AppletScript::eventFilter(watched, event);
}

#include "skappletscript.moc"
