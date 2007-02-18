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
#include "meters/bar.h"
#include "meters/graph.h"
#include "meters/imagelabel.h"

class Karamba;
class ImageLabel;

class KarambaInterface : public QObject
{
  Q_OBJECT

  public:
    KarambaInterface(Karamba *k);
    virtual ~KarambaInterface();

    // Calls to scripts ----------------------
    void callInitWidget(Karamba *k);
    void callWidgetUpdated(Karamba *k);
    void callMeterClicked(Karamba *k, Meter *m, int button);
    void callMeterClicked(Karamba *k, QString str, int button);

  Q_SIGNALS:
    void initWidget(QObject*);
    void widgetUpdated(QObject*);

  private:
    Karamba *m_karamba;

    Kross::Action *m_action;

    bool checkKaramba(Karamba *k);
    bool checkMeter(Karamba *k, Meter *m, QString type);
    bool checkKarambaAndMeter(Karamba *k, Meter *m, QString type);
    
    QVariantList getMeterMinMax(Karamba *k, Meter *m, QString type);
    QVariantList getMeterSize(Karamba *k, Meter *m, QString type);
    QVariantList getMeterPos(Karamba *k, Meter *m, QString type);
    QString getMeterSensor(Karamba *k, Meter *m, QString type);
    int getMeterValue(Karamba *k, Meter *m, QString type);
    QObject* getThemeMeter(Karamba *k, QString meter, QString type);
    bool hideMeter(Karamba *k, Meter *m, QString type);
    bool moveMeter(Karamba *k, Meter *m, QString type, int x, int y);
    bool resizeMeter(Karamba *k, Meter *m, QString type, int width, int height);
    bool setMeterMinMax(Karamba *k, Meter *m, QString type, int min, int max);
    bool setMeterSensor(Karamba *k, Meter *m, QString type, QString sensor);
    QObject* setMeterValue(Karamba *k, Meter *m, QString type, int value);
    bool showMeter(Karamba *k, Meter *m, QString type);
    bool setMeterColor(Karamba *k, Meter *m, QString type, int red, int green, int blue);
    QVariantList getMeterColor(Karamba *k, Meter *m, QString type);
    QString getMeterStringValue(Karamba *k, Meter *m, QString type);
    QObject* setMeterStringValue(Karamba *k, Meter *m, QString type, QString value);



  public Q_SLOTS:
    // Bar
    QObject* createBar(Karamba *k, int x, int y, int w, int h, QString path = "");
    bool deleteBar(Karamba *k, Bar *bar);
    bool setBarMinMax(Karamba *k, Bar *bar, int min, int max);  
    QVariantList getBarMinMax(Karamba *k, Bar *bar);
    bool moveBar(Karamba *k, Bar *bar, int x, int y);
    QVariantList getBarPos(Karamba *k, Bar *bar);
    bool setBarSensor(Karamba *k, Bar *bar, QString sensor);
    QString getBarSensor(Karamba *k, Bar *bar);
    bool resizeBar(Karamba *k, Bar *bar, int width, int height);
    QVariantList getBarSize(Karamba *k, Bar *bar);
    QObject* setBarValue(Karamba *k, Bar *bar, int value);
    int getBarValue(Karamba *k, Bar *bar);
    QObject* getThemeBar(Karamba *k, QString meter);
    bool hideBar(Karamba *k, Bar *bar);
    bool showBar(Karamba *k, Bar *bar);
    //---------------------------------------------------------------
    bool setBarVertical(Karamba *k, Bar *bar, bool vert);
    bool getBarVertical(Karamba *k, Bar *bar);
    bool setBarImage(Karamba *k, Bar *bar, QString image);
    QString getBarImage(Karamba *k, Bar *bar);
/*
    // Config
    long addMenuConfigOption
    long readConfigEntry
    long readMenuConfigOption
    long setMenuConfigOption
    long writeConfigEntry
*/
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
/*
    // InputBox
    long changeInputBox
    long changeInputBoxBackgroundColor
    long changeInputBoxFont
    long changeInputBoxFontColor
    long changeInputBoxFontSize
    long changeInputBoxFrameColor
    long changeInputBoxSelectedTextColor
    long changeInputBoxSelectionColor
    long clearInputFocus
    long createInputBox
    long deleteInputBox
    long getInputBoxBackgroundColor
    long getInputBoxFont
    long getInputBoxFontColor
    long getInputBoxFontSize
    long getInputBoxFrameColor
    long getInputBoxPos
    long getInputBoxSelectedTextColor
    long getInputBoxSelectionColor
    long getInputBoxSize
    long getInputBoxValue
    long getInputFocus
    long getThemeInputBox
    long hideInputBox
    long moveInputBox
    long resizeInputBox
    long setInputFocus
    long showInputBox

    // Menu
    long addMenuItem
    long addMenuSeparator
    long createMenu
    long deleteMenu
    long popupMenu
    long removeMenuItem

    // Misc
    long acceptDrops
    long attachClickArea
    long callTheme
    long changeInterval
    long createClickArea
    long createServiceClickArea
    long execute
    long executeInteractive
    long getIncomingData
    long getIp
    long getNumberOfDesktop
    long getPrettyThemeName
    long getServiceGroups
    long getThemePath
    long getUpdateTime
    long hide
    long language
    long managementPopup
    long openNamedTheme
    long openTheme
    long readThemeFile
    long reloadTheme
    long removeClickArea
    long run
    long setIncomingData
    long setUpdateTime
    long show
    long toggleShowDesktop
    long translateAll
    long userLanguage
    long wantRightButton

    // RichText
    long changeRichText
    long changeRichTextFont
    long changeRichTextSize
    long createRichText
    long deleteRichText
    long getRichTextFont
    long getRichTextFontSize
    long getRichTextPos
    long getRichTextSensor
    long getRichTextSize
    long getRichTextValue
    long getThemeRichText
    long hideRichText
    long moveRichText
    long resizeRichText
    long setRichTextSensor
    long setRichTextWidth
    long showRichText

    // Systray
    long createSystray
    long getCurrentWindowCount
    long hideSystray
    long moveSystray
    long showSystray
    long updateSystrayLayout
    long getStartupInfo
    long getStartupList
    long getTaskInfo
    long getTaskList
    long getTaskNames
    long performTaskAction

    // Text
    long changeText
    long changeTextColor
    long changeTextFont
    long changeTextShadow
    long changeTextSize
    long createText
    long deleteText
    long getTextAlign
    long getTextColor
    long getTextFont
    long getTextFontSize
    long getTextPos
    long getTextSensor
    long getTextShadow
    long getTextSize
    long getTextValue
    long getThemeText
    long hideText
    long moveText
    long resizeText
    long setTextAlign
    long setTextSensor
    long showText

    // Widget
    long createWidgetMask
    long getWidgetPosition
    long moveWidget
    long redrawWidget
    long redrawWidgetBackground
    long resizeWidget
    long toggleWidgetRedraw
    */
};

#endif
