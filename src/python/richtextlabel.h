/****************************************************************************
*  richtextlabel_python.h  -  Functions for richtext python api
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
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
****************************************************************************/

#ifndef RICHTEXTLABEL_PYTHON_H
#define RICHTEXTLABEL_PYTHON_H

/** RichText/createRichText
*
* SYNOPSIS
*   long createRichText(widget, text, underlineLinks)
* DESCRIPTION
*   This creates creates a rich text string. underlineLinks is a boolean that
*   determines if html links will be automatically underlined so that the
*   user knows that the links can be clicked on. You need to save the return
*   value of this function to call other functions on your rich text field,
*   such as changeRichText().
*
*   The differance between Rich Text and a regular text field is that rich
*   text fields can display HTML code embedded in your text.
*
*   In a <a href="command"> ... </a> tag command is executed if the link is
*   click with the left mouse button.
*
*   Except if command starts with an '#' (ie: href="#value" ) the callback
*   meterClicked is called with value (without the #) as the meter argument.
*
*   Also inline images work. Unfortunatly currently only when using absolute
*   paths.
* ARGUMENTS
*   * long widget -- karamba
*   * string text -- text for richtext
*   * long underlineLinks -- should the links be underlined
* RETURN VALUE
*   Pointer to new richtext meter
*/
PyObject* py_createRichText(PyObject *self, PyObject *args);

/** RichText/deleteRichText
*
* SYNOPSIS
*   long deleteRichText(widget, richtext)
* DESCRIPTION
*   This removes a richt text object from memory. Please do not call
*   functions on "text" after calling deleteRichText, as it does not exist
*   anymore and that could cause crashes in some cases.
* ARGUMENTS
*   * long widget -- karamba
*   * long widget -- richtext
* RETURN VALUE
*   1 if successful
*/
PyObject* py_deleteRichText(PyObject *self, PyObject *args);

/** RichText/getThemeRichText
*
* SYNOPSIS
*   long getThemeRichText(widget, name)
* DESCRIPTION
*   You can reference richtext in your python code that was created in the
*   theme file. Basically, you just add a NAME= value to the GRAPH line in
*   the .theme file. Then if you want to use that object, instead of calling
*   createRichText, you can call this function.
*
*   The name you pass to the function is the same one that you gave it for
*   the NAME= parameter in the .theme file.
* ARGUMENTS
*   * long widget -- karamba
*   * string name -- name of the richtext to get
* RETURN VALUE
*   Pointer to richtext
*/
PyObject* py_getThemeRichText(PyObject *self, PyObject *args);

/** RichText/getRichTextSize
*
* SYNOPSIS
*   tuple getRichTextSize(widget, richtext)
* DESCRIPTION
*   Given a reference to a richtext object, this will return a tuple
*   containing the height and width of a richtext object.
* ARGUMENTS
*   * long widget -- karamba
*   * long richtext -- pointer to richtext
* RETURN VALUE
*   size
*/
PyObject* py_getRichTextSize(PyObject *self, PyObject *args);

/** RichText/resizeRichText
*
* SYNOPSIS
*   long resizeRichText(widget, richtext, w, h)
* DESCRIPTION
*   This will resize richtext to new height and width.
* ARGUMENTS
*   * long widget -- karamba
*   * long richtext -- pointer to richtext
*   * long w -- new width
*   * long h -- new height
* RETURN VALUE
*   1 if successful
*/
PyObject* py_resizeRichText(PyObject *self, PyObject *args);

/** RichText/getRichTextPos
*
* SYNOPSIS
*   tuple getRichTextPos(widget, richtext)
* DESCRIPTION
*   Given a reference to a richtext object, this will return a tuple
*   containing the x and y coordinate of a richtext object.
* ARGUMENTS
*   * long widget -- karamba
*   * long richtext -- pointer to richtext
* RETURN VALUE
*   pos
*/
PyObject* py_getRichTextPos(PyObject *self, PyObject *args);

/** RichText/moveRichText
*
* SYNOPSIS
*   long moveRichText(widget, richtext, x, y)
* DESCRIPTION
*   This moves a text object to a new x, y relative to your widget. In other
*   words, (0,0) is the top corner of your widget, not the screen.
* ARGUMENTS
*   * long widget -- karamba
*   * long richtext -- pointer to richtext
*   * long x -- x coordinate
*   * long y -- y coordinate
* RETURN VALUE
*   1 if successful
*/
PyObject* py_moveRichText(PyObject *self, PyObject *args);

/** RichText/hideRichText
*
* SYNOPSIS
*   long hideRichText(widget, richtext)
* DESCRIPTION
*   This hides an richtext. In other words, during subsequent calls to
*   widgetUpdate(), this richtext will not be drawn.
* ARGUMENTS
*   * long widget -- karamba
*   * long richtext -- pointer to richtext
* RETURN VALUE
*   1 if successful
*/
PyObject* py_hideRichText(PyObject *self, PyObject *args);

