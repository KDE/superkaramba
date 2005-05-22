#this import statement allows access to the karamba functions
import karamba

menu1 = 0
menu2 = 0
id1 = 0
id2 = 0
id3 = 0
id4 = 0
id5 = 0

#this is called when you widget is initialized
def initWidget(widget):
	global menu1
	global menu2
	global id1
	global id2
	global id3
	global id4
	global id5

	menu1 = karamba.createMenu(widget)
	print "menu 1 created!"
	menu2 = karamba.createMenu(widget)
	print "menu 2 created!"

	id1 = karamba.addMenuItem(widget, menu1, "menu 1 first item", "kword")
	print "item 1 entered in menu 1"
	id2 = karamba.addMenuItem(widget, menu1, "menu 1 second item", "kate")
	print "item 2 entered in menu 1"

	id3 = karamba.addMenuItem(widget, menu2, "menu 2 first item", "kword")
	print "item 1 entered in menu 2"
	id4 = karamba.addMenuItem(widget, menu2, "menu 2 second item", "kate")
	print "item 2 entered in menu 2"
	id5 = karamba.addMenuItem(widget, menu2, "menu 2 third item", "/opt/kde/share/icons/kdeclassic/16x16/apps/kicker.png")
	print "item 3 entered in menu 2"



#this is called everytime your widget is updated
#the update inverval is specified in the .theme file
def widgetUpdated(widget):
    pass


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
	global menu1
	global menu2

	if(button == 1):
		karamba.popupMenu(widget, menu1, -100, -100)
	elif(button == 2):
		karamba.popupMenu(widget, menu2, 200, 200)


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


def menuItemClicked(widget, menu, id):
	global menu1
	global menu2
	global id1
	global id2
	global id3
	global id4
	global id5

	if (menu == menu1):
		if(id == id1):
			print "item 1 in menu 1 clicked."
		elif(id == id2):
			print "item 2 in menu 1 clicked, removing item 2 in menu 2"
			karamba.removeMenuItem(widget, menu2, id4)
	elif (menu == menu2):
		if(id == id3):
			print "item 1 in menu 2 clicked."
		elif(id == id4):
			print "item 2 in menu 2 clicked, deleting menu 1 (if menu1 still exists...)"
			karamba.deleteMenu(widget, menu1)
		elif(id == id5):
			print "item 3 in menu 2 clicked, removing item 2 in menu 1 (if menu1 still exists...)"
			karamba.removeMenuItem(widget, menu1, id2)



# This will be printed when the widget loads.
print "Loaded my python extension!"


