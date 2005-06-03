/*
 * Copyright (C) 2005 Petri Damstén <petri.damsten@iki.fi>
 *
 * This file is part of Superkaramba.
 *
 *  Superkaramba is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Superkaramba is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Superkaramba; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ****************************************************************************/
#include "kwidgetlistbox.h"
#include <kdebug.h>
#include <kglobalsettings.h>

KWidgetListbox::KWidgetListbox(QWidget *parent, const char *name)
 : QTable(parent, name)
{
  setNumRows(0);
  setNumCols(1);
  setColumnStretchable(0, true);
  setLeftMargin(0);
  setTopMargin(0);
  horizontalHeader()->hide();
  verticalHeader()->hide();
  setSelectionMode(QTable::NoSelection);
  setFocusStyle(QTable::FollowStyle);
  connect(this, SIGNAL(currentChanged(int, int)),
          this, SLOT(selectionChanged(int, int)));
}

KWidgetListbox::~KWidgetListbox()
{
  clear();
}

void KWidgetListbox::clear()
{
  for(int i = 0; i < numRows(); ++i)
    clearCellWidget(i, 0);
  setNumRows(0);
}

void KWidgetListbox::insertItem(QWidget* item, int index)
{
  int row;

  if(index == -1)
  {
    row = numRows();
    setNumRows(row + 1);
  }
  else
    return;

  setRowHeight(row, item->height());
  setCellWidget(row, 0, item);
  setItemColors(row);
}

void KWidgetListbox::setSelected(QWidget* item)
{
  setSelected(index(item));
}

void KWidgetListbox::selectionChanged(int row, int col)
{
  ensureCellVisible(row, col);
  updateColors();
  emit selected(row);
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
  for(int i = 0; i < numRows(); ++i)
    if(item(i) == itm)
      return i;
  return -1;
}

void KWidgetListbox::updateColors()
{
  for(int i = 0; i < numRows(); ++i)
    setItemColors(i);
}

void KWidgetListbox::setItemColors(int index)
{
  QWidget* itm = item(index);

  if(index == selected())
  {
    itm->setPaletteBackgroundColor(KGlobalSettings::highlightColor());
    itm->setPaletteForegroundColor(KGlobalSettings::highlightedTextColor());
  }
  else if(index % 2 != 0)
  {
    itm->setPaletteBackgroundColor(KGlobalSettings::baseColor());
    itm->setPaletteForegroundColor(KGlobalSettings::textColor());
  }
  else
  {
    itm->setPaletteBackgroundColor(
        KGlobalSettings::alternateBackgroundColor());
    itm->setPaletteForegroundColor(KGlobalSettings::textColor());
  }
}

void KWidgetListbox::paintCell(QPainter*, int, int, const QRect&,
                               bool, const QColorGroup&)
{
}

#include "kwidgetlistbox.moc"
