kde4_header()




########### next target ###############


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#INCLUDES = -I$(srcdir)/../.. -I$(srcdir)/../../sensors  \
#           -I$(srcdir)/../../meters $(all_includes) $(PYTHONINC) 
#
#noinst_LTLIBRARIES = libsk_python.la
#
#libsk_python_la_SOURCES = bar_python.cpp imagelabel_python.cpp menu_python.cpp \
#	richtextlabel_python.cpp widget_python.cpp config_python.cpp input_python.cpp meter_python.cpp \
#	task_python.cpp systray_python.cpp graph_python.cpp karamba_python.cpp misc_python.cpp
#
#noinst_HEADERS = bar_python.h graph_python.h input_python.h menu_python.h \
#	misc_python.h systray_python.h widget_python.h config_python.h imagelabel_python.h \
#	karamba_python.h meter_python.h richtextlabel_python.h task_python.h
#
#libsk_sensors_la_LDFLAGS = $(all_libraries) $(PYTHONLIB)
#libsk_sensors_la_LIBADD = $(LIB_KDEUI) $(LIBPYTHON)
#libsk_sensors_la_METASOURCES = AUTO
#
