/****************************************************************************
*  karamba_python.cpp  -  Functions for calling python scripts
*
*  Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>
*  Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>
*  Copyright (c) 2004 Petri Damst� <damu@iki.fi>
*  Copyright (c) 2004 Luke Kenneth Casson Leighton <lkcl@lkcl.net>
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

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#include <Python.h>
#include "karambaapp.h"
#include "themefile.h"

#include "karamba_python.h"
#include "meter_python.h"
#include "bar_python.h"
#include "graph_python.h"
#include "textlabel_python.h"
#include "richtextlabel_python.h"
#include "imagelabel_python.h"
#include "widget_python.h"
#include "menu_python.h"
#include "config_python.h"
#include "task_python.h"
#include "systray_python.h"
#include "svcgrp_python.h"
#include "misc_python.h"
#include "input_python.h"

/*******************************************
 * Python methods are defined here.
 *   Each method accessible from python should have:
 *     - A wrapper function that returns a PyObject or appropriate python type
 *     - A C++ implementation of the python method, named the same as the python call
 *     - An entry in the python methods array so the call is accessible from python
 *
 *   Example:
 *     py_move_systay - wrapper function
 *     moveSystray - actual implementation of method
 *     {"moveSystray", py_move_systray, METH_VARARGS, "Move the Systray"} - array entry
 */

