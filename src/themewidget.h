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
#ifndef THEMEWIDGET_H
#define THEMEWIDGET_H

#include "themefile.h"
#include <themewidget_layout.h>

/**
@author See README for the list of authors
*/

class ThemeWidget : public ThemeWidgetLayout
{
    Q_OBJECT
  public:
    ThemeWidget(QWidget *parent = 0, const char *name = 0);
    ThemeWidget(ThemeFile* tf);
    ~ThemeWidget();

    ThemeFile* themeFile() const { return m_themeFile; };

    int  addInstance();
    int  instances() const { return m_instancePool.count(); };
    void removeInstance(int instance);

    void setDescriptionText(QString text);
    void setHeaderText(QString text);
    void showButton(bool show);

  protected:
    void updateRunning();
    void setDescriptionMaxHeight();

  private:
    ThemeFile* m_themeFile;
    QValueList<int> m_instancePool;
};

#endif
