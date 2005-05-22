#Unicode Example

#For Symbol Tables look at www.unicode.org/charts/


#this import statement allows access to the karamba functions
import karamba

#this is called when you widget is initialized
def initWidget(widget):

    text=u"""
<h1>UNICODE Example</h1>
<h3>Greek:</h3>
\u03B1\u03B2\u03B3\u03B4\u03B5\u03B6\u03B7\u03B8\u03B9\u03BA\u03BB\u03BC\u03BD\u03BE\u03BF\u03C0
\u03C1\u03C2\u03C3\u03C4\u03C5\u03C6\u03C7\u03C8\u03C9
<br>
\u0391\u0392\u0393\u0394\u0395\u0396\u0397\u0398\u0399\u039A\u039B\u039C\u039D\u039E\u039F\u03A0
\u03A1\u03A3\u03A4\u03A5\u03A6\u03A7\u03A8\u03A9
<h3>Cyrillic:</h3>
\u0430\u0431\u0432\u0433\u0434\u0435\u0436\u0437\u0438\u0439\u043A\u043B\u043C\u043D\u043E\u043F
\u0440\u0441\u0442\u0443\u0444\u0445\u0446\u0447\u0448\u0449\u044A\u044B\u044C\u044D\u044E\u044F
<br>
\u0410\u0411\u0412\u0413\u0414\u0415\u0416\u0417\u0418\u0419\u041A\u041B\u041C\u041D\u041E\u041F
\u0420\u0421\u0422\u0423\u0424\u0425\u0426\u0427\u0428\u0429\u042A\u042B\u042C\u042D\u042E\u042F
<h3>Hebrew:</h3>
\u05D0\u05D1\u05D2\u05D3\u05D4\u05D5\u05D6\u05D7\u05D8\u05D9\u05DA\u05DB\u05DC\u05DD\u05DE\u05DF
\u05E0\u05E1\u05E2\u05E3\u05E4\u05E5\u05E6\u05E7\u05E8\u05E9\u05EA
<h3>Arabic:</h3>
\u0621\u0622\u0623\u0624\u0625\u0626\u0627\u0628\u0629\u062A\u062B\u062C\u062D\u062E\u062F\u0630
\u0631\u0632\u0633\u0634\u0635\u0636\u0637\u0638\u0639\u063A\u0641\u0642\u0643\u0644\u0645\u0646
\u0647\u0648\u064A
<p>
For a complete reference on unicode caracter codes look at:
<center><a href="kfmclient openURL http://www.unicode.org/charts/">www.unicode.org/charts/</a></center>
</p>
"""
    karamba.resizeWidget(widget, 400, 400)

    richtext = karamba.createRichText(widget, text)

    karamba.setRichTextWidth(widget, richtext, 370)

#    greek_heading = karamba.createText(widget, 0, 5, 100, 20, "Greek:")
#    greek_small = karamba.createText(widget, 0, 25, 400, 15, u"\u03B1\u03B2\u03B3\u03B4\u03B5\u03B6\u03B7\u03B8\u03B9\u03BA\u03BB\u03BC\u03BD\u03BE\u03BF\u03C0\u03C1\u03C2\u03C3\u03C4\u03C5\u03C6\u03C7\u03C8\u03C9");
#    greek_big = karamba.createText(widget, 0, 40, 400, 15, u"\u0391\u0392\u0393\u0394\u0395\u0396\u0397\u0398\u0399\u039A\u039B\u039C\u039D\u039E\u039F\u03A0\u03A1\u03A3\u03A4\u03A5\u03A6\u03A7\u03A8\u03A9");

#    hebrew_heading = karamba.createText(widget, 0, 60, 100, 20, "Hebrew:")
#    hebrew = karamba.createText(widget, 0, 80, 400, 15, u"\u05D0\u05D1\u05D2\u05D3\u05D4\u05D5\u05D6\u05D7\u05D8\u05D9\u05DA\u05DB\u05DC\u05DD\u05DE\u05DF\u05E0\u05E1\u05E2\u05E3\u05E4\u05E5\u05E6\u05E7\u05E8\u05E9\u05EA");

#    arabic_heading = karamba.createText(widget, 0, 100, 100, 20, "Arabic:")
#    arabic = karamba.createText(widget, 0, 120, 400, 15, u"\u0621\u0622\u0623\u0624\u0625\u0626\u0627\u0628\u0629\u062A\u062B\u062C\u062D\u062E\u062F\u0630\u0631\u0632\u0633\u0634\u0635\u0636\u0637\u0638\u0639\u063A\u0641\u0642\u0643\u0644\u0645\u0646\u0647\u0648\u064A");

#    cyrillic_heading = karamba.createText(widget, 0, 140, 100, 20, "Cyrillic:")
#    cyrillic_small = karamba.createText(widget, 0, 160, 400, 15, u"\u0430\u0431\u0432\u0433\u0434\u0435\u0436\u0437\u0438\u0439\u043A\u043B\u043C\u043D\u043E\u043F\u0440\u0441\u0442\u0443\u0444\u0445\u0446\u0447\u0448\u0449\u044A\u044B\u044C\u044D\u044E\u044F")
#    cyrillic_big = karamba.createText(widget, 0, 175, 400, 15, u"\u0410\u0411\u0412\u0413\u0414\u0415\u0416\u0417\u0418\u0419\u041A\u041B\u041C\u041D\u041E\u041F\u0420\u0421\u0422\u0423\u0424\u0425\u0426\u0427\u0428\u0429\u042A\u042B\u042C\u042D\u042E\u042F")

#    karamba.changeTextColor(widget, greek_heading, 0, 0, 0)
#    karamba.changeTextColor(widget, greek_small, 0, 0, 0)
#    karamba.changeTextColor(widget, greek_big, 0, 0, 0)
#    karamba.changeTextColor(widget, hebrew_heading, 0, 0, 0)
#    karamba.changeTextColor(widget, hebrew, 0, 0, 0)
#    karamba.changeTextColor(widget, arabic_heading, 0, 0, 0)
#    karamba.changeTextColor(widget, arabic, 0, 0, 0)
#    karamba.changeTextColor(widget, cyrillic_heading, 0, 0, 0)
#    karamba.changeTextColor(widget, cyrillic_small, 0, 0, 0)
#    karamba.changeTextColor(widget, cyrillic_big, 0, 0, 0)

#    karamba.changeTextSize(widget, greek_heading, 12)
#    karamba.changeTextSize(widget, hebrew_heading, 12)
#    karamba.changeTextSize(widget, arabic_heading, 12)
#    karamba.changeTextSize(widget, cyrillic_heading, 12)



#this is called everytime your widget is updated
#the update inverval is specified in the .theme file
def widgetUpdated(widget):
    pass


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
    pass

def meterClicked(widget, meter, button):
    pass
    
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
