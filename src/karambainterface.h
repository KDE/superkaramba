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
    /// \internal d-pointer class.
    class Private;
    /// \internal d-pointer instance.
    Private* const d;

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
    bool setMeterColor(const Karamba *k, Meter *m, const QString &type, int red, int green, int
            blue) const;
    QVariantList getMeterColor(const Karamba *k, const Meter *m, const QString &type) const;
    QString getMeterStringValue(const Karamba *k, const Meter *m, const QString &type) const;
    QObject* setMeterStringValue(const Karamba *k, Meter *m, const QString &type, const QString
            &value) const;
    bool menuExists(const Karamba* currTheme, const KMenu* menu) const;



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
    bool addMenuConfigOption(Karamba *k, const QString &key, const QString &name) const;
    QString readConfigEntry(const Karamba *k, const QString &key) const;
    bool readMenuConfigOption(const Karamba *k, const QString &key) const;
    bool setMenuConfigOption(Karamba *k, const QString &key, bool value) const;
    bool writeConfigEntry(const Karamba *k, const QString &key, const QString &value) const;

    // Graph
    QObject* createGraph(Karamba* k, int x, int y, int w, int h, int points) const;
    bool deleteGraph(Karamba *k, Graph *graph) const;
    bool setGraphMinMax(const Karamba *k, Graph *bar, int min, int max) const;
    QVariantList getGraphMinMax(const Karamba *k, const Graph *graph) const;
    bool moveGraph(const Karamba *k, Graph *graph, int x, int y) const;
    QVariantList getGraphPos(const Karamba *k, const Graph *graph) const;
    bool setGraphSensor(Karamba *k, Graph *graph, const QString &sensor) const;
    QString getGraphSensor(const Karamba *k, const Graph *graph) const;
    bool resizeGraph(const Karamba *k, Graph *graph, int width, int height) const;
    QVariantList getGraphSize(const Karamba *k, const Graph *graph) const;
    QObject* setGraphValue(const Karamba *k, Graph *graph, int value) const;
    int getGraphValue(const Karamba *k, const Graph *graph) const;
    QObject* getThemeGraph(const Karamba *k, const QString &meter) const;
    bool hideGraph(const Karamba *k, Graph *graph) const;
    bool showGraph(const Karamba *k, Graph *graph) const;
    bool setGraphColor(const Karamba *k, Graph *graph, int red, int green, int blue) const;
    QVariantList getGraphColor(const Karamba *k, const Graph *graph) const;

    // ImageLabel
    QObject* createImage(Karamba* k, int x, int y, const QString &image) const;
    bool deleteImage(Karamba *k, ImageLabel *image) const;
    bool moveImage(Karamba *k, ImageLabel *image, int x, int y) const;
    QVariantList getImagePos(const Karamba *k, const ImageLabel *image) const;
    bool setImageSensor(Karamba *k, ImageLabel *image, const QString &sensor) const;
    QString getImageSensor(const Karamba *k, const ImageLabel *image) const;
    bool resizeImage(const Karamba *k, ImageLabel *image, int width, int height) const;
    QVariantList getImageSize(const Karamba *k, const ImageLabel *image) const;
    QObject* setImagePath(const Karamba *k, ImageLabel *image, const QString &path) const;
    QString getImagePath(const Karamba *k, const ImageLabel *image) const;
    QObject* getThemeImage(const Karamba *k, const QString &meter) const;
    bool hideImage(const Karamba *k, ImageLabel *image) const;
    bool showImage(const Karamba *k, ImageLabel *image) const;
    bool addImageTooltip(const Karamba *k, ImageLabel *image, const QString &text) const;
    bool changeImageChannelIntensity(const Karamba *k, ImageLabel *image, double ratio, const
            QString &channel, int ms = 0) const;
    bool changeImageIntensity(const Karamba *k, ImageLabel *image, double ratio, int ms = 0) const;
    bool changeImageToGray(const Karamba *k, ImageLabel *image, int ms = 0) const;
    QObject* createBackgroundImage(Karamba *k, int x, int y, const QString &imagePath) const;
    QObject* createTaskIcon(Karamba *k, int x, int y, int ctask) const;
    int getImageHeight(const Karamba *k, const ImageLabel *image) const;
    int getImageWidth(const Karamba *k, const ImageLabel *image) const;
    bool removeImageEffects(const Karamba *k, ImageLabel *image) const;
    bool removeImageTransformations(const Karamba *k, ImageLabel *image) const;
    bool resizeImageSmooth(Karamba *k, ImageLabel *image, int width, int height) const;
    bool rotateImage(const Karamba *k, ImageLabel *image, int deg) const;

    // InputBox
    QObject* createInputBox(Karamba* k, int x, int y, int w, int h, const QString &text) const;
    bool deleteInputBox(Karamba *k, Input *input) const;
    bool moveInputBox(Karamba *k, Input *input, int x, int y) const;
    QVariantList getInputBoxPos(const Karamba *k, const Input *input) const;
    bool resizeInputBox(const Karamba *k, Input *input, int width, int height) const;
    QVariantList getInputBoxSize(const Karamba *k, const Input *input) const;
    QObject* changeInputBox(const Karamba *k, Input *input, const QString &text) const;
    QString getInputBoxValue(const Karamba *k, const Input *input) const;
    QObject* getThemeInputBox(const Karamba *k, const QString &meter) const;
    bool hideInputBox(const Karamba *k, Input *input) const;
    bool showInputBox(const Karamba *k, Input *input) const;
    bool changeInputBoxFont(const Karamba *k, Input *input, const QString &font) const;
    bool changeInputBoxFontColor(const Karamba *k, Input *input, int red, int green, int blue) const;
    QString getInputBoxFont(const Karamba *k, const Input *input) const;
    QVariantList getInputBoxFontColor(const Karamba *k, const Input *input) const;
    bool changeInputBoxSelectionColor(const Karamba *k, Input *input, int red, int green, int blue)
        const;
    QVariantList getInputBoxSelectionColor(const Karamba *k, const Input *input) const;
    bool changeInputBoxBackgroundColor(const Karamba *k, Input *input, int red, int green, int blue)
        const;
    QVariantList getInputBoxBackgroundColor(const Karamba *k, const Input *input) const;
    bool changeInputBoxFrameColor(const Karamba *k, Input *input, int red, int green, int blue)
        const;
    QVariantList getInputBoxFrameColor(const Karamba *k, const Input *input) const;
    bool changeInputBoxSelectedTextColor(const Karamba *k, Input *input, int red, int green, int
            blue) const;
    QVariantList getInputBoxSelectedTextColor(const Karamba *k, const Input *input) const;
    bool changeInputBoxFontSize(const Karamba *k, Input *input, int size) const;
    int getInputBoxFontSize(const Karamba *k, const Input *input) const;
    bool setInputFocus(const Karamba *k, Input *input) const;
    bool clearInputFocus(const Karamba *k, Input *input) const;
    QObject* getInputFocus(const Karamba *k) const;

    // Menu
    QObject* addMenuItem(Karamba *k, KMenu *menu, const QString &text, const QString &icon)
        const;
    QObject* addMenuSeparator(Karamba *k, KMenu *menu) const;
    QObject* createMenu(Karamba *k) const;
    bool deleteMenu(Karamba *k, KMenu *menu) const;
    bool popupMenu(const Karamba *k, KMenu *menu, int x, int y) const;
    bool removeMenuItem(Karamba *k, KMenu *menu, QAction *action) const;

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
