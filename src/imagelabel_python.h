/****************************************************************************
*  imagelabel_python.h  -  Imagelabel functions for python api
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

#ifndef IMAGELABEL_PYTHON_H
#define IMAGELABEL_PYTHON_H

/** Image/createImage
*
* SYNOPSIS
*   long createImage(widget, x, y, image)
* DESCRIPTION
*   This creates an image on your widget at x, y. The filename should be
*   given as the path parameter. In theory the image could be local or could
*   be a url. It works just like adding an image in your theme file. You
*   will need to save the return value to be able to call other functions on
*   your image, such as moveImage()
* ARGUMENTS
*   * long widget -- karamba
*   * long x -- x coordinate
*   * long y -- y coordinate
*   * string image -- image for the imagelabel
* RETURN VALUE
*   Pointer to new image meter
*/
PyObject* py_createImage(PyObject *self, PyObject *args);

/** Image/createBackgroundImage
*
* SYNOPSIS
*   long createBackgroundImage(widget, x, y, w, h, image)
* DESCRIPTION
*   This creates an background image on your widget at x, y. The filename
*   should be given as the path parameter. In theory the image could be
*   local or could be a url. It works just like adding an image in your
*   theme file. You will need to save the return value to be able to call
*   other functions on your image, such as moveImage()
* ARGUMENTS
*   * long widget -- karamba
*   * long x -- x coordinate
*   * long y -- y coordinate
*   * string image -- image for the imagelabel
* RETURN VALUE
*   Pointer to new image meter
*/
PyObject* py_createBackgroundImage(PyObject *self, PyObject *args);

/** Image/createTaskIcon
*
* SYNOPSIS
*   long createTaskIcon(widget, x, y, ctask)
* DESCRIPTION
*   This creates a task image at x,y.
* ARGUMENTS
*   * long widget -- karamba
*   * long x -- x coordinate
*   * long y -- y coordinate
*   * long task -- task
* RETURN VALUE
*   Pointer to new image meter
*/
PyObject* py_createTaskIcon(PyObject *self, PyObject *args);

/** Image/deleteImage
*
* SYNOPSIS
*   long deleteImage(widget, image)
* DESCRIPTION
*   This removes image from memory. Please do not call functions on "image"
*   after calling deleteImage, as it does not exist anymore and that could
*   cause crashes in some cases.
* ARGUMENTS
*   * long widget -- karamba
*   * long widget -- image
* RETURN VALUE
*   1 if successful
*/
PyObject* py_deleteImage(PyObject *self, PyObject *args);

/** Image/getThemeImage
*
* SYNOPSIS
*   long getThemeImage(widget, name)
* DESCRIPTION
*   You can reference an image in your python code that was created in the
*   .theme file. Basically, you just add a NAME= value to the IMAGE line in
*   the .theme file. Then if you want to use that object, instead of calling
*   createImage, you can call this function.
*
*   The name you pass to the function is the same one that you gave it for
*   the NAME= parameter in the .theme file.
* ARGUMENTS
*   * long widget -- karamba
*   * string name -- name of the image to get
* RETURN VALUE
*   Pointer to image
*/
PyObject* py_getThemeImage(PyObject *self, PyObject *args);

/** Image/getImagePos
*
* SYNOPSIS
*   tuple getImagePos(widget, image)
* DESCRIPTION
*   Given a reference to a image object, this will return a tuple
*   containing the x and y coordinate of a image object.
* ARGUMENTS
*   * long widget -- karamba
*   * long image -- pointer to image
* RETURN VALUE
*   pos
*/
PyObject* py_getImagePos(PyObject *self, PyObject *args);

/** Image/getImageSize
*
* SYNOPSIS
*   tuple getImageSize(widget, image)
* DESCRIPTION
*   Given a reference to a image object, this will return a tuple
*   containing the height and width of a image object.
* ARGUMENTS
*   * long widget -- karamba
*   * long image -- pointer to image
* RETURN VALUE
*   size
*/
PyObject* py_getImageSize(PyObject *self, PyObject *args);

/** Image/getImageHeight
*
* SYNOPSIS
*   long getImageSize(widget, image)
* DESCRIPTION
*   This returns the height of an image. This is useful if you have rotated
*   an image and its size changed, so you do not know how big it is anymore.
* ARGUMENTS
*   * long widget -- karamba
*   * long image -- pointer to image
* RETURN VALUE
*   height
*/
PyObject* py_getImageHeight(PyObject *self, PyObject *args);

