kde4_header()

add_subdirectory( sensors ) 
add_subdirectory( bindings ) 
add_subdirectory( meters ) 

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############

set(superkaramba_SRCS 
   main.cpp 
   karambawidget.cpp 
   meter.cpp 
   sensor.cpp 
   sensorparams.cpp 
   textfield.cpp 
   showdesktop.cpp 
   karambasessionmanaged.cpp 
   taskmanager.cpp 
   systemtray.cpp 
   karambalistboxitem.cpp 
   karambainterface.cpp 
   karambaapp.cpp 
   lineparser.cpp 
   themefile.cpp 
   themesdlg.cpp 
   themewidget.cpp 
   kwidgetlistbox.cpp 
   sknewstuff.cpp 
   themelocale.cpp 
   themeinfo.cpp 
   skthemehandler.cpp )

kde4_automoc(${superkaramba_SRCS})

kde4_add_ui_files(superkaramba_SRCS themes_layout.ui themewidget_layout.ui )

kde4_add_dcop_skels(superkaramba_SRCS dcopinterface.h )

kde4_add_dcop_stubs(superkaramba_SRCS dcopinterface.h )

kde4_add_kcfg_files(superkaramba_SRCS superkarambasettings.kcfgc )

kde4_add_executable(superkaramba ${superkaramba_SRCS})

target_link_libraries(superkaramba  ${QT_AND_KDECORE_LIBS} sk_sensors sk_meters sk_python kdeui )

install_targets(/bin superkaramba )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/superkaramba FILES superkarambaui.rc )
install_files( ${APPLNK_INSTALL_DIR}/Utilities FILES superkaramba.desktop )

kde4_footer()



#original Makefile.am contents follow:

#INCLUDES = -I$(srcdir)/sensors -I$(srcdir)/bindings/python -I$(srcdir)/meters \
#             $(all_includes) $(XMMS_INCLUDES) $(PYTHONINC)
#
#SUBDIRS = sensors bindings meters
#
#bin_PROGRAMS = superkaramba
#lib_LITLIBRARIES = 
#kdeinit_LTLIBRARIES = 
#
#superkaramba_LDADD = sensors/libsk_sensors.la meters/libsk_meters.la \
#                     bindings/python/libsk_python.la \
#                     $(all_libraries) $(LIB_KDEUI) $(LIB_KDEIO) $(XMMS_LIBS) \
#                     $(LIBPYTHON) $(LIBKVM) $(MY_LIBKNEWSTUFF)
#
#superkaramba_LDFLAGS = -Wl,-export-dynamic \
#               $(KDE_RPATH) $(all_libraries) $(PYTHONLIB) $(XMMS_LDFLAGS)
#
#noinst_HEADERS = karambawidget.h karambaapp.h lineparser.h themefile.h \
#		themesdlg.h themewidget.h kwidgetlistbox.h themelocale.h themeinfo.h sktheme.dtd
#
#superkaramba_SOURCES = main.cpp karambawidget.cpp meter.cpp sensor.cpp \
#	sensorparams.cpp textfield.cpp showdesktop.cpp karambasessionmanaged.cpp taskmanager.cpp \
#	systemtray.cpp karambalistboxitem.cpp dcopinterface.skel dcopinterface.stub \
#	karambainterface.cpp karambaapp.cpp lineparser.cpp themefile.cpp themesdlg.cpp \
#	themes_layout.ui themewidget_layout.ui themewidget.cpp kwidgetlistbox.cpp sknewstuff.cpp \
#	themelocale.cpp themeinfo.cpp superkarambasettings.kcfgc skthemehandler.cpp
#
#
#METASOURCES = AUTO
#
#messages: rc.cpp
#	$(EXTRACTRC) *.ui >> rc.cpp
#	$(XGETTEXT) sensors/*.cpp bindings/python/*.cpp *.cpp -o $(podir)/superkaramba.pot
#
## this is where the desktop file will go
#shelldesktopdir = $(kde_appsdir)/Utilities
#shelldesktop_DATA = superkaramba.desktop
#
## this is where the shell's XML-GUI resource file goes
#shellrcdir = $(kde_datadir)/superkaramba
#shellrc_DATA = superkarambaui.rc
#
