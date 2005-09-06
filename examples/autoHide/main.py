#this import statement allows access to the karamba functions
import karamba

hidden = 0
counter = 0

def initWidget(widget):
    global hidden
    hidden = 0

def widgetUpdated(widget):
    global hidden
    global counter
    if hidden == 0:
        counter = counter + 1

    if (counter > 5):
        hidden = 1
        karamba.moveWidget(widget, 0, -210)
        

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
    global hidden
    global counter
    if (hidden==1):
        karamba.moveWidget(widget, 0, 0)
    hidden = 0
    counter = 0
        


# This will be printed when the widget loads.
print "Loaded my python extension!"

