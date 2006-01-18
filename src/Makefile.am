# set the include path for X, qt and KDE
INCLUDES = $(all_includes) $(XMMS_INCLUDES) $(PYTHONINC)

# these are the headers for your project
noinst_HEADERS = karamba.h karambaapp.h karamba_python.h lineparser.h \
	themefile.h themesdlg.h themewidget.h kwidgetlistbox.h themelocale.h input.h \
	sklineedit.h input_python.h

KDE_OPTIONS = nofinal

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(EXTRACTRC) *.ui >> rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/superkaramba.pot

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  its name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = superkaramba

# the application source, library search path, and link libraries
superkaramba_SOURCES = main.cpp karamba.cpp meter.cpp bar.cpp sensor.cpp \
	datesensor.cpp textlabel.cpp memsensor.cpp uptimesensor.cpp cpusensor.cpp \
	networksensor.cpp imagelabel.cpp graph.cpp xmmssensor.cpp programsensor.cpp \
	disksensor.cpp sensorparams.cpp sensorsensor.cpp textfilesensor.cpp clickarea.cpp \
	noatunsensor.cpp karambarootpixmap.cpp clickmap.cpp rsssensor.cpp textfield.cpp \
	taskmanager.cpp showdesktop.cpp richtextlabel.cpp karambasessionmanaged.cpp \
	systemtray.cpp bar_python.cpp meter_python.cpp textlabel_python.cpp \
	richtextlabel_python.cpp imagelabel_python.cpp config_python.cpp misc_python.cpp \
	systray_python.cpp task_python.cpp widget_python.cpp menu_python.cpp \
	karambalistboxitem.cpp graph_python.cpp dcopinterface.skel dcopinterface.stub \
	karambainterface.cpp karambaapp.cpp karamba_python.cpp lineparser.cpp themefile.cpp \
	themesdlg.cpp themes_layout.ui themewidget_layout.ui themewidget.cpp \
	kwidgetlistbox.cpp sknewstuff.h sknewstuff.cpp superkarambasettings.kcfgc themelocale.cpp \
	input.cpp sklineedit.cpp input_python.cpp svcgrp_python.cpp

# kde_cfg_DATA = superkaramba.kcfg

superkaramba_LDFLAGS = -Wl,-export-dynamic  $(KDE_RPATH) $(all_libraries) $(PYTHONLIB) $(XMMS_LDFLAGS)
#superkaramba_LDADD = -lkio $(LIB_KDEUI) $(XMMS_LDADD) $(LIBPYTHON) $(LIBKVM) $(MY_LIBKNEWSTUFF)
superkaramba_LDADD = -lkio $(LIB_KDEUI) $(XMMS_LIBS) $(LIBPYTHON) $(LIBKVM) $(MY_LIBKNEWSTUFF)

# this is where the desktop file will go
shelldesktopdir = $(kde_appsdir)/Utilities
shelldesktop_DATA = superkaramba.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir = $(kde_datadir)/superkaramba
shellrc_DATA = superkarambaui.rc

