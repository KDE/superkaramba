INCLUDES = -I$(srcdir)/sensors -I$(srcdir)/libpython \
	   $(all_includes) $(XMMS_INCLUDES) $(PYTHONINC)

SUBDIRS = sensors libpython

bin_PROGRAMS = superkaramba
lib_LITLIBRARIES = 
kdeinit_LTLIBRARIES = 

superkaramba_LDADD = sensors/libsk_sensors.la libpython/libsk_python.la\
		     $(all_libraries) $(LIB_KDEUI) $(LIB_KDEIO) $(XMMS_LIBS) \
		     $(LIBPYTHON) $(LIBKVM) $(MY_LIBKNEWSTUFF)

superkaramba_LDFLAGS = -Wl,-export-dynamic \
		       $(KDE_RPATH) $(all_libraries) $(PYTHONLIB) $(XMMS_LDFLAGS)


# these are the headers for your project
noinst_HEADERS = karambawidget.h karambaapp.h  lineparser.h themefile.h themesdlg.h \
		 themewidget.h kwidgetlistbox.h themelocale.h input.h sklineedit.h 
		 
superkaramba_SOURCES = main.cpp karambawidget.cpp meter.cpp bar.cpp sensor.cpp \
                       textlabel.cpp imagelabel.cpp graph.cpp sensorparams.cpp \
		       clickarea.cpp karambarootpixmap.cpp clickmap.cpp \
		       textfield.cpp showdesktop.cpp richtextlabel.cpp \
		       karambasessionmanaged.cpp taskmanager.cpp \
                       systemtray.cpp karambalistboxitem.cpp \
		       dcopinterface.skel dcopinterface.stub karambainterface.cpp \
		       karambaapp.cpp lineparser.cpp themefile.cpp \
                       themesdlg.cpp themes_layout.ui themewidget_layout.ui themewidget.cpp \
                       kwidgetlistbox.cpp sknewstuff.h sknewstuff.cpp themelocale.cpp \
                       input.cpp sklineedit.cpp superkarambasettings.kcfgc


METASOURCES = AUTO

messages: rc.cpp
	$(EXTRACTRC) *.ui >> rc.cpp
	$(XGETTEXT) sensors/*.cpp libpython/*.cpp *.cpp -o $(podir)/superkaramba.pot

# this is where the desktop file will go
shelldesktopdir = $(kde_appsdir)/Utilities
shelldesktop_DATA = superkaramba.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir = $(kde_datadir)/superkaramba
shellrc_DATA = superkarambaui.rc