static PyMethodDef karamba_methods[] = {
    // Bar - bar_python.cpp
    {(char*)"createBar", py_createBar, METH_VARARGS, (char*)"Create new Bar."},
    {(char*)"deleteBar", py_deleteBar, METH_VARARGS, (char*)"Delete Bar."},
    {(char*)"getThemeBar", py_getThemeBar, METH_VARARGS, (char*)"Get Bar from .theme using it's name."},
    {(char*)"getBarSize", py_getBarSize, METH_VARARGS, (char*)"Get Bar size."},
    {(char*)"resizeBar", py_resizeBar, METH_VARARGS, (char*)"Resize Bar."},
    {(char*)"getBarPos", py_getBarPos, METH_VARARGS, (char*)"Get Bar position."},
    {(char*)"moveBar", py_moveBar, METH_VARARGS, (char*)"Move Bar."},
    {(char*)"hideBar", py_hideBar, METH_VARARGS, (char*)"Hide Bar."},
    {(char*)"showBar", py_showBar, METH_VARARGS, (char*)"Show Bar."},
    {(char*)"getBarSensor", py_getBarSensor, METH_VARARGS, (char*)"Get Bar sensor."},
    {(char*)"setBarSensor", py_setBarSensor, METH_VARARGS, (char*)"Set Bar sensor."},
    {(char*)"setBarImage", py_setBarImage, METH_VARARGS, (char*)"Set bar image"},
    {(char*)"getBarImage", py_getBarImage, METH_VARARGS, (char*)"Get bar image"},
    {(char*)"setBarVertical", py_setBarVertical, METH_VARARGS, (char*)"Set bar orientation"},
    {(char*)"getBarVertical", py_getBarVertical, METH_VARARGS, (char*)"Get bar orientation"},
    {(char*)"setBarValue", py_setBarValue, METH_VARARGS, (char*)"Set bar value"},
    {(char*)"getBarValue", py_getBarValue, METH_VARARGS, (char*)"Get bar value"},
    {(char*)"setBarMinMax", py_setBarMinMax, METH_VARARGS, (char*)"Set bar min & max"},
    {(char*)"getBarMinMax", py_getBarMinMax, METH_VARARGS, (char*)"Get bar min & max"},
    {(char*)"getIncomingData", py_get_incoming_data, METH_VARARGS, (char*)"Get incoming data passed from another theme"},
    {(char*)"setIncomingData", py_set_incoming_data, METH_VARARGS, (char*)"Set incoming data passed in another theme"},

    // Graph - graph_python.cpp
    {(char*)"createGraph", py_createGraph, METH_VARARGS, (char*)"Create new Graph."},
    {(char*)"deleteGraph", py_deleteGraph, METH_VARARGS, (char*)"Delete Graph."},
    {(char*)"getThemeGraph", py_getThemeGraph, METH_VARARGS, (char*)"Get Graph from .theme using it's name."},
    {(char*)"getGraphSize", py_getGraphSize, METH_VARARGS, (char*)"Get Graph size."},
    {(char*)"resizeGraph", py_resizeGraph, METH_VARARGS, (char*)"Resize Graph."},
    {(char*)"getGraphPos", py_getGraphPos, METH_VARARGS, (char*)"Get Graph position."},
    {(char*)"moveGraph", py_moveGraph, METH_VARARGS, (char*)"Move Graph."},
    {(char*)"hideGraph", py_hideGraph, METH_VARARGS, (char*)"Hide Graph."},
    {(char*)"showGraph", py_showGraph, METH_VARARGS, (char*)"Show Graph."},
    {(char*)"getGraphSensor", py_getGraphSensor, METH_VARARGS, (char*)"Get Graph sensor."},
    {(char*)"setGraphSensor", py_setGraphSensor, METH_VARARGS, (char*)"Set Graph sensor."},
    {(char*)"setGraphValue", py_setGraphValue, METH_VARARGS, (char*)"Set graph value"},
    {(char*)"getGraphValue", py_getGraphValue, METH_VARARGS, (char*)"Get graph value"},
    {(char*)"setGraphMinMax", py_setGraphMinMax, METH_VARARGS, (char*)"Set graph min & max"},
    {(char*)"getGraphMinMax", py_getGraphMinMax, METH_VARARGS, (char*)"Get graph min & max"},
    {(char*)"setGraphColor", py_setGraphColor, METH_VARARGS, (char*)"Change a Graph Sensor's Color"},
    {(char*)"getGraphColor", py_getGraphColor, METH_VARARGS, (char*)"Get a Graph Sensor's Color"},

    // TextLabel - textlabel_python.cpp
    {(char*)"createText", py_createText, METH_VARARGS, (char*)"Create new Text."},
    {(char*)"deleteText", py_deleteText, METH_VARARGS, (char*)"Delete Text."},
    {(char*)"getThemeText", py_getThemeText, METH_VARARGS, (char*)"Get Text from .theme using it's name."},
    {(char*)"getTextSize", py_getTextSize, METH_VARARGS, (char*)"Get Text size."},
    {(char*)"resizeText", py_resizeText, METH_VARARGS, (char*)"Resize Text."},
    {(char*)"getTextPos", py_getTextPos, METH_VARARGS, (char*)"Get Text position."},
    {(char*)"moveText", py_moveText, METH_VARARGS, (char*)"Move Text."},
    {(char*)"hideText", py_hideText, METH_VARARGS, (char*)"Hide Text."},
    {(char*)"showText", py_showText, METH_VARARGS, (char*)"Show Text."},
    {(char*)"getTextSensor", py_getTextSensor, METH_VARARGS, (char*)"Get Text sensor."},
    {(char*)"setTextSensor", py_setTextSensor, METH_VARARGS, (char*)"Set Text sensor."},
    {(char*)"changeText", py_setTextValue, METH_VARARGS, (char*)"Change a Text Sensor's Text"},
    {(char*)"getTextValue", py_getTextValue, METH_VARARGS, (char*)"Get Text value"},
    {(char*)"changeTextShadow", py_setTextShadow, METH_VARARGS, (char*)"Change a Text Shadow size"},
    {(char*)"getTextShadow", py_getTextShadow, METH_VARARGS, (char*)"Get a Text Shadow size"},
    {(char*)"changeTextFont", py_setTextFont, METH_VARARGS, (char*)"Change a Text Sensor's Font"},
    {(char*)"getTextFont", py_getTextFont, METH_VARARGS, (char*)"Get a Text Sensor's Font"},
    {(char*)"changeTextColor", py_setTextColor, METH_VARARGS, (char*)"Change a Text Sensor's Color"},
    {(char*)"getTextColor", py_getTextColor, METH_VARARGS, (char*)"Get a Text Sensor's Color"},
    {(char*)"changeTextSize", py_setTextFontSize, METH_VARARGS, (char*)"Change a Text Sensor's Font Size"},
    {(char*)"getTextFontSize", py_getTextFontSize, METH_VARARGS, (char*)"Get a Text Sensor's Font Size"},
    {(char*)"getTextAlign", py_getTextAlign, METH_VARARGS, (char*)"Get Text alignment."},
    {(char*)"setTextAlign", py_setTextAlign, METH_VARARGS, (char*)"Set Text alignment."},
    {(char*)"setTextScroll", py_setTextScroll, METH_VARARGS, (char*)"Set Text scroll."},

    // RichTextLabel - richtextlabel_python.cpp
    {(char*)"createRichText", py_createRichText, METH_VARARGS, (char*)"Create a Rich Text Sensor"},
    {(char*)"deleteRichText", py_deleteRichText, METH_VARARGS, (char*)"Deletes a Rich Text Sensor"},
    {(char*)"getThemeRichText", py_getThemeRichText, METH_VARARGS, (char*)"Get Rich Text from .theme using it's name."},
    {(char*)"getRichTextSize", py_getRichTextSize, METH_VARARGS, (char*)"Get the (width, height) of a Rich Text Sensor"},
    {(char*)"resizeRichText", py_resizeRichText, METH_VARARGS, (char*)"Resize Rich Text."},
    {(char*)"setRichTextWidth", py_set_rich_text_width, METH_VARARGS, (char*)"Sets the width of a Rich Text Sensor"},
    {(char*)"getRichTextPos", py_getRichTextPos, METH_VARARGS, (char*)"Get Rich Text position."},
    {(char*)"moveRichText", py_moveRichText, METH_VARARGS, (char*)"Moves a Rich Text Sensor"},
    {(char*)"hideRichText", py_hideRichText, METH_VARARGS, (char*)"hides a Rich Text Sensor"},
    {(char*)"showRichText", py_showRichText, METH_VARARGS, (char*)"shows a Rich Text Sensor"},
    {(char*)"getRichTextSensor", py_getRichTextSensor, METH_VARARGS, (char*)"Get Rich Text sensor."},
    {(char*)"setRichTextSensor", py_setRichTextSensor, METH_VARARGS, (char*)"Set Rich Text sensor."},
    {(char*)"changeRichText", py_setRichTextValue, METH_VARARGS, (char*)"Change the content of a Rich Text Sensor"},
    {(char*)"getRichTextValue", py_getRichTextValue, METH_VARARGS, (char*)"Get Rich Text value"},
    {(char*)"changeRichTextFont", py_setRichTextFont, METH_VARARGS, (char*)"Change a Rich Text Sensor's Font"},
    {(char*)"getRichTextFont", py_getRichTextFont, METH_VARARGS, (char*)"Get a Rich Text Sensor's Font"},
    {(char*)"changeRichTextSize", py_setRichTextFontSize, METH_VARARGS, (char*)"Change a Rich Text Sensor's Font Size"},
    {(char*)"getRichTextFontSize", py_getRichTextFontSize, METH_VARARGS, (char*)"Get a Rich Text Sensor's Font Size"},

    // ImageLabel - imagelabel_python.cpp
    {(char*)"createImage", py_createImage, METH_VARARGS, (char*)"Create an Image"},
    {(char*)"createTaskIcon", py_createTaskIcon, METH_VARARGS, (char*)"Create an Image of the Icon for a Task"},
    {(char*)"createBackgroundImage", py_createBackgroundImage, METH_VARARGS, (char*)"Create an Image (only redraw it when background changes)"},
    {(char*)"deleteImage", py_deleteImage, METH_VARARGS, (char*)"Delete an Image"},
    {(char*)"getThemeImage", py_getThemeImage, METH_VARARGS, (char*)"Get image meter from .theme using it's name"},
    {(char*)"getImageSize", py_getImageSize, METH_VARARGS, (char*)"Get Image size."},
    {(char*)"getImageWidth", py_getImageWidth, METH_VARARGS, (char*)"Get the width of an Image"},
    {(char*)"getImageHeight", py_getImageHeight, METH_VARARGS, (char*)"Get the height of an Image"},
    {(char*)"getImagePos", py_getImagePos, METH_VARARGS, (char*)"Get Image position."},
    {(char*)"moveImage", py_moveImage, METH_VARARGS, (char*)"Move an Image"},
    {(char*)"hideImage", py_hideImage, METH_VARARGS, (char*)"Hide an Image"},
    {(char*)"showImage", py_showImage, METH_VARARGS, (char*)"Show an Image"},
    {(char*)"getImagePath", py_getImageValue, METH_VARARGS, (char*)"Get Image path."},
    {(char*)"setImagePath", py_setImageValue, METH_VARARGS, (char*)"Set Image path."},
    {(char*)"getImageSensor", py_getImageSensor, METH_VARARGS, (char*)"Get Image sensor."},
    {(char*)"setImageSensor", py_setImageSensor, METH_VARARGS, (char*)"Set Image sensor."},
    {(char*)"addImageTooltip", py_addImageTooltip, METH_VARARGS, (char*)"Create a Tooltip for an Image"},
    {(char*)"resizeImage", py_resizeImage, METH_VARARGS, (char*)"Scale an Image"},
    {(char*)"resizeImageSmooth", py_resizeImageSmooth, METH_VARARGS, (char*)"Scale an Image (slower, better looking)"},
    {(char*)"rotateImage", py_rotateImage, METH_VARARGS, (char*)"Rotate an Image"},
    {(char*)"removeImageTransformations", py_removeImageTransformations, METH_VARARGS, (char*)"Restore original size and orientation of an Image"},
    {(char*)"removeImageEffects", py_removeImageEffects, METH_VARARGS, (char*)"Remove Effects of an Image"},
    {(char*)"changeImageIntensity", py_changeImageIntensity, METH_VARARGS, (char*)"Change Intensity of an Image"},
    {(char*)"changeImageChannelIntensity", py_changeImageChannelIntensity, METH_VARARGS, (char*)"Change Intensity of an Image Channel"},
    {(char*)"changeImageToGray", py_changeImageToGray, METH_VARARGS, (char*)"Converts an Image to Grayscale"},

    // Menu - menu_python.cpp
    {(char*)"createMenu", py_create_menu, METH_VARARGS, (char*)"Create a popup menu"},
    {(char*)"deleteMenu", py_delete_menu, METH_VARARGS, (char*)"Delete a popup menu"},
    {(char*)"addMenuItem", py_add_menu_item, METH_VARARGS, (char*)"Add a popup menu entry"},
    {(char*)"addMenuSeparator", py_add_menu_separator, METH_VARARGS, (char*)"Add a popup menu seperator item"},
    {(char*)"removeMenuItem", py_remove_menu_item, METH_VARARGS, (char*)"Remove a popup menu entry"},
    {(char*)"popupMenu", py_popup_menu, METH_VARARGS, (char*)"Popup a menu at a specified location"},

    // Config - config_python.cpp
    {(char*)"addMenuConfigOption", py_add_menu_config_option, METH_VARARGS, (char*)"Add a configuration entry to the menu"},
    {(char*)"setMenuConfigOption", py_set_menu_config_option, METH_VARARGS, (char*)"Set a configuration entry in the menu"},
    {(char*)"readMenuConfigOption", py_read_menu_config_option, METH_VARARGS, (char*)"Read a configuration entry in the menu"},
    {(char*)"readConfigEntry", py_read_config_entry, METH_VARARGS, (char*)"Read a configuration entry"},
    {(char*)"writeConfigEntry", py_write_config_entry, METH_VARARGS, (char*)"Writes a configuration entry"},

    // Widget - widget_python.cpp
    {(char*)"moveWidget", py_move_widget, METH_VARARGS, (char*)"Move Widget to x,y"},
    {(char*)"resizeWidget", py_resize_widget, METH_VARARGS, (char*)"Resize Widget to width,height"},
    {(char*)"createWidgetMask", py_create_widget_mask, METH_VARARGS, (char*)"Create a clipping mask for this widget"},
    {(char*)"redrawWidget", py_redraw_widget, METH_VARARGS, (char*)"Update Widget to reflect your changes"},
    {(char*)"redrawWidgetBackground", py_redraw_widget_background, METH_VARARGS, (char*)"Update Widget to reflect background image changes"},
    {(char*)"getWidgetPosition", py_get_widget_position, METH_VARARGS, (char*)"Get Widget Position"},
    {(char*)"toggleWidgetRedraw", py_toggle_widget_redraw, METH_VARARGS, (char*)"Toggle Widget redrawing"},

    // Task - task_python.cpp
    {(char*)"getStartupList", py_get_startup_list, METH_VARARGS, (char*)"Get the system startup list"},
    {(char*)"getStartupInfo", py_get_startup_info, METH_VARARGS, (char*)"Get all the info for a startup"},
    {(char*)"getTaskList", py_get_task_list, METH_VARARGS, (char*)"Get the system task list"},
    {(char*)"getTaskNames", py_get_task_names, METH_VARARGS, (char*)"Get the system task list in name form"},
    {(char*)"getTaskInfo", py_get_task_info, METH_VARARGS, (char*)"Get all the info for a task"},
    {(char*)"performTaskAction", py_perform_task_action, METH_VARARGS, (char*)"Do something with a task, such as minimize it"},

    // System Tray - systray_python.cpp
    {(char*)"createSystray", py_create_systray, METH_VARARGS, (char*)"Create a Systray"},
    {(char*)"hideSystray", py_hide_systray, METH_VARARGS, (char*)"Hide the Systray"},
    {(char*)"showSystray", py_show_systray, METH_VARARGS, (char*)"Show the Systray"},
    {(char*)"moveSystray", py_move_systray, METH_VARARGS, (char*)"Move the Systray"},
    {(char*)"getCurrentWindowCount", py_get_current_window_count, METH_VARARGS, (char*)"Get current Window count"},
    {(char*)"updateSystrayLayout", py_update_systray_layout, METH_VARARGS, (char*)"Update Systray layout"},

    // Misc - misc_python.cpp
    {(char*)"getThemePath", py_get_theme_path, METH_VARARGS,  (char*)"Get the file path of the theme"},
    {(char*)"readThemeFile", py_read_theme_file, METH_VARARGS,
        (char*)"Read file from theme."},
    {(char*)"language", py_language, METH_VARARGS,
        (char*)"Return default language of a translation file."},
    {(char*)"userLanguage", py_userLanguage, METH_VARARGS,
        (char*)"Return user language."},
    {(char*)"userLanguages", py_userLanguages, METH_VARARGS,
        (char*)"Return preferred user languages."},
    {(char*)"openTheme", py_open_theme, METH_VARARGS,
        (char*)"Open a new theme"},
    {(char*)"reloadTheme", py_reload_theme, METH_VARARGS,
            (char*)"Reload current theme"},
    {(char*)"acceptDrops", py_accept_drops, METH_VARARGS,
            (char*)"Allows widget to receive Drop (I.E. Drag and Drop) events"},
    {(char*)"toggleShowDesktop", py_toggle_show_desktop, METH_VARARGS,
            (char*)"Show/Hide the desktop"},
    {(char*)"execute", py_execute_command, METH_VARARGS, (char*)"Execute a command"},
    {(char*)"executeInteractive", py_execute_command_interactive, METH_VARARGS, (char*)"Execute a command and get it's output (stdout)"},
    {(char*)"attachClickArea", (PyCFunction)py_attach_clickArea, METH_VARARGS|METH_KEYWORDS, (char*)"Add a clickArea to the given text or image"},
    {(char*)"createClickArea", py_create_click_area, METH_VARARGS, (char*)"Create a Click Area Sensor"},
    {(char*)"getNumberOfDesktops", py_get_number_of_desktops, METH_VARARGS, (char*)"Get current number of virtual desktops"},
    {(char*)"getIp", py_get_ip, METH_VARARGS, (char*)"Get current host's IP address"},
    {(char*)"translateAll", py_translate_all, METH_VARARGS, (char*)"Translate all widgets in a theme"},
    {(char*)"show", py_show, METH_VARARGS, (char*)"Show theme"},
    {(char*)"hide", py_hide, METH_VARARGS, (char*)"Hide theme"},

    // Input Box - input_python.cpp
    {(char*)"createInputBox", py_createInputBox, METH_VARARGS,
        (char*)"Create new Input Box."},
    {(char*)"deleteInputBox", py_deleteInputBox, METH_VARARGS,
        (char*)"Delete Input Box."},
    {(char*)"getThemeInputBox", py_getThemeInputBox, METH_VARARGS,
        (char*)"Get Input Box from .theme using it's name."},
    {(char*)"getInputBoxValue", py_getInputBoxValue, METH_VARARGS,
        (char*)"Get Input Box value"},
    {(char*)"changeInputBox", py_setInputBoxValue, METH_VARARGS,
        (char*)"Change a Input Box Text"},
    {(char*)"hideInputBox", py_hideInputBox, METH_VARARGS,
        (char*)"Hide Input Box."},
    {(char*)"showInputBox", py_showInputBox, METH_VARARGS,
        (char*)"Show Input Box."},
    {(char*)"getInputBoxPos", py_getInputBoxPos, METH_VARARGS,
        (char*)"Get InputBox position."},
    {(char*)"moveInputBox", py_moveInputBox, METH_VARARGS,
        (char*)"Moves a Input Box"},
    {(char*)"getInputBoxSize", py_getInputBoxSize, METH_VARARGS,
        (char*)"Get the (width, height) of a Input Box"},
    {(char*)"resizeInputBox", py_resizeInputBox, METH_VARARGS,
        (char*)"Resize Input Box."},
    {(char*)"changeInputBoxFont", py_setInputBoxFont, METH_VARARGS,
        (char*)"Change a Input Box Font"},
    {(char*)"getInputBoxFont", py_getInputBoxFont, METH_VARARGS,
        (char*)"Get a Input Box Font"},
    {(char*)"changeInputBoxFontColor", py_setInputBoxFontColor, METH_VARARGS,
        (char*)"Change a Input Box Font Color"},
    {(char*)"getInputBoxFontColor", py_getInputBoxFontColor, METH_VARARGS,
        (char*)"Get a Input Box Font Color"},
    {(char*)"changeInputBoxSelectionColor", py_setInputBoxSelectionColor,
        METH_VARARGS, (char*)"Change a Input Box Selection Color"},
    {(char*)"getInputBoxSelectionColor", py_getInputBoxSelectionColor,
        METH_VARARGS, (char*)"Get a Input Box Selection Color"},
    {(char*)"changeInputBoxBackgroundColor", py_setInputBoxBGColor,
        METH_VARARGS, (char*)"Change a Input Box Background Color"},
    {(char*)"getInputBoxBackgroundColor", py_getInputBoxBGColor, METH_VARARGS,
        (char*)"Get a Input Box Background Color"},
    {(char*)"changeInputBoxFrameColor", py_setInputBoxFrameColor, METH_VARARGS,
        (char*)"Change a Input Box Frame Color"},
    {(char*)"getInputBoxFrameColor", py_getInputBoxFrameColor, METH_VARARGS,
        (char*)"Get a Input Box Frame Color"},
    {(char*)"changeInputBoxSelectedTextColor", py_setInputBoxSelectedTextColor,
        METH_VARARGS, (char*)"Change a Input Box Selected Text Color"},
    {(char*)"getInputBoxSelectedTextColor", py_getInputBoxSelectedTextColor,
        METH_VARARGS, (char*)"Get a Input Box Selected Text Color"},
    {(char*)"changeInputBoxFontSize", py_setInputBoxFontSize, METH_VARARGS,
        (char*)"Change a Input Box Font Size"},
    {(char*)"getInputBoxFontSize", py_getInputBoxFontSize, METH_VARARGS,
        (char*)"Get a Input Box Font Size"},
    {(char*)"setInputFocus", py_setInputFocus, METH_VARARGS,
        (char*)"Set the Input Focus to the Input Box"},
    {(char*)"clearInputFocus", py_clearInputFocus, METH_VARARGS,
        (char*)"Clear the Input Focus of the Input Box"},
    {(char*)"getInputFocus", py_getInputFocus, METH_VARARGS,
        (char*)"Get the Input Box currently focused"},

    {(char*)"setWidgetOnTop", py_set_widget_on_top, METH_VARARGS,
      (char*)"changes 'on top' status"},
    {(char*)"getSystraySize", py_get_systray_size, METH_VARARGS, 
      (char*)"Get the size of the Systray"},
    {(char*)"getPrettyThemeName", py_get_pretty_name, METH_VARARGS,  
      (char*)"Get the pretty name of the theme"},
    {(char*)"openNamedTheme", py_open_named_theme, METH_VARARGS, 
      (char*)"Open a new theme giving it a new name"},
    {(char*)"callTheme", py_call_theme, METH_VARARGS, 
      (char*)"Pass a string to another theme"},
    {(char*)"changeInterval", py_change_interval, METH_VARARGS, 
      (char*)"Change the refresh interval"},
    {(char*)"run", py_run_command, METH_VARARGS, 
      (char*)"Execute a command with KRun"},
    {(char*)"createServiceClickArea", py_create_service_click_area, METH_VARARGS, 
      (char*)"Create a Service-named Click Area Sensor"},
    {(char*)"removeClickArea", py_remove_click_area, METH_VARARGS, 
      (char*)"Remove a Click Area Sensor"},
    {(char*)"setUpdateTime", py_set_update_time, METH_VARARGS, 
      (char*)"Set last updated time"},
    {(char*)"getUpdateTime", py_get_update_time, METH_VARARGS, 
      (char*)"Get last updated time"},
    {(char*)"setWantRightButton", py_want_right_button, METH_VARARGS, 
      (char*)"Set to 1 to deactivate management popups"},
    {(char*)"setWantMeterWheelEvent", py_want_wheel_event, METH_VARARGS, 
      (char*)"Enables wheel events over meters."},
    {(char*)"managementPopup", py_management_popup, METH_VARARGS, 
      (char*)"Activates the Management Popup menu"},

  // service groups
    {(char*)"getServiceGroups", py_get_service_groups, METH_VARARGS, 
      (char*)"Get KDE Service Groups"},

    {NULL, NULL, 0 ,NULL}
};

