#!/usr/bin/env superkaramba
# coding: utf-8

import karamba, Kross
khtml_part = None

def initWidget(widget):
    forms = Kross.module("forms")
    frame = forms.createWidget("QWidget")
    forms.createLayout(frame, "QHBoxLayout")
    khtml_part = forms.loadPart(frame, "libkhtmlpart")
    khtml_part.javaScriptEnabled = True
    khtml_part.javaEnabled = False
    khtml_part.pluginsEnabled = False
    proxywidget = karamba.createCanvasWidget(widget, frame)
    def selectionChanged():
        print khtml_part.executeScript("window.scrollBy(0,50);");
    khtml_part.connect("selectionChanged()", selectionChanged)
    khtml_part.openUrl("http://www.kde.org")

def widgetClosed(widget):
    if khtml_part:
        khtml_part.delayedDestruct()
