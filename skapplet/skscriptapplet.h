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

#ifndef SKSCRIPTAPPLET_H
#define SKSCRIPTAPPLET_H

#include <plasma/scripting/appletscript.h>
#include <plasma/dataengine.h>
#include <plasma/package.h>
#include <plasma/applet.h>

class SkScriptApplet : public Plasma::AppletScript
{
    Q_OBJECT
public:
    SkScriptApplet(QObject *parent, const QVariantList &args);
    virtual ~SkScriptApplet();

    /**
     * Called when it is safe to initialize the internal state.
     */
    virtual bool init();

    /**
     * Called to discover the content size hint for the item.
     * The default implementation simply returns the Applet's contentSizeHint
     */
    virtual QSizeF contentSizeHint() const;

    /**
     * Called when the applet should be painted.
     *
     * @param painter the QPainter to use
     * @param option the style option containing such flags as selection, level of detail, etc
     * @param contentsRect the rect to paint within; automatically adjusted for
     *                     the background, if any
     **/
    virtual void paintInterface(QPainter *painter, const QStyleOptionGraphicsItem *option, const QRect &contentsRect);

/*
public slots:
    void dataUpdated( const QString &name, const Plasma::DataEngine::Data &data );
    void showConfigurationInterface();
    void configAccepted();
*/

private Q_SLOTS:
    void loadKaramba();
    void karambaStarted(QGraphicsItemGroup* group);
    void karambaClosed(QGraphicsItemGroup* group);

private:
    class Private;
    Private *const d;
};

#endif
