/****************************************************************************
*  input_python.h  -  Functions for input box python api
*
*  Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>
*  Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>
*  Copyright (c) 2004 Petri Damstén <damu@iki.fi>
*  Copyright (c) 2005 Alexander Wiedenbruch <mail@wiedenbruch.de>
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

#ifndef INPUT_PYTHON_H
#define INPUT_PYTHON_H

/** InputBox/createInputBox
*
* SYNOPSIS
*   long createInputBox(widget, x, y, w, h, text)
* DESCRIPTION
*   This creates a Input Box at x, y with width and height w, h. You need to save
*   the return value of this function to call other functions on your Input Box
*   field, such as changeInputBox().
*   The karamba widget is automatically set active, to allow user interactions.
* ARGUMENTS
*   * long widget -- karamba
*   * long x -- x coordinate
*   * long y -- y coordinate
*   * long w -- width
*   * long h -- height
*   * string text -- text for the Input Box
* RETURN VALUE
*   Pointer to new Input Box
*/
PyObject* py_createInputBox(PyObject *, PyObject *args);

/** InputBox/deleteInputBox
*
* SYNOPSIS
*   long deleteInputBox(widget, inputBox)
* DESCRIPTION
*   This removes a Input Box object from memory. Please do not call functions of
*   the Input Box after calling deleteInputBox, as it does not exist anymore and that
*   could cause crashes in some cases.
*   The karamba widget ist automatically set passive, when no more Input Boxes are on
*   the karamba widget.
* ARGUMENTS
*   * long widget -- karamba
*   * long widget -- inputBox
* RETURN VALUE
*   1 if successful
*/
PyObject* py_deleteInputBox(PyObject *, PyObject *args);

/** InputBox/getThemeInputBox
*
* SYNOPSIS
*   long getThemeInputBox(widget, name)
* DESCRIPTION
*   You can reference text in your python code that was created in the
*   theme file. Basically, you just add a NAME= value to the INPUT line in
*   the .theme file. Then if you want to use that object, instead of calling
*   createInputBox, you can call this function.
*
*   The name you pass to the function is the same one that you gave it for
*   the NAME= parameter in the .theme file.
* ARGUMENTS
*   * long widget -- karamba
*   * string name -- name of the Input Box to get
* RETURN VALUE
*   Pointer to Input Box
*/
PyObject* py_getThemeInputBox(PyObject *self, PyObject *args);

/** InputBox/getInputBoxValue
*
* SYNOPSIS
*   string getInputBoxValue(widget, inputBox)
* DESCRIPTION
*   Returns current Input Box text.
* ARGUMENTS
*   * long widget -- karamba
*   * long inputBox -- pointer to text
* RETURN VALUE
*   value
*/
PyObject* py_getInputBoxValue(PyObject *self, PyObject *args);

/** InputBox/changeInputBox
*
* SYNOPSIS
*   long changeInputBox(widget, inputBox, value)
* DESCRIPTION
*   This will change the contents of a input box widget.
* ARGUMENTS
*   * long widget -- karamba
*   * long text -- pointer to Input Box
*   * long value -- new value
* RETURN VALUE
*   1 if successful
*/
PyObject* py_setInputBoxValue(PyObject *self, PyObject *args);

/** InputBox/hideInputBox
*
* SYNOPSIS
*   long hideInputBox(widget, inputBox)
* DESCRIPTION
*   Hides a Input Box that is visible.
* ARGUMENTS
*   * long widget -- karamba
*   * long inputBox -- pointer to Input Box
* RETURN VALUE
*   1 if successful
*/
PyObject* py_hideInputBox(PyObject *self, PyObject *args);

/** InputBox/showInputBox
*
* SYNOPSIS
*   long showInputBox(widget, inputBox)
* DESCRIPTION
*   Shows Input Box that has been hidden with hideInputBox()
* ARGUMENTS
*   * long widget -- karamba
*   * long inputBox -- pointer to Input Box
* RETURN VALUE
*   1 if successful
*/
PyObject* py_showInputBox(PyObject *self, PyObject *args);

/** InputBox/getInputBoxPos
*
* SYNOPSIS
*   tuple getInputBoxPos(widget, inputBox)
* DESCRIPTION
*   Given a reference to a Input Box object, this will return a tuple
*   containing the x and y coordinate of a Input Box object.
* ARGUMENTS
*   * long widget -- karamba
*   * long inputBox -- pointer to Input Box
* RETURN VALUE
*   pos
*/
PyObject* py_getInputBoxPos(PyObject *self, PyObject *args);

/** InputBox/moveInputBox
*
* SYNOPSIS
*   long moveInputBox(widget, inputBox, x, y)
* DESCRIPTION
*   This moves a Input Box object to a new x, y relative to your widget. In other
*   words, (0,0) is the top corner of your widget, not the screen.
* ARGUMENTS
*   * long widget -- karamba
*   * long inputBox -- pointer to Input Box
*   * long x -- x coordinate
*   * long y -- y coordinate
* RETURN VALUE
*   1 if successful
*/
PyObject* py_moveInputBox(PyObject *self, PyObject *args);

