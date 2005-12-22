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

#include <kapplication.h>
#include <kfiledialog.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kconfig.h>

#include <qfileinfo.h>
#include <qstringlist.h>

#include <iostream.h>

#include "taskbartest.h"

static const char *description =
    I18N_NOOP("A KDE Eye-candy Application");

static const char *version = "0.17";

static KCmdLineOptions options[] =
    {
        //    { "+[URL]", I18N_NOOP( "Document to open" ), 0 },
        { "+file", I18N_NOOP("A required argument 'file'"), 0 },
        { 0, 0, 0 }

    };


int main(int argc, char **argv)
{
    KAboutData about("karamba", I18N_NOOP("karamba"), version, description,
                     KAboutData::License_GPL, "(C) 2003 Hans Karlsson", 0, 0, "karlsson.h@home.se");
    about.addAuthor( "Hans Karlsson", 0, "karlsson.h@home.se" );
    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions( options );

    KApplication app;
//     karamba *mainWin = 0;

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

//     //KSGRD::SensorManager *f32 = new KSGRD::SensorManager();
//     //f32->engage("localhost");
//     //      KSGRD::SensorMgr  foo ;//  p->engage( "" );


//     //KConfig *kconfig = KGlobal::config();
//     //kconfig->setGroup("karamba");
//     //kconfig->writeEntry("test", "/home/hk/foofoo");
//     //kconfig->sync();


//     bool OK = false;

//     // initialize Python
//     Py_Initialize();

//     // initialize thread support
//     PyEval_InitThreads();

//     mainThreadState = NULL;

//     // save a pointer to the main PyThreadState object
//     mainThreadState = PyThreadState_Get();

//     // release the lock
//     PyEval_ReleaseLock();
    

//     if(args->count() > 0)
//     {
//         for (int i = 0; i < (args->count()); i++)
//         {
//             if( args->arg(i) != "" )
//             {
//                 QFileInfo file( args->arg(i) );
//                 //qDebug( file.dirPath(true) );
//                 if( file.exists() && !file.isDir() )
//                 {
//                     //qDebug( "File exists" );
//                     mainWin = new karamba( ( args->arg(i) ));
//                     mainWin->show();
//                     OK = true;
//                 }
//             }
//         }

//         //app.setMainWidget( mainWin );
//         int ret = 0;
//         if( OK )
//             ret = app.exec();
   
// 	// shut down the interpreter
// 	PyInterpreterState * mainInterpreterState = mainThreadState->interp;
// 	// create a thread state object for this thread
// 	PyThreadState * myThreadState = PyThreadState_New(mainInterpreterState);
// 	PyThreadState_Swap(myThreadState);
	
// 	PyEval_AcquireLock();
// 	Py_Finalize();

//         return ret;
//     }
//     else
//     {
//         QStringList fileNames;
//         fileNames = KFileDialog::getOpenFileNames(QString::null, "*.theme", 0, "Open configurations");
//         for ( QStringList::Iterator it = fileNames.begin(); it != fileNames.end(); ++it )
//         {
//             QFileInfo file( *it );
//             if( file.exists() && !file.isDir() )
//             {
//                 mainWin = new karamba( *it );
//                 mainWin->show();
//                 OK = true;
//             }
//         }
//         int ret = 0;
//         if( OK )
//             ret = app.exec();

// 	// shut down the interpreter
// 	PyInterpreterState * mainInterpreterState = mainThreadState->interp;
// 	// create a thread state object for this thread
// 	PyThreadState * myThreadState = PyThreadState_New(mainInterpreterState);
// 	PyThreadState_Swap(myThreadState);
// 	PyEval_AcquireLock();
// 	Py_Finalize();
//         return ret;
//     }

//     args->clear();

//     // shut down the interpreter

//     PyInterpreterState * mainInterpreterState = mainThreadState->interp;
//     // create a thread state object for this thread
//     PyThreadState * myThreadState = PyThreadState_New(mainInterpreterState);
//     PyThreadState_Swap(myThreadState);
//     PyEval_AcquireLock();
//     Py_Finalize();
  
  
  TaskManager t;
  
  printf("%d %d", t.numberOfDesktops(), t.currentDesktop());

  TaskList list = t.tasks();

    Task *task;
    for ( task = list.first(); task; task = list.next() ) {
        cout << task->name().latin1() << endl;
	task->restore();
    }
    cout << endl;

  return 0;
    


}
