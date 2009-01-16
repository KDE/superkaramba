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

#include "karambaapp.h"
#include "karambasessionmanaged.h"
#include "python/karamba.h"

#include "config-superkaramba.h"

#include <stdlib.h>

#include <KLocale>
#include <KConfig>
#include <KDebug>
#include <KStandardDirs>
#include <KCmdLineArgs>
#include <KAboutData>
#include <KWindowSystem>

#include <X11/extensions/Xrender.h>

static const char *description =
    I18N_NOOP("A KDE Eye-candy Application");

static const char *version = "0.53";

int main(int argc, char **argv)
{
    Display *dpy = XOpenDisplay(0); // open default display
    if (!dpy) {
        kWarning() << "Cannot connect to the X server";
        exit(1);
    }

    Colormap colormap = 0;
    Visual *visual = 0;

    if (KWindowSystem::compositingActive()) {
        int screen = DefaultScreen(dpy);
        int eventBase, errorBase;

        if (XRenderQueryExtension(dpy, &eventBase, &errorBase)) {
            int nvi;
            XVisualInfo templ;
            templ.screen  = screen;
            templ.depth   = 32;
            templ.c_class = TrueColor;
            XVisualInfo *xvi = XGetVisualInfo(dpy, VisualScreenMask |
                                              VisualDepthMask |
                                              VisualClassMask,
                                              &templ, &nvi);
            for (int i = 0; i < nvi; ++i) {
                XRenderPictFormat *format = XRenderFindVisualFormat(dpy,
                                            xvi[i].visual);
                if (format->type == PictTypeDirect && format->direct.alphaMask) {
                    visual = xvi[i].visual;
                    colormap = XCreateColormap(dpy, RootWindow(dpy, screen),
                                               visual, AllocNone);
                    break;
                }
            }
        }
    }

    KAboutData about("superkaramba", 0, ki18n("SuperKaramba"),
                     version, ki18n(description),
                     KAboutData::License_GPL,
                     ki18n("(c) 2003-2007 The SuperKaramba developers"), KLocalizedString(),
                     "http://utils.kde.org/projects/superkaramba");
    about.addAuthor(ki18n("Adam Geitgey"), KLocalizedString(), "adam@rootnode.org");
    about.addAuthor(ki18n("Hans Karlsson"), KLocalizedString(), "karlsson.h@home.se");
    about.addAuthor(ki18n("Ryan Nickell"), KLocalizedString(), "p0z3r@earthlink.net");
    about.addAuthor(ki18n("Petri Damstén"), KLocalizedString(), "petri.damsten@iki.fi");
    about.addAuthor(ki18n("Alexander Wiedenbruch"), KLocalizedString(), "mail@wiedenbruch.de");
    about.addAuthor(ki18n("Luke Kenneth Casson Leighton"), KLocalizedString(), "lkcl@lkcl.net");
    about.addCredit(ki18n("Sebastian Sauer"), ki18n("Work on Kross, tutorials and examples"), "mail@dipe.org");
    KCmdLineArgs::init(argc, argv, &about);

    KCmdLineOptions options;
    // { "+[URL]", I18N_NOOP( "Document to open" ), 0 },
// { "!nosystray", I18N_NOOP("Disable systray icon"), 0 },
#ifdef PYTHON_INCLUDE_PATH
   options.add("usefallback", ki18n("Use the original python bindings as scripting backend. Off by default."));
#endif
    options.add("+file", ki18n("A required argument 'file'"));
    KCmdLineArgs::addCmdLineOptions(options);
    KarambaApplication::addCmdLineOptions();
    KarambaSessionManaged ksm;

    if (!KarambaApplication::start()) {
        fprintf(stderr, "SuperKaramba is already running!\n");
        exit(0);
    }

#ifdef PYTHON_INCLUDE_PATH
    bool noUseKross = false;
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    if (args->isSet("usefallback")) {
        noUseKross = true;
        kDebug() << "Using fallback python scripting backend!" ;
    }
#endif

    KarambaApplication app(dpy, Qt::HANDLE(visual), Qt::HANDLE(colormap));

    app.setupSysTray(&about);
    int ret = 0;

#ifdef PYTHON_INCLUDE_PATH
    if (noUseKross) {
        KarambaPython::initPython();
    }
#endif

    ret = app.exec();

#ifdef PYTHON_INCLUDE_PATH
    if (noUseKross) {
        KarambaPython::shutdownPython();
    }
#endif

    return ret;
}
