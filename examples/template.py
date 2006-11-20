#this import statement allows access to the karamba functions
import karamba


#this is called when your widget is initialized
def initWidget(widget):
    pass

#This is called when your widget is closed.  You can use this to clean
#up open files, etc.  You don't need to delete text and images in your
#theme.  That is done automatically.  This callback is just for cleaning up
#external things.  Most people don't need to put anything here.
def widgetClosed(widget):
    pass

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
    #Warning:  Don't do anything too intensive here
    #You don't want to run some complex piece of code everytime the mouse moves
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

#This gets called when another theme sends a message.
#  widget = reference to your theme
#  caller = "pretty" name or regular name (name of theme file with a number attached) of sending theme
#  output = the text sent.
def themeNotify(widget, caller, message):
    pass

#This gets called when an item is dropped on this widget.
# NOTE you have to call acceptDrops() before your widget will accept drops.
#  widget = reference to your theme
#  dropText = the text of the dropped item (probably a URL to it's location in KDE)
#  x = the x position on the theme relative to the upper left corner
#  y = the y position on the theme relative to the upper left corner
def itemDropped(widget, dropText, x, y):
    pass


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

#This is called everytime the systray you created with createSystray() is updated
def systrayUpdated(widget):
    pass

#This is called everytime the current desktop changes
#  widget = reference to your widget
#  desktop = the current desktop
def desktopChanged(widget, desktop):
    pass

#This is called everytime the wallpaper changes on a desktop 
#  widget = reference to your widget
#  desktop = the desktop whose wallpaper changed
def wallpaperChanged(widget, desktop):
    pass

#This is called everytime there is a key press in any focused input field
#  widget = reference to your widget
#  meter = reference to an input box
#  char = the key that was pressed
def keyPressed(widget, meter, char):
    pass

# This will be printed when the widget loads.
print "Loaded my python extension!"
