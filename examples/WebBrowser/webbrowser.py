#!/usr/bin/env superkaramba
# coding: utf-8

import karamba, Kross
khtml_part = None

def initWidget(widget):
    # the Kross forms module does provide methods to create
    # widgets for us. You are for example able to load a UI
    # file, create layouts or just any other supported widget.
    forms = Kross.module("forms")
    frame = forms.createWidget("QWidget")
    forms.createLayout(frame, "QHBoxLayout")
    # here we load the KHTML KPart that is our full powered
    # webbrowser widget.
    khtml_part = forms.loadPart(frame, "libkhtmlpart")
    khtml_part.javaScriptEnabled = True
    khtml_part.javaEnabled = False
    khtml_part.pluginsEnabled = False
    # now we add the frame to our widget and will earn a
    # proxywidget for it.
    proxywidget = karamba.createCanvasWidget(widget, frame)
    # the following lines demonstrate, how to control the
    # webbrowser using the JavaScript language. The KHTML
    # KPart does provide different signals we are able to
    # connect our own functions too. Just see here the
    # kdelibs/khtml/khtmlpart.h file.
    def selectionChanged():
        print khtml_part.executeScript("window.scrollBy(0,50);");
    khtml_part.connect("selectionChanged()", selectionChanged)
    # and finally we like to load an url and display something.
    khtml_part.openUrl("http://www.kde.org")
 
def widgetClosed(widget):
    # if the job is done, we may like to free our KPart again.
    if khtml_part:
        khtml_part.delayedDestruct()
