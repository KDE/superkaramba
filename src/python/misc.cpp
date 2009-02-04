/****************************************************************************
*  misc_python.cpp  -  Misc Functions for python api
*
*  Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>
*  Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>
*  Copyright (C) 2004 Petri Damst� <damu@iki.fi>
*  Copyright (C) 2004, 2005 Luke Kenneth Casson Leighton <lkcl@lkcl.net>
*
*  This file is part of SuperKaramba.
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

#if defined(_XOPEN_SOURCE) && !defined(__SUNPRO_CC)
#undef _XOPEN_SOURCE
#endif

#include <Python.h>

#include <QObject>
#include <QList>

#include <kglobal.h>
#include <klocale.h>
#include <kservice.h>
#include <krun.h>
#include <kdebug.h>

#include "meters/meter.h"
#include "meters/imagelabel.h"
#include "meters/textlabel.h"
#include "meters/clickarea.h"

#include "python/meter.h"
#include "python/misc.h"

#include "showdesktop.h"
#include "karambaapp.h"
#include "themefile.h"
#include "themelocale.h"
#include "../karamba.h"
#include "../systemtray.h"

/* now a method we need to expose to Python */
long acceptDrops(long widget)
{
    Karamba* currTheme = (Karamba*)widget;

    currTheme->setAcceptDrops(true);

    return 1;
}

PyObject* py_accept_drops(PyObject *, PyObject *args)
{
    long widget;

    if (!PyArg_ParseTuple(args, (char*)"l", &widget))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;
    return Py_BuildValue((char*)"l", acceptDrops(widget));
}

// Runs a command, returns 0 if it could not start command
PyObject* py_run_command(PyObject*, PyObject* args)
{
    char* name;
    char* command;
    char* icon;
    PyObject *lst;
    if (!PyArg_ParseTuple(args, (char*)"sssO:run", &name, &command, &icon, &lst) ||
            lst == NULL || !PyList_Check(lst))
        return NULL;

    QString n;
    QString c;
    QString i;

    n = QString::fromAscii(name);
    c = QString::fromAscii(command);
    i = QString::fromAscii(icon);

    KService svc(n, c, i);
    KUrl::List l;

    for (int i = 0; i < PyList_Size(lst); i++) {
        l.append(PyString2QString(PyList_GetItem(lst, i)));
    }
    KRun::run(svc, l, 0);
    return Py_BuildValue("l", 1);
}

// Runs a command, returns 0 if it could not start command
PyObject* py_execute_command(PyObject *, PyObject* args)
{
    PyObject* s;

    if (!PyArg_ParseTuple(args, (char*)"O:execute", &s))
        return NULL;
    return Py_BuildValue((char*)"l", KRun::runCommand(PyString2QString(s),0L));
}

// Runs a command, returns 0 if it could not start command
PyObject* py_execute_command_interactive(PyObject *, PyObject* args)
{
    long widget;
    //if (!PyArg_ParseTuple(args, (char*)"ls", &widget, &command))
    //  return NULL;

    int numLines;       /* how many lines we passed for parsing */
    QString line;       /* pointer to the line as a string */

    PyObject * listObj; /* the list of strings */
    PyObject * strObj;  /* one string in the list */

    /* the O! parses for a Python object (listObj) checked
       to be of type PyList_Type */
    if (! PyArg_ParseTuple(args, (char*)"lO!", &widget, &PyList_Type, &listObj))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;

    Karamba* currTheme = (Karamba*)widget;

    K3Process *currProcess = new K3Process;
    currTheme->setProcess(currProcess);

    /* get the number of lines passed to us */
    numLines = PyList_Size(listObj);

    /* should raise an error here. */
    if (numLines < 0) return NULL; /* Not a list */

    /* iterate over items of the list, grabbing strings, and parsing
       for numbers */
    for (int i = 0; i < numLines; i++) {

        /* grab the string object from the next element of the list */
        strObj = PyList_GetItem(listObj, i); /* Can't fail */

        /* make it a string */
        line = PyString2QString(strObj);

        /* now do the parsing */
        *(currProcess) << line;

    }
    QApplication::connect(currProcess,
                          SIGNAL(processExited(K3Process *)),
                          currTheme,
                          SLOT(processExited(K3Process *)));
    QApplication::connect(currProcess,
                          SIGNAL(receivedStdout(K3Process *, char *, int)),
                          currTheme,
                          SLOT(receivedStdout(K3Process *, char *, int)));
    currProcess->start(K3Process::NotifyOnExit, K3Process::Stdout);

    return Py_BuildValue((char*)"l", (int)(currProcess->pid()));
}