PyThreadState* KarambaPython::mainThreadState = 0;

KarambaPython::KarambaPython(const ThemeFile& theme, bool reloading):
  pythonThemeExtensionLoaded(false), pName(0), pModule(0), pDict(0)
{
  PyThreadState* myThreadState;
  char pypath[1024];

  getLock(&myThreadState);

  // load the .py file for this .theme
  PyRun_SimpleString((char*)"import sys");
  //Add theme path to python path so that we can find the python file
  snprintf(pypath, 1023, "sys.path.insert(0, '%s')", theme.path().ascii());
  PyRun_SimpleString(pypath);
  PyRun_SimpleString((char*)"sys.path.insert(0, '')");

  PyImport_AddModule((char*)"karamba");
  Py_InitModule((char*)"karamba", karamba_methods);

  pName = PyString_FromString(theme.pythonModule().ascii());
  pModule = PyImport_Import(pName);
  
  fprintf(stderr, "%s\n", pypath);
  
  //Make sure the module is up to date.
  if (reloading)
    PyImport_ReloadModule(pModule);

  if (pModule != NULL)
  {
    pDict = PyModule_GetDict(pModule);
    if (pDict != NULL)
    {
      pythonThemeExtensionLoaded = true;
    }
  }
  else
  {
    PyErr_Print();
    fprintf(stderr,
            "------------------------------------------------------\n");
    fprintf(stderr, "What does ImportError mean?\n");
    fprintf(stderr, "\n");
    fprintf(stderr,
            "It means that I couldn't load a python add-on %s.py\n",
            theme.pythonModule().ascii());
    fprintf(stderr, "If this is a regular theme and doesn't use python\n");
    fprintf(stderr, "extensions, then nothing is wrong.\n");
    fprintf(stderr,
            "------------------------------------------------------\n");
  }
  releaseLock(myThreadState);
}

