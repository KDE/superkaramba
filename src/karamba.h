/*
 * Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>
 * Copyright (C) 2004,2005 Luke Kenneth Casson Leighton <lkcl@lkcl.net>
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
#include "input.h"

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
#include "themefile.h"

/**
 * @short Application Main Window
 * @author Adam Geitgey <adam@rootnode.org>
 * @author Hans Karlsson <karlsson.h@home.se>
 * @author Luke Kenneth Casson Leighton <lkcl@lkcl.net>
 * @version 0.26
 */

class KarambaPython;
class LineParser;

class karamba :  public QWidget
{
    Q_OBJECT

public:
    karamba(QString fn, QString name, bool reloading = false,
            int instance = -1, bool sub_theme = false);
    QObjectList *menuList;

    virtual ~karamba();
    const ThemeFile& theme() const { return m_theme; };

    QObjectList *meterList;
    QObjectList *imageList;
    QObjectList *clickList;
    void setSensor(const LineParser& lineParser, Meter* meter);
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

    void changeInterval(int interval);
    void setWidgetUpdate(bool wu) { widgetUpdate = wu; };
    bool getWidgetUpdate() { return widgetUpdate; };
    bool hasMeter(Meter* meter) { return meterList->containsRef(meter) > 0; };
    char getTempUnit() { return tempUnit; };
    void addMenuConfigOption(QString key, QString name);
    bool setMenuConfigOption(QString key, bool value);
    bool readMenuConfigOption(QString key);
    void writeConfigData();
    TextField* getDefaultTextProps() { return defaultTextField; };
    int instance() const { return m_instance; };
    void setInstance(int instance) { m_instance = instance; };
    void closeTheme(bool reloading = false);
    void keyPressed(const QString& s, const Meter* meter);

    int numberOfConfMenuItems;
    KConfig* config;
    QString prettyName;
    bool m_sub_theme;
    bool isSubTheme() { return m_sub_theme; }

    void toggleWidgetUpdate( bool );

    KWinModule*    kWinModule;

    QString incomingData;
    QString getIncomingData() { return incomingData; }
    void _setIncomingData(QString data) { incomingData = data; }
    void setIncomingData(QString theme, QString data);

    void themeNotify(QString theme, QString txt);
    void callTheme(QString theme, QString txt);

    double getUpdateTime() { return update_time; }
    void setUpdateTime(double time) { update_time = time; }

    void makeActive();
    void makePassive();

    void showMenuExtension();
    void hideMenuExtension();

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
    //bool reloading;
    bool want_right_button;
    bool want_meter_wheel_event;

    NETWinInfo* info;
    bool onTop;
    bool managed;
    bool fixedPosition;
    bool haveUpdated;
    char tempUnit;
    double update_time;
    int m_instance;

    bool parseConfig();

    void passClick( QMouseEvent* );
    void passWheelClick( QWheelEvent* );
    void meterClicked(QMouseEvent*, Meter*);

    QMap<QString, Sensor*> sensorMap;
    QObjectList *sensorList;
    QObjectList *timeList;

    QTime lowerTimer;
    // use only the first occurance of KARAMBA in a config file
    bool foundKaramba;

    KPopupMenu* themeConfMenu;
    KPopupMenu* toDesktopMenu;
    KPopupMenu* kglobal;

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
    ThemeFile m_theme;

  int trayMenuSeperatorId;
  int trayMenuQuitId;
  int trayMenuToggleId;
  int trayMenuThemeId;
  void start();

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
    void processExited (KProcess *proc);
    void receivedStdout (KProcess *proc, char *buffer, int buflen);
    void toDesktop(int desktopid, int menuid);
    const char *getPrettyName() { return prettyName.ascii(); }

    // Systray
    void systrayUpdated();

    // Task Manager
    void startupAdded(Startup*);
    void startupRemoved(Startup*);

    void taskAdded(Task*);
    void taskRemoved(Task*);
    void activeTaskChanged(Task*);
    void reloadConfig();

    void setAlwaysOnTop(bool stay);

    /**
     * If true, then when a right button is pressed on the theme,
     * the theme's python widgetMouseMoved function is called.
     */
    void setWantRightButton(bool yesno) { want_right_button = yesno; }

    void setWantMeterWheelEvent(bool yesno) { want_meter_wheel_event = yesno; }

    /**
     * can be used to fire up the karamba management popup menu
     */
    void management_popup( void );

private:
    bool m_reloading;
    bool themeStarted;
    QTimer *m_sysTimer;
    int m_interval;

private slots:
    void initPythonInterface();
    void killWidget();
    void editConfig();
    void editScript();
    void slotToggleLocked();
    void slotToggleFastTransforms();
    void popupNotify(int);
    void slotFileChanged( const QString & );

    void slotToggleSystemTray();
    void slotQuit();
    void slotShowTheme();
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

/** SignalBridge is an ungulate that lives in the forests of wild Wisconsin. */
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
