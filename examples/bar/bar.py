#this import statement allows access to the karamba functions
import karamba

bars = [0,0,0,0,0,0,0,0]
b = 0

#this is called when you widget is initialized
def initWidget(widget):
    global bars, b
    bars[0] = karamba.getThemeBar(widget, "bar0")
    bars[1] = karamba.getThemeBar(widget, "bar1")
    bars[2] = karamba.getThemeBar(widget, "bar2")
    bars[3] = karamba.getThemeBar(widget, "bar3")
    bars[4] = karamba.getThemeBar(widget, "bar4")
    bars[5] = karamba.getThemeBar(widget, "bar5")
    bars[6] = karamba.getThemeBar(widget, "bar6")
    bars[7] = karamba.getThemeBar(widget, "bar7")
        

#this is called everytime your widget is updated
#the update inverval is specified in the .theme file
def widgetUpdated(widget):
    global bars, b
    
    # vertical & bitmap
    b = karamba.getBarVertical(widget, bars[7])
    print "getVertical: " + str(b)
    bmp = karamba.getBarImage(widget, bars[7])
    print "getBitmap: " + str(bmp)
    b = (b+1)%2
    karamba.setBarVertical(widget, bars[7], b)
    if(b):
      karamba.setBarImage(widget, bars[7], "ver.png")
      karamba.resizeBar(widget, bars[7], 20, 200)
    else:
      karamba.setBarImage(widget, bars[7], "hor.png")
      karamba.resizeBar(widget, bars[7], 200, 20)

    # size & resize
    size = karamba.getBarSize(widget, bars[1])
    print "getBarSize: " + str(size)
    size = ((b * 100) + 100, size[1])    
    karamba.resizeBar(widget, bars[1], size[0], size[1])
    
    # pos & move
    pos = karamba.getBarPos(widget, bars[2])
    print "getBarPos: " + str(pos)
    pos = (b * 200, pos[1])    
    karamba.moveBar(widget, bars[2], pos[0], pos[1])
    
    # Hide & Show
    if(b):
      karamba.hideBar(widget, bars[3])
    else:
      karamba.showBar(widget, bars[3])
    
    # Value
    v = karamba.getBarValue(widget, bars[5])
    print "getBarValue: ", v
    v = (v + 10) % 110
    karamba.setBarValue(widget, bars[5], v)
    
    # Min Max
    minmax = karamba.getBarMinMax(widget, bars[6])
    print "getBarMinMax: " + str(minmax)
    minmax = (0, (b * 100) + 100)    
    karamba.setBarMinMax(widget, bars[6], minmax[0], minmax[1])
    
    # Sensor
    sensor = karamba.getBarSensor(widget, bars[4])
    print "getSensor: " + str(sensor)
    if(b):
      karamba.setBarSensor(widget, bars[4], 'SENSOR=SENSOR TYPE="cpu_temp"')
    else:
      karamba.setBarSensor(widget, bars[4], 'SENSOR=CPU')
      
    # Create & delete
    if(bars[0]):
      karamba.deleteBar(widget, bars[0])
      bars[0] = 0
    else:
      bars[0] = karamba.createBar(widget, 0, 20, 200, 20)
      karamba.setBarImage(widget, bars[0], "hor.png")
      karamba.setBarValue(widget, bars[0], 100)
    
def widgetClicked(widget, x, y, button):
    pass

def widgetMouseMoved(widget, x, y, button):
    pass

# This will be printed when the widget loads.
print "Loaded Bar test python extension!"
