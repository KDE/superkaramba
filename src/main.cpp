/*
 * Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>
 * Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>
 * Copyright (c) 2005 Ryan Nickell <p0z3r@earthlink.net>
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

#include <karambaapp.h>
#include <qobject.h>

#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kconfig.h>
#include <kmainwindow.h>
#include <qfileinfo.h>
#include <qstringlist.h>
#include <kconfig.h>
#include <kstandarddirs.h>
#include <kdeversion.h>

#include "karambawidget.h"
#include "karambasessionmanaged.h"
#include "karambainterface.h"
#include "karamba_python.h"

static const char *description =
    I18N_NOOP("A KDE Eye-candy Application");

static const char *version = "0.37-RC2";

static KCmdLineOptions options[] =
    {
        // { "+[URL]", I18N_NOOP( "Document to open" ), 0 },
        // { "!nosystray", I18N_NOOP("Disable systray icon"), 0 },
        { "+file", I18N_NOOP("A required argument 'file'"), 0 },
        { 0, 0, 0 }
    };

// This is for redirecting all qWarning, qDebug,... messages to file.
// Usefull when testing session management issues etc.
// #define KARAMBA_LOG 1

#ifdef KARAMBA_LOG

void karambaMessageOutput(QtMsgType type, const char *msg)
{
    FILE* fp = fopen("/tmp/karamba.log", "a");
    if(fp)
    {
        pid_t pid = getpid();

        switch ( type )
        {
        case QtDebugMsg:
            fprintf( fp, "Debug (%d): %s\n", pid, msg );
            break;
        case QtWarningMsg:
            if (strncmp(msg, "X Error", 7) != 0)
                fprintf( fp, "Warning (%d): %s\n", pid, msg );
            break;
        case QtFatalMsg:
            fprintf( fp, "Fatal (%d): %s\n", pid, msg );
            abort();                    // deliberately core dump
        }
        fclose(fp);
    }
}

#endif

int main(int argc, char **argv)
{
#ifdef KARAMBA_LOG
    qInstallMsgHandler(karambaMessageOutput);
#endif

    KAboutData about("superkaramba", I18N_NOOP("SuperKaramba"),
                     version, description,
                     KAboutData::License_GPL,
                     "(c) 2003-2005 The SuperKaramba developers");
    about.addAuthor("Adam Geitgey", 0, "adam@rootnode.org");
    about.addAuthor("Hans Karlsson", 0, "karlsson.h@home.se");
    about.addAuthor("Ryan Nickell", 0, "p0z3r@earthlink.net");
    about.addAuthor("Petri Damstén", 0, "petri.damsten@iki.fi");

    KCmdLineArgs::init(argc, argv, &about);

    // Create ~/.superkaramba if necessary
    QDir configDir(QDir::home().absolutePath() + "/.superkaramba");
    if (!configDir.exists())
    {
        qWarning("~/.superkaramba doesn't exist");
        if(!configDir.mkdir(QDir::home().absolutePath() + "/.superkaramba"))
        {
            qWarning("Couldn't create Directory ~/.superkaramba");
        }
        else
        {
            qWarning("created ~/.superkaramba");
        }
    }

    KarambaApplication app;
    return app.exec();
}
