#this import statement allows access to the karamba functions
import karamba
import os
import time

######################################
## Globals
######################################

init = 0
numOfTasks = 0

## Default res
## You don't need to change it.  It should auto-detect.
resX = 1024
resY = 768

# list of tasks
taskList = []

# focused task
activeTask = 0

## Task display stuff
taskPanels = []
taskText = []
taskMenu = 0
taskMenuLookup = {}
timeText = 0

## My code to detect resolution
havexwi = os.system("which xwininfo")

if (havexwi == 0):
        pass
else:
        print "\nCan't find xwininfo in your path."

fp = os.popen("xwininfo -root -stats")
output = fp.read()
output = output.splitlines()
i = 0
for x in output:
        param = x.split()
        if (len(param) > 1):
                if param[0].find("Width:") != -1:
                        resX = int(param[1])
                if param[0].find("Height:") != -1:
                        resY = int(param[1])




def drawTaskbar(widget):
    global taskPanels
    global taskText
    global numOfTasks

    taskList = karamba.getTaskList(widget)

    for image in taskPanels:
        karamba.hideImage(widget, image)

    for text in taskText:
        karamba.changeText(widget, text, "")
        karamba.changeTextShadow(widget, text, 0)
        karamba.changeTextColor(widget,text,0,0,0)

    length = len(taskList)
    if (numOfTasks < length):
        length = numOfTasks


    knownGroups = {}
    groups = []
    groupRefs = {}

    activeGroup = 0
    groupCount = 0
    
    #build groups list that contains sub-lists of taskInfo sorted by group
    for j in range(len(taskList)):
        taskinfo = karamba.getTaskInfo(widget, taskList[j])
	
	if knownGroups.has_key(taskinfo[2]) == 0:
		groupCount = groupCount + 1
		if (taskinfo[7] == 1):
			activeGroup = groupCount
		knownGroups[taskinfo[2]] = 1
		thisGroup = []
		thisGroup.append(taskinfo)
		groupRefs[taskinfo[2]] = len(groups)
		groups.append(thisGroup)
	else:
		if (taskinfo[7] == 1):
			activeGroup = groupRefs[taskinfo[2]] + 1
		knownGroups[taskinfo[2]] = knownGroups[taskinfo[2]] + 1
		thisGroup = groups[groupRefs[taskinfo[2]]]
		thisGroup.append(taskinfo)
	    
    #fill out the task bar
    j = 0
    for group in groups:	    	    
        #safety check (could be more task groups than bar is long)
        if (j < length):
		karamba.showImage(widget, taskPanels[j])
		if len(group) != 1:
			karamba.changeText(widget, taskText[j], group[0][2] + " [" + str(len(group)) + "]")
		else:
			karamba.changeText(widget, taskText[j], (group[0][0]))
	j = j + 1

    if (activeGroup != 0):
	    karamba.changeTextShadow(widget, taskText[activeGroup - 1], 1)
	    karamba.changeTextColor(widget, taskText[activeGroup - 1], 239, 220, 11)

    
    karamba.redrawWidget(widget)

#this is called when your widget is initialized
def initWidget(widget):
    global resX
    global numOfTasks
    global taskPanels
    global taskText
    global timeText

    karamba.createImage(widget, resX - 149, 0, "pics/rightend_new.png")
    numOfTasks = (resX - 198 - 149)/121

    timeText = karamba.createText(widget, resX - 149 + 54, 10, 140, 20, "time")
    karamba.changeTextColor(widget, timeText, 0,0,0)
    karamba.changeTextSize(widget, timeText, 9)

    for i in range(numOfTasks):
        taskPanels.append(karamba.createImage(widget, 198 + (121*i), 3, "pics/task_new.png"))
        taskText.append(karamba.createText(widget,  198 + (121*i) +9, 9, 100, 20, ""))

    for text in taskText:
        karamba.changeTextSize(widget,text,9)

    drawTaskbar(widget)

# called to indicate that a new task is currently started
def startupAdded(widget, startup):
	pass

# called whenever a startup is removed. Which either means the task is
# successfully started (and taskAdded will be called), or the task could
# not be started for some reason.
def startupRemoved(widget, startup):
	pass

# called whenever a new task has been started
def taskAdded(widget, task):
    drawTaskbar(widget)

# called whenever a task is removed
def taskRemoved(widget, task):
    drawTaskbar(widget)

# called whenever a new task has gained focus.
# task = the active task
def activeTaskChanged(widget, task):
    drawTaskbar(widget)

#this is called everytime your widget is updated
#the update inverval is specified in the .theme file
def widgetUpdated(widget):
    global timeText

    karamba.changeText(widget, timeText, time.strftime("%I:%M %p %a",time.localtime(time.time())))
    karamba.redrawWidget(widget)


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
    global numOfTasks
    global taskMenu
    global taskMenuLookup
    
    taskSelected = (x-198)/121

    ## Make sure its a valid task
    if (0 <= taskSelected < numOfTasks):
        taskList = karamba.getTaskList(widget)

        #free last menu
	karamba.deleteMenu(widget, taskMenu)

	#create new menu
	taskMenu = karamba.createMenu(widget)
	taskMenuLoopup = {}
	    
	length = len(taskList)
	if (numOfTasks < length):
		length = numOfTasks


	knownGroups = {}
	groups = []
	groupRefs = {}
	for j in range(len(taskList)):
		taskinfo = karamba.getTaskInfo(widget, taskList[j])
	
		if knownGroups.has_key(taskinfo[2]) == 0:
			knownGroups[taskinfo[2]] = 1
			thisGroup = []
			thisGroup.append(taskinfo)
			groupRefs[taskinfo[2]] = len(groups)
			groups.append(thisGroup)
		else:
			knownGroups[taskinfo[2]] = knownGroups[taskinfo[2]] + 1
			thisGroup = groups[groupRefs[taskinfo[2]]]
			thisGroup.append(taskinfo)

	if taskSelected < len(groups):
		taskGroup = groups[taskSelected]
	    

		if len(taskGroup) == 1:
			#only one task in group - just switch to that app
			## perform the task action (see Task API for list of numbers)
			karamba.performTaskAction(widget, taskGroup[0][8], 8)
		    
		else:		    		
			#more than one task in this group, make a popup
			for task in taskGroup:
				#if it's minimized, put []'s around name
				if task[5] == 1:
					item = karamba.addMenuItem(widget, taskMenu, "[ " + task[0] + " ]", task[2]) 
				else:
					item = karamba.addMenuItem(widget, taskMenu, task[0], task[2]) 
			        #save the taskInfo item for later use
				#so we will know info for the task that
				#will be clicked in the callback
				taskMenuLookup[item] = task

			numOfItems = len(taskGroup)
			karamba.popupMenu(widget, taskMenu, 198 + (121*taskSelected), -1 * (numOfItems * 26))

	    

    if (taskSelected == numOfTasks):
        karamba.toggleShowDesktop(widget)

#This gets called when an item is clicked in a popup menu you have created.
#  menu = a reference to the menu
#  id = the number of the item that was clicked.
def menuItemClicked(widget, menu, id):
	global taskMenuLookup

	taskinfo = taskMenuLookup[id]
	karamba.performTaskAction(widget, taskinfo[8], 8)
	

