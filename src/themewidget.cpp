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
#include "themewidget.h"
#include <kpushbutton.h>
#include <qlabel.h>

ThemeWidget::ThemeWidget(QWidget *parent, const char *name)
  : ThemeWidgetLayout(parent, name), m_themeFile(0)
{
}

ThemeWidget::ThemeWidget(ThemeFile* tf)
  : m_themeFile(tf)
{
  QPixmap pixmap = m_themeFile->icon();
  if(!pixmap.isNull())
    icon->setPixmap(pixmap);
  themeName->setText(m_themeFile->name());
  description->setText(m_themeFile->description());
  buttonGo->hide();
}

ThemeWidget::~ThemeWidget()
{
  delete m_themeFile;
}

int ThemeWidget::addInstance()
{
  int i = 1;
  while(m_instancePool.find(i) != m_instancePool.end())
    ++i;
  m_instancePool.append(i);
  return i;
}

void ThemeWidget::removeInstance(int instance)
{
  m_instancePool.remove(instance);
}

#include "themewidget.moc"