long attachClickArea(long widget, long meter, QString LeftButton, QString MiddleButton, QString RightButton)
{
    Q_UNUSED(widget);

    Meter* currMeter = (Meter*) meter;

    // if currMeter is of type ImageLabel*
    if (ImageLabel* image = dynamic_cast<ImageLabel*>(currMeter)) {
        image->attachClickArea(LeftButton, MiddleButton, RightButton);
        image->allowClick(true);
    }
    // else if currMeter is of type TextLabel*
    else if (TextLabel* text = dynamic_cast<TextLabel*>(currMeter)) {
        text->attachClickArea(LeftButton, MiddleButton, RightButton);
        text->allowClick(true);
    } else {
        //The given meter does not support attached clickAreas.
        qWarning("The given meter is not of type image or text");
        return 0;
    }
    return 1;
}

PyObject* py_attach_clickArea(PyObject*, PyObject* args, PyObject* dict)
{
    long widget;
    long meter;
    char* LeftButton = NULL;
    char* MiddleButton = NULL;
    char* RightButton = NULL;
    const char* const mouseButtons[] = {"Widget", "Meter", "LeftButton", "MiddleButton",
                                        "RightButton", NULL
                                       };
    if (!PyArg_ParseTupleAndKeywords(args, dict, (char*)"ll|sss:attachClickArea",
                                     (char**)mouseButtons, &widget, &meter, &LeftButton, &MiddleButton, &RightButton))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;
    QString lB, mB, rB;
    if (LeftButton != NULL) {
        lB = QString::fromAscii(LeftButton);
    } else {
        lB = QString::fromAscii("");
    }
    if (MiddleButton != NULL) {
        mB = QString::fromAscii(MiddleButton);
    } else {
        mB = QString::fromAscii("");
    }
    if (RightButton != NULL) {
        rB = QString::fromAscii(RightButton);
    } else {
        rB = QString::fromAscii("");
    }
    return Py_BuildValue((char*)"l", attachClickArea(widget, meter, lB, mB, rB));
}

/* now a method we need to expose to Python */
long toggleShowDesktop(long)
{
    ShowDesktop *s = ShowDesktop::self();
    s->toggle();
    return 1;
}

PyObject* py_toggle_show_desktop(PyObject *, PyObject *args)
{
    long widget;
    if (!PyArg_ParseTuple(args, (char*)"l:toggleShowDesktop", &widget))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;
    return Py_BuildValue((char*)"l", toggleShowDesktop(widget));
}

/* now a method we need to expose to Python */
const char* getPrettyName(long widget)
{
    Karamba* currTheme = (Karamba*)widget;

    return currTheme->prettyName().toAscii().constData();
}

PyObject* py_get_pretty_name(PyObject *, PyObject *args)
{
    long widget;
    if (!PyArg_ParseTuple(args, (char*)"l:getPrettyThemeName", &widget))
        return NULL;
    return Py_BuildValue((char*)"s", getPrettyName(widget));
}

/* now a method we need to expose to Python */
const char* getThemePath(long widget)
{
    Karamba* currTheme = (Karamba*)widget;
    return currTheme->theme().path().toAscii().constData();
}

