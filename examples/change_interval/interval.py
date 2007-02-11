# 
# Written by Luke Kenneth Casson Leighton <lkcl@lkcl.net>

# This theme is demonstrates the interval timer changing.

#this import statement allows access to the karamba functions
import karamba

seq = 0
text = None

#this is called when you widget is initialized
def initWidget(widget):

	karamba.redrawWidget(widget)

# sequence drops down to zero and changes the time interval to 1 second.
# keeps counting down thereafter...

def widgetUpdated(widget):
	global seq
	global text

	seq -= 1

	if seq <= 0:
		karamba.changeInterval(widget, 1000)

	if seq <= 0:
		message = "biding-time seq:%d" % -seq
	else:
		message = "wiggle seq:%d" % seq

	# delete previous text if exists.
	if text is not None:
		karamba.deleteText(widget, text)

	# display new message
	text = karamba.createText(widget, 0, 20, 300, 20, message)
	karamba.changeTextColor(widget, text, 252,252,252)

	karamba.redrawWidget(widget)

# wiggle the mouse over the widget to get the sequence number increased more.
# also set the refresh rate to 50ms so that the theme has to fight against
# the wiggling.

def widgetMouseMoved(widget, x, y, button):
	global seq

	if seq <= 0:
		seq = 0
		karamba.changeInterval(widget, 50)

	seq += 1
	

# This will be printed when the widget loads.
print "Loaded my python extension!"

