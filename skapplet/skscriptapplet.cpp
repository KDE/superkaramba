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

using namespace Plasma;

K_EXPORT_PLASMA_APPLETSCRIPTENGINE(superkaramba, SkScriptApplet)

SkScriptApplet::SkScriptApplet(QObject *parent, const QVariantList &args)
    : Plasma::AppletScript(parent)
{
    kDebug();
}

SkScriptApplet::~SkScriptApplet()
{
    kDebug();
}

bool SkScriptApplet::init()
{
    kDebug();
    return true;
}

void SkScriptApplet::paintInterface(QPainter *p, const QStyleOptionGraphicsItem *option, const QRect &contentsRect)
{
    kDebug();
}

#include "skscriptapplet.moc"