/** Image/getImageWidth
*
* SYNOPSIS
*   long getImageSize(widget, image)
* DESCRIPTION
*   This returns the width of an image. This is useful if you have rotated
*   an image and its size changed, so you do not know how big it is anymore. // ARGUMENTS
*   * long widget -- karamba
*   * long image -- pointer to image
* RETURN VALUE
*   width
*/
PyObject* py_getImageWidth(PyObject *self, PyObject *args);

/** Image/showImage
*
* SYNOPSIS
*   long showImage(widget, image)
* DESCRIPTION
*   This shows a previously hidden image. It does not actually refresh the
*   image on screen. That is what redrawWidget() does.
* ARGUMENTS
*   * long widget -- karamba
*   * long image -- pointer to image
* RETURN VALUE
*   1 if successful
*/
PyObject* py_showImage(PyObject *self, PyObject *args);

/** Image/hideImage
*
* SYNOPSIS
*   long hideImage(widget, image)
* DESCRIPTION
*   This hides an image. In other words, during subsequent calls to
*   widgetUpdate(), this image will not be drawn.
* ARGUMENTS
*   * long widget -- karamba
*   * long image -- pointer to image
* RETURN VALUE
*   1 if successful
*/
PyObject* py_hideImage(PyObject *self, PyObject *args);

/** Image/moveImage
*
* SYNOPSIS
*   long moveImage(widget, image, x, y)
* DESCRIPTION
*   This moves an image to a new x, y relative to your widget. In other
*   words, (0,0) is the top corner of your widget, not the screen. The
*   imageToMove parameter is a reference to the image to move that you saved
*   as the return value from createImage()
* ARGUMENTS
*   * long widget -- karamba
*   * long image -- pointer to image
*   * long x -- x coordinate
*   * long y -- y coordinate
* RETURN VALUE
*   1 if successful
*/
PyObject* py_moveImage(PyObject *self, PyObject *args);

/** Image/getImagePath
*
* SYNOPSIS
*   string getImagePath(widget, image)
* DESCRIPTION
*   Returns current image path.
* ARGUMENTS
*   * long widget -- karamba
*   * long image -- pointer to image
* RETURN VALUE
*   path
*/
PyObject* py_getImageValue(PyObject *self, PyObject *args);

/** Image/setImagePath
*
* SYNOPSIS
*   long setImagePath(widget, image, path)
* DESCRIPTION
*   This will change image of a image widget.
* ARGUMENTS
*   * long widget -- karamba
*   * long image -- pointer to image
*   * long path -- new path
* RETURN VALUE
*   1 if successful
*/
PyObject* py_setImageValue(PyObject *self, PyObject *args);

/** Image/getImageSensor
*
* SYNOPSIS
*   string getImageSensor(widget, image)
* DESCRIPTION
*   Get current sensor string
* ARGUMENTS
*   * long widget -- karamba
*   * long image -- pointer to image
* RETURN VALUE
*   sensor string
*/
PyObject* py_getImageSensor(PyObject *self, PyObject *args);

/** Image/setImageSensor
*
* SYNOPSIS
*   long setImageSensor(widget, image, sensor)
* DESCRIPTION
*   Get current sensor string
* ARGUMENTS
*   * long widget -- karamba
*   * long image -- pointer to image
*   * string sensor -- new sensor as in theme files
* RETURN VALUE
*   1 if successful
*/
PyObject* py_setImageSensor(PyObject *self, PyObject *args);

/** Image/removeImageEffects
*
* SYNOPSIS
*   long removeImageEffects(widget, image)
* DESCRIPTION
*   If you have called image effect commands on your image (ex:
*   changeImageIntensity), you can call this to restore your image to it's
*   original form.
* ARGUMENTS
*   * long widget -- karamba
*   * long image -- pointer to image
* RETURN VALUE
*   1 if successful
*/
PyObject* py_removeImageEffects(PyObject *self, PyObject *args);

/** Image/changeImageIntensity
*
* SYNOPSIS
*   long changeImageIntensity(widget, image, ratio, millisec)
* DESCRIPTION
*   Changes the "intensity" of the image, which is similar to it's
*   brightness. ratio is a floating point number from -1.0 to 1.0 that
*   determines how much to brighten or darken the image. Millisec specifies
*   how long in milliseconds before the image is restored to it's original
*   form. This is useful for "mouse over" type animations. Using 0 for
*   millisec disables this feature and leaves the image permanently
*   affected.
* ARGUMENTS
*   * long widget -- karamba
*   * long image -- pointer to image
*   * float ratio -- -1.0 to 1.0 (dark to bright)
*   * long millisec -- milliseconds before the image is restored
* RETURN VALUE
*   1 if successful
*/
PyObject* py_changeImageIntensity(PyObject *self, PyObject *args);

