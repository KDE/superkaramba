#this import statement allows access to the karamba functions
import karamba

graphs = [0,0,0,0,0,0,0,0]
b = 0

#this is called when you widget is initialized
def initWidget(widget):
    global graphs
    graphs[0] = karamba.getThemeGraph(widget, "graph0")
    graphs[1] = karamba.getThemeGraph(widget, "graph1")
    graphs[2] = karamba.getThemeGraph(widget, "graph2")
    graphs[3] = karamba.getThemeGraph(widget, "graph3")
    graphs[4] = karamba.getThemeGraph(widget, "graph4")
    graphs[5] = karamba.getThemeGraph(widget, "graph5")
    graphs[6] = karamba.getThemeGraph(widget, "graph6")
    graphs[7] = karamba.getThemeGraph(widget, "graph7")
        

#this is called everytime your widget is updated
#the update inverval is specified in the .theme file
def widgetUpdated(widget):
    global graphs, b

    b = (b+1)%2
        
    # Create & delete
    if(graphs[0]):
      karamba.deleteGraph(widget, graphs[0])
      graphs[0] = 0
      print "Deleted graph."
    else:
      graphs[0] = karamba.createGraph(widget, 0, 20, 400, 30, 400)
      print "Created graph: " + str(graphs[0])
      
    # size & resize
    size = karamba.getGraphSize(widget, graphs[1])
    print "getGraphSize: " + str(size)
    size = ((b * 200) + 200, size[1])    
    karamba.resizeGraph(widget, graphs[1], size[0], size[1])
    
    # pos & move
    pos = karamba.getGraphPos(widget, graphs[2])
    print "getGraphPos: " + str(pos)
    pos = (b * 200, pos[1])    
    karamba.moveGraph(widget, graphs[2], pos[0], pos[1])
    
    # Hide & Show
    if(b):
      karamba.hideGraph(widget, graphs[3])
    else:
      karamba.showGraph(widget, graphs[3])
    
    # Sensor
    sensor = karamba.getGraphSensor(widget, graphs[4])
    print "getSensor: " + str(sensor)
    if(b):
      karamba.setGraphSensor(widget, graphs[4], 'SENSOR=NETWORK FORMAT="%in"')
    else:
      karamba.setGraphSensor(widget, graphs[4], 'SENSOR=CPU')
      
    # Min Max
    minmax = karamba.getGraphMinMax(widget, graphs[5])
    print "getGraphMinMax: " + str(minmax)
    minmax = (0, (b * 25) + 25)    
    karamba.setGraphMinMax(widget, graphs[5], minmax[0], minmax[1])
    
    # Value
    v = karamba.getGraphValue(widget, graphs[6])
    print "getGraphValue: ", v
    v = (v + 1) % 30
    karamba.setGraphValue(widget, graphs[6], v)
    
    # Color
    c = karamba.getGraphColor(widget, graphs[7])
    print "getGraphColor: ", c
    r = (c[0] + 10) % 255
    g = (c[1] + 10) % 255
    bl = (c[2] + 10) % 255
    karamba.setGraphColor(widget, graphs[7], r, g, bl)
    
def widgetClicked(widget, x, y, button):
    pass

def widgetMouseMoved(widget, x, y, button):
    pass

# This will be printed when the widget loads.
print "Loaded Graph test python extension!"
