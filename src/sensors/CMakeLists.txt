kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### next target ###############


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#INCLUDES = -I$(srcdir)/.. -I$(srcdir)/../bindings/python \
#           $(all_includes) $(XMMS_INCLUDES) 
#
#noinst_LTLIBRARIES = libsk_sensors.la
#
#libsk_sensors_la_SOURCES = cpusensor.cpp datesensor.cpp disksensor.cpp \
#		 	   memsensor.cpp networksensor.cpp noatunsensor.cpp \
#		 	   programsensor.cpp rsssensor.cpp sensorsensor.cpp \
#			   textfilesensor.cpp uptimesensor.cpp xmmssensor.cpp
#
#noinst_HEADERS = cpusensor.h datesensor.h disksensor.h \
#		 memsensor.h networksensor.h noatunsensor.h \
#		 programsensor.h rsssensor.h sensorsensor.h \
#		 textfilesensor.h uptimesensor.h xmmssensor.h
#
#libsk_sensors_la_LDFLAGS = $(all_libraries) $(XMMS_LDFLAGS)
#libsk_sensors_la_LIBADD = $(LIB_KDEUI) $(XMMS_LIBS) 
#libsk_sensors_la_METASOURCES = AUTO
#