/** InputBox/getInputBoxSize
*
* SYNOPSIS
*   tuple getInputBoxSize(widget, inputBox)
* DESCRIPTION
*   Given a reference to a Input Box object, this will return a tuple
*   containing the height and width of a Input Box object.
* ARGUMENTS
*   * long widget -- karamba
*   * long inputBox -- pointer to Input Box
* RETURN VALUE
*   size
*/
PyObject* py_getInputBoxSize(PyObject *self, PyObject *args);

/** InputBox/resizeInputBox
*
* SYNOPSIS
*   long resizeInputBox(widget, inputBox, w, h)
* DESCRIPTION
*   This will resize Input Box to new height and width.
* ARGUMENTS
*   * long widget -- karamba
*   * long inputBox -- pointer to Input Box
*   * long w -- new width
*   * long h -- new height
* RETURN VALUE
*   1 if successful
*/
PyObject* py_resizeInputBox(PyObject *self, PyObject *args);

/** InputBox/changeInputBoxFont
*
* SYNOPSIS
*   long changeInputBoxFont(widget, inputBox, font)
* DESCRIPTION
*   This will change the font of a Input Box widget. InputBox is the reference to the
*   Input Box object to change. Font is a string the the name of the font to use.
* ARGUMENTS
*   * long widget -- karamba
*   * long inputBox -- pointer to inputBox
*   * string font -- font name
* RETURN VALUE
*   1 if successful
*/
PyObject* py_setInputBoxFont(PyObject *, PyObject *args);

/** InputBox/getInputBoxFont
*
* SYNOPSIS
*   string getInputBoxFont(widget, inputBox)
* DESCRIPTION
*   Get current Input Box font name
* ARGUMENTS
*   * long widget -- karamba
*   * long inputBox -- pointer to Input Box
* RETURN VALUE
*   font name
*/
PyObject* py_getInputBoxFont(PyObject *, PyObject *args);

/** InputBox/changeInputBoxFontColor
*
* SYNOPSIS
*   long changeInputBoxFontColor(widget, inputBox, r, g, b)
* DESCRIPTION
*   This will change the color of a text of a Input Box widget.
*   InputBox is the reference to the text object to change
*   r, g, b are ints from 0 to 255 that represent red, green, and blue.
* ARGUMENTS
*   * long widget -- karamba
*   * long inputBox -- pointer to Input Box
*   * long red -- red component of color
*   * long green -- green component of color
*   * long blue -- blue component of color
* RETURN VALUE
*   1 if successful
*/
PyObject* py_setInputBoxFontColor(PyObject *, PyObject *args);

/** InputBox/getInputBoxFontColor
*
* SYNOPSIS
*   tuple getInputBoxFontColor(widget, inputBox)
* DESCRIPTION
*   Get current text color of a Input Box
* ARGUMENTS
*   * long widget -- karamba
*   * long inputBox -- pointer to Input Box
* RETURN VALUE
*   (red, green, blue)
*/
PyObject* py_getInputBoxFontColor(PyObject *, PyObject *args);

/** InputBox/changeInputBoxSelectionColor
*
* SYNOPSIS
*   long changeInputBoxSelectionColor(widget, inputBox, r, g, b)
* DESCRIPTION
*   This will change the color of the selection of a Input Box widget.
*   InputBox is the reference to the text object to change
*   r, g, b are ints from 0 to 255 that represent red, green, and blue.
* ARGUMENTS
*   * long widget -- karamba
*   * long inputBox -- pointer to Input Box
*   * long red -- red component of color
*   * long green -- green component of color
*   * long blue -- blue component of color
* RETURN VALUE
*   1 if successful
*/
PyObject* py_setInputBoxSelectionColor(PyObject *, PyObject *args);

/** InputBox/getInputBoxSelectionColor
*
* SYNOPSIS
*   tuple getInputBoxSelectionColor(widget, inputBox)
* DESCRIPTION
*   Get current selection color of a Input Box
* ARGUMENTS
*   * long widget -- karamba
*   * long inputBox -- pointer to Input Box
* RETURN VALUE
*   (red, green, blue)
*/
PyObject* py_getInputBoxSelectionColor(PyObject *, PyObject *args);

/** InputBox/changeInputBoxBackgroundColor
*
* SYNOPSIS
*   long changeInputBoxBackgroundColor(widget, inputBox, r, g, b)
* DESCRIPTION
*   This will change the background color of a Input Box widget.
*   InputBox is the reference to the text object to change
*   r, g, b are ints from 0 to 255 that represent red, green, and blue.
* ARGUMENTS
*   * long widget -- karamba
*   * long inputBox -- pointer to Input Box
*   * long red -- red component of color
*   * long green -- green component of color
*   * long blue -- blue component of color
* RETURN VALUE
*   1 if successful
*/
PyObject* py_setInputBoxBGColor(PyObject *, PyObject *args);

