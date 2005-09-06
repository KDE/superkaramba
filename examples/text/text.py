#this import statement allows access to the karamba functions
import karamba

align = ['LEFT', 'CENTER', 'RIGHT']
texts = [0,0,0,0,0,0,0,0,0,0,0]
b = 0
a = 0

#this is called when you widget is initialized
def initWidget(widget):
    global texts, b
    texts[0] = karamba.getThemeText(widget, "text0")
    texts[1] = karamba.getThemeText(widget, "text1")
    texts[2] = karamba.getThemeText(widget, "text2")
    texts[3] = karamba.getThemeText(widget, "text3")
    texts[4] = karamba.getThemeText(widget, "text4")
    texts[5] = karamba.getThemeText(widget, "text5")
    texts[6] = karamba.getThemeText(widget, "text6")
    texts[7] = karamba.getThemeText(widget, "text7")
    texts[8] = karamba.getThemeText(widget, "text8")
    texts[9] = karamba.getThemeText(widget, "text9")
    texts[10] = karamba.getThemeText(widget, "text10")
        

#this is called everytime your widget is updated
#the update inverval is specified in the .theme file
def widgetUpdated(widget):
    global texts, b, a
    
    b = (b+1)%2

    # Create & delete
    if(texts[0]):
      karamba.deleteText(widget, texts[0])
      texts[0] = 0
    else:
      texts[0] = karamba.createText(widget, 0, 20, 200, 20, "Text meter")
      
    # size & resize
    size = karamba.getTextSize(widget, texts[1])
    print "getTextSize: " + str(size)
    size = ((b * 200) + 200, size[1])    
    karamba.resizeText(widget, texts[1], size[0], size[1])
    
    # pos & move
    pos = karamba.getTextPos(widget, texts[2])
    print "getTextPos: " + str(pos)
    pos = (b * 200, pos[1])    
    karamba.moveText(widget, texts[2], pos[0], pos[1])
    
    # Hide & Show
    if(b):
      karamba.hideText(widget, texts[3])
    else:
      karamba.showText(widget, texts[3])
    
    # Sensor
    sensor = karamba.getTextSensor(widget, texts[4])
    print "getSensor: " + str(sensor)
    if(b):
      karamba.setTextSensor(widget, texts[4], 'SENSOR=SENSOR TYPE="cpu_temp"')
    else:
      karamba.setTextSensor(widget, texts[4], 'SENSOR=CPU')
      
    # Value
    v = karamba.getTextValue(widget, texts[5])
    print "getTextValue: ", v
    v += '.'
    karamba.changeText(widget, texts[5], v)
    
    # Shadow
    v = karamba.getTextShadow(widget, texts[6])
    print "getTextShadow: ", v
    v = (v+1)%10;
    karamba.changeTextShadow(widget, texts[6], v)
    
    # Font size
    v = karamba.getTextFontSize(widget, texts[7])
    print "getTextFontSize: ", v
    v = 10 + ((v-10)+1)%10;
    karamba.changeTextSize(widget, texts[7], v)
    
    # Text color
    v = karamba.getTextColor(widget, texts[8])
    print "getTextColor: ", v
    karamba.changeTextColor(widget, texts[8], b*255, b*255, b*255)

    # Text Font
    v = karamba.getTextFont(widget, texts[9])
    print "getTextFont: ", v
    if(b):
      v = 'Bitstream Vera Sans'
    else:
      v = 'Bitstream Vera Serif'
    karamba.changeTextFont(widget, texts[9], v)

    # Text Alignment
    a = (a+1)%3
    v = karamba.getTextAlign(widget, texts[10])
    print "getTextAlign: ", v
    karamba.setTextAlign(widget, texts[10], align[a])
       
def widgetClicked(widget, x, y, button):
    pass

def widgetMouseMoved(widget, x, y, button):
    pass

# This will be printed when the widget loads.
print "Loaded Text test python extension!"
