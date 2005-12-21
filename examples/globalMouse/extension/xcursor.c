/*
 * This module is based on code by found in comp.lang.python:
 * http://groups.google.at/groups?q=%22Re:+X+Gurus%22+group:comp.lang.python.*&hl=de&lr=&ie=UTF-8&selm=imnv4.314%24qL4.17232%40newsread1.prod.itd.earthlink.net&rnum=2
 *
 * Build with:
 * 	python setup.py build
 * 
 * Copyright (C) 2003 Hans Karlsson <karlsson.h@home.se>
 * Copyright (C) 2003-2004 Adam Geitgey <adam@rootnode.org>
 * Copyright (c) 2005 Ryan Nickell <p0z3r@earthlink.net>
 *
 * This file is part of Superkaramba.
 *
 *  Superkaramba is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Superkaramba is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Superkaramba; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 ****************************************************************************/

#include <Python.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <stdio.h>

static Display* display = 0;
static Window root;
static int screen;


static PyObject* xcursor_position(self, args)
	PyObject* self;
	PyObject* args;
{
	/* Determine X Cursor coordinates */
	Window root_return, child_return;
	char* display_name;
 	int root_x_return, root_y_return;
 	int win_x_return, win_y_return;
 	unsigned int mask_return;

 	if (! (display_name = getenv("DISPLAY")) )
	{
  		/* add error handling! */

		/*
		 * fprintf(stderr,"environment variable DISPLAY must be set\n");
  		 * exit(-1);
		 */
		
		return NULL;
 	}

	if ( display == 0)
	{
 		if (! (display = XOpenDisplay(display_name)) )
		{
			/* add error handling! */
		
			/*
  		 	 * fprintf(stderr,"%s: Cannot open display %s\n", argv[0], 
   			 * display_name);
  			 * exit(-1);
 			 */

			return NULL;
		}

 		screen = DefaultScreen(display);
 		root = RootWindow(display, screen);
	}
 	
	XQueryPointer(display, root, &root_return, &child_return,
        		&root_x_return, &root_y_return,
        		&win_x_return, &win_y_return,
        		&mask_return);

	/* return Python Object */
	return Py_BuildValue("(i,i)", root_x_return, root_y_return);
}

/* Method Table */
static PyMethodDef xcursorMethods[] =
{
	{"position", xcursor_position, METH_VARARGS, "Query X Cursor Coordinates"},
	{NULL, NULL, 0, NULL} /* Sentinel */
};

void initxcursor(void)
{
	(void) Py_InitModule("xcursor", xcursorMethods);
}


