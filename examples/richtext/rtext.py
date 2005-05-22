#!/usr/bin/python
# -*- coding: latin-1 -*-
#this import statement allows access to the karamba functions
import karamba

align = ['LEFT', 'CENTER', 'RIGHT']
texts = [0,0,0,0,0,0,0,0,0,0,0]
b = 0
a = 0

#this is called when you widget is initialized
def initWidget(widget):
    global texts, b
    texts[0] = karamba.getThemeRichText(widget, "text0")
    texts[1] = karamba.getThemeRichText(widget, "text1")
    texts[2] = karamba.getThemeRichText(widget, "text2")
    texts[3] = karamba.getThemeRichText(widget, "text3")
    texts[4] = karamba.getThemeRichText(widget, "text4")
    texts[5] = karamba.getThemeRichText(widget, "text5")
    texts[7] = karamba.getThemeRichText(widget, "text7")
    texts[9] = karamba.getThemeRichText(widget, "text9")


#this is called everytime your widget is updated
#the update inverval is specified in the .theme file
def widgetUpdated(widget):
    global texts, b, a

    b = (b+1)%2
    text = "Unicode text: Ähtärissä on Öljyä"

    # Create & delete
    if(texts[0]):
      karamba.deleteRichText(widget, texts[0])
      texts[0] = 0
    else:
      texts[0] = karamba.createRichText(widget, text)
      karamba.moveRichText(widget, texts[0], 0, 20)
      karamba.resizeRichText(widget, texts[0], 200, 20)
      pos = karamba.getRichTextPos(widget, texts[0])
      print "--getRichTextPos: " + str(pos)
      size = karamba.getRichTextSize(widget, texts[0])
      print "--getRichTextSize: " + str(size)

    # size & resize
    size = karamba.getRichTextSize(widget, texts[1])
    print "getRichTextSize: " + str(size)
    size = ((b * 200) + 200, size[1])
    karamba.resizeRichText(widget, texts[1], size[0], size[1])

    # pos & move
    pos = karamba.getRichTextPos(widget, texts[2])
    print "getRichTextPos: " + str(pos)
    pos = (b * 200, pos[1])
    karamba.moveRichText(widget, texts[2], pos[0], pos[1])

    # Hide & Show
    if(b):
      karamba.hideRichText(widget, texts[3])
    else:
      karamba.showRichText(widget, texts[3])

    # Sensor
    sensor = karamba.getRichTextSensor(widget, texts[4])
    print "getSensor: " + str(sensor)
    if(b):
      karamba.setRichTextSensor(widget, texts[4], 'SENSOR=SENSOR TYPE="cpu_temp"')
    else:
      karamba.setRichTextSensor(widget, texts[4], 'SENSOR=CPU')

    # Value
    v = karamba.getRichTextValue(widget, texts[5])
    print "getRichTextValue: ", v
    v += '.'
    karamba.changeRichText(widget, texts[5], v)

    # Font size
    v = karamba.getRichTextFontSize(widget, texts[7])
    print "getRichTextFontSize: ", v
    v = 10 + ((v-10)+1)%10;
    karamba.changeRichTextSize(widget, texts[7], v)

    # RichText Font
    v = karamba.getRichTextFont(widget, texts[9])
    print "getRichTextFont: ", v
    if(b):
      v = 'Bitstream Vera Sans'
    else:
      v = 'Bitstream Vera Serif'
    karamba.changeRichTextFont(widget, texts[9], v)

def widgetClicked(widget, x, y, button):
    pass

def widgetMouseMoved(widget, x, y, button):
    pass

# This will be printed when the widget loads.
print "Loaded RichText test python extension!"
