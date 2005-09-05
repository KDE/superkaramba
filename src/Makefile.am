INCLUDES = $(all_includes) $(XMMS_INCLUDES) $(PYTHONINC)

SUBDIRS = sensors 

bin_PROGRAMS = superkaramba
lib_LITLIBRARIES = 
kdeinit_LTLIBRARIES = 

superkaramba_LDADD = sensors/libsk_sensors.la \
		     $(all_libraries) $(LIB_KDEUI) $(LIB_KDEIO) $(XMMS_LIBS) \
		     $(LIBPYTHON) $(LIBKVM) $(MY_LIBKNEWSTUFF)

superkaramba_LDFLAGS = -Wl,-export-dynamic \
		       $(KDE_RPATH) $(all_libraries) $(PYTHONLIB) $(XMMS_LDFLAGS)


# these are the headers for your project
noinst_HEADERS = karamba.h karambaapp.h karamba_python.h lineparser.h \
		 themefile.h themesdlg.h themewidget.h kwidgetlistbox.h themelocale.h input.h \
		 sklineedit.h input_python.h
		 
superkaramba_SOURCES = main.cpp karamba.cpp meter.cpp bar.cpp sensor.cpp \
                       textlabel.cpp imagelabel.cpp graph.cpp sensorparams.cpp \
		       sensorsensor.cpp clickarea.cpp karambarootpixmap.cpp clickmap.cpp \
		       textfield.cpp showdesktop.cpp richtextlabel.cpp \ 
		       karambasessionmanaged.cpp taskmanager.cpp bar_python.cpp \
		       meter_python.cpp textlabel_python.cpp richtextlabel_python.cpp \
		       imagelabel_python.cpp config_python.cpp misc_python.cpp \
                       systemtray.cpp systray_python.cpp task_python.cpp widget_python.cpp \
		       menu_python.cpp karambalistboxitem.cpp graph_python.cpp \
		       dcopinterface.skel dcopinterface.stub karambainterface.cpp \
		       karambaapp.cpp karamba_python.cpp lineparser.cpp themefile.cpp \
                       themesdlg.cpp themes_layout.ui themewidget_layout.ui themewidget.cpp \
                       kwidgetlistbox.cpp sknewstuff.h sknewstuff.cpp themelocale.cpp \
                       input.cpp sklineedit.cpp input_python.cpp \
		       superkarambasettings.kcfgc


METASOURCES = AUTO

messages: rc.cpp
	$(EXTRACTRC) *.ui >> rc.cpp
	$(XGETTEXT) sensors/*.cpp *.cpp -o $(podir)/superkaramba.pot

# this is where the desktop file will go
shelldesktopdir = $(kde_appsdir)/Utilities
shelldesktop_DATA = superkaramba.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir = $(kde_datadir)/superkaramba
shellrc_DATA = superkarambaui.rc

