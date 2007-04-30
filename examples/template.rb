#this require statement allows access to the karamba functions
require 'karamba'

# this is called when your widget is initialized
def initWidget(widget)
    puts ">>> initWidget"
end

# This is called when your widget is closed.  You can use this to clean
# up open files, etc.  You don't need to delete text and images in your
# theme.  That is done automatically.  This callback is just for cleaning up
# external things.  Most people don't need to put anything here.
def widgetClosed(widget)
    puts ">>> widgetClosed"
end

# this is called everytime your widget is updated
# the update inverval is specified in the .theme file
def widgetUpdated(widget)
    puts ">>> widgetUpdated"
end

# This gets called when an item is clicked in a popup menu you have created.
#   menu = a reference to the menu
#   id = the number of the item that was clicked.
def menuItemClicked(widget, menu, id)
    puts ">>> menuItemClicked"
end

# This gets called when an item is clicked in the theme CONFIGURATION menu,
# not the popup menus that you create.
#   key = the reference to the configuration key that was changed
#   value = the new value (true or false) that was selected
def menuOptionChanged(widget, key, value)
    puts ">>> menuOptionChanged"
end

# This gets called when a meter (image, text, etc) is clicked.
# NOTE you must use attachClickArea() to make a meter
# clickable.
#   widget = reference to your theme
#   meter = the meter clicked
#   button = the button clicked (see widgetClicked for button numbers)
def meterClicked(widget, meter, button)
    puts ">>> meterClicked"
end

# This gets called when an item is dropped on this widget.
# NOTE you have to call acceptDrops() before your widget will accept drops.
#   widget = reference to your theme
#   dropText = the text of the dropped item (probably a URL to it's location in KDE)
def itemDropped(widget, dropText)
    puts ">>> itemDropped"
end

# This is called everytime there is a key press in any focused input field
#   widget = reference to your widget
#   meter = reference to an input box
#   char = the key that was pressed
def keyPressed(widget, meter, char)
    puts ">>> keyPressed"
end

# This gets called everytime our widget is clicked.
# Notes:
#   widget = reference to our widget
#   x = x position (relative to our widget)
#   y = y position (relative to our widget)
#   botton = button clicked:
#                    1 = Left Mouse Button
#                    2 = Middle Mouse Button
#                    3 = Right Mouse Button, but this will never happen
#                        because the right mouse button brings up the
#                        Karamba menu.
#                    4,5 = Scroll wheel up and down
def widgetClicked(widget, x, y, button)
    puts ">>> widgetClicked"
end

# This gets called everytime our widget is clicked.
# Notes
#   widget = reference to our widget
#   x = x position (relative to our widget)
#   y = y position (relative to our widget)
#   button = button being held:
#                    0 = No Mouse Button
#                    1 = Left Mouse Button
#                    2 = Middle Mouse Button
#                    3 = Right Mouse Button, but this will never happen
#                        because the right mouse button brings up the
#                        Karamba menu.
#                    4,5 = Scroll wheel up and down
def widgetMouseMoved(widget, x, y, button)
    puts ">>> widgetMouseMoved"
end

# This gets called when a new program is LOADING in KDE.  When it is done
# loading, startupRemoved() is called, followed by taskAdded().
#   widget = reference to your widget
#   task = A refence to the task that is starting.  
def startupAdded(widget, startup)
    puts ">>> startupAdded"
end

# This gets called when a new program is done LOADING in KDE.
#   widget = reference to your widget
#   task = A refence to the task that just finished loading.  
def startupRemoved(widget, startup)
    puts ">>> startupRemoved"
end

# This is called every time a new task (program) is started in KDE.
#   widget = reference to your widget
#   task = A refence to the new task.  Call getTaskInfo() with this reference
#          to get the name, etc of this new task.
def taskAdded(widget, task)
    puts ">>> taskAdded"
end

# This is called everytime a task (program) is closed in KDE.
#   widget = reference to your widget
#   task = A refence to the task.
def taskRemoved(widget, task)
    puts ">>> taskRemoved"
end

# This is called everytime a different task gains focus (IE, the user clicks
# on a different window).  
#   widget = reference to your widget
#   task = A refence to the task.  Call getTaskInfo() with this reference
#          to get the name, etc of this new task.
def activeTaskChanged(widget, task)
    puts ">>> activeTaskChanged"
end

# This is called everytime the systray you created with createSystray() is updated
def systrayUpdated(widget)
    puts ">>> systrayUpdated"
end

# This is called everytime the current desktop changes
#   widget = reference to your widget
#   desktop = the current desktop
def desktopChanged(widget, desktop)
    puts ">>> desktopChanged"
end

# This is called everytime the wallpaper changes on a desktop 
#   widget = reference to your widget
#   desktop = the desktop whose wallpaper changed
def wallpaperChanged(widget, desktop)
    puts ">>> wallpaperChanged"
end

# This will be printed when the widget loads.
puts "Loaded my ruby extension!"
