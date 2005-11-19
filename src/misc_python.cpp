/****************************************************************************
*  misc_python.cpp  -  Misc Functions for python api
*
*  Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>
*  Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>
*  Copyright (c) 2004 Petri Damstén <damu@iki.fi>
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
*  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
****************************************************************************/

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include <Python.h>
#include <qglobal.h>
#include <qobject.h>
#include "kdebug.h"
#include "karamba.h"
#include "karambaapp.h"
#include "themefile.h"
#include "themelocale.h"
#include "meter.h"
#include "meter_python.h"
#include "misc_python.h"

/* now a method we need to expose to Python */
long acceptDrops(long widget)
{
  karamba* currTheme = (karamba*)widget;

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
PyObject* py_execute_command(PyObject *, PyObject* args)
{
  PyObject* s;

  if (!PyArg_ParseTuple(args, (char*)"O:execute", &s))
      return NULL;
  return Py_BuildValue((char*)"l", KRun::runCommand(PyString2QString(s)));
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

  karamba* currTheme = (karamba*)widget;

  currTheme->currProcess = new KProcess;

  /* get the number of lines passed to us */
  numLines = PyList_Size(listObj);

  /* should raise an error here. */
  if (numLines < 0) return NULL; /* Not a list */

  /* iterate over items of the list, grabbing strings, and parsing
     for numbers */
  for (int i=0; i<numLines; i++){

    /* grab the string object from the next element of the list */
    strObj = PyList_GetItem(listObj, i); /* Can't fail */

    /* make it a string */
    line = PyString2QString(strObj);

    /* now do the parsing */
    *(currTheme->currProcess) << line;

  }
  QApplication::connect(currTheme->currProcess,
                        SIGNAL(processExited(KProcess *)),
                        currTheme,
                        SLOT(processExited(KProcess *)));
  QApplication::connect(currTheme->currProcess,
                        SIGNAL(receivedStdout(KProcess *, char *, int)),
                        currTheme,
                        SLOT(receivedStdout(KProcess *, char *, int)));
  currTheme->currProcess->start(KProcess::NotifyOnExit, KProcess::Stdout);

  return Py_BuildValue((char*)"l", (int)(currTheme->currProcess->pid()));
}

long attachClickArea(long widget, long meter, QString LeftButton, QString MiddleButton, QString RightButton)
{
  karamba* currTheme = (karamba*) widget;
  Meter* currMeter = (Meter*) meter;

  // Look if currMeter has an ClickArea attached.
  bool meterAlreadyClickable = currTheme->clickList->containsRef(currMeter);

  // if currMeter is of type ImageLabel*
  if (ImageLabel* image = dynamic_cast<ImageLabel*>(currMeter))
  {
      image -> attachClickArea(LeftButton, MiddleButton, RightButton);
      if (!meterAlreadyClickable)
      {
          //qWarning("attachClickArea : meter is image");
          currTheme -> clickList -> append(image);
      }
  }
  // else if currMeter is of type TextLabel*
  else if (TextLabel* text = dynamic_cast<TextLabel*>(currMeter))
  {
      text -> attachClickArea(LeftButton, MiddleButton, RightButton);
      if (!meterAlreadyClickable)
      {
          //qWarning("attachClickArea : meter is text");
          currTheme -> clickList -> append(text);
      }
  }
  else
  {
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
  const char* mouseButtons[] = {"Widget", "Meter", "LeftButton", "MiddleButton",
                                "RightButton", NULL};
  if (!PyArg_ParseTupleAndKeywords(args, dict, (char*)"ll|sss:attachClickArea",
   (char**)mouseButtons, &widget, &meter, &LeftButton, &MiddleButton, &RightButton))
    return NULL;
  if (!checkKaramba(widget))
    return NULL;
  QString lB, mB, rB;
  if (LeftButton != NULL)
  {
      lB.setAscii(LeftButton);
  }
  else
  {
      lB.setAscii("");
  }
  if (MiddleButton != NULL)
  {
      mB.setAscii(MiddleButton);
  }
  else
  {
      mB.setAscii("");
  }
  if (RightButton != NULL)
  {
       rB.setAscii(RightButton);
  }
  else
  {
       rB.setAscii("");
  }
  return Py_BuildValue((char*)"l", attachClickArea(widget, meter, lB, mB, rB));
}

/* now a method we need to expose to Python */
long toggleShowDesktop(long)
{
  ShowDesktop *s = ShowDesktop::the();
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
const char* getThemePath(long widget)
{
  karamba* currTheme = (karamba*)widget;

  return currTheme->theme().path().ascii();
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
      ((karamba*)widget)->theme().locale()->language().ascii());
}

PyObject* py_read_theme_file(PyObject *, PyObject *args)
{
  long widget;
  char *file;
  if (!PyArg_ParseTuple(args, (char*)"ls:readThemeFile", &widget, &file))
    return NULL;
  if (!checkKaramba(widget))
    return NULL;
  karamba* k = (karamba*)widget;
  QByteArray ba = k->theme().readThemeFile(file);
  return PyString_FromStringAndSize(ba.data(), ba.size());
}

/* now a method we need to expose to Python */
long createClickArea(long widget, long x, long y, long w, long h, char* text) {

  karamba* currTheme = (karamba*)widget;
  ClickArea *tmp = new ClickArea(currTheme, x, y, w, h );
  QString onclick;

  onclick.setAscii(text);

  tmp->setOnClick(onclick );

  currTheme -> clickList -> append(tmp);
  return (long)tmp;
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

/* now a method we need to expose to Python */
long openTheme(char* path)
{

  QString filename;
  karamba* currTheme = 0;

  filename.setAscii(path);

  QFileInfo file( filename );

  if( file.exists() )
    {
      currTheme = new karamba( filename, false );
      currTheme->show();
    }

  return (long)currTheme;
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
  ((karamba*)widget)->reloadConfig();
  return Py_BuildValue((char*)"l", 1);
}

/* now a method we need to expose to Python */
int getNumberOfDesktops(long widget)
{
  karamba* currTheme = (karamba*)widget;

  return currTheme->kWinModule->numberOfDesktops();
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
  karamba* currTheme = (karamba*)widget;

  QObjectListIt it2( *currTheme->meterList ); // iterate over meters

  while ( it2 != 0 )
  {
    ((Meter*) *it2)->setSize(((Meter*) *it2)->getX()+x,
                             ((Meter*) *it2)->getY()+y,
                             ((Meter*) *it2)->getWidth(),
                             ((Meter*) *it2)->getHeight());
    ++it2;
  }

  if (currTheme->systray != 0)
  {
    currTheme->systray->move(currTheme->systray->x()+x,
                             currTheme->systray->y()+y);
  }
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
  karamba* currTheme = (karamba*)widget;
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
  karamba* currTheme = (karamba*)widget;
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
#if defined(Q_OS_SOLARIS)
#include <sys/sockio.h>
#endif
/* now a method we need to expose to Python */
QString getIp(char *device_name)
{
  int i, sd, numdevs;
  struct ifconf ifc_conf;
  char ifc_conf_buf[sizeof ( struct ifreq ) * 32];
  struct ifreq *devptr;
  int ifc_conf_buf_size;
  static struct in_addr host;
  QString retval;

  retval = "Disconnected";

  /*
   * Open a socket, any type will do so we choose UDP, and ask it with
   * an ioctl call what devices are behind it.
   */
  if ((sd = socket(AF_INET,SOCK_DGRAM,0)) < 0)
  {
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
  if (ioctl(sd,SIOCGIFCONF,&ifc_conf) < 0)
  {
    qWarning("Error: Unable to get network interface conf (ioctl)");
    close(sd);
    return "Error";
  }

  /*
   * An array of devices were returned.  Which ones are up right now and
   * have broadcast capability?
   */
  numdevs = ifc_conf.ifc_len / sizeof (struct ifreq);
  //qDebug("numdevs = %d", numdevs);
  for (i = 0; i < numdevs; i++)
  {
    //qDebug("iterations: %d", i);
    /* devptr points into an array of ifreq structs. */
    devptr = &ifc_conf.ifc_req[i];

    if (ioctl(sd, SIOCGIFADDR, devptr) < 0 || devptr->ifr_addr.sa_family != AF_INET)
      continue;

    if (ioctl(sd,SIOCGIFFLAGS,devptr) < 0)
    {
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
  if (ioctl(sd,SIOCGIFFLAGS,devptr) < 0)
  {
    qWarning("Error: Unable to get device interface flags (ioctl).");
    close(sd);
    return "Error";
  }
  else
  {
    if (!strcmp((char*)devptr->ifr_name, device_name))
    {
    host.s_addr = ((struct sockaddr_in*)&devptr->ifr_addr)->sin_addr.s_addr;
    retval = inet_ntoa(host);
    break;
    }
  }
  }
  close(sd);
  return retval;
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