/** RichText/showRichText
*
* SYNOPSIS
*   long showRichText(widget, richtext)
* DESCRIPTION
*   This shows an richtext. In other words, during subsequent calls to
*   widgetUpdate(), this richtext will be drawn.
* ARGUMENTS
*   * long widget -- karamba
*   * long richtext -- pointer to richtext
* RETURN VALUE
*   1 if successful
*/
PyObject* py_showRichText(PyObject *self, PyObject *args);

/** RichText/getRichTextValue
*
* SYNOPSIS
*   string getRichTextValue(widget, richtext)
* DESCRIPTION
*   Returns current richtext value.
* ARGUMENTS
*   * long widget -- karamba
*   * long richtext -- pointer to richtext
* RETURN VALUE
*   value
*/
PyObject* py_getRichTextValue(PyObject *self, PyObject *args);

/** RichText/changeRichText
*
* SYNOPSIS
*   long changeRichText(widget, richtext, value)
* DESCRIPTION
*   This will change the contents of a rich text widget. richText is the
*   reference to the text object to change that you saved from the
*   createRichText() call. text is a string containing the new value for the
*   rich text object.
*
*   The differance between Rich Text and a regular text field is that rich
*   text fields can display HTML code embedded in your text.
*
*   In a <a href="command"> ... </a> tag command is executed if the link is
*   click with the left mouse button.
*
*   Except if command starts with an '#' (ie: href="#value" ) the callback
*   meterClicked is called with value (without the #) as the meter argument.
*
*   Also inline images work. Unfortunatly currently only when using absolute
*   paths.
* ARGUMENTS
*   * long widget -- karamba
*   * long richtext -- pointer to richtext
*   * string value -- new text
* RETURN VALUE
*   1 if successful
*/
PyObject* py_setRichTextValue(PyObject *self, PyObject *args);

/** RichText/getRichTextSensor
*
* SYNOPSIS
*   string getRichTextSensor(widget, richtext)
* DESCRIPTION
*   Get current sensor string
* ARGUMENTS
*   * long widget -- karamba
*   * long richtext -- pointer to richtext
* RETURN VALUE
*   sensor string
*/
PyObject* py_getRichTextSensor(PyObject *self, PyObject *args);

/** RichText/setRichTextSensor
*
* SYNOPSIS
*   long setRichTextSensor(widget, richtext, sensor)
* DESCRIPTION
*   Get current sensor string
* ARGUMENTS
*   * long widget -- karamba
*   * long richtext -- pointer to richtext
*   * string sensor -- new sensor as in theme files
* RETURN VALUE
*   1 if successful
*/
PyObject* py_setRichTextSensor(PyObject *self, PyObject *args);

/** RichText/changeRichTextSize
*
* SYNOPSIS
*   long changeRichTextSize(widget, richtext, size)
* DESCRIPTION
*   This will change the font size of a richtext widget.
* ARGUMENTS
*   * long widget -- karamba
*   * long richtext -- pointer to richtext
*   * long size -- new font point size
* RETURN VALUE
*   1 if successful
*/
PyObject* py_setRichTextFontSize(PyObject *self, PyObject *args);

/** RichText/getRichTextFontSize
*
* SYNOPSIS
*   long getRichTextFontSize(widget, richtext)
* DESCRIPTION
*   This will get the font size of a richtext widget.
* ARGUMENTS
*   * long widget -- karamba
*   * long richtext -- pointer to richtext
* RETURN VALUE
*   font point size
*/
PyObject* py_getRichTextFontSize(PyObject *self, PyObject *args);

/** RichText/changeRichTextFont
*
* SYNOPSIS
*   long changeRichTextFont(widget, richtext, font)
* DESCRIPTION
*   This will change the font of a richtext widget.
* ARGUMENTS
*   * long widget -- karamba
*   * long richtext -- pointer to richtext
*   * string font -- name of the new font
* RETURN VALUE
*   1 if successful
*/
PyObject* py_setRichTextFont(PyObject *self, PyObject *args);

/** RichText/getRichTextFont
*
* SYNOPSIS
*   string getRichTextFont(widget, richtext)
* DESCRIPTION
*   This will get the font of a richtext widget.
* ARGUMENTS
*   * long widget -- karamba
*   * long richtext -- pointer to richtext
* RETURN VALUE
*   font name
*/
PyObject* py_getRichTextFont(PyObject *self, PyObject *args);

/** RichText/setRichTextWidth
*
* SYNOPSIS
*   long setRichTextWidth(widget, richtext, width)
* DESCRIPTION
*   Given a reference to a rich text object, this function changes it's width
*   to the specified value in pixels.
*
*   The height adjusts automatically as the contents are changed with
*   changeRichText.
* ARGUMENTS
*   * long widget -- karamba
*   * long richtext -- pointer to richtext
*   * long width -- new width in pixels
* RETURN VALUE
*   1 if successful
*/
PyObject* py_set_rich_text_width(PyObject* self, PyObject* args);

#endif // RICHTEXTLABEL_PYTHON_H
