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
#include <KDialog>
#include <KToggleAction>
#include <KColorScheme>
#include <KGlobalSettings>

K_EXPORT_PLASMA_APPLETSCRIPTENGINE(superkaramba, SkAppletScript)

class SkAppletScript::Private
{
    public:
        QString themeFile;
        QPointer<Karamba> theme;
        QList<QAction*> actions;
        QStringList errors;

        enum { Always = 0, Never, Immutable, NotImmutable };
        KDialog* dialog;
        QComboBox *backgroundComboBox, *readonlyComboBox;

        int backgroundType, readonlyType;

        Private() : theme(0), dialog(0) {}
        ~Private() { delete dialog; delete theme; }
};

SkAppletScript::SkAppletScript(QObject *parent, const QVariantList &args)
    : Plasma::AppletScript(parent)
    , d(new Private)
{
    Q_UNUSED(args);
    d->backgroundType = Private::Immutable;
    d->readonlyType = Private::Never;
}

SkAppletScript::~SkAppletScript()
{
    kDebug();
    delete d;
}

bool SkAppletScript::init()
{
    Q_ASSERT( applet() );
    applet()->setHasConfigurationInterface(true);
    applet()->setDrawStandardBackground(false);
    applet()->setAspectRatioMode(Qt::IgnoreAspectRatio);
    //applet()->setRemainSquare(true);
    applet()->setContentSize(100, 60);

    KConfigGroup cg = applet()->config();
    d->backgroundType = cg.readEntry("background", d->backgroundType);
    d->readonlyType = cg.readEntry("readonly", d->readonlyType);

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

void SkAppletScript::loadKaramba()
{
    d->errors.clear();

    Q_ASSERT( applet() );
    Q_ASSERT( applet()->scene() );
    Q_ASSERT( applet()->scene()->views().count() > 0 );
    QGraphicsScene *scene = applet()->scene();
    QGraphicsView* view = scene->views()[0];

    Q_ASSERT( KarambaManager::self() );
    connect(KarambaManager::self(), SIGNAL(karambaStarted(QGraphicsItemGroup*)), this, SLOT(karambaStarted(QGraphicsItemGroup*)));
    connect(KarambaManager::self(), SIGNAL(karambaClosed(QGraphicsItemGroup*)), this, SLOT(karambaClosed(QGraphicsItemGroup*)));

    Q_ASSERT( ! d->theme );
    d->theme = new Karamba(d->themeFile, view);
    d->theme->setParentItem(applet());
    const QRectF geometry = applet()->geometry();
    d->theme->moveToPos(QPoint(int(geometry.x()), int(geometry.y())));

    //view->viewport()->installEventFilter(this);

    if( KToggleAction* lockedAction = d->theme->findChild<KToggleAction*>("lockedAction") ) {
        // disable locked action since Plasma will handle it for us.
        if( ! lockedAction->isChecked() )
            lockedAction->setChecked(true);
        lockedAction->setVisible(false);
    }

    if( QAction* configAction = d->theme->findChild<QAction*>("configureTheme") ) {
        d->actions << configAction;
    }

    new SkAppletAdaptor(d->theme, applet());

    connect(d->theme, SIGNAL(error(QString)), this, SLOT(scriptError(QString)));
}

void SkAppletScript::scriptError(const QString& err)
{
    d->errors << err;
    //applet()->setFailedToLaunch(true, e);
}

QSizeF SkAppletScript::contentSizeHint() const
{
    return d->theme ? d->theme->boundingRect().size() : Plasma::AppletScript::contentSizeHint();
}

void SkAppletScript::paintInterface(QPainter *painter, const QStyleOptionGraphicsItem *, const QRect &contentsRect)
{
    //if( d->appletadaptor ) d->appletadaptor->paintInterface(painter, option, contentsRect);

    if( d->errors.count() > 0 ) {
        QColor fontcolor = KColorScheme(QPalette::Active, KColorScheme::View, Plasma::Theme::self()->colors()).foreground().color();
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
}

QList<QAction*> SkAppletScript::contextActions()
{
    return d->actions;
}

void SkAppletScript::constraintsUpdated(Plasma::Constraints constraints)
{
    if( constraints & Plasma::SizeConstraint ) {
        if( d->theme ) {
            const QRectF r = d->theme->boundingRect();
            const QSizeF s = applet()->contentSize();
            const qreal x = s.width() / r.width();
            const qreal y = s.height() / r.height();

            QTransform t = d->theme->transform();
            t.reset();
            d->theme->setTransform(t.scale(x, y));
        }
    }
    if( constraints & Plasma::ImmutableConstraint ) {
        Q_ASSERT( applet() );
        applet()->setDrawStandardBackground(
            ( d->backgroundType == Private::Always ) ||
            ( d->backgroundType == Private::Immutable && ! applet()->isImmutable() ) ||
            ( d->backgroundType == Private::NotImmutable && applet()->isImmutable() )
        );
        applet()->setHandlesChildEvents(
            ( d->readonlyType == Private::Always ) ||
            ( d->readonlyType == Private::Immutable && applet()->isImmutable() ) ||
            ( d->readonlyType == Private::NotImmutable && ! applet()->isImmutable() )
        );
        //if( d->theme ) d->theme->update();
        //applet()->update();
        if( applet()->drawStandardBackground() && d->theme ) {
            //QRectF geometry = applet()->geometry();
            //geometry.setSize( );
            applet()->setContentSize( d->theme->boundingRect().size() );
        }
    }
}

void setCurrentItem(QComboBox* combo, int currentIndex)
{
    for(int i = combo->count() - 1; i >= 0; --i) {
        if( combo->itemData(i).toInt() == currentIndex ) {
            combo->setCurrentIndex(i);
            return;
        }
    }
    if( combo->count() > 0 ) {
        combo->setCurrentIndex(0);
    }
}

void SkAppletScript::showConfigurationInterface()
{
    if (! d->dialog) {
        d->dialog = new KDialog();
        d->dialog->setCaption( i18nc("@title:window","Configure SuperKaramba") );
        d->dialog->setButtons( KDialog::Ok | KDialog::Cancel | KDialog::Apply );
        connect(d->dialog, SIGNAL(applyClicked()), this, SLOT(configAccepted()));
        connect(d->dialog, SIGNAL(okClicked()), this, SLOT(configAccepted()));

        QWidget *p = d->dialog->mainWidget();
        QGridLayout *l = new QGridLayout(p);
        p->setLayout(l);

        QLabel *backgroundLabel = new QLabel(i18n("Show Background:"), p);
        l->addWidget(backgroundLabel, 0, 0);
        d->backgroundComboBox = new QComboBox(p);
        backgroundLabel->setBuddy(d->backgroundComboBox);
        d->backgroundComboBox->addItem(i18n("Always"), int(Private::Always));
        d->backgroundComboBox->addItem(i18n("Never"), int(Private::Never));
        d->backgroundComboBox->addItem(i18n("If widgets are locked"), int(Private::Immutable));
        d->backgroundComboBox->addItem(i18n("If widgets are unlocked"), int(Private::NotImmutable));
        l->addWidget(d->backgroundComboBox, 0, 1);

        QLabel *readonlyLabel = new QLabel(i18n("Read Only:"), p);
        l->addWidget(readonlyLabel, 1, 0);
        d->readonlyComboBox = new QComboBox(p);
        readonlyLabel->setBuddy(d->readonlyComboBox);
        d->readonlyComboBox->addItem(i18n("Always"), Private::Always);
        d->readonlyComboBox->addItem(i18n("Never"), Private::Never);
        d->readonlyComboBox->addItem(i18n("If widgets are locked"), Private::Immutable);
        d->readonlyComboBox->addItem(i18n("If widgets are unlocked"), Private::NotImmutable);
        l->addWidget(d->readonlyComboBox, 1, 1);

        l->setColumnStretch(1,1);
    }

    setCurrentItem(d->backgroundComboBox, d->backgroundType);
    setCurrentItem(d->readonlyComboBox, d->readonlyType);
    d->dialog->show();
}

void SkAppletScript::configAccepted()
{
    kDebug() << ">>>>>>>>>>>> SkAppletScript::configAccepted" ;
    d->backgroundType = d->backgroundComboBox->itemData(d->backgroundComboBox->currentIndex()).toInt();
    d->readonlyType = d->readonlyComboBox->itemData(d->readonlyComboBox->currentIndex()).toInt();

    KConfigGroup cg = applet()->config();
    cg.writeEntry("background", d->backgroundType);
    cg.writeEntry("readonly", d->readonlyType);

    applet()->updateConstraints(Plasma::AllConstraints);
    //cg.config()->sync();
}

void SkAppletScript::karambaStarted(QGraphicsItemGroup* group)
{
    if( d->theme && d->theme == group ) {
        kDebug()<<">>>>>>>>>>>> SkAppletScript::karambaStarted theme-name="<<d->theme->theme().name();
        applet()->setContentSize(d->theme->boundingRect().size());
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
