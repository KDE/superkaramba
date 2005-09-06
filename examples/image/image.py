#this import statement allows access to the karamba functions
import karamba

images = [0,0,0,0,0,0,0,0,0,0,0,0,0,0]
c = 0

#this is called when you widget is initialized
def initWidget(widget):
    global images
    images[0] = karamba.getThemeImage(widget, "image0")
    images[1] = karamba.getThemeImage(widget, "image1")
    images[2] = karamba.getThemeImage(widget, "image2")
    images[3] = karamba.getThemeImage(widget, "image3")
    images[4] = karamba.getThemeImage(widget, "image4")
    images[5] = karamba.getThemeImage(widget, "image5")
    images[6] = karamba.getThemeImage(widget, "image6")
    images[7] = karamba.getThemeImage(widget, "image7")
    images[8] = karamba.getThemeImage(widget, "image8")
    images[9] = karamba.getThemeImage(widget, "image9")
    images[10] = karamba.getThemeImage(widget, "image10")
    images[11] = karamba.getThemeImage(widget, "image11")
    images[12] = karamba.getThemeImage(widget, "image12")
    images[13] = karamba.getThemeImage(widget, "image13")

#this is called everytime your widget is updated
#the update inverval is specified in the .theme file
def widgetUpdated(widget):
    global images, c
    
    b = c%2

    # Create & delete
    if(images[0]):
      karamba.deleteImage(widget, images[0])
      images[0] = 0
    else:
      images[0] = karamba.createImage(widget, 0, 20, "flag.png")
			
    # Hide & Show
    if(b):
      karamba.hideImage(widget, images[1])
    else:
      karamba.showImage(widget, images[1])
    
    # size & resize
    size = karamba.getImageSize(widget, images[1])
    print "getImageSize: " + str(size)
    print "getImageWidth: " + str(karamba.getImageWidth(widget, images[1]))
    print "getImageHeight: " + str(karamba.getImageHeight(widget, images[1]))
    # Auto size
    #size = ((b * 200) + 200, size[1])    
    #karamba.resizeImage(widget, images[1], size[0], size[1])
    
    # pos & move
    pos = karamba.getImagePos(widget, images[2])
    print "getImagePos: " + str(pos)
    pos = (b * 200, pos[1])    
    karamba.moveImage(widget, images[2], pos[0], pos[1])
    
    # Sensor
    sensor = karamba.getImageSensor(widget, images[3])
    print "getSensor: " + str(sensor)
    if(b):
      karamba.setImageSensor(widget, images[3], 'SENSOR=PROGRAM  PROGRAM="/tmp/test1.sh"')
    else:
      karamba.setImageSensor(widget, images[3], 'SENSOR=PROGRAM  PROGRAM="/tmp/test2.sh"')
      
    # Value
    v = karamba.getImagePath(widget, images[4])
    print "getImagePath: ", v
    if(b):
      v = 'flag.png'
    else:
      v = 'flag2.png'
    karamba.setImagePath(widget, images[4], v)
    
    if((c % 10) == 0):
      karamba.removeImageEffects(widget, images[5])
      karamba.removeImageEffects(widget, images[6])
      karamba.removeImageEffects(widget, images[7])
    else:
      karamba.changeImageIntensity(widget, images[5], (float(c%10) / 5 - 1.0))
      karamba.changeImageChannelIntensity(widget, images[6], (float(c%10) / 5 - 1.0), 'blue')
      karamba.changeImageToGray(widget, images[7], 0)

    if((c % 9) == 0):
      karamba.removeImageTransformations(widget, images[8])
      karamba.removeImageTransformations(widget, images[9])
      karamba.removeImageTransformations(widget, images[10])
    else:
      karamba.rotateImage(widget, images[8], (c%9)*20 + 45)
      karamba.resizeImage(widget, images[9], 50 + (c%5)*10, size[1])
      karamba.resizeImageSmooth(widget, images[10], 50 + (c%5)*10, size[1])
      
    if((c % 10) == 0):
      karamba.addImageTooltip(widget, images[11], str(c))
      
    if((c % 20) == 0):
      if(images[12]):
        karamba.deleteImage(widget, images[12])
        images[12] = 0
      else:
        images[12] = karamba.createBackgroundImage(widget, 0, 340, "flag.png")
    
    if(images[13]):
      karamba.deleteImage(widget, images[13])
      images[13] = 0
    else:
      tlist = karamba.getTaskList(widget)
      images[13] = karamba.createTaskIcon(widget, 50, 340, tlist[c % len(tlist)])
      
    c += 1
                 
def widgetClicked(widget, x, y, button):
    pass

def widgetMouseMoved(widget, x, y, button):
    pass

# This will be printed when the widget loads.
print "Loaded Image test python extension!"