PyObject* py_get_theme_path(PyObject *, PyObject *args)
{
    long widget;
    if (!PyArg_ParseTuple(args, (char*)"l:getThemePath", &widget))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;
    return Py_BuildValue((char*)"s", getThemePath(widget));
}

PyObject* py_language(PyObject *, PyObject *args)
{
    long widget;
    if (!PyArg_ParseTuple(args, (char*)"l:language", &widget))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;
    return Py_BuildValue((char*)"s",
                         ((Karamba*)widget)->theme().locale()->language().toAscii().constData());
}

PyObject* py_userLanguage(PyObject *, PyObject *args)
{
    long widget;
    if (!PyArg_ParseTuple(args, (char*)"l:language", &widget))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;
    return Py_BuildValue((char*)"s", KGlobal::locale()->language().toAscii().constData());
}

PyObject* py_userLanguages(PyObject *, PyObject *args)
{
    long widget;
    if (!PyArg_ParseTuple(args, (char*)"l:language", &widget))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;

    unsigned int noOfLangs = KGlobal::locale()->languageList().count();

    PyObject *list, *item;
    list = PyList_New(noOfLangs);

    for (unsigned int i = 0; i < noOfLangs; i++) {
        item = Py_BuildValue((char*)"s", KGlobal::locale()->languageList()[i].toAscii().constData());
        PyList_SetItem(list, i, item);
    }

    return list;
}

PyObject* py_read_theme_file(PyObject *, PyObject *args)
{
    long widget;
    char *file;
    if (!PyArg_ParseTuple(args, (char*)"ls:readThemeFile", &widget, &file))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;
    Karamba* k = (Karamba*)widget;
    QByteArray ba = k->theme().readThemeFile(file);
    return PyString_FromStringAndSize(ba.data(), ba.size());
}

/* now a method we need to expose to Python */
long removeClickArea(long widget, long click)
{
    Karamba* currTheme = (Karamba*) widget;
    ClickArea* currMeter = (ClickArea*) click;

    if (currTheme->removeMeter(currMeter))
	currMeter = 0; // deleted
    return (long)currMeter;
}

/* now a method we need to expose to Python */
long createServiceClickArea(long widget, long x, long y, long w, long h, char *name, char* exec, char *icon)
{
    Karamba* currTheme = (Karamba*)widget;
    ClickArea *tmp = new ClickArea(currTheme, false, x, y, w, h);
    QString n;
    QString e;
    QString i;

    n = QString::fromAscii(name);
    e = QString::fromAscii(exec);
    i = QString::fromAscii(icon);

    tmp->setServiceOnClick(n, e, i);

    return (long)tmp;
}

long createClickArea(long widget, long x, long y, long w, long h, char* text)
{
    Karamba* currTheme = (Karamba*)widget;
    ClickArea *tmp = new ClickArea(currTheme, false, x, y, w, h);
    QString onclick;

    onclick = QString::fromAscii(text);

    tmp->setOnClick(onclick);

    return (long)tmp;
}

PyObject* py_remove_click_area(PyObject *, PyObject *args)
{
    long widget, click;
    if (!PyArg_ParseTuple(args, (char*)"ll:removeClickArea", &widget, &click))
        return NULL;
    return Py_BuildValue((char*)"l", removeClickArea(widget, click));
}

PyObject* py_create_service_click_area(PyObject *, PyObject *args)
{
    long widget, x, y, w, h;
    char *name;
    char *exec;
    char *icon;
    if (!PyArg_ParseTuple(args, (char*)"lllllsss:createServiceClickArea", &widget, &x, &y,
                          &w, &h, &name, &exec, &icon))
        return NULL;
    return Py_BuildValue((char*)"l", createServiceClickArea(widget, x, y, w, h, name, exec, icon));
}

PyObject* py_create_click_area(PyObject *, PyObject *args)
{
    long widget, x, y, w, h;
    char *text;
    if (!PyArg_ParseTuple(args, (char*)"llllls:createClickArea", &widget, &x, &y,
                          &w, &h, &text))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;
    return Py_BuildValue((char*)"l", createClickArea(widget, x, y, w, h, text));
}