/** InputBox/getInputBoxBackgroundColor
*
* SYNOPSIS
*   tuple getInputBoxBackgroundColor(widget, inputBox)
* DESCRIPTION
*   Get current background color of a Input Box
* ARGUMENTS
*   * long widget -- karamba
*   * long inputBox -- pointer to Input Box
* RETURN VALUE
*   (red, green, blue)
*/
PyObject* py_getInputBoxBGColor(PyObject *, PyObject *args);

/** InputBox/changeInputBoxFrameColor
*
* SYNOPSIS
*   long changeInputBoxFrameColor(widget, inputBox, r, g, b)
* DESCRIPTION
*   This will change the frame color of a Input Box widget.
*   InputBox is the reference to the text object to change
*   r, g, b are ints from 0 to 255 that represent red, green, and blue.
* ARGUMENTS
*   * long widget -- karamba
*   * long inputBox -- pointer to Input Box
*   * long red -- red component of color
*   * long green -- green component of color
*   * long blue -- blue component of color
* RETURN VALUE
*   1 if successful
*/
PyObject* py_setInputBoxFrameColor(PyObject *, PyObject *args);

/** InputBox/getInputBoxFrameColor
*
* SYNOPSIS
*   tuple getInputBoxFrameColor(widget, inputBox)
* DESCRIPTION
*   Get current frame color of a Input Box
* ARGUMENTS
*   * long widget -- karamba
*   * long inputBox -- pointer to Input Box
* RETURN VALUE
*   (red, green, blue)
*/
PyObject* py_getInputBoxFrameColor(PyObject *, PyObject *args);

/** InputBox/changeInputBoxSelectedTextColor
*
* SYNOPSIS
*   long changeInputBoxSelectedTextColor(widget, inputBox, r, g, b)
* DESCRIPTION
*   This will change the selected text color of a Input Box widget.
*   InputBox is the reference to the text object to change
*   r, g, b are ints from 0 to 255 that represent red, green, and blue.
* ARGUMENTS
*   * long widget -- karamba
*   * long inputBox -- pointer to Input Box
*   * long red -- red component of color
*   * long green -- green component of color
*   * long blue -- blue component of color
* RETURN VALUE
*   1 if successful
*/
PyObject* py_setInputBoxSelectedTextColor(PyObject *, PyObject *args);

/** InputBox/getInputBoxSelectedTextColor
*
* SYNOPSIS
*   tuple getInputBoxSelectedTextColor(widget, inputBox)
* DESCRIPTION
*   Get current selected text color of a Input Box
* ARGUMENTS
*   * long widget -- karamba
*   * long inputBox -- pointer to Input Box
* RETURN VALUE
*   (red, green, blue)
*/
PyObject* py_getInputBoxSelectedTextColor(PyObject *, PyObject *args);

/** InputBox/changeInputBoxFontSize
*
* SYNOPSIS
*   long changeInputBoxFontSize(widget, text, size)
* DESCRIPTION
*   This will change the font size of a Input Box widget.
*   InputBox is the reference to the text object to change.
*   Size is the new font point size.
* ARGUMENTS
*   * long widget -- karamba
*   * long inputBox -- pointer to Input Box
*   * long size -- new size for text
* RETURN VALUE
*   1 if successful
*/
PyObject* py_setInputBoxFontSize(PyObject *, PyObject *args);

/** InputBox/getInputBoxFontSize
*
* SYNOPSIS
*   long getInputBoxFontSize(widget, inputBox)
* DESCRIPTION
*   Get current text font size
* ARGUMENTS
*   * long widget -- karamba
*   * long inputBox -- pointer to Input Box
* RETURN VALUE
*   text font size
*/
PyObject* py_getInputBoxFontSize(PyObject *, PyObject *args);

/** InputBox/setInputFocus
*
* SYNOPSIS
*   long setInputFocus(widget, inputBox)
* DESCRIPTION
*   Sets the Input Focus to the Input Box
* ARGUMENTS
*   * long widget -- karamba
*   * long inputBox -- pointer to Input Box
* RETURN VALUE
*   1 if successful
*/
PyObject* py_setInputFocus(PyObject *, PyObject *args);

/** InputBox/clearInputFocus
*
* SYNOPSIS
*   long clearInputFocus(widget, inputBox)
* DESCRIPTION
*   releases the Input Focus from the Input Box
* ARGUMENTS
*   * long widget -- karamba
*   * long inputBox -- pointer to Input Box
* RETURN VALUE
*   1 if successful
*/
PyObject* py_clearInputFocus(PyObject *, PyObject *args);

/** InputBox/getInputFocus
*
* SYNOPSIS
*   long getInputFocus(widget)
* DESCRIPTION
*   Get the Input Box currently focused
* ARGUMENTS
*   * long widget -- karamba
* RETURN VALUE
*   the input box or 0
*/
PyObject* py_getInputFocus(PyObject *, PyObject *args);

#endif
