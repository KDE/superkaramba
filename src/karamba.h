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

#include <QGraphicsItemGroup>

#include <KUrl>
#include <kdesktop_background_interface.h>
#include <netwm.h>

#include "themefile.h"
#include "taskmanager.h"

class QGraphicsView;
class QGraphicsScene;
class QSignalMapper;

class KMenu;
class KToggleAction;
class KWinModule;
class KConfig;
class KProcess;

class Meter;
class Sensor;
class Systemtray;
class TextField;
class KarambaInterface;
class KarambaPython;

class Karamba : public QObject, public QGraphicsItemGroup
{
    Q_OBJECT

public:
    explicit Karamba(const KUrl &themeFile, int instance = -1, bool subTheme = false);

    virtual ~Karamba();

    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);

    QGraphicsScene* getScene() const;
    QGraphicsView* getView() const;

    void keyPressed(const QString& s, const Meter* meter);

    const ThemeFile& theme() const;

    bool hasMeter(const Meter* meter) const;
    bool removeMeter(Meter *meter);
    QString getSensor(const Meter* meter) const;
    void setSensor(const LineParser& lineParser, Meter* meter);
    void deleteMeterFromSensors(Meter* meter);
    TextField* getDefaultTextProps();
    void updateSensors();
    void setFixedSize(u_int w, u_int h);
    bool readMenuConfigOption(const QString &key) const;
    KConfig* getConfig() const;
    //void removePopupMenu(KMenu *menu);
    bool popupMenuExisting(const KMenu *menu) const;
    void setPrettyName(const QString &prettyThemeName);
    QString prettyName() const;
    int getNumberOfDesktops() const;
    double getUpdateTime() const;
    void setUpdateTime(double newTime);
    void setWantRightButton(bool enable);
    void changeInterval(u_int newInterval);
    void addMenuConfigOption(const QString &key, const QString &name);
    bool setMenuConfigOption(const QString &key, bool value);
    KMenu* addPopupMenu();
    QAction* addMenuItem(KMenu *menu, const QString &text,
                         const QString &icon);
    void popupMenu(KMenu *menu, const QPoint &pos) const;
    void deletePopupMenu(KMenu *menu);
    void deleteMenuItem(QAction *action);
    void scaleImageLabel(Meter *meter, int width,
                         int height) const;
    void moveMeter(Meter *meter, int x, int y) const;
    void popupGlobalMenu() const;

    void writeConfigData();

    void setMenuExtension(KMenu*);
    void removeMenuExtension();

    int instance();
    void setInstance(int instance);

    void moveToPos(QPoint pos);
    void resizeTo(int width, int height);
    QPoint getPosition() const;

    void setIncomingData(const QString &data);
    void notifyTheme(const QString &sender, const QString &data);
    bool sendDataToTheme(const QString &prettyThemeName, const QString &data) const;
    QString retrieveReceivedData() const;
    bool sendData(const QString &prettyThemeName, const QString &data) const;

    void setOnTop(bool stayOnTop);
    bool isSubTheme() const;

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
    void startKaramba();
    void slotToggleLocked();
    void currentDesktopChanged(int i);
    void slotToggleConfigOption(QObject*);
    void slotDesktopChanged(int desktop);
    void currentWallpaperChanged(int desktop);

    void step();

Q_SIGNALS:
    void widgetStarted(Karamba*, bool);
    void widgetClosed(Karamba*);

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

    bool m_useKross;
    KarambaPython *m_python;
    KarambaInterface *m_interface;

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

    QList<Sensor*> m_sensorList;
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

    QList<KMenu*> m_menuList;

    QString m_prettyName;

    QString m_storedData;

    double m_updateTime;

    bool m_wantRightButton;

    QPoint m_mouseClickPos;

    bool m_globalView;

    bool m_subTheme;

    OrgKdeKdesktopBackgroundInterface* m_backgroundInterface;

    bool parseConfig();
    Sensor *findSensorFromList(const Meter *meter) const;
    QString findSensorFromMap(const Sensor *sensor) const;

    void preparePopupMenu();
};

#endif // KARAMBA_H
