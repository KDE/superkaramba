/*
 * Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>
 * Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>
 * Copyright (c) 2005 Ryan Nickell <p0z3r@earthlink.net>
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

#ifndef _KARAMBA_H_
#define _KARAMBA_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qwidget.h>
#include <kapplication.h>

#include <kwinmodule.h>
#include <kwin.h>

#include <qfile.h>
#include <kfile.h>
#include <qfileinfo.h>
#include <kaction.h>
#include <qtimer.h>
#include <qpixmap.h>
#include <qpainter.h>

//#include <krootpixmap.h>

#include <qregexp.h>
#include <qlabel.h>
#include <qobjectlist.h>
#include <qstring.h>
#include <qstringlist.h>
#include <ksharedpixmap.h>
#include <qvaluestack.h>
#include <dcopclient.h>
#include <kpopupmenu.h>
#include <qcursor.h>
#include <netwm.h>
#include <kiconloader.h>
#include <kfiledialog.h>
#include <qmap.h>
#include <kurl.h>
#include <krun.h>
#include <qdatetime.h>
#include <qbitmap.h>
#include <kconfig.h>
#include  <kprocess.h>
#include <qdragobject.h>

#include "karambarootpixmap.h"

#include "bar.h"
#include "textlabel.h"
#include "imagelabel.h"
#include "graph.h"

#include "clickarea.h"

#include "sensorparams.h"
#include "memsensor.h"
#include "datesensor.h"
#include "uptimesensor.h"
#include "memsensor.h"
#include "cpusensor.h"
#include "networksensor.h"
#include "xmmssensor.h"
#include "noatunsensor.h"
#include "programsensor.h"
#include "disksensor.h"
#include "sensorsensor.h"
#include "textfilesensor.h"

#include "clickmap.h"
#include "rsssensor.h"
//#include "clickable.h"
#include "taskmanager.h"
#include "showdesktop.h"
#include "systemtray.h"
/**
 * @short Application Main Window
 * @author Adam Geitgey <adam@rootnode.org>
 * @author Hans Karlsson <karlsson.h@home.se>
 * @version 0.26
 */

class KarambaPython;

class karamba :  public QWidget
{
    Q_OBJECT

public:
    karamba(QString fn, bool reloading);
    QObjectList *menuList;

    virtual ~karamba();
    QObjectList *meterList;
    QObjectList *imageList;
    QString themePath;
    QObjectList *clickList;
    void setSensor( QString &line , Meter* meter);
    QString getSensor(Meter* meter);
    QString findSensorFromMap(Sensor* sensor);
    void deleteMeterFromSensors(Meter* meter);
    Sensor* findSensorFromList(Meter* meter);
    KPopupMenu* keditpop;
    KPopupMenu *kpop;
    QBitmap* widgetMask;
    KarambaRootPixmap *kroot;
    TaskManager taskManager;
    Systemtray* systray;
    KProcess* currProcess;
    bool useSmoothTransforms();

    void setWidgetUpdate(bool wu) { widgetUpdate = wu; };
    bool getWidgetUpdate() { return widgetUpdate; };
    bool hasMeter(Meter* meter) { return meterList->containsRef(meter) > 0; };
    char getTempUnit() { return tempUnit; };
    void addMenuConfigOption(QString key, QString name);
    bool setMenuConfigOption(QString key, bool value);
    bool readMenuConfigOption(QString key);
    void writeConfigData();
    TextField* getDefaultTextProps() { return defaultTextField; };
    QByteArray readZipFile(const QString& filename);
    bool zipTheme() const { return m_zipTheme; };
    static bool isZipFile(const QString& filename);

    int numberOfConfMenuItems;
    KConfig* config;
    QString themeFile;
    QString themeName;

    void toggleWidgetUpdate( bool );

