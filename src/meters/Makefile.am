INCLUDES = -I$(srcdir)/.. -I$(srcdir)/../sensors -I$(srcdir)/../bindings/python  \
           $(all_includes) 

noinst_LTLIBRARIES = libsk_meters.la

libsk_meters_la_SOURCES = bar.cpp clickarea.cpp clickmap.cpp \
		 	   graph.cpp imagelabel.cpp input.cpp \
		 	   richtextlabel.cpp textlabel.cpp

noinst_HEADERS = bar.h clickarea.h clickmap.h \
                 graph.h imagelabel.h input.h \
                 richtextlabel.h textlabel.h

libsk_meters_la_LDFLAGS = $(all_libraries) 
libsk_meters_la_LIBADD = $(LIB_KDEUI) $(XMMS_LIBS) 
libsk_meters_la_METASOURCES = AUTO