KarambaPython::~KarambaPython()
{
  //Clean up Python references
  if (pythonThemeExtensionLoaded) {
    PyThreadState* myThreadState;
    getLock(&myThreadState);

    //Displose of current python module so we can reload in constructor.
    Py_DECREF(pModule);
    Py_DECREF(pName);

    releaseLock(myThreadState);
  }
}

void KarambaPython::initPython()
{
  // initialize Python
  Py_Initialize();

  // initialize thread support
  PyEval_InitThreads();

  // save a pointer to the main PyThreadState object
  mainThreadState = PyThreadState_Get();

  // release the lock
  PyEval_ReleaseLock();
}

void KarambaPython::shutdownPython()
{
  // shut down the interpreter
  PyInterpreterState * mainInterpreterState = mainThreadState->interp;
  // create a thread state object for this thread
  PyThreadState * myThreadState = PyThreadState_New(mainInterpreterState);
  PyThreadState_Swap(myThreadState);
  PyEval_AcquireLock();
  Py_Finalize();
}

void KarambaPython::getLock(PyThreadState** myThreadState)
{
  // get the global lock
  PyEval_AcquireLock();

  // create a thread state object for this thread
  *myThreadState = PyThreadState_New(mainThreadState->interp);
  PyThreadState_Swap(*myThreadState);
}

