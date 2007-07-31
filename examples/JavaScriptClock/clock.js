var textWidget = 0;

// this returns the current time as string
function currentTime()
{
    var now = new Date();
    var hours = now.getHours();
    hours = hours < 10 ? '0' + hours : hours;
    var mins = now.getMinutes();
    mins = mins < 10 ? '0' + mins : mins;
    var secs = now.getSeconds();
    secs = secs < 10 ? '0' + secs : secs;
    return hours + ':' + mins + '.' + secs;
}

// this is called when your widget is initialized
function initWidget(widget)
{
    println(">>> initWidget");
    textWidget = karamba.createText(widget, 0, 20, 200, 20, currentTime());
    karamba.redrawWidget(widget)
}

// this is called everytime your widget is updated
// the update inverval is specified in the .theme file
function widgetUpdated(widget)
{
    println(">>> widgetUpdated");
    karamba.changeText(widget, textWidget, currentTime());
    karamba.redrawWidget(widget)
}
