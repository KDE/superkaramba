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
#include "kwidgetlistbox.h"
#include <kdebug.h>
#include <kglobalsettings.h>
#include <QShowEvent>
#include <QScrollBar>

KWidgetListbox::KWidgetListbox(QWidget *parent, const char *name)
        : QTableWidget(parent)
{
    setRowCount(0);
    setColumnCount(1);

    horizontalHeader()->hide();
    verticalHeader()->hide();

    setSelectionMode(QAbstractItemView::SingleSelection);

    connect(this, SIGNAL(cellClicked(int, int)),
            this, SLOT(selectionChanged(int, int)));

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    horizontalHeader()->setStretchLastSection(true);
    setAlternatingRowColors(true);
}

KWidgetListbox::~KWidgetListbox()
{
    clear();
}

void KWidgetListbox::clear()
{
    QTableWidget::clear();
}

int KWidgetListbox::insertItem(QWidget* item, int index)
{
    int row = 0;
    if (index == -1) {
        row = rowCount();
        setRowCount(row + 1);
    } else
        return -1;

    setRowHeight(row, item->height());
    setCellWidget(row, 0, item);

    return row;
}

void KWidgetListbox::setSelected(QWidget* item)
{
    setSelected(index(item));
}

void KWidgetListbox::selectionChanged(int row, int column)
{
    setCurrentCell(row, column);
    emit selected(row);
}

void KWidgetListbox::removeItem(QWidget* item)
{
    removeItem(index(item));
}

void KWidgetListbox::removeItem(int index)
{
    removeRow(index);
}

void KWidgetListbox::setSelected(int index)
{
    setCurrentCell(index, 0);
}

int KWidgetListbox::selected() const
{
    return currentRow();
}

QWidget* KWidgetListbox::selectedItem() const
{
    return item(selected());
}

QWidget* KWidgetListbox::item(int index) const
{
    return cellWidget(index, 0);
}

int KWidgetListbox::index(QWidget* itm) const
{
    for (int i = 0; i < rowCount(); ++i)
        if (item(i) == itm)
            return i;
    return -1;
}

void KWidgetListbox::showItems(show_callback func, void* data)
{
    for (int i = 0; i < rowCount(); ++i) {
        if (func == 0)
            showRow(i);
        else {
            if (func(i, item(i), data))
                showRow(i);
            else
                hideRow(i);
        }
    }
}

void KWidgetListbox::showEvent(QShowEvent*)
{
    repaint();
}

#include "kwidgetlistbox.moc"