void KarambaPython::releaseLock(PyThreadState* myThreadState)
{
  // swap my thread state out of the interpreter
  PyThreadState_Swap(NULL);
  // clear out any cruft from thread state object
  PyThreadState_Clear(myThreadState);
  // delete my thread state object
  PyThreadState_Delete(myThreadState);
  // release the lock
  PyEval_ReleaseLock();
}

PyObject* KarambaPython::getFunc(const char* function)
{
  PyObject* pFunc = PyDict_GetItemString(pDict, (char*)function);
  if (pFunc && PyCallable_Check(pFunc))
    return pFunc;
  return NULL;
}

bool KarambaPython::callObject(const char* func, PyObject* pArgs, bool lock)
{
  bool result = false;
  PyThreadState* myThreadState;

  //qDebug("Calling %s", func);

  if (lock)
    getLock(&myThreadState);
  PyObject* pFunc = getFunc(func);

  if (pFunc != NULL)
  {
    PyObject* pValue = PyObject_CallObject(pFunc, pArgs);

    if (pValue != NULL)
    {
      Py_DECREF(pValue);
      result = true;
    }
    else
    {
      qWarning("Call to %s failed", func);
      PyErr_Print();
    }
  }
  Py_DECREF(pArgs);
  if (lock)
    releaseLock(myThreadState);
  return result;
}

