/*
 * Copyright (C) 2005 Petri Damst� <petri.damsten@iki.fi>
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
#ifndef KWIDGETLISTBOX_H
#define KWIDGETLISTBOX_H

#include "themewidget.h"

#include <QTableWidget>
#include <QShowEvent>
#include <QHeaderView>
#include <QPoint>

/**
@author See README for the list of authors
*/

typedef bool(*show_callback)(int index, QWidget* widget, void* data);

class KWidgetListbox : public QTableWidget
{
    Q_OBJECT

public:
    explicit KWidgetListbox(QWidget *parent = 0);
    ~KWidgetListbox();

    int insertItem(QWidget* item, int index = -1);
    void setSelected(QWidget* item);
    void setSelected(int index);
    void removeItem(QWidget* item);
    void removeItem(int index);
    void clear();
    int selected() const;
    QWidget* selectedItem() const;
    QWidget* item(int index) const;
    int index(QWidget* itm) const;
    uint count() const
    {
        return rowCount();
    }

    void showItems(show_callback func = 0, void* data = 0);

protected:
    virtual void showEvent(QShowEvent* e);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);

protected slots:
    void selectionChanged(int row, int col);

signals:
    void selected(int index);
    void itemDropped(QPoint, ThemeWidget*);

private:
    QPoint m_dragStartPosition;
};

#endif
