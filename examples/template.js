// this is called when your widget is initialized
function initWidget(widget)
{
    println("initWidget");
}

// This is called when your widget is closed.  You can use this to clean
// up open files, etc.  You don't need to delete text and images in your
// theme.  That is done automatically.  This callback is just for cleaning up
// external things.  Most people don't need to put anything here.
function widgetClosed(widget)
{
    println("widgetClosed");
}

// this is called everytime your widget is updated
// the update inverval is specified in the .theme file
function widgetUpdated(widget)
{
    println("widgetUpdated");
}

// This gets called everytime our widget is clicked.
// Notes:
//   widget = reference to our widget
//   x = x position (relative to our widget)
//   y = y position (relative to our widget)
//   botton = button clicked:
//                     1 = Left Mouse Button
//                     2 = Middle Mouse Button
//                     3 = Right Mouse Button, but this will never happen
//                         because the right mouse button brings up the
//                         Karamba menu.
//                     4,5 = Scroll wheel up and down
function widgetClicked(widget, x, y, button)
{
    println("widgetClicked x="+x+" y="+y);
}

// This gets called everytime our widget is clicked.
// Notes
//   widget = reference to our widget
//   x = x position (relative to our widget)
//   y = y position (relative to our widget)
//   botton = button being held:
//                     0 = No Mouse Button
//                     1 = Left Mouse Button
//                     2 = Middle Mouse Button
//                     3 = Right Mouse Button, but this will never happen
//                         because the right mouse button brings up the
//                         Karamba menu.
function widgetMouseMoved(widget, x, y, button)
{
    // Warning:  Don't do anything too intensive here
    // You don't want to run some complex piece of code everytime the mouse moves
    println("widgetMouseMoved x="+x+" y="+y+" button="+button);
}

// This gets called when an item is clicked in a popup menu you have created.
//   menu = a reference to the menu
//   id = the number of the item that was clicked.
// function menuItemClicked(widget, menu, id)
// {
//     println("menuItemClicked id="+id);
// }

// This gets called when an item is clicked in the theme CONFIGURATION menu,
// not the popup menus that you create.
//   key = the reference to the configuration key that was changed
//   value = the new value (true or false) that was selected
// function menuOptionChanged(widget, key, value)
// {
//     println("menuOptionChanged key="+key+" value="+value);
// }

// This gets called when a meter (image, text, etc) is clicked.
//  NOTE you must use attachClickArea() to make a meter
//  clickable.  
//   widget = reference to your theme
//   meter = the meter clicked
//   button = the button clicked (see widgetClicked for button numbers)
// function meterClicked(widget, meter, button)
// {
//     println("meterClicked button="+button);
// }

// This gets called when a command you have executed with executeInteractive() outputs something
// to stdout.  This way you can get the output of for example kdialog without freezing up the widget
// waiting for kdialog to end.
//   widget = reference to your theme
//   pid = process number of the program outputting (use this if you execute more than out process)
//   output = the text the program outputted to stdout
// function commandOutput(widget, pid, output)
// {
//     println("commandOutput pid="+pid);
// }

// This gets called when an item is dropped on this widget.
//  NOTE you have to call acceptDrops() before your widget will accept drops.
//   widget = reference to your theme
//   dropText = the text of the dropped item (probably a URL to it's location in KDE)
//   x = the x position on the theme relative to the upper left corner
//   y = the y position on the theme relative to the upper left corner
// function itemDropped(widget, dropText, x, y)
// {
//     println("itemDropped x="+x+" y="+y+" dropText="+dropText);
// }

// This gets called when a new program is LOADING in KDE.  When it is done
// loading, startupRemoved() is called, followed by taskAdded().
//   widget = reference to your widget
//   task = A refence to the task that is starting.  
// function startupAdded(widget, startup)
// {
//     println("startupAdded");
// }

// This gets called when a new program is done LOADING in KDE.
//   widget = reference to your widget
//   task = A refence to the task that just finished loading.  
// function startupRemoved(widget, startup)
// {
//     println("startupRemoved");
// }

// This is called every time a new task (program) is started in KDE.
//   widget = reference to your widget
//   task = A refence to the new task.  Call getTaskInfo() with this reference
//          to get the name, etc of this new task.
// function taskAdded(widget, task)
// {
//     println("taskAdded");
// }

// This is called everytime a task (program) is closed in KDE.
//   widget = reference to your widget
//   task = A refence to the task.  
// function taskRemoved(widget, task)
// {
//     println("taskRemoved");
// }

// This is called everytime a different task gains focus (IE, the user clicks
// on a different window).  
//   widget = reference to your widget
//   task = A refence to the task.  Call getTaskInfo() with this reference
//          to get the name, etc of this new task.
// function activeTaskChanged(widget, task)
// {
//     println("activeTaskChanged");
// }

// This is called everytime the systray you created with createSystray() is updated
// function systrayUpdated(widget)
// {
//     println("systrayUpdated");
// }

// This is called everytime the current desktop changes
//   widget = reference to your widget
//   desktop = the current desktop
// function desktopChanged(widget, desktop)
// {
//     println("desktopChanged");
// }

// This is called everytime the wallpaper changes on a desktop 
//   widget = reference to your widget
//   desktop = the desktop whose wallpaper changed
// function wallpaperChanged(widget, desktop)
// {
//     println("wallpaperChanged");
// }

// This is called everytime there is a key press in any focused input field
//   widget = reference to your widget
//   meter = reference to an input box
//   char = the key that was pressed
function keyPressed(widget, meter, char)
{
    println("keyPressed char="+char);
}

//  This will be printed when the widget loads.
println("Loaded my javascript extension!");
