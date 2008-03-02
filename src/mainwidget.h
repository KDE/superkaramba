/*
 * Copyright (c) 2007 Alexander Wiedenbruch <mail@wiedenbruch.de>
 *
 * This file is part of SuperKaramba.
 *
 *  SuperKaramba is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  SuperKaramba is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SuperKaramba; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 ****************************************************************************/

#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPaintEvent>

class MainWidget : public QGraphicsView
{
public:
    explicit MainWidget(QGraphicsScene *scene, QWidget *parent = 0);

    ~MainWidget();

protected:
    void paintEvent(QPaintEvent *e);
    void keyPressEvent ( QKeyEvent * ) {};

private:
    bool m_hasCompManager;
};

#endif
