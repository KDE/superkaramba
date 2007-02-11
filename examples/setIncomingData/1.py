import karamba

sequence_num = 0

def initWidget(widget):
    karamba.openTheme('2.theme')

# pass over a sequence number as well - the reason is to
# make it possible to identify the clicks uniquely.
# 
# unfortunately, the recipient has to do "polling"
# by calling getIncomingData - see 2.py for more info

def widgetClicked(widget, x, y, button):

    # do as you wish, but this is a good way:
    # compact your stuff with repr() it can cope
    # with pretty much any basic types - dictionaries, lists -
    # and then use eval() at the other end.

    global sequence_num
    sequence_num += 1

    vars = (sequence_num, x, y, button)
    message = repr(vars)
    karamba.setIncomingData(widget, "2", message)

