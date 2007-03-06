/*
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

#ifndef KARAMBAINTERFACE_H_
#define KARAMBAINTERFACE_H_

#include <QObject>

#include <kross/core/manager.h>
#include <kross/core/guiclient.h>
#include <kross/core/action.h>

#include "karamba.h"
#include "themelocale.h"

#include "meters/bar.h"
#include "meters/graph.h"
#include "meters/imagelabel.h"
#include "meters/input.h"
#include "meters/richtextlabel.h"
#include "meters/textlabel.h"
#include "meters/clickarea.h"

class Karamba;
class ImageLabel;
class Input;
class RichTextLabel;
class TextLabel;
class ClickArea;
class ThemeLocale;

class KarambaInterface : public QObject
{
    Q_OBJECT

public:
    KarambaInterface(Karamba *k);
    virtual ~KarambaInterface();

    // Calls to scripts ----------------------
    void callInitWidget(Karamba *k);
    void callWidgetUpdated(Karamba *k);
    void callWidgetClosed(Karamba *k);
    void callMenuOptionChanged(Karamba *k, QString key, bool value);
    void callMenuItemClicked(Karamba* k, KMenu* menu, QAction* id);
    void callActiveTaskChanged(Karamba *k, Task* t);
    void callTaskAdded(Karamba *k, Task *t);
    void callTaskRemoved(Karamba *k, Task *t);
    void callStartupAdded(Karamba *k, Startup *t);
    void callStartupRemoved(Karamba *k, Startup *t);
    void callCommandFinished(Karamba *k, int pid);
    void callCommandOutput(Karamba *k, int pid, char* buffer);
    void callItemDropped(Karamba *k, QString text, int x, int y);
    void callMeterClicked(Karamba *k, Meter *m, int button);
    void callMeterClicked(Karamba *k, QString str, int button);
    void callWidgetClicked(Karamba *k, int x, int y, int button);
    void callDesktopChanged(Karamba *k, int desktop);
    void callWidgetMouseMoved(Karamba *k, int x, int y, int button);
    void callKeyPressed(Karamba *k, Meter *meter, QString key);

Q_SIGNALS:
    void initWidget(QObject*);
    void widgetUpdated(QObject*);
    void widgetClosed(QObject*);
    void menuItemClicked(QObject*, QObject*, QObject*);
    void menuOptionChanged(QObject*, QString, bool);
    void activeTaskChanged(QObject*, long long);
    void taskAdded(QObject*, long long);
    void taskRemoved(QObject*, long long);
    void startupAdded(QObject*, long long);
    void startupRemoved(QObject*, long long);
    void commandFinished(QObject*, int);
    void commandOutput(QObject*, int, QString);
    void itemDropped(QObject*, QString, int, int);
    void meterClicked(QObject*, QObject*, int);
    void meterClicked(QObject*, QString, int);
    void widgetClicked(QObject*, int, int, int);
    void desktopChanged(QObject*, int);
    void widgetMouseMoved(QObject*, int, int, int);
    void keyPressed(QObject*, QObject*, QString);

private:
    /**
    * This is the in the constructor passed \a Karamba instance
    * and used e.g. at the \a getThemePath method to be able
    * to fetch the theme-path even outside of the functions if
    * there is no access to the Karamba-pointer.
    */
    Karamba *m_karamba;

    /**
    * The \a Kross::Action instance that provides us access to
    * the scripting backends.
    */
    Kross::Action *m_action;

    bool checkKaramba(const Karamba *k) const;
    bool checkMeter(const Karamba *k, const Meter *m, const QString &type) const;
    bool checkKarambaAndMeter(const Karamba *k, const Meter *m, const QString &type) const;

    QVariantList getMeterMinMax(const Karamba *k, const Meter *m, const QString &type) const;
    QVariantList getMeterSize(const Karamba *k, const Meter *m, const QString &type) const;
    QVariantList getMeterPos(const Karamba *k, const Meter *m, const QString &type) const;
    QString getMeterSensor(const Karamba *k, const Meter *m, const QString &type) const;
    int getMeterValue(const Karamba *k, const Meter *m, const QString &type) const;
    QObject* getThemeMeter(const Karamba *k, const QString &meter, const QString &type) const;
    bool hideMeter(const Karamba *k, Meter *m, const QString &type) const;
    bool moveMeter(const Karamba *k, Meter *m, const QString &type, int x, int y) const;
    bool resizeMeter(const Karamba *k, Meter *m, const QString &type, int width, int height) const;
    bool setMeterMinMax(const Karamba *k, Meter *m, const QString &type, int min, int max) const;
    bool setMeterSensor(Karamba *k, Meter *m, const QString &type, const QString &sensor) const;
    QObject* setMeterValue(const Karamba *k, Meter *m, const QString &type, int value) const;
    bool showMeter(const Karamba *k, Meter *m, const QString &type) const;
    bool setMeterColor(Karamba *k, Meter *m, QString type, int red, int green, int blue);
    QVariantList getMeterColor(Karamba *k, Meter *m, QString type);
    QString getMeterStringValue(Karamba *k, Meter *m, QString type);
    QObject* setMeterStringValue(Karamba *k, Meter *m, QString type, QString value);
    bool menuExists(Karamba* currTheme, KMenu* menu);



