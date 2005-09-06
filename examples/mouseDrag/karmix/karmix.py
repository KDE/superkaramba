# Karmix 0.1
# Written by Tian ( http://www.c-sait.net )
# Functionality moved by Ryan Nickell (p0z3r@mail.com)

# This theme is demonstrates how to use mouse buttons
# being dragged in a theme. 
# It shows how to use it for slider bars, but you can
# also apply this to anything you want like moving images
# and other things programmatically.

#this import statement allows access to the karamba functions
import karamba

import ossaudiodev

mixer = ""
onLeftButton = 0
onMidButton = 0
#this is called when you widget is initialized
def initWidget(widget):
    global mixer
    mixer = ossaudiodev.openmixer()

    vol_main = karamba.getThemeImage(widget, "vol_main")
    (l, r) = mixer.get(ossaudiodev.SOUND_MIXER_VOLUME)
    vol_main_value = max(l, r)
    karamba.resizeImage(widget, vol_main, vol_main_value, 10)

    vol_pcm = karamba.getThemeImage(widget, "vol_pcm")
    (l, r) = mixer.get(ossaudiodev.SOUND_MIXER_PCM)
    vol_pcm_value = max(l, r)
    karamba.resizeImage(widget, vol_pcm, vol_pcm_value, 10)

    vol_cd = karamba.getThemeImage(widget, "vol_cd")
    (l, r) = mixer.get(ossaudiodev.SOUND_MIXER_CD)
    vol_cd_value = max(l, r)
    karamba.resizeImage(widget, vol_cd, vol_cd_value, 10)

    vol_mic = karamba.getThemeImage(widget, "vol_mic")
    (l, r) = mixer.get(ossaudiodev.SOUND_MIXER_MIC)
    vol_mic_value = max(l, r)
    karamba.resizeImage(widget, vol_mic, vol_mic_value, 10)

    karamba.redrawWidget(widget)

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
    global onMidButton,onLeftButton
    global mixer
    if(button == 1 and onLeftButton == 0):
        # We are holding the left button here
        if ((y>=17) and (y<=28)):
            vol_main = karamba.getThemeImage(widget, "vol_main")
            vol_main_value = max(min(x-40,100),0)
            karamba.resizeImage(widget, vol_main, vol_main_value, 10)
            mixer.set(ossaudiodev.SOUND_MIXER_VOLUME, (vol_main_value, vol_main_value))
            karamba.redrawWidget(widget)

        if ((y>=47) and (y<=58)):
            vol_pcm = karamba.getThemeImage(widget, "vol_pcm")
            vol_pcm_value = max(min(x-40,100),0)
            karamba.resizeImage(widget, vol_pcm, vol_pcm_value, 10)
            mixer.set(ossaudiodev.SOUND_MIXER_PCM, (vol_pcm_value, vol_pcm_value))
            karamba.redrawWidget(widget)

        if ((y>=77) and (y<=88)):
            vol_cd = karamba.getThemeImage(widget, "vol_cd")
            vol_cd_value = max(min(x-40,100),0)
            karamba.resizeImage(widget, vol_cd, vol_cd_value, 10)
            mixer.set(ossaudiodev.SOUND_MIXER_CD, (vol_cd_value, vol_cd_value))
            karamba.redrawWidget(widget)

        if ((y>=107) and (y<=118)):
            vol_mic = karamba.getThemeImage(widget, "vol_mic")
            vol_mic_value = max(min(x-40,100),0)
            karamba.resizeImage(widget, vol_mic, vol_mic_value, 10)
            mixer.set(ossaudiodev.SOUND_MIXER_MIC, (vol_mic_value, vol_mic_value))
            karamba.redrawWidget(widget)
                
    if(button == 2 and onMidButton == 0):
        # We are holding the middle button here
        pass

#This gets called when an item is clicked in a popup menu you have created.
#  menu = a reference to the menu
#  id = the number of the item that was clicked.
def menuItemClicked(widget, menu, id):
    pass

#This gets called when an item is clicked in the theme CONFIGURATION menu,
#not the popup menus that you create.
#  key = the reference to the configuration key that was changed
#  value = the new value (true or false) that was selected
def menuOptionChanged(widget, key, value):
    pass

#This gets called when a meter (image, text, etc) is clicked.
# NOTE you must use attachClickArea() to make a meter
# clickable.  
#  widget = reference to your theme
#  meter = the meter clicked
#  button = the button clicked (see widgetClicked for button numbers)
def meterClicked(widget, meter, button):
    pass

#This gets called when a command you have executed with executeInteractive() outputs something
#to stdout.  This way you can get the output of for example kdialog without freezing up the widget
#waiting for kdialog to end.
#  widget = reference to your theme
#  pid = process number of the program outputting (use this if you execute more than out process)
#  output = the text the program outputted to stdout
def commandOutput(widget, pid, output):
    pass

#This gets called when an item is dropped on this widget.
# NOTE you have to call acceptDrops() before your widget will accept drops.
#  widget = reference to your theme
#  dropText = the text of the dropped item (probably a URL to it's location in KDE)
def itemDropped(widget, dropText):
    pass


#This gets called when a new program is LOADING in KDE.  When it is done
#loading, startupRemoved() is called, followed by taskAdded().
#  widget = reference to your widget
#  task = A refence to the task that is starting.  
def startupAdded(widget, startup):
    pass

#This gets called when a new program is done LOADING in KDE.
#  widget = reference to your widget
#  task = A refence to the task that just finished loading.  
def startupRemoved(widget, startup):
    pass

#This is called every time a new task (program) is started in KDE.
#  widget = reference to your widget
#  task = A refence to the new task.  Call getTaskInfo() with this reference
#         to get the name, etc of this new task.
def taskAdded(widget, task):
    pass

#This is called everytime a task (program) is closed in KDE.
#  widget = reference to your widget
#  task = A refence to the task.  
def taskRemoved(widget, task):
    pass

#This is called everytime a different task gains focus (IE, the user clicks
#on a different window).  
#  widget = reference to your widget
#  task = A refence to the task.  Call getTaskInfo() with this reference
#         to get the name, etc of this new task.
def activeTaskChanged(widget, task):
    pass

# This will be printed when the widget loads.
print "Loaded my python extension!"

