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
//Added by qt3to4:
#include <QShowEvent>
#include <QScrollBar>

KWidgetListbox::KWidgetListbox(QWidget *parent, const char *name)
  : QTableWidget(parent)
{
  setRowCount(0);
  setColumnCount(1);

  horizontalHeader()->hide();
  verticalHeader()->hide();
  
  setSelectionMode(QAbstractItemView::NoSelection);
  //setFocusStyle(QTableView::FollowStyle);
 
  connect(this, SIGNAL(cellClicked(int, int)),
          this, SLOT(selectionChanged(int, int)));
   
  /*
  setHScrollBarMode(Q3ScrollView::AlwaysOff);
  setVScrollBarMode(Q3ScrollView::Auto);
  */
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
  int row;

  if(index == -1)
  {
    row = rowCount();
    setRowCount(row + 1);
  }
  else
    return -1;

  setRowHeight(row, item->height());
 
  setCellWidget(row, 0, item);
  setItemColors(row, even(row));

  return row;
}

void KWidgetListbox::setSelected(QWidget* item)
{
  setSelected(index(item));
}

void KWidgetListbox::selectionChanged(int row, int column)
{
  setCurrentCell(row, column);
  updateColors();
  emit selected(row);
}

void KWidgetListbox::removeItem(QWidget* item)
{
  removeItem(index(item));
}

void KWidgetListbox::removeItem(int index)
{
  removeRow(index);
  updateColors();
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
  for(int i = 0; i < rowCount(); ++i)
    if(item(i) == itm)
      return i;
  return -1;
}

bool KWidgetListbox::even(int index)
{
  int v = 0;
  for(int i = 0; i < rowCount(); ++i)
  {
    if(index == i)
      break;
    if(!isRowHidden(i))
      ++v;
  }
  return (v%2 == 0);
}

void KWidgetListbox::updateColors()
{
  int v = 0;
  for(int i = 0; i < rowCount(); ++i)
  {
    if(!isRowHidden(i))
    {
      setItemColors(i, (v%2 == 0));
      ++v;
    }
  }
}

void KWidgetListbox::setItemColors(int index, bool even)
{
  QWidget* itm = item(index);

  if(index == selected())
  {
    QPalette bgPalette;
    bgPalette.setColor(itm->backgroundRole(), KGlobalSettings::highlightColor());
    itm->setPalette(bgPalette);

    QPalette fgPalette;
    fgPalette.setColor(itm->foregroundRole(), KGlobalSettings::highlightedTextColor());
    itm->setPalette(fgPalette);
  }
  else if(even)
  {
    QPalette bgPalette;
    bgPalette.setColor(itm->backgroundRole(), KGlobalSettings::baseColor());
    itm->setPalette(bgPalette);

    QPalette fgPalette;
    fgPalette.setColor(itm->foregroundRole(), KGlobalSettings::textColor());
    itm->setPalette(fgPalette);
  }
  else
  {
    QPalette bgPalette;
    bgPalette.setColor(itm->backgroundRole(), KGlobalSettings::alternateBackgroundColor());
    itm->setPalette(bgPalette);

    QPalette fgPalette;
    fgPalette.setColor(itm->foregroundRole(), KGlobalSettings::textColor());
    itm->setPalette(fgPalette);
  }
}

void KWidgetListbox::showItems(show_callback func, void* data)
{
  for(int i = 0; i < rowCount(); ++i)
  {
    if(func == 0)
      showRow(i);
    else
    {
      if(func(i, item(i), data))
        showRow(i);
      else
        hideRow(i);
    }
  }
  updateColors();
}

void KWidgetListbox::showEvent(QShowEvent*)
{
  //kDebug() << k_funcinfo << endl;
  repaint();
}

void KWidgetListbox::paintCell(QPainter*, int, int, const QRect&,
                               bool, const QPalette&)
{
  //kDebug() << k_funcinfo << endl;
}

void KWidgetListbox::resizeEvent(QResizeEvent *e)
{
  QTableWidget::resizeEvent(e);

  #ifdef __GNUC__
    #warning No better way?
  #endif
  unsigned int wscroll = 0;

  QScrollBar *bar = verticalScrollBar();
  if(bar->isVisible())
    wscroll = 5;
  
  setColumnWidth(0, width() - 5 - wscroll);

  for(unsigned int i = 0; i < rowCount(); i++)
  {
    setRowHeight(i, 150);
  }
}

#include "kwidgetlistbox.moc"
