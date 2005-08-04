/*
 * Copyright (C) 2005 Petri Damstén <petri.damsten@iki.fi>
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
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 ****************************************************************************/
#include "themewidget.h"
#include "themelocale.h"
#include <kpushbutton.h>
#include <klocale.h>
#include <qlabel.h>

ThemeWidget::ThemeWidget(QWidget *parent, const char *name)
  : ThemeWidgetLayout(parent, name), m_themeFile(0)
{
  running->setText("");
}

ThemeWidget::ThemeWidget(ThemeFile* tf)
  : m_themeFile(tf)
{
  QPixmap pixmap = m_themeFile->icon();
  if(!pixmap.isNull())
    icon->setPixmap(pixmap);
  QString version;
  if(!m_themeFile->version().isEmpty())
    version = " - " + m_themeFile->version();
  themeName->setText(
      m_themeFile->locale()->translate(m_themeFile->name().ascii()) + version);
  description->setText(
      m_themeFile->locale()->translate(m_themeFile->description().ascii()));
  running->setText("");
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
  updateRunning();
  return i;
}

void ThemeWidget::removeInstance(int instance)
{
  m_instancePool.remove(instance);
  updateRunning();
}

void ThemeWidget::updateRunning()
{
  int i = instances();
  if(i > 0)
    running->setText(i18n("<p align=\"center\">%1 running</p>").arg(i));
  else
    running->setText("");
}

#include "themewidget.moc"