static long callTheme(long widget, char* path, char *str)
{

    Karamba* currTheme = (Karamba*) widget;
    if (currTheme)
        currTheme->sendDataToTheme(QString(path), QString(str));

    return (long)currTheme;
}

static long setIncomingData(long widget, char* path, char *obj)
{
    Karamba* currTheme = (Karamba*) widget;
    if (currTheme)
        currTheme->sendData(QString(path), QString(obj));

    return (long)currTheme;
}

static QString getIncomingData(long widget)
{
    Karamba* currTheme = (Karamba*) widget;

    if (currTheme)
        return currTheme->retrieveReceivedData();

    return QString("");
}

/*
 * openNamedTheme.  this function checks to see whether the theme
 * being opened is unique or not (against all running Karamba widgets).
 * this is important, as loading themes with the same name causes
 * grief.
 */
long openNamedTheme(char* path, char *name, bool is_sub_theme)
{
    Q_UNUSED(name)
    QString filename;
    Karamba* newTheme = 0;

    filename = QString::fromAscii(path);

    QFileInfo file(filename);

    if (file.exists()) {
/*        QString prettyName(name);
        KarambaApplication* app = (KarambaApplication*)qApp;
        if (!app->themeExists(prettyName)) {*/
            newTheme = new Karamba(KUrl(filename), 0, -1, is_sub_theme);

            newTheme->show();
//        }
    }
    return (long)newTheme;
}

/* now a method we need to expose to Python */
long openTheme(char* path)
{

    QString filename;
    Karamba* newTheme = 0;

    filename = QString::fromAscii(path);

    QFileInfo file(filename);

    if (file.exists()) {
        newTheme = new Karamba(KUrl(filename));
        newTheme->show();
    }

    return (long)newTheme;
}

PyObject* py_get_incoming_data(PyObject *, PyObject *args)
{
    long widget;
    if (!PyArg_ParseTuple(args, (char*)"l:getIncomingData", &widget))
        return NULL;
    return Py_BuildValue((char*)"O", QString2PyString(getIncomingData(widget)));
}

PyObject* py_set_incoming_data(PyObject *, PyObject *args)
{
    char *themePath;
    long widget;
    char *obj;
    if (!PyArg_ParseTuple(args, (char*)"lss:setIncomingData", &widget, &themePath, &obj))
        return NULL;
    return Py_BuildValue((char*)"l", setIncomingData(widget, themePath, obj));
}

PyObject* py_call_theme(PyObject *, PyObject *args)
{
    char *themePath;
    char *str;
    long widget;
    if (!PyArg_ParseTuple(args, (char*)"lss:callTheme", &widget, &themePath, &str))
        return NULL;
    return Py_BuildValue((char*)"l", callTheme(widget, themePath, str));
}

PyObject* py_open_named_theme(PyObject *, PyObject *args)
{
    char *themePath;
    char *themeName;
    long is_sub_theme;
    if (!PyArg_ParseTuple(args, (char*)"ssl:openNamedTheme", &themePath, &themeName, &is_sub_theme))
        return NULL;
    return Py_BuildValue((char*)"l", openNamedTheme(themePath, themeName, is_sub_theme ? true : false));
}

PyObject* py_open_theme(PyObject *, PyObject *args)
{
    char *themePath;
    if (!PyArg_ParseTuple(args, (char*)"s:openTheme", &themePath))
        return NULL;
    return Py_BuildValue((char*)"l", openTheme(themePath));
}

PyObject* py_reload_theme(PyObject *, PyObject *args)
{
    long widget;
    if (!PyArg_ParseTuple(args, (char*)"l:reloadTheme", &widget))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;
    ((Karamba*)widget)->reloadConfig();
    return Py_BuildValue((char*)"l", 1);
}

