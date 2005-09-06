#Unicode Example

#For Symbol Tables look at www.unicode.org/charts/

hidden = 0

penguin = 0
penguin_hidden = 1

#this import statement allows access to the karamba functions
import karamba

#this is called when you widget is initialized
def initWidget(widget):
    global richtext
    global penguin

    karamba.resizeWidget(widget, 360, 520)

    penguin = karamba.createImage(widget, 250, 150, karamba.getThemePath(widget) + "/penguin_executive.png")
    karamba.hideImage(widget, penguin)


    text = """
<h1>Richtext Example</h1>
<p>A rich text object allows to display a string interpreted as rich text.
To create a rich text object use the command:<br> <p align="center">karamba.createRichText(widget, text)</p>
A simple subset of <a href="kfmclient openURL http://www.selfhtml.org"><font color="black"><i>HTML</i></font></a>-tags
is used to encode the formatting commands.</p>
<p><font size=+2>Some features:</font>
<ul>
<li>Numbered and unnumbered lists</li>
<li>Inline Images <img src=\"""" + karamba.getThemePath(widget) + u"""info.png\"</li>
<li>Various <font color="red">different</font><font color="blue"> text</font><font color="green"> colours</font></li>
<li>Hyperlinks: <a href="kfmclient openURL http://netdragon.sourceforge.net"> Superkaramba Homepage</a></li>
<li>Links can also <a href="#trigger">trigger</a> actions in the script</li>
<li><i>Various</i> <b>different</b> <u>caracter</u> <s>styles</s></li>
<li><a href="kfmclient openURL http://www.unicode.org">Unicode</a>: \u03B6 \u03B3 \u03BB \u03A3 \u03A9 </li>
<li>Simple Tables:
<table bgcolor="darkgray" border="1" width="80%">
<tr><th colspan="3">Header</th></tr>
<tr><td>Cell (1,1)</td><td>Cell (1,2)</td><td>Cell (1,3)</td><tr>
<tr><td>Cell (2,1)</td><td>Cell (2,2)</td><td>Cell (2,3)</td><tr>
</table></li>
</ul>
For a complete documentation of all supported tags look at:
<a href="kfmclient openURL http://doc.trolltech.com/3.0/qstylesheet.html">
http://doc.trolltech.com/3.0/qstylesheet.html</a>.
</p><br>
"""

    richtext = karamba.createRichText(widget, text)

    karamba.moveRichText(widget, richtext, 10, 10)

    print "richText Size = ", karamba.getRichTextSize(widget, richtext)

    karamba.setRichTextWidth(widget, richtext, 345)

    #karamba.deleteRichText(widget, richtext)

    karamba.redrawWidget(widget)




#this is called everytime your widget is updated
#the update inverval is specified in the .theme file
def widgetUpdated(widget):
    global hidden
    global richtext

    #if hidden == 0:
    #    hidden = 1
    #    karamba.hideRichText(widget, richtext)
    #else:
    #    hidden = 0
    #    karamba.showRichText(widget, richtext)


#This gets called everytime our widget is clicked.
#Notes:
#  widget = reference to our widget
#  x = x position (relative to our widget)
#  y = y position (relative to our widget)
#  botton = button clicked:
#                    1 = Left Mouse Button
#                    2 = Middle Mouse Button
#                    3 = Right Mouse Button, but this will never happen
#                        because the right mouse button brings up the
#                        Karamba menu.
#                    4,5 = Scroll wheel up and down
def widgetClicked(widget, x, y, button):
    global richtext
    global hidden

    #hidden = 0
    #karamba.showRichText(widget, richtext)

    #karamba.changeRichText(widget, richtext, "hihi\n    huhu")

def meterClicked(widget, meter, button):
    global penguin
    global penguin_hidden
    global richtext

    print "Meter clicked", meter
    if meter == "trigger":
        if penguin_hidden:
            karamba.showImage(widget, penguin)
            penguin_hidden = 0
        else:
            karamba.hideImage(widget, penguin)
            penguin_hidden = 1
    if meter == "delete":
        karamba.deleteRichText(widget, richtext)

    karamba.redrawWidget(widget)

#This gets called everytime our widget is clicked.
#Notes
#  widget = reference to our widget
#  x = x position (relative to our widget)
#  y = y position (relative to our widget)
#  botton = button being held:
#                    0 = No Mouse Button
#                    1 = Left Mouse Button
#                    2 = Middle Mouse Button
#                    3 = Right Mouse Button, but this will never happen
#                        because the right mouse button brings up the
#                        Karamba menu.
def widgetMouseMoved(widget, x, y, button):
    pass

# This will be printed when the widget loads.
print "Loaded Karamba Unicode Test"
