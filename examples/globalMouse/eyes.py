#this import statement allows access to the karamba functions
import karamba

#import os
import string
import math
import re

import xcursor

linkePupille = ""
rechtePupille = ""

init = 0

#old curser pos
x_old = -1
y_old = -1

#widget size
w_width  = 167
w_height = 111

#widget pos
w_x = 10
w_y = 850

#eye center
lx, ly = 39, 55
rx, ry = 126, 55

#eye size
la, lb = 25, 38
ra, rb = 25, 38

#pupille size
lp_width, lp_height = 11, 17
rp_width, rp_height = 11, 17

def pupille(mouse_x, mouse_y, eye_center_x, eye_center_y, eye_a, eye_b, widget_x, widget_y):
	x = mouse_x - eye_center_x - widget_x
	y = mouse_y - eye_center_y - widget_y
	#print x, y

	r = math.sqrt(x * x + y * y)
	phi = math.atan2(y, x)
	#print phi * math.pi

	eye_x = eye_a * math.cos(phi)
	eye_y = eye_b * math.sin(phi)
	
	eye_r = math.sqrt(eye_x * eye_x + eye_y * eye_y)

	if eye_r < r:
		return int(eye_x + eye_center_x), int(eye_y + eye_center_y)
	
	return int(x + eye_center_x), int(y + eye_center_y)

	
#this is called when you widget is initialized
def initWidget(widget):
	pass	

#this is called everytime your widget is updated
#the update inverval is specified in the .theme file
def widgetUpdated(widget):
	global init
	global linkePupille
	global rechtePupille
	
	global w_width
	global w_height

	global w_x
	global w_y

	global lx
	global ly
	global la
	global lb

	global lp_width
	global lp_height

	global rx
	global ry
	global ra
	global rb

	global rp_width
	global rp_height

	global x_old
	global y_old

	if init == 0:
		theme_path = karamba.getThemePath(widget) + "/"
		
		# read widget coordinates from eyes.theme
		# f = open(theme_path + "eyes.theme")

		# karamba_line = ""
		#while re.compile('KARAMBA').search(karamba_line) == None:
		# 	karamba_line = f.readline()

		#w_x = int(re.compile('X=([0-9]+)').search(karamba_line).group(1))
		#w_y = int(re.compile('Y=([0-9]+)').search(karamba_line).group(1))

		#f.close()

		#karamba.createWidgetMask(widget, theme_path + "pics/mask.png")

		linkePupille = karamba.createImage(widget, 15, 30, theme_path + "pics/pupille.png")
		rechtePupille = karamba.createImage(widget, 100, 30, theme_path + "pics/pupille.png")	
		init = 1

		karamba.redrawWidget(widget)

	# query mouse-cursor position
	x, y = xcursor.position()
	
	#fp = os.popen("./xpos")
	#output = fp.read()
	#x, y = output.split()
	
	#print x, y

	if x != x_old or y != y_old:		
		x_old, y_old = x, y
		# Get Widget Position
		w_x, w_y = karamba.getWidgetPosition(widget)

		# Calc left pupille
		xp, yp = pupille (int(x), int(y), lx, ly, la, lb, w_x, w_y)

		xp = xp - lp_width / 2
		yp = yp - lp_height / 2
		#print xp, yp

		karamba.moveImage(widget, linkePupille, xp, yp)

		# Calc right pupille
		xp, yp = pupille (int(x), int(y), rx, ry, ra, rb, w_x, w_y)

		xp = xp - rp_width / 2
		yp = yp - rp_height / 2
		#print xp, yp

		karamba.moveImage(widget, rechtePupille, xp, yp)

		karamba.redrawWidget(widget)

#This gets called everytime our widget is clicked.
#Notes:
#  widget = reference to our widget
#  x = x position (relative to our widget)
#  y = y position (relative to our widget)
#  botton = button clicked:
#                    1 = Left Mouse Button
#                    2 = Middle Mouse Button
#                    3 = Right Mouse Button, but this will never happen
#                        because the right mouse button brings up the
#                        Karamba menu.
#                    4,5 = Scroll wheel up and down
def widgetClicked(widget, x, y, button):
    pass

#This gets called everytime our widget is clicked.
#Notes
#  widget = reference to our widget
#  x = x position (relative to our widget)
#  y = y position (relative to our widget)
#  botton = button being held:
#                    0 = No Mouse Button
#                    1 = Left Mouse Button
#                    2 = Middle Mouse Button
#                    3 = Right Mouse Button, but this will never happen
#                        because the right mouse button brings up the
#                        Karamba menu.
def widgetMouseMoved(widget, x, y, button):
    #Warning:  Don't do anything too intensive here
    #You don't want to run some complex piece of code everytime the mouse moves
	pass
	#global linkePupille

	#karamba.moveImage(widget, linkePupille, x, y)
	#karamba.redrawWidget(widget)


# This will be printed when the widget loads.
print "Loaded Karamba Eyes"