/* now a method we need to expose to Python */
int getNumberOfDesktops(long widget)
{
    Karamba* currTheme = (Karamba*)widget;

    return currTheme->getNumberOfDesktops();
}

PyObject* py_get_number_of_desktops(PyObject *, PyObject *args)
{
    long widget;
    if (!PyArg_ParseTuple(args, (char*)"l:getNumberOfDesktops", &widget))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;
    return Py_BuildValue((char*)"l", getNumberOfDesktops(widget));
}

/* now a method we need to expose to Python */
int translateAll(long widget, int x, int y)
{
    Q_UNUSED(widget);
    Q_UNUSED(x);
    Q_UNUSED(y);
    /*
    Karamba* currTheme = (Karamba*)widget;
    QList <QGraphicsItem*> items = ((QGraphicsItemGroup*)currTheme)->children();

    QGraphicsItem *meter;
    foreach(meter, items) {
        ((Meter*) meter)->setSize(((Meter*) meter)->getX() + x,
                                  ((Meter*) meter)->getY() + y,
                                  ((Meter*) meter)->getWidth(),
                                  ((Meter*) meter)->getHeight());
    }
    if (currTheme->systray != 0) {
        currTheme->systray->move(currTheme->systray->x() + x,
                                 currTheme->systray->y() + y);
    }
    */
    return 0;
}

PyObject* py_translate_all(PyObject *, PyObject *args)
{
    long widget;
    int x, y;
    if (!PyArg_ParseTuple(args, (char*)"lii:translateAll", &widget, &x, &y))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;
    return Py_BuildValue((char*)"lii", translateAll(widget, x, y));
}

/* now a method we need to expose to Python */
int show(long widget)
{
    Karamba* currTheme = (Karamba*)widget;
    currTheme->show();
    return 0;
}

PyObject* py_show(PyObject *, PyObject *args)
{
    long widget;
    if (!PyArg_ParseTuple(args, (char*)"l:show", &widget))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;
    return Py_BuildValue((char*)"l", show(widget));
}

/* now a method we need to expose to Python */
int hide(long widget)
{
    Karamba* currTheme = (Karamba*)widget;
    currTheme->hide();
    return 0;
}

PyObject* py_hide(PyObject *, PyObject *args)
{
    long widget;
    if (!PyArg_ParseTuple(args, (char*)"l:hide", &widget))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;
    return Py_BuildValue((char*)"l", hide(widget));
}

/*Putting includes here to show the dependency for the call(s) below (if we ever decide to move the networking callbacks into a separate file*/
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#ifdef __FreeBSD__
#include <netinet/in.h>
#endif
#ifdef __OpenBSD__
#include <sys/types.h>
#include <netinet/in.h>
#endif
#if defined(Q_OS_SOLARIS)
#include <sys/sockio.h>
#endif
/* now a method we need to expose to Python */
QString getIp(char *device_name)
{
    int i, sd, numdevs;
    struct ifconf ifc_conf;
    char ifc_conf_buf[sizeof(struct ifreq) * 32];
    struct ifreq *devptr;
    int ifc_conf_buf_size;
    static struct in_addr host;
    QString retval;

    retval = "Disconnected";

    /*
     * Open a socket, any type will do so we choose UDP, and ask it with
     * an ioctl call what devices are behind it.
     */
    if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        qWarning("Error: Unable to create socket (socket)");
        return "Error";
    }

    /*
     * Fill the buffer with our static buffer, probably big enough, and get
     * the interface configuration.
     */
    ifc_conf_buf_size = sizeof ifc_conf_buf;
    ifc_conf.ifc_len = ifc_conf_buf_size;
    ifc_conf.ifc_buf = ifc_conf_buf;
    if (ioctl(sd, SIOCGIFCONF, &ifc_conf) < 0) {
        qWarning("Error: Unable to get network interface conf (ioctl)");
        close(sd);
        return "Error";
    }

    /*
     * An array of devices were returned.  Which ones are up right now and
     * have broadcast capability?
     */
    numdevs = ifc_conf.ifc_len / sizeof(struct ifreq);
    //qDebug("numdevs = %d", numdevs);
    for (i = 0; i < numdevs; i++) {
        //qDebug("iterations: %d", i);
        /* devptr points into an array of ifreq structs. */
        devptr = &ifc_conf.ifc_req[i];

        if (ioctl(sd, SIOCGIFADDR, devptr) < 0 || devptr->ifr_addr.sa_family != AF_INET)
            continue;

        if (ioctl(sd, SIOCGIFFLAGS, devptr) < 0) {
            qWarning("Error: Unable to get device interface flags (ioctl).");
            close(sd);
            return "Error";
        }

        //We generally don't want probing of the loopback devices
        if ((devptr->ifr_flags & IFF_LOOPBACK) != 0)
            continue;

        if ((devptr->ifr_flags & IFF_UP) == 0)
            continue;

        if ((devptr->ifr_flags & IFF_BROADCAST) == 0)
            continue;

        /* Get the broadcast address. */
        if (ioctl(sd, SIOCGIFFLAGS, devptr) < 0) {
            qWarning("Error: Unable to get device interface flags (ioctl).");
            close(sd);
            return "Error";
        } else {
            if (!strcmp((char*)devptr->ifr_name, device_name)) {
                host.s_addr = ((struct sockaddr_in*) & devptr->ifr_addr)->sin_addr.s_addr;
                retval = inet_ntoa(host);
                break;
            }
        }
    }
    close(sd);
    return retval;
}

