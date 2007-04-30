require 'karamba'

# this is called when your widget is initialized
def initWidget(widget)
    puts ">>> initWidget"
    puts Karamba.getThemePath()
    Karamba.resizeWidget(widget, 300, 120)
    @richtext = Karamba.createRichText(widget, Time.now.to_s)
    Karamba.moveRichText(widget, @richtext, 10, 10)
    print "richText Size = ", Karamba.getRichTextSize(widget, @richtext)
    Karamba.setRichTextWidth(widget, @richtext, 280)
    #Karamba.deleteRichText(widget, @richtext)
    Karamba.redrawWidget(widget)
end

# this is called everytime your widget is updated
# the update inverval is specified in the .theme file
def widgetUpdated(widget)
    puts ">>> widgetUpdated"
    Karamba.changeRichText(widget, @richtext, Time.now.to_s)
    Karamba.redrawWidget(widget)
end
