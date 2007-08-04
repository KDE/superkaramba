#!/usr/bin/env superkaramba
# coding: utf-8

import PlasmaApplet
mousePos = []

engine = PlasmaApplet.dataEngine("time")
engine.setProperty("reportSeconds", True)

#this is called when your widget is initialized
def initWidget(widget):
    svg = PlasmaApplet.widget("Svg")
    svg.setImageFile("widgets/clock")
    svg.setContentType("ImageSet")
    svg.setSize(150.0,150.0)
    engine.connectSource("Local", svg)

    widget = PlasmaApplet.widget("Label")
    engine.connectSource("Local", widget)

    class Button:
        def __init__(self, pos, text):
            self.pos = pos
            self.text = text
            self.rect = [pos[0]-20,pos[1]-16,100,26]
            self.setSelected(False)
        def checkSelected(self):
            if len(mousePos) > 1:
                selected = False
                x = mousePos[0]
                y = mousePos[1]
                if x>self.rect[0] and y>self.rect[1] and x<self.rect[0]+self.rect[2] and y<self.rect[1]+self.rect[3]:
                    selected = True
                self.setSelected(selected)
        def setSelected(self, selected):
            self.isSelected = selected
            if selected: self.pencolor = "#ff0000"
            else: self.pencolor = "#0000ff"
        def paint(self, painter):
            painter.setColor("#aaaaff")
            painter.setPenColor(self.pencolor)
            painter.drawEllipse(self.rect)
            painter.drawText(self.pos, self.text)

    analogButton = Button([30.0,30.0], "Analog Clock")
    digitalButton = Button([150.0,30.0], "Digital Clock")
    analogButton.setSelected(True)

    def paintApplet(painter, rect):
        global mousePos
        #svg.paint( painter, svg.elementRect('ClockFace'), 'ClockFace' )

        analogButton.checkSelected()
        digitalButton.checkSelected()

        if analogButton.isSelected:
            svg.paint(painter, [50.0, 50.0])
        if digitalButton.isSelected:
            data = engine.query("Local")
            painter.setColor("#aaaaff")
            painter.setPenColor("#0000ff")
            painter.drawRect([50,80,150,36])
            painter.drawText([70.0,100.0], "%s %s" % (data["Date"],data["Time"]))

        analogButton.paint(painter)
        digitalButton.paint(painter)

    PlasmaApplet.setPainterEnabled(True)
    PlasmaApplet.connect("paint(QObject*,QRect)", paintApplet)

#This is called when your widget is closed.
def widgetClosed(widget):
    print "widgetClosed"

#This gets called everytime our widget is clicked.
def widgetClicked(widget, x, y, button):
    if button == 1 or button == 2: # left or middle click
        global mousePos
        mousePos = [x,y]
