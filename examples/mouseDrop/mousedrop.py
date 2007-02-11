# Drop Detect
# Written by Luke Kenneth Casson Leighton <lkcl@lkcl.net>

# This theme is demonstrates how to 

#this import statement allows access to the karamba functions
import karamba

drop_txt = None

#this is called when you widget is initialized
def initWidget(widget):

	karamba.acceptDrops(widget)
	karamba.redrawWidget(widget)

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
        pass

#This gets called when an item is clicked in a popup menu you have created.
#  menu = a reference to the menu
#  id = the number of the item that was clicked.
def menuItemClicked(widget, menu, id):
    pass

#This gets called when an item is clicked in the theme CONFIGURATION menu,
#not the popup menus that you create.
#  key = the reference to the configuration key that was changed
#  value = the new value (true or false) that was selected
def menuOptionChanged(widget, key, value):
    pass

#This gets called when a meter (image, text, etc) is clicked.
# NOTE you must use attachClickArea() to make a meter
# clickable.  
#  widget = reference to your theme
#  meter = the meter clicked
#  button = the button clicked (see widgetClicked for button numbers)
def meterClicked(widget, meter, button):
    pass

#This gets called when a command you have executed with executeInteractive() outputs something
#to stdout.  This way you can get the output of for example kdialog without freezing up the widget
#waiting for kdialog to end.
#  widget = reference to your theme
#  pid = process number of the program outputting (use this if you execute more than out process)
#  output = the text the program outputted to stdout
def commandOutput(widget, pid, output):
    pass

#This gets called when an item is dropped on this widget.
# NOTE you have to call acceptDrops() before your widget will accept drops.
#  widget = reference to your theme
#  dropText = the text of the dropped item (probably a URL to it's location in KDE)
# x and y are coordinates of mouse where the drop occurred.
# this you can use to e.g. detect which icon or object in your
# theme that someone wants to "receive" the icon you were dragging
def itemDropped(widget, dropText, x, y):
	global drop_txt
	# delete previous text if exists.
	if drop_txt is not None:
		karamba.deleteText(widget, drop_txt)

	disp = "x:%d y:%d command: %s" % (x, y, dropText)
	drop_txt = karamba.createText(widget, 10, 40, 500, 20, disp)
	karamba.changeTextColor(widget, drop_txt, 252,252,252)

	karamba.redrawWidget(widget)

#This gets called when a new program is LOADING in KDE.  When it is done
#loading, startupRemoved() is called, followed by taskAdded().
#  widget = reference to your widget
#  task = A refence to the task that is starting.  
def startupAdded(widget, startup):
    pass

#This gets called when a new program is done LOADING in KDE.
#  widget = reference to your widget
#  task = A refence to the task that just finished loading.  
def startupRemoved(widget, startup):
    pass

#This is called every time a new task (program) is started in KDE.
#  widget = reference to your widget
#  task = A refence to the new task.  Call getTaskInfo() with this reference
#         to get the name, etc of this new task.
def taskAdded(widget, task):
    pass

#This is called everytime a task (program) is closed in KDE.
#  widget = reference to your widget
#  task = A refence to the task.  
def taskRemoved(widget, task):
    pass

#This is called everytime a different task gains focus (IE, the user clicks
#on a different window).  
#  widget = reference to your widget
#  task = A refence to the task.  Call getTaskInfo() with this reference
#         to get the name, etc of this new task.
def activeTaskChanged(widget, task):
    pass

# This will be printed when the widget loads.
print "Loaded my python extension!"

