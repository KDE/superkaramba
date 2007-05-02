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

#include <QApplication>

KWidgetListbox::KWidgetListbox(QWidget *parent)
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

void KWidgetListbox::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        m_dragStartPosition = event->pos();
    }

    QTableWidget::mousePressEvent(event);
}

void KWidgetListbox::mouseMoveEvent(QMouseEvent *event)
{
    if(!(event->buttons() & Qt::LeftButton)) {
        return;
    }

    if ((event->pos() - m_dragStartPosition).manhattanLength() < QApplication::startDragDistance()) {
        return;
    }

    if (selected() <= 1) {
        return;
    }

    ThemeWidget *theme = qobject_cast<ThemeWidget*>(selectedItem());
    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

//    mimeData->setData("superkaramba/theme", theme->path().toAscii());
    drag->setMimeData(mimeData);
    drag->setPixmap(theme->icon());

    drag->start(Qt::IgnoreAction);

    emit itemDropped(QCursor::pos(), theme);
}

#include "kwidgetlistbox.moc"