/** Image/changeImageChannelIntensity
*
* SYNOPSIS
*   long changeImageChannelIntensity(widget, image, ratio, channel, millisec)
* DESCRIPTION
*   Changes the "intensity" of the image color channel, which is similar to
*   it's brightness.
* ARGUMENTS
*   * long widget -- karamba
*   * long image -- pointer to image
*   * float ratio -- -1.0 to 1.0 (dark to bright)
*   * string channel -- color channel (red|green|blue)
*   * long millisec -- milliseconds before the image is restored
* RETURN VALUE
*   1 if successful
*/
PyObject* py_changeImageChannelIntensity(PyObject *self, PyObject *args);

/** Image/changeImageToGray
*
* SYNOPSIS
*   long changeImageToGray(widget, image, millisec)
* DESCRIPTION
*   Turns the given image into a grayscale image. Millisec specifies how
*   long in milliseconds before the image is restored to it's original form.
*   This is useful for "mouse over" type animations. Using 0 for millisec
*   disables this feature and leaves the image permanently affected.
* ARGUMENTS
*   * long widget -- karamba
*   * long image -- pointer to image
*   * long millisec -- milliseconds before the image is restored
* RETURN VALUE
*   1 if successful
*/
PyObject* py_changeImageToGray(PyObject *self, PyObject *args);

/** Image/removeImageTransformations
*
* SYNOPSIS
*   long removeImageTransformations(widget, image)
* DESCRIPTION
*   If you have rotated or resized your image, you can call this to restore
*   your image to it's original form.
* ARGUMENTS
*   * long widget -- karamba
*   * long image -- pointer to image
* RETURN VALUE
*   1 if successful
*/
PyObject* py_removeImageTransformations(PyObject *self, PyObject *args);

/** Image/rotateImage
*
* SYNOPSIS
*   long rotateImage(widget, image, deg)
* DESCRIPTION
*   This rotates your image to by the specified amount of degrees. The
*   imageToRotate parameter is a reference to an image that you saved as the
*   return value from createImage()
* ARGUMENTS
*   * long widget -- karamba
*   * long image -- pointer to image
*   * long deg -- degrees to rotate
* RETURN VALUE
*   1 if successful
*/
PyObject* py_rotateImage(PyObject *self, PyObject *args);

/** Image/resizeImageSmooth
*
* SYNOPSIS
*   long resizeImageSmooth(widget, image, w, h)
* DESCRIPTION
*    DEPRECATED: resizeImage now allows the user to pick whether to use fast
*    or smooth resizing from the SuperKaramba menu - This resizes your image
*    to width, height. The imageToResize parameter is a reference to an
*    image that you saved as the return value from createImage()
* ARGUMENTS
*   * long widget -- karamba
*   * long image -- pointer to image
*   * long w -- width
*   * long h -- height
* RETURN VALUE
*   1 if successful
*/
PyObject* py_resizeImageSmooth(PyObject *self, PyObject *args);

/** Image/resizeImage
*
* SYNOPSIS
*   long resizeImage(widget, image, w, h)
* DESCRIPTION
*   This resizes your image to width, height. The imageToResize parameter is
*   a reference to an image that you saved as the return value from
*   createImage()
* ARGUMENTS
*   * long widget -- karamba
*   * long image -- pointer to image
*   * long w -- width
*   * long h -- height
* RETURN VALUE
*   1 if successful
*/
PyObject* py_resizeImage(PyObject *self, PyObject *args);

/** Image/addImageTooltip
*
* SYNOPSIS
*   long addImageTooltip(widget, image, text)
* DESCRIPTION
*   This creats a tooltip for image with tooltip_text.
*
*   Note:
*   * If you move the image, the tooltip does not move! It stays! Do not
*     create a tooltip if the image is off-screen because you will not be
*     able to ever see it.
* ARGUMENTS
*   * long widget -- karamba
*   * long image -- pointer to image
*   * string text -- tooltip text
* RETURN VALUE
*   1 if successful
*/
PyObject* py_addImageTooltip(PyObject *self, PyObject *args);

#endif // IMAGELABEL_PYTHON_H