bool KarambaPython::initWidget(karamba* k)
{
  PyObject* pArgs = Py_BuildValue((char*)"(l)", k);
  return callObject("initWidget", pArgs);
}

bool KarambaPython::widgetUpdated(karamba* k)
{
  PyObject* pArgs = Py_BuildValue((char*)"(l)", k);
  return callObject("widgetUpdated", pArgs);
}

bool KarambaPython::widgetClosed(karamba* k)
{
  PyObject* pArgs = Py_BuildValue((char*)"(l)", k);
  return callObject("widgetClosed", pArgs);
}

bool KarambaPython::menuOptionChanged(karamba* k, QString key, bool value)
{
  PyObject* pArgs = Py_BuildValue((char*)"(lsi)", k, key.ascii(), (int)value);
  return callObject("menuOptionChanged", pArgs);
}

bool KarambaPython::menuItemClicked(karamba* k, KPopupMenu* menu, long id)
{
  PyObject* pArgs = Py_BuildValue((char*)"(lll)", k, menu, id);
  return callObject("menuItemClicked", pArgs);
}

bool KarambaPython::meterClicked(karamba* k, Meter* meter, int button)
{
  PyObject* pArgs = Py_BuildValue((char*)"(lli)", k, meter, button);
  return callObject("meterClicked", pArgs);
}