    KWinModule*    kWinModule;

protected:
    void mousePressEvent( QMouseEvent *);
    void wheelEvent( QWheelEvent *);
    void mouseReleaseEvent( QMouseEvent *);
    void mouseDoubleClickEvent( QMouseEvent *);
    void mouseMoveEvent( QMouseEvent *);
    void keyPressEvent ( QKeyEvent * e );
    void closeEvent ( QCloseEvent *);
    void paintEvent ( QPaintEvent *);
    void saveProperties(KConfig *);
    void readProperties(KConfig *);
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);

private:
    bool widgetUpdate;
    bool repaintInProgress;

    NETWinInfo* info;
    bool onTop;
    bool managed;
    bool fixedPosition;
    bool haveUpdated;
    char tempUnit;

    bool parseConfig();
    int getInt( QString, QString & );
    int getInt( QString, QString &, bool & );
    // get3Int reads an int triplet, e.g. COLOR:0,50,100
    void get3Int(QString w, QString &line, int&, int&, int& );
    void get3Int(QString w, QString &line, int&, int&, int&, bool& );
    QString getString( QString, QString & );
    QString getString( QString, QString &, bool& );
    bool getBoolean( QString, QString & );
    bool getBoolean( QString, QString &, bool& );


    void passClick( QMouseEvent* );
    void passWheelClick( QWheelEvent* );
    void meterClicked(QMouseEvent*, Meter*);

    QMap<QString, Sensor*> sensorMap;
    QObjectList *sensorList;
    QObjectList *timeList;

    QTime lowerTimer;
    // use only the first occurance of KARAMBA in a config file
    bool foundKaramba;
    bool m_zipTheme;


    KPopupMenu* themeConfMenu;
    KPopupMenu* toDesktopMenu;

    DCOPClient *client;
    QCString appId;

    QPixmap pm;
    QPixmap background;
    QPainter p;

    QPoint clickPos;
    KActionCollection* accColl;
    KActionCollection* menuAccColl;
    KToggleAction *toggleLocked;
    // use highquality scale and rotate algorithms
    KToggleAction *toggleFastTransforms;

    // Python module references
    KarambaPython* pythonIface;
    TextField *defaultTextField;

    int  desktop;

public slots:
    void step();
    void externalStep();
    void widgetClosed();
    void updateSensors();
    void currentDesktopChanged(int);
    void currentWallpaperChanged(int);
    void slotToggleConfigOption(QString key, bool);
    void updateBackground(KSharedPixmap*);
    void passMenuOptionChanged(QString key, bool);
    void passMenuItemClicked(int);
    void quitKaramba();
    void processExited (KProcess *proc);
    void receivedStdout (KProcess *proc, char *buffer, int buflen);
    void downloadThemes();
    void toDesktop(int desktopid, int menuid);

    // Systray
    void systrayUpdated();

    // Task Manager
    void startupAdded(Startup*);
    void startupRemoved(Startup*);

    void taskAdded(Task*);
    void taskRemoved(Task*);
    void activeTaskChanged(Task*);
    void reloadConfig();

private:
    bool m_reloading;
private slots:
    void initPythonInterface();
    void startNewKaramba();
    void killWidget();
    void editConfig();
    void editScript();
    void slotToggleLocked();
    void slotToggleFastTransforms();
    void popupNotify(int);

};

/*
 * Slot to receive the event of moving the karamba object
 * to a new desktop. Generated by karamba::toDesktopMenu items
 */
class DesktopChangeSlot : public QObject
{
  Q_OBJECT

  public:
  DesktopChangeSlot(QObject *parent, int desktop_id);
  /* Parent should be the karamba object
   * desktop id of 0 indicates all desktops */
  void setMenuId(int id);
  int menuId();

  public slots:
      void receive();

 protected:
  int desktopid;
  int menuid;
};

class SignalBridge : public QObject
{
  Q_OBJECT

  public:
    SignalBridge(QObject* parent, QString, KActionCollection*);

  signals:
    void enabled(QString, bool);

  public slots:
    void receive();

  private:
    KActionCollection* collection;
};

#endif // _KARAMBA_H_
