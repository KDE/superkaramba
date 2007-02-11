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

#include <klocale.h>
#include <kconfig.h>
#include <kstandarddirs.h>
#include <kdeversion.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

#include <X11/extensions/Xrender.h>

static const char *description =
    I18N_NOOP("A KDE Eye-candy Application");

static const char *version = "0.50";

static KCmdLineOptions options[] =
{
  // { "+[URL]", I18N_NOOP( "Document to open" ), 0 },
  // { "!nosystray", I18N_NOOP("Disable systray icon"), 0 },
  { "+file", I18N_NOOP("A required argument 'file'"), 0 },
  { 0, 0, 0 }
};

int main(int argc, char **argv)
{
    // Taken from KRunner by A. Seigo
    Display *dpy = XOpenDisplay(0); // open default display
    if (!dpy)
    {
        qWarning("Cannot connect to the X server");
        exit(1);
    }

    bool argbVisual = false ;
    bool haveCompManager = !XGetSelectionOwner(dpy, XInternAtom(dpy,
                                                 "_NET_WM_CM_S0", false));
    haveCompManager = true;
    Colormap colormap = 0;
    Visual *visual = 0;

    kDebug() << "Composition Manager: " << haveCompManager << endl;

    if(haveCompManager)
    {
        int screen = DefaultScreen(dpy);
        int eventBase, errorBase;

        if(XRenderQueryExtension(dpy, &eventBase, &errorBase))
        {
            int nvi;
            XVisualInfo templ;
            templ.screen  = screen;
            templ.depth   = 32;
            templ.c_class = TrueColor;
            XVisualInfo *xvi = XGetVisualInfo(dpy, VisualScreenMask |
                                                   VisualDepthMask |
                                                   VisualClassMask,
                                              &templ, &nvi);
            for(int i = 0; i < nvi; ++i)
            {
                XRenderPictFormat *format = XRenderFindVisualFormat(dpy,
                                                                    xvi[i].visual);
                if(format->type == PictTypeDirect && format->direct.alphaMask)
                {
                    visual = xvi[i].visual;
                    colormap = XCreateColormap(dpy, RootWindow(dpy, screen),
                                               visual, AllocNone);
                    argbVisual = true;
                    break;
                }
            }
        }
    }

    KAboutData about("superkaramba", I18N_NOOP("SuperKaramba"),
                     version, description,
                     KAboutData::License_GPL,
                     "(c) 2003-2006 The SuperKaramba developers");
    about.addAuthor("Adam Geitgey", 0, "adam@rootnode.org");
    about.addAuthor("Hans Karlsson", 0, "karlsson.h@home.se");
    about.addAuthor("Ryan Nickell", 0, "p0z3r@earthlink.net");
    about.addAuthor("Petri Damstén", 0, "petri.damsten@iki.fi");
    about.addAuthor("Alexander Wiedenbruch", 0, "mail@wiedenbruch.de");
    about.addAuthor("Luke Kenneth Casson Leighton", 0, "lkcl@lkcl.net");
    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions(options);
    KarambaApplication::addCmdLineOptions();
    KarambaSessionManaged ksm;

    if(!KarambaApplication::start())
    {
      fprintf(stderr, "SuperKaramba is already running!\n");
      exit(0);
    }

    KarambaApplication app(dpy, Qt::HANDLE(visual), Qt::HANDLE(colormap));

    app.setUpSysTray(&about);

    int ret = 0;
    KarambaPython::initPython();
    ret = app.exec();
    KarambaPython::shutdownPython();

    return ret;
}