bool KarambaPython::meterClicked(karamba* k, QString anchor, int button)
{
  PyObject* pArgs = Py_BuildValue((char*)"(lsi)", k, anchor.ascii(), button);
  return callObject("meterClicked", pArgs);
}

bool KarambaPython::widgetClicked(karamba* k, int x, int y, int button)
{
  PyObject* pArgs = Py_BuildValue((char*)"(liii)", k, x, y, button);
  return callObject("widgetClicked", pArgs);
}

bool KarambaPython::keyPressed(karamba* k, const Meter* meter,
                               const QString& text)
{
  PyObject* pArgs = Py_BuildValue((char*)"(lls)", k, meter, text.ucs2());
  return callObject("keyPressed", pArgs);
}

bool KarambaPython::widgetMouseMoved(karamba* k, int x, int y, int button)
{
  PyObject* pArgs = Py_BuildValue((char*)"(liii)", k, x, y, button);
  return callObject("widgetMouseMoved", pArgs);
}

bool KarambaPython::activeTaskChanged(karamba* k, Task* t)
{
  PyObject* pArgs = Py_BuildValue((char*)"(ll)", k, t);
  return callObject("activeTaskChanged", pArgs);
}

bool KarambaPython::taskAdded(karamba* k, Task* t)
{
  PyObject* pArgs = Py_BuildValue((char*)"(ll)", k, t);
  return callObject("taskAdded", pArgs);
}

