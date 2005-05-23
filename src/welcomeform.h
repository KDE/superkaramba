/*
 * Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>
 * Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>
 * Copyright (c) 2005 Petri Damsten <damu@iki.fi>
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

#ifndef WELCOMEFORM_H
#define WELCOMEFORM_H

#include "welcomeform_layout.h"
#include <kio/job.h>

class WelcomeForm : public WelcomeFormLayout
{
    Q_OBJECT

  public:
    WelcomeForm(QWidget* parent = 0, const char* name = 0,
                bool modal = FALSE, WFlags fl = 0);

  public slots:
    virtual void exitButton_clicked();
    virtual void helpButton_clicked();
    virtual void downloadButton_clicked();
    virtual void openButton_clicked();
    void downloadResult(KIO::Job* job);
    void linkClicked(const QString& link);
};

#endif // WELCOMEFORM_H
