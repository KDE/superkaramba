# 
# Written by Luke Kenneth Casson Leighton <lkcl@lkcl.net>

#this import statement allows access to the karamba functions
import karamba

do_nothing_txt = None
mgmt_txt = None


#this is called when you widget is initialized
def initWidget(widget):
	global do_nothing_txt
	global do_something_txt

	# display new message
	do_nothing_txt = karamba.createText(widget, 0, 00, 300, 20,
	                                   "Right mouse click me!")
	karamba.changeTextColor(widget, do_nothing_txt, 252,252,252)
	mgmt_txt = karamba.createText(widget, 0, 20, 300, 20,
	                                   "Righ mouse click me too!")
	karamba.changeTextColor(widget, mgmt_txt, 252,252,252)

	karamba.redrawWidget(widget)

	karamba.setWantRightButton(widget, 1)

def widgetUpdated(widget):
	karamba.redrawWidget(widget)

def widgetClicked(widget, x, y, button):
	global do_nothing_txt

	if y < 20:
		if do_nothing_txt is not None:
			karamba.deleteText(widget, do_nothing_txt)
			do_nothing_txt = karamba.createText(widget,
			        0, 0, 300, 20, "I don't do anything when clicking here.")
			karamba.changeTextColor(widget, do_nothing_txt,
			                         255,200,200)
			karamba.redrawWidget(widget)
			return

	karamba.managementPopup(widget)
	

# This will be printed when the widget loads.
print "Loaded my python extension!"