PyObject* py_set_update_time(PyObject *, PyObject *args)
{
    long widget;
    double time;
    if (!PyArg_ParseTuple(args, (char*)"ld:setUpdateTime", &widget, &time))
        return NULL;
    Karamba* currTheme = (Karamba*)widget;
    currTheme->setUpdateTime(time);
    return Py_BuildValue((char*)"l", 1);
}

PyObject* py_get_update_time(PyObject *, PyObject *args)
{
    long widget;
    double time;
    if (!PyArg_ParseTuple(args, (char*)"l:getUpdateTime", &widget, &time))
        return NULL;
    Karamba* currTheme = (Karamba*)widget;
    return Py_BuildValue((char*)"d", currTheme->getUpdateTime());
}

PyObject* py_get_ip(PyObject *, PyObject *args)
{
    long widget;
    char *interface;
    if (!PyArg_ParseTuple(args, (char*)"ls:getIp", &widget, &interface))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;
    return Py_BuildValue((char*)"O", QString2PyString(getIp(interface)));
}

static void management_popup(long widget)
{
    Karamba* currTheme = (Karamba*)widget;
    currTheme->popupGlobalMenu();
}

PyObject* py_management_popup(PyObject *, PyObject *args)
{
    long widget;
    if (!PyArg_ParseTuple(args, (char*)"l:managementPopup", &widget))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;
    management_popup(widget);
    return Py_BuildValue((char*)"l", 1);
}

static void set_want_right_button(long widget, long yesno)
{
    Karamba* currTheme = (Karamba*)widget;
    currTheme->setWantRightButton(yesno);
}

PyObject* py_want_right_button(PyObject *, PyObject *args)
{
    long widget, i;
    if (!PyArg_ParseTuple(args, (char*)"ll:wantRightButton", &widget, &i))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;
    set_want_right_button(widget, i);
    return Py_BuildValue((char*)"l", 1);
}


static void changeInterval(long widget, long interval)
{
    Karamba* currTheme = (Karamba*)widget;
    currTheme->changeInterval(interval);
}

PyObject* py_change_interval(PyObject *, PyObject *args)
{
    long widget, i;
    if (!PyArg_ParseTuple(args, (char*)"ll:changeInterval", &widget, &i))
        return NULL;
    if (!checkKaramba(widget))
        return NULL;
    changeInterval(widget, i);
    return Py_BuildValue((char*)"l", 1);
}


