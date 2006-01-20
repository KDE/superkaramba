# 
# Written by Luke Kenneth Casson Leighton <lkcl@lkcl.net>

# This theme is demonstrates how to 

#this import statement allows access to the karamba functions
import karamba

drop_txt = None

#this is called when you widget is initialized
def initWidget(widget):

    # this resets the text to "" so we know we've never
    # received anything yet from the other theme
    name = karamba.getPrettyThemeName(widget)
    print "2.py name: ", name
    karamba.setIncomingData(widget, name, "")

    karamba.redrawWidget(widget)

# this is a pain.  in order to avoid memory-related threading problems,
# and also locking, with the communication between themes, the
# communication is done asynchronously.  so you have to POLL for the
# information, by reading getIncomingData().
# 
# therefore, you must set an interval=time_in_ms in your receiving .theme
# file (see 2.theme) and then call getIncomingData() in here.
#
# it's not ideal - but it works.
#
# NOTE: the data received - by getIncomingData - is NOT, i repeat NOT
# deleted when you call getIncomingData.
# so, obviously, you need to take care to not activate repeatedly.
# you could do this in several ways.  one of them is to send, in
# the calling theme (the one that calls setIncomingData) a sequential
# number as part of the message.  
#
# alternatively, you could reset the text to "" (see above) 
 

expected_seq = 0

def widgetUpdated(widget):

    global expected_seq # i hate globals.  please write better code than this example.

    # get the "message"...
    disp = karamba.getIncomingData(widget)
    if disp == "":
        return

    # decode it...
    (seq, x, y, button) = eval(disp)

    # if it's been seen before, skip it...
    if seq <= expected_seq:
        pass

    expected_seq += 1

    message = "seq:%d x:%d y:%d btn:%d" % (seq, x, y, button)
    # delete previous text if exists.
    global drop_txt
    if drop_txt is not None:
        karamba.deleteText(widget, drop_txt)

    # display it...
    drop_txt = karamba.createText(widget, 0, 20, 300, 20, message)
    karamba.changeTextColor(widget, drop_txt, 252,252,252)

    karamba.redrawWidget(widget)

    pass

# This will be printed when the widget loads.
print "Loaded my python 2.py extension!"

