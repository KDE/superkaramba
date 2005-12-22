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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 ****************************************************************************/
#include "themewidget.h"
#include "themelocale.h"
#include <kpushbutton.h>
#include <kdebug.h>
#include <klocale.h>
#include <qlabel.h>
#include <qlayout.h>

ThemeWidget::ThemeWidget(QWidget *parent, const char *name)
  : ThemeWidgetLayout(parent, name), m_themeFile(0)
{
  running->setText("");
  setDescriptionMaxHeight();
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
  setDescriptionMaxHeight();
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

void ThemeWidget::setDescriptionText(QString text)
{
  description->setText(text);
}

void ThemeWidget::setHeaderText(QString text)
{
  themeName->setText(text);
}

void ThemeWidget::showButton(bool show)
{
  if(show)
    buttonGo->show();
  else
    buttonGo->hide();
  setDescriptionMaxHeight();
}

void ThemeWidget::setDescriptionMaxHeight()
{
  if(layoutText->geometry().height() <= 0)
    return;
  int height = layoutText->geometry().height() - themeName->height() -
               layoutText->spacing();
  if(buttonGo->isVisible())
    height -= layoutButton->geometry().height() + layoutText->spacing();
  description->setMaximumHeight(height);
}

#include "themewidget.moc"
