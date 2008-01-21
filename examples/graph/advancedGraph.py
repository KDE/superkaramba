#this import statement allows access to the karamba functions
import karamba

graphs = [0,0,0,0]
labels = [0,0,0,0]
counter = 0
graphValue = 0
incr = 5

#this is called when you widget is initialized
def initWidget(widget):
    global graphs, labels
    graphs[0] = karamba.getThemeGraph(widget, "graphLeft")
    graphs[1] = karamba.getThemeGraph(widget, "graphDown")
    graphs[2] = karamba.getThemeGraph(widget, "graphFilled")
    graphs[3] = karamba.getThemeGraph(widget, "graphMin")

    labels[0] = karamba.getThemeText(widget, "downValue")
    labels[1] = karamba.getThemeText(widget, "leftValue")
    labels[2] = karamba.getThemeText(widget, "filledValue")
    labels[3] = karamba.getThemeText(widget, "minValue")

    # most of these values are already present in the theme
    # these are here to show examples of how to work with the
    # advanced graphing attributes in python

    # set fill color for last two graphs
    karamba.setGraphFillColor(widget, graphs[2], 0,150,255,100)
    karamba.setGraphFillColor(widget, graphs[3], 0,150,255,100)

    # setting shouldFill only available in python but it is not 
    # needed if the fill color is set. This call allows the fill
    # to be easily toggled programmatically.
    karamba.setGraphShouldFill(widget, graphs[0], 0)
    karamba.setGraphShouldFill(widget, graphs[1], 0)
    karamba.setGraphShouldFill(widget, graphs[2], 1)
    karamba.setGraphShouldFill(widget, graphs[3], 1)
    
    karamba.setGraphPlot(widget, graphs[0], "up")
    karamba.setGraphPlot(widget, graphs[1], "down")
    karamba.setGraphPlot(widget, graphs[2], "up")
    karamba.setGraphPlot(widget, graphs[3], "up")

    karamba.setGraphScroll(widget, graphs[0], "left")
    karamba.setGraphScroll(widget, graphs[1], "right")
    karamba.setGraphScroll(widget, graphs[2], "right")
    karamba.setGraphScroll(widget, graphs[3], "right")


#this is called everytime your widget is updated
#the update inverval is specified in the .theme file
def widgetUpdated(widget):
    global graphs, labels, counter, incr

    if (counter <=0):
        incr = 5
    if (counter >= 100):
        incr = -5
    
    counter = counter+incr
    # show examples of negative values
    graphValue = counter - 50
        
    karamba.setGraphValue(widget, graphs[0], counter)
    karamba.changeText(widget, labels[0], str(counter))

    karamba.setGraphValue(widget, graphs[1], counter)
    karamba.changeText(widget, labels[1], str(counter))

    karamba.setGraphValue(widget, graphs[2], counter)
    karamba.changeText(widget, labels[2], str(counter))

    karamba.setGraphValue(widget, graphs[3], graphValue)
    karamba.changeText(widget, labels[3], str(graphValue))
    
def widgetClicked(widget, x, y, button):
    pass

def widgetMouseMoved(widget, x, y, button):
    pass

# This will be printed when the widget loads.
print "Loaded Graph test python extension!"
