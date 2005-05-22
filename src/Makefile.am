# set the include path for X, qt and KDE
INCLUDES = $(all_includes) $(XMMS_INCLUDES) $(PYTHONINC)

# these are the headers for your project
noinst_HEADERS = karamba.h karambaapp.h karamba_python.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/karamba.pot

KDE_ICON = superkaramba

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = superkaramba

# the application source, library search path, and link libraries
superkaramba_SOURCES = main.cpp karamba.cpp meter.h meter.cpp bar.h bar.cpp sensor.h sensor.cpp datesensor.h datesensor.cpp textlabel.h textlabel.cpp memsensor.h memsensor.cpp uptimesensor.h uptimesensor.cpp cpusensor.h cpusensor.cpp networksensor.h networksensor.cpp imagelabel.h imagelabel.cpp graph.h graph.cpp xmmssensor.h xmmssensor.cpp programsensor.h programsensor.cpp disksensor.h disksensor.cpp sensorparams.h sensorparams.cpp sensorsensor.h sensorsensor.cpp textfilesensor.h textfilesensor.cpp clickarea.h clickarea.cpp noatunsensor.h noatunsensor.cpp karambarootpixmap.h karambarootpixmap.cpp clickmap.h clickmap.cpp rsssensor.h rsssensor.cpp textfield.h textfield.cpp taskmanager.h taskmanager.cpp showdesktop.h showdesktop.cpp richtextlabel.h richtextlabel.cpp karambasessionmanaged.h karambasessionmanaged.cpp welcomeform.h welcomeform.cpp systemtray.h systemtray.cpp bar_python.cpp bar_python.h meter_python.cpp meter_python.h textlabel_python.cpp textlabel_python.h richtextlabel_python.cpp richtextlabel_python.h imagelabel_python.cpp imagelabel_python.h config_python.cpp config_python.h misc_python.cpp misc_python.h systray_python.cpp systray_python.h task_python.cpp task_python.h widget_python.cpp widget_python.h menu_python.cpp menu_python.h themelistwindow.cpp themelistwindow.h karambalistboxitem.h karambalistboxitem.cpp graph_python.cpp graph_python.h dcopinterface.skel dcopinterface.stub karambainterface.cpp karambainterface.h karambaapp.cpp karamba_python.cpp
superkaramba_LDFLAGS = -Wl,-export-dynamic  $(KDE_RPATH) $(all_libraries) $(PYTHONLIB)  
superkaramba_LDADD = -lkio -lXpm $(LIB_KDEUI) $(XMMS_LIBS) $(LIBPYTHON) $(LIBKVM)

# this is where the desktop file will go 
shelldesktopdir = $(kde_appsdir)/Utilities
shelldesktop_DATA = karamba.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir = $(kde_datadir)/superkaramba
shellrc_DATA = karambaui.rc

AM_CFLAGS = 
AM_CXXFLAGS = 
AM_FFLAGS = 
SUBDIRS = 
