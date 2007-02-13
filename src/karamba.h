/*
 * Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>
 * Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>
 * Copyright (C) 2004,2005 Luke Kenneth Casson Leighton <lkcl@lkcl.net>
 * Copyright (c) 2005 Ryan Nickell <p0z3r@earthlink.net>
 * Copyright (c) 2007 Alexander Wiedenbruch <mail@wiedenbruch.de>
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


#ifndef KARAMBA_H
#define KARAMBA_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QThread>
#include <QTimer>
#include <QMutex>
#include <QMutexLocker>
#include <QSignalMapper>

#include <kurl.h>
#include <kmenu.h>
#include <netwm.h>
#include <kactioncollection.h>
#include <ktoggleaction.h>
#include <kwinmodule.h>
#include <kconfig.h>

#include "karambaapp.h"
#include "themefile.h"
#include "taskmanager.h"
#include "systemtray.h"

#include "meters/textfield.h"
#include "meters/richtextlabel.h"
#include "meters/bar.h"
#include "meters/graph.h"

#include "sensors/sensor.h"
#include "sensors/mem.h"
#include "sensors/disk.h"
#include "sensors/network.h"
#include "sensors/date.h"
#include "sensors/program.h"
#include "sensors/sensorparams.h"
#include "sensors/textfile.h"
#include "sensors/rss.h"
#include "sensors/uptime.h"
#include "sensors/lmsensor.h"

#include "python/karamba.h"

#include <QGraphicsItemGroup>

class Meter;

class Karamba : public QObject, public QGraphicsItemGroup
{
    Q_OBJECT

  public:
    Karamba(KUrl themeFile, QGraphicsView *view = 0,
            QGraphicsScene *scene = 0, int instance = -1);

    virtual ~Karamba();

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                    QWidget *widget);

    QGraphicsScene* getScene();
    QGraphicsView* getView();

    void keyPressed(const QString& s, const Meter* meter);

    ThemeFile& theme();

    int testCallback(int value);

    bool hasMeter(Meter* meter);
    bool removeMeter(Meter *meter);
    QString getSensor(Meter* meter);
    void setSensor(const LineParser& lineParser, Meter* meter);
    void deleteMeterFromSensors(Meter* meter);
    TextField* getDefaultTextProps();
    void updateSensors();
    void setFixedSize(u_int w, u_int h);
    void move(u_int x, u_int y);
    bool readMenuConfigOption(QString key);
    KConfig* getConfig();
    //void removePopupMenu(KMenu *menu);
    bool popupMenuExisting(KMenu *menu);
    QString prettyName(); 
    int getNumberOfDesktops();
    double getUpdateTime();
    void setUpdateTime(double newTime);
    void setWantRightButton(bool enable);
    void changeInterval(u_int newInterval);
    void addMenuConfigOption(QString key, QString name);
    bool setMenuConfigOption(QString key, bool value);
    KMenu* addPopupMenu();
    QAction* addMenuItem(KMenu *menu, QString text,
              QString icon);
    void popupMenu(KMenu *menu, QPoint pos);
    void deletePopupMenu(KMenu *menu);
    void deleteMenuItem(QAction *action);
    void scaleImageLabel(Meter *meter, int width,
              int height);
    void moveMeter(Meter *meter, int x, int y);
    void popupGlobalMenu();

    void writeConfigData();
    
    void showMenuExtension();
    void hideMenuExtension();

    int instance();
    void setInstance(int instance);

    KProcess *currProcess;
    Systemtray *systray;

  public Q_SLOTS:
    void closeWidget();
    void reloadConfig();
    void processExited(KProcess *proc);
    void receivedStdout(KProcess *proc, char *buffer, int buflen);
    void startupAdded(Startup::StartupPtr);
    void startupRemoved(Startup::StartupPtr);
    void taskAdded(Task::TaskPtr);
    void taskRemoved(Task::TaskPtr);
    void activeTaskChanged(Task::TaskPtr);
    void passMenuItemClicked(QAction* action);
    void slotFileChanged(const QString &file);

  private Q_SLOTS:
    void initPythonInterface();
    void slotToggleLocked();
    void slotToggleSystemTray();
    void slotShowTheme();
    void slotQuit();
    void currentDesktopChanged(int i);
    void slotToggleConfigOption(QObject*);
    void slotDesktopChanged(int desktop);

    void step();

  protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    void wheelEvent(QGraphicsSceneWheelEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dropEvent(QGraphicsSceneDragDropEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void timerEvent(QTimerEvent *event);
    void keyPressEvent(QKeyEvent *event);

  private:
    ThemeFile m_theme;
    QGraphicsScene *m_scene;
    QGraphicsView *m_view;

    KWinModule *m_KWinModule;

    KarambaPython *m_python;
    
    bool m_foundKaramba;
    bool m_onTop;
    bool m_managed;

    NETWinInfo *m_info;

    QRect size;

    u_int m_desktop;

    u_int m_interval;

    char m_tempUnit;

    TextField *m_defaultTextField;

    int m_scaleStep;
    bool m_showMenu;

    QList<Sensor*> *m_sensorList;
    QMap<QString, Sensor*> m_sensorMap;

    KMenu *m_popupMenu;
    KToggleAction *m_toggleLocked;
    KMenu *m_themeConfMenu;
    KMenu *m_toDesktopMenu;
    KMenu *m_globalMenu;

    QTimer *m_stepTimer;

    QSignalMapper *m_signalMapperConfig;
    QSignalMapper *m_signalMapperDesktop;

    KConfig *m_config;

    int m_instance;

    QList<KMenu*> *m_menuList;

    QString m_prettyName;

    double m_updateTime;

    bool m_wantRightButton;

    QPoint m_mouseClickPos;

    bool m_globalView;

    bool parseConfig();
    Sensor *findSensorFromList(Meter *meter);
    QString findSensorFromMap(Sensor *sensor);

    void preparePopupMenu();
};

#endif // KARAMBA_H
