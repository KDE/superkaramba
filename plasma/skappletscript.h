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

#ifndef SKAPPLETSCRIPT_H
#define SKAPPLETSCRIPT_H

#include <plasma/scripting/appletscript.h>
#include <plasma/dataengine.h>
#include <plasma/package.h>
#include <plasma/applet.h>
#include <plasma/containment.h>
#include <plasma/theme.h>

/**
 * The SkAppletScript class implements a \a Plasma::AppletScript to
 * implement a \a Plasma::ScriptEngine for SuperKaramba.
 */
class SkAppletScript : public Plasma::AppletScript
{
    Q_OBJECT
public:

    /**
     * Constructor.
     *
     * \param parent The parent QObject this QObject will be
     * child of.
     * \param args The optional list of arguments.
     */
    SkAppletScript(QObject *parent, const QVariantList &args);

    /**
     * Destructor.
     */
    virtual ~SkAppletScript();

    /**
     * Called when it is safe to initialize the internal state.
     */
    virtual bool init();

#if 0
    /**
     * Called to discover the content size hint for the item.
     * \return returns the contentSizeHint of the SuperKaramba Theme.
     */
    virtual QSizeF contentSizeHint() const;
#endif

    /**
     * Called when any of the geometry constraints have been updated.
     *
     * This is always called prior to painting and should be used as an
     * opportunity to layout the widget, calculate sizings, etc.
     *
     * Do not call update() from this method; an update() will be triggered
     * at the appropriate time for the applet.
     *
     * @param constraints the type of constraints that were updated
     */
    virtual void constraintsEvent(Plasma::Constraints constraints);

    /**
     * Called when the applet should be painted.
     *
     * @param painter the QPainter to use
     * @param option the style option containing such flags as selection, level of detail, etc
     * @param contentsRect the rect to paint within; automatically adjusted for
     *                     the background, if any
     */
    virtual void paintInterface(QPainter *painter, const QStyleOptionGraphicsItem *option, const QRect &contentsRect);

    /**
     * Returns a list of context-related QAction instances.
     *
     * @return A list of actions. The default implementation returns an
     *         empty list.
     */
    virtual QList<QAction*> contextualActions();

public Q_SLOTS:

    //void dataUpdated( const QString &name, const Plasma::DataEngine::Data &data );

    /**
     * Shows the configuration dialog for general SkAppletScript settings.
     */
    virtual void showConfigurationInterface();

private Q_SLOTS:
    void loadKaramba();
    void karambaStarted(QGraphicsItemGroup* group);
    void karambaClosed(QGraphicsItemGroup* group);
    void positionChanged();
    void sizeChanged();
    void scriptError(const QString& err);

private:
    class Private;
    Private *const d;
    bool eventFilter(QObject* watched, QEvent* event);
};

#endif