public Q_SLOTS:
    // Bar
    QObject* createBar(Karamba *k, int x, int y, int w, int h, const QString &path = "") const;
    bool deleteBar(Karamba *k, Bar *bar) const;
    bool setBarMinMax(const Karamba *k, Bar *bar, int min, int max) const;
    QVariantList getBarMinMax(const Karamba *k, const Bar *bar) const;
    bool moveBar(const Karamba *k, Bar *bar, int x, int y) const;
    QVariantList getBarPos(const Karamba *k, const Bar *bar) const;
    bool setBarSensor(Karamba *k, Bar *bar, const QString &sensor) const;
    QString getBarSensor(const Karamba *k, const Bar *bar) const;
    bool resizeBar(const Karamba *k, Bar *bar, int width, int height) const;
    QVariantList getBarSize(const Karamba *k, const Bar *bar) const;
    QObject* setBarValue(const Karamba *k, Bar *bar, int value) const;
    int getBarValue(const Karamba *k, const Bar *bar) const;
    QObject* getThemeBar(const Karamba *k, const QString &meter) const;
    bool hideBar(const Karamba *k, Bar *bar) const;
    bool showBar(const Karamba *k, Bar *bar) const;
    bool setBarVertical(const Karamba *k, Bar *bar, bool vert) const;
    bool getBarVertical(const Karamba *k, const Bar *bar) const;
    bool setBarImage(const Karamba *k, Bar *bar, const QString &image) const;
    QString getBarImage(const Karamba *k, const Bar *bar) const;


    // Config
    bool addMenuConfigOption(Karamba *k, QString key, QString name);
    QString readConfigEntry(Karamba *k, QString key);
    bool readMenuConfigOption(Karamba *k, QString key);
    bool setMenuConfigOption(Karamba *k, QString key, bool value);
    bool writeConfigEntry(Karamba *k, QString key, QString value);

    // Graph
    QObject* createGraph(Karamba* k, int x, int y, int w, int h, int points);
    bool deleteGraph(Karamba *k, Graph *graph);
    bool setGraphMinMax(Karamba *k, Graph *bar, int min, int max);
    QVariantList getGraphMinMax(Karamba *k, Graph *graph);
    bool moveGraph(Karamba *k, Graph *graph, int x, int y);
    QVariantList getGraphPos(Karamba *k, Graph *graph);
    bool setGraphSensor(Karamba *k, Graph *graph, QString sensor);
    QString getGraphSensor(Karamba *k, Graph *graph);
    bool resizeGraph(Karamba *k, Graph *graph, int width, int height);
    QVariantList getGraphSize(Karamba *k, Graph *graph);
    QObject* setGraphValue(Karamba *k, Graph *graph, int value);
    int getGraphValue(Karamba *k, Graph *graph);
    QObject* getThemeGraph(Karamba *k, QString meter);
    bool hideGraph(Karamba *k, Graph *graph);
    bool showGraph(Karamba *k, Graph *graph);
    bool setGraphColor(Karamba *k, Graph *graph, int red, int green, int blue);
    QVariantList getGraphColor(Karamba *k, Graph *graph);

    // ImageLabel
    QObject* createImage(Karamba* k, int x, int y, QString image);
    bool deleteImage(Karamba *k, ImageLabel *image);
    bool setImageMinMax(Karamba *k, ImageLabel *image, int min, int max);
    QVariantList getImageMinMax(Karamba *k, ImageLabel *image);
    bool moveImage(Karamba *k, ImageLabel *image, int x, int y);
    QVariantList getImagePos(Karamba *k, ImageLabel *image);
    bool setImageSensor(Karamba *k, ImageLabel *image, QString sensor);
    QString getImageSensor(Karamba *k, ImageLabel *image);
    bool resizeImage(Karamba *k, ImageLabel *image, int width, int height);
    QVariantList getImageSize(Karamba *k, ImageLabel *image);
    QObject* setImagePath(Karamba *k, ImageLabel *image, QString path);
    QString getImagePath(Karamba *k, ImageLabel *image);
    QObject* getThemeImage(Karamba *k, QString meter);
    bool hideImage(Karamba *k, ImageLabel *image);
    bool showImage(Karamba *k, ImageLabel *image);
    bool setImageColor(Karamba *k, ImageLabel *image, int red, int green, int blue);
    QVariantList getImageColor(Karamba *k, ImageLabel *image);
    bool addImageTooltip(Karamba *k, ImageLabel *image, QString text);
    bool changeImageChannelIntensity(Karamba *k, ImageLabel *image, double ratio, QString channel, int ms = 0);
    bool changeImageIntensity(Karamba *k, ImageLabel *image, double ratio, int ms = 0);
    bool changeImageToGray(Karamba *k, ImageLabel *image, int ms);
    QObject* createBackgroundImage(Karamba *k, int x, int y, QString imagePath);
    QObject* createTaskIcon(Karamba *k, int x, int y, int ctask);
    int getImageHeight(Karamba *k, ImageLabel *image);
    int getImageWidth(Karamba *k, ImageLabel *image);
    bool removeImageEffects(Karamba *k, ImageLabel *image);
    bool removeImageTransformations(Karamba *k, ImageLabel *image);
    bool resizeImageSmooth(Karamba *k, ImageLabel *image, int width, int height);
    bool rotateImage(Karamba *k, ImageLabel *image, int deg);

    // InputBox
    QObject* createInputBox(Karamba* k, int x, int y, int w, int h, QString text);
    bool deleteInputBox(Karamba *k, Input *input);
    bool moveInputBox(Karamba *k, Input *input, int x, int y);
    QVariantList getInputBoxPos(Karamba *k, Input *input);
    bool resizeInputBox(Karamba *k, Input *input, int width, int height);
    QVariantList getInputBoxSize(Karamba *k, Input *input);
    QObject* changeInputBoxValue(Karamba *k, Input *input, QString text);
    QString getInputBoxValue(Karamba *k, Input *input);
    QObject* getThemeInputBox(Karamba *k, QString meter);
    bool hideInputBox(Karamba *k, Input *input);
    bool showInputBox(Karamba *k, Input *input);
    bool changeInputBoxFont(Karamba *k, Input *input, QString font);
    bool changeInputBoxFontColor(Karamba *k, Input *input, int red, int green, int blue);
    QString getInputBoxFont(Karamba *k, Input *input);
    QVariantList getInputBoxFontColor(Karamba *k, Input *input);
    bool changeInputBoxSelectionColor(Karamba *k, Input *input, int red, int green, int blue);
    QVariantList getInputBoxSelectionColor(Karamba *k, Input *input);
    bool changeInputBoxBackgroundColor(Karamba *k, Input *input, int red, int green, int blue);
    QVariantList getInputBoxBackgroundColor(Karamba *k, Input *input);
    bool changeInputBoxFrameColor(Karamba *k, Input *input, int red, int green, int blue);
    QVariantList getInputBoxFrameColor(Karamba *k, Input *input);
    bool changeInputBoxSelectedTextColor(Karamba *k, Input *input, int red, int green, int blue);
    QVariantList getInputBoxSelectedTextColor(Karamba *k, Input *input);
    bool changeInputBoxFontSize(Karamba *k, Input *input, int size);
    int getInputBoxFontSize(Karamba *k, Input *input);
    bool setInputFocus(Karamba *k, Input *input);
    bool clearInputFocus(Karamba *k, Input *input);
    QObject* getInputFocus(Karamba *k);

    // Menu
    QObject* addMenuItem(Karamba *k, KMenu *menu, QString text, QString icon);
    QObject* addMenuSeparator(Karamba *k, KMenu *menu);
    QObject* createMenu(Karamba *k);
    bool deleteMenu(Karamba *k, KMenu *menu);
    bool popupMenu(Karamba *k, KMenu *menu, int x, int y);
    bool removeMenuItem(Karamba *k, KMenu *menu, QAction *action);

    // Misc
    bool acceptDrops(Karamba *k) const;
    bool attachClickArea(const Karamba *k, Meter *m, const QString &leftButton = QString(),
                         const QString &middleButton = QString(), const QString &rightButton =
                         QString()) const;
    bool callTheme(const Karamba *k, const QString &theme, const QString &info) const;
    bool changeInterval(Karamba *k, int interval) const;
    int execute(const QString &command) const;
    QObject* createClickArea(Karamba *k, int x, int y, int width, int height,
            const QString &onClick) const;
    QObject* createServiceClickArea(Karamba *k, int x, int y, int width, int height,
        const QString &name, const QString &exec, const QString &icon) const;
    int executeInteractive(Karamba *k, const QStringList &command);
    QString getIP(const Karamba *k, QString interface) const;
    int getNumberOfDesktops(const Karamba *k) const;
    QString getPrettyThemeName(const Karamba *k) const;
    QStringList getServiceGroups(const Karamba *k, QString path) const;
    QString getThemePath(const Karamba *k = 0) const;
    double getUpdateTime(const Karamba *k) const;
    bool setUpdateTime(Karamba *k, double updateTime) const;
    bool hide(Karamba *k) const;
    bool show(Karamba *k) const;
    QString language(const Karamba *k) const;
    bool managementPopup(const Karamba *k) const;
    Karamba* openNamedTheme(const QString &themePath, const QString &themeName,
        bool startAsSubTheme) const;
    Karamba* openTheme(const QString &themePath) const;
    QString readThemeFile(const Karamba *k, const QString &file) const;
    bool reloadTheme(Karamba *k) const;
    bool removeClickArea(Karamba *k, ClickArea *area) const;
    bool run(const QString &appName, const QString &command, const QString &icon, const QStringList
            &arguments);
    QVariant getIncommingData(const Karamba *k) const;
    bool setIncommingData(const Karamba *k, const QString &themePath, QVariant data) const;
    bool toggleShowDesktop(const Karamba *k) const;
    bool translateAll(const Karamba *k, int x, int y) const;
    QString userLanguage(const Karamba *k) const;
    QStringList userLanguages(const Karamba *k) const;
    bool wantRightButton(Karamba *k, bool enable) const;

    // RichText
    QObject* createRichText(Karamba* k, QString text, bool underline = false);
    bool deleteRichText(Karamba *k, RichTextLabel *label);
    bool moveRichText(Karamba *k, RichTextLabel *label, int x, int y);
    QVariantList getRichTextPos(Karamba *k, RichTextLabel *label);
    bool resizeRichText(Karamba *k, RichTextLabel *label, int width, int height);
    bool setRichTextWidth(Karamba *k, RichTextLabel *label, int width);
    QVariantList getRichTextSize(Karamba *k, RichTextLabel *label);
    QObject* changeRichText(Karamba *k, RichTextLabel *label, QString text);
    QString getRichTextValue(Karamba *k, RichTextLabel *label);
    QObject* getThemeRichText(Karamba *k, QString meter);
    bool hideRichText(Karamba *k, RichTextLabel *label);
    bool showRichText(Karamba *k, RichTextLabel *label);
    bool changeRichTextFont(Karamba *k, RichTextLabel *label, QString font);
    QString getRichTextFont(Karamba *k, RichTextLabel *label);
    bool changeRichTextSize(Karamba *k, RichTextLabel *label, int size);
    int getRichTextFontSize(Karamba *k, RichTextLabel *label);
    bool setRichTextSensor(Karamba *k, RichTextLabel *label, QString sensor);
    QString getRichTextSensor(Karamba *k, RichTextLabel *label);

    /*
        // Systray
        long createSystray
        long getCurrentWindowCount
        long hideSystray
        long moveSystray
        long showSystray
        long updateSystrayLayout

        // Task
        long getStartupInfo
        long getStartupList
        long getTaskInfo
        long getTaskList
        long getTaskNames
        long performTaskAction
    */
    // Text
    QObject* createText(Karamba* k, int x, int y, int width, int height, QString text);
    bool deleteText(Karamba *k, TextLabel *text);
    bool moveText(Karamba *k, TextLabel *text, int x, int y);
    QVariantList getTextPos(Karamba *k, TextLabel *text);
    bool setTextSensor(Karamba *k, TextLabel *text, QString sensor);
    QString getTextSensor(Karamba *k, TextLabel *text);
    bool resizeText(Karamba *k, TextLabel *text, int width, int height);
    QVariantList getTextSize(Karamba *k, TextLabel *text);
    QObject* getThemeText(Karamba *k, QString meter);
    bool hideText(Karamba *k, TextLabel *text);
    bool showText(Karamba *k, TextLabel *text);
    QObject* changeText(Karamba *k, TextLabel *label, QString text);
    QString getTextValue(Karamba *k, TextLabel *label);
    bool changeTextShadow(Karamba *k, TextLabel *label, int shadow);
    int getTextShadow(Karamba *k, TextLabel *text);
    bool changeTextFont(Karamba *k, TextLabel *text, QString font);
    QString getTextFont(Karamba *k, TextLabel *text);
    bool changeTextColor(Karamba *k, TextLabel *text, int red, int green, int blue);
    QVariantList getTextColor(Karamba *k, TextLabel *text);
    bool changeTextSize(Karamba *k, TextLabel *text, int size);
    int getTextFontSize(Karamba *k, TextLabel *text);
    QString getTextAlign(Karamba *k, TextLabel *text);
    bool setTextAlign(Karamba *k, TextLabel *text, QString alignment);
    bool setTextScroll(Karamba *k, TextLabel *text, QString type, int x = 0, int y = 0, int gap = 0, int pause = 0);

    // Widget
    bool createWidgetMask(Karamba *k, QString mask);
    QVariantList getWidgetPosition(Karamba *k);
    bool moveWidget(Karamba *k, int x, int y);
    bool redrawWidget(Karamba *k);
    bool redrawWidgetBackground(Karamba *k);
    bool resizeWidget(Karamba *k, int width, int height);
    bool toggleWidgetRedraw(Karamba *k, bool enable);
};

#endif
