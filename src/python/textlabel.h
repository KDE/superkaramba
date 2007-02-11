/****************************************************************************
*  textlabel_python.h  -  Functions for textlabel python api
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

#ifndef TEXTLABEL_PYTHON_H
#define TEXTLABEL_PYTHON_H

/** Text/createText
*
* SYNOPSIS
*   long createText(widget, x, y, w, h, text)
* DESCRIPTION
*   This creates a text at x,y with width and height w,h. You need to save
*   the return value of this function to call other functions on your text
*   field, such as changeText()
* ARGUMENTS
*   * long widget -- karamba
*   * long x -- x coordinate
*   * long y -- y coordinate
*   * long w -- width
*   * long h -- height
*   * string text -- text for the textlabel
* RETURN VALUE
*   Pointer to new text meter
*/
PyObject* py_createText(PyObject *self, PyObject *args);

/** Text/deleteText
*
* SYNOPSIS
*   long deleteText(widget, text)
* DESCRIPTION
*   This removes a text object from memory. Please do not call functions on
*   "text" after calling deleteText, as it does not exist anymore and that
*   could cause crashes in some cases.
* ARGUMENTS
*   * long widget -- karamba
*   * long widget -- text
* RETURN VALUE
*   1 if successful
*/
PyObject* py_deleteText(PyObject *self, PyObject *args);

/** Text/getThemeText
*
* SYNOPSIS
*   long getThemeText(widget, name)
* DESCRIPTION
*   You can reference text in your python code that was created in the
*   theme file. Basically, you just add a NAME= value to the TEXT line in
*   the .theme file. Then if you want to use that object, instead of calling
*   createText, you can call this function.
*
*   The name you pass to the function is the same one that you gave it for
*   the NAME= parameter in the .theme file.
* ARGUMENTS
*   * long widget -- karamba
*   * string name -- name of the text to get
* RETURN VALUE
*   Pointer to text
*/
PyObject* py_getThemeText(PyObject *self, PyObject *args);

/** Text/getTextSize
*
* SYNOPSIS
*   tuple getTextSize(widget, text)
* DESCRIPTION
*   Given a reference to a text object, this will return a tuple
*   containing the height and width of a text object.
* ARGUMENTS
*   * long widget -- karamba
*   * long text -- pointer to text
* RETURN VALUE
*   size
*/
PyObject* py_getTextSize(PyObject *self, PyObject *args);

/** Text/resizeText
*
* SYNOPSIS
*   long resizeText(widget, text, w, h)
* DESCRIPTION
*   This will resize text to new height and width.
* ARGUMENTS
*   * long widget -- karamba
*   * long text -- pointer to text
*   * long w -- new width
*   * long h -- new height
* RETURN VALUE
*   1 if successful
*/
PyObject* py_resizeText(PyObject *self, PyObject *args);

/** Text/getTextPos
*
* SYNOPSIS
*   tuple getTextPos(widget, text)
* DESCRIPTION
*   Given a reference to a text object, this will return a tuple
*   containing the x and y coordinate of a text object.
* ARGUMENTS
*   * long widget -- karamba
*   * long text -- pointer to text
* RETURN VALUE
*   pos
*/
PyObject* py_getTextPos(PyObject *self, PyObject *args);

/** Text/moveText
*
* SYNOPSIS
*   long moveText(widget, text, x, y)
* DESCRIPTION
*   This moves a text object to a new x, y relative to your widget. In other
*   words, (0,0) is the top corner of your widget, not the screen.
* ARGUMENTS
*   * long widget -- karamba
*   * long text -- pointer to text
*   * long x -- x coordinate
*   * long y -- y coordinate
* RETURN VALUE
*   1 if successful
*/
PyObject* py_moveText(PyObject *self, PyObject *args);

/** Text/hideText
*
* SYNOPSIS
*   long hideText(widget, text)
* DESCRIPTION
*   Hides text that is visible. You need to call redrawWidget() afterwords
*   to actually hide the text on screen.
* ARGUMENTS
*   * long widget -- karamba
*   * long text -- pointer to text
* RETURN VALUE
*   1 if successful
*/
PyObject* py_hideText(PyObject *self, PyObject *args);

/** Text/showText
*
* SYNOPSIS
*   long showText(widget, text)
* DESCRIPTION
*   Shows text that has been hidden with hideText()
* ARGUMENTS
*   * long widget -- karamba
*   * long text -- pointer to text
* RETURN VALUE
*   1 if successful
*/
PyObject* py_showText(PyObject *self, PyObject *args);

/** Text/getTextValue
*
* SYNOPSIS
*   string getTextValue(widget, text)
* DESCRIPTION
*   Returns current text value.
* ARGUMENTS
*   * long widget -- karamba
*   * long text -- pointer to text
* RETURN VALUE
*   value
*/
PyObject* py_getTextValue(PyObject *self, PyObject *args);

/** Text/changeText
*
* SYNOPSIS
*   long changeText(widget, text, value)
* DESCRIPTION
*   This will change the contents of a text widget.
* ARGUMENTS
*   * long widget -- karamba
*   * long text -- pointer to text
*   * long value -- new value
* RETURN VALUE
*   1 if successful
*/
PyObject* py_setTextValue(PyObject *self, PyObject *args);