bool KarambaPython::taskRemoved(karamba* k, Task* t)
{
  PyObject* pArgs = Py_BuildValue((char*)"(ll)", k, t);
  return callObject("taskRemoved", pArgs);
}

bool KarambaPython::startupAdded(karamba* k, Startup* t)
{
  PyObject* pArgs = Py_BuildValue((char*)"(ll)", k, t);
  return callObject("startupAdded", pArgs);
}

bool KarambaPython::startupRemoved(karamba* k, Startup* t)
{
  PyObject* pArgs = Py_BuildValue((char*)"(ll)", k, t);
  return callObject("startupRemoved", pArgs);
}

bool KarambaPython::commandOutput(karamba* k, int pid, char *buffer)
{
  PyObject* pArgs = Py_BuildValue((char*)"(lis)", k, pid, buffer);
  return callObject("commandOutput", pArgs);
}

bool KarambaPython::commandFinished(karamba* k, int pid)
{
  PyObject* pArgs = Py_BuildValue((char*)"(li)", k, pid);
  return callObject("commandFinished", pArgs);
}

bool KarambaPython::itemDropped(karamba* k, QString text, int x, int y)
{
  PyObject* pArgs = Py_BuildValue((char*)"(lOii)", k, QString2PyString(text), x, y);
  return callObject("itemDropped", pArgs);
}

bool KarambaPython::themeNotify(karamba* k, const char *from, const char *str)
{
  // WARNING WARNING WARNING i had to switch off thread locking to get
  // this to work.  callNotify is called from INSIDE another locked thread,
  // so can never complete because themeNotify will expect locking to be
  // released...
  //
  PyObject* pArgs = Py_BuildValue((char*)"(lss)", k, from, str);
  return callObject("themeNotify", pArgs, false);
}

bool KarambaPython::systrayUpdated(karamba* k)
{
  PyObject* pArgs = Py_BuildValue((char*)"(l)", k);
  return callObject("systrayUpdated", pArgs);
}

bool KarambaPython::desktopChanged(karamba* k, int desktop)
{
  PyObject* pArgs = Py_BuildValue((char*)"(li)", k, desktop);
  return callObject("desktopChanged", pArgs);
}

bool KarambaPython::wallpaperChanged(karamba* k, int desktop)
{
  PyObject* pArgs = Py_BuildValue((char*)"(li)", k, desktop);
  return callObject("wallpaperChanged", pArgs);
}