/** Text/getTextSensor
*
* SYNOPSIS
*   string getTextSensor(widget, text)
* DESCRIPTION
*   Get current sensor string
* ARGUMENTS
*   * long widget -- karamba
*   * long text -- pointer to text
* RETURN VALUE
*   sensor string
*/
PyObject* py_getTextSensor(PyObject *self, PyObject *args);

/** Text/setTextSensor
*
* SYNOPSIS
*   long setTextSensor(widget, text, sensor)
* DESCRIPTION
*   Get current sensor string
* ARGUMENTS
*   * long widget -- karamba
*   * long text -- pointer to text
*   * string sensor -- new sensor as in theme files
* RETURN VALUE
*   1 if successful
*/
PyObject* py_setTextSensor(PyObject *self, PyObject *args);

/** Text/changeTextShadow
*
* SYNOPSIS
*   long changeTextShadow(widget, text, shadow)
* DESCRIPTION
*    This will change the shadow size of a text widget (only ones you
*    created through python currently). textToChange is the reference to the
*    text object to change that you saved from the createText() call. size
*    is the offset of the shadow in pixels. 1 or 2 is a good value in most
*    cases. Get current sensor string
* ARGUMENTS
*   * long widget -- karamba
*   * long text -- pointer to text
*   * long shadow -- shadow offset
* RETURN VALUE
*   1 if successful
*/
PyObject* py_setTextShadow(PyObject *self, PyObject *args);

/** Text/getTextShadow
*
* SYNOPSIS
*   long getTextShadow(widget, text)
* DESCRIPTION
*   Get current shadow offset
* ARGUMENTS
*   * long widget -- karamba
*   * long text -- pointer to text
* RETURN VALUE
*   shadow offset
*/
PyObject* py_getTextShadow(PyObject *self, PyObject *args);

/** Text/changeTextSize
*
* SYNOPSIS
*   long changeTextSize(widget, text, size)
* DESCRIPTION
*   This will change the font size of a text widget (only ones you created
*   through python currently). textToChange is the reference to the text
*   object to change that you saved from the createText() call. size is the
*   new font point size.
* ARGUMENTS
*   * long widget -- karamba
*   * long text -- pointer to text
*   * long size -- new size for text
* RETURN VALUE
*   1 if successful
*/
PyObject* py_setTextFontSize(PyObject *self, PyObject *args);

/** Text/getTextFontSize
*
* SYNOPSIS
*   long getTextFontSize(widget, text)
* DESCRIPTION
*   Get current text font size
* ARGUMENTS
*   * long widget -- karamba
*   * long text -- pointer to text
* RETURN VALUE
*   text font size
*/
PyObject* py_getTextFontSize(PyObject *self, PyObject *args);

/** Text/changeTextColor
*
* SYNOPSIS
*   long changeTextColor(widget, text, r, g, b)
* DESCRIPTION
*   This will change the color of a text widget (only ones you created
*   through python currently). textToChange is the reference to the text
*   object to change that you saved from the createText() call. r, g, b are
*   ints from 0 to 255 that represent red, green, and blue.
* ARGUMENTS
*   * long widget -- karamba
*   * long text -- pointer to text
*   * long red -- red component of color
*   * long green -- green component of color
*   * long blue -- blue component of color
* RETURN VALUE
*   1 if successful
*/
PyObject* py_setTextColor(PyObject *self, PyObject *args);

/** Text/getTextColor
*
* SYNOPSIS
*   tuple getTextColor(widget, text)
* DESCRIPTION
*   Get current text color
* ARGUMENTS
*   * long widget -- karamba
*   * long text -- pointer to text
* RETURN VALUE
*   (red, green, blue)
*/
PyObject* py_getTextColor(PyObject *self, PyObject *args);

/** Text/changeTextFont
*
* SYNOPSIS
*   long changeTextFont(widget, text, font)
* DESCRIPTION
*   This will change the font of a text widget (only ones you created
*   through python currently). Text is the reference to the text
*   object to change that you saved from the createText() call. Font is a
*   string the the name of the font to use.
* ARGUMENTS
*   * long widget -- karamba
*   * long text -- pointer to text
*   * string font -- font name
* RETURN VALUE
*   1 if successful
*/
PyObject* py_setTextFont(PyObject *self, PyObject *args);

/** Text/getTextFont
*
* SYNOPSIS
*   string getTextFont(widget, text)
* DESCRIPTION
*   Get current text font name
* ARGUMENTS
*   * long widget -- karamba
*   * long text -- pointer to text
* RETURN VALUE
*   font name
*/
PyObject* py_getTextFont(PyObject *self, PyObject *args);

/** Text/setTextAlign
*
* SYNOPSIS
*   long setTextAlign(widget, text, align)
* DESCRIPTION
*   Sets text label align.
* ARGUMENTS
*   * long widget -- karamba
*   * long text -- pointer to text
*   * string align -- LEFT, CENTER or RIGHT
* RETURN VALUE
*   1 if successful
*/
PyObject* py_setTextAlign(PyObject *self, PyObject *args);

/** Text/getTextAlign
*
* SYNOPSIS
*   string getTextAlign(widget, text)
* DESCRIPTION
*   Get current text align.
* ARGUMENTS
*   * long widget -- karamba
*   * long text -- pointer to text
* RETURN VALUE
*   LEFT, CENTER or RIGHT
*/
PyObject* py_getTextAlign(PyObject *self, PyObject *args);

// XXX: Is this valid for new release
PyObject* py_setTextScroll(PyObject *self, PyObject *args);

#endif // TEXTLABEL_PYTHON_H
