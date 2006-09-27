PROJECT(superkaramba)

SET(CMAKE_MODULE_PATH $ENV{KDEDIR}/share/apps/cmake/modules)

INCLUDE(KDE4Macros)

FIND_PACKAGE(KDE4 REQUIRED)
FIND_PACKAGE(PythonLibs REQUIRED)
#FIND_PACKAGE(XMMS REQUIRED)

ADD_DEFINITIONS(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

INCLUDE_DIRECTORIES(
${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_CURRENT_BINARY_DIR}
${QT_INCLUDES}
${KDE4_INCLUDE_DIR}
${PYTHON_INCLUDE_PATH}
)

ADD_SUBDIRECTORY( src )
ADD_SUBDIRECTORY( icons )
ADD_SUBDIRECTORY( mimetypes )



########### install files ###############


#original Makefile.am contents follow:

#SUBDIRS = src icons mimetypes
#
#include $(top_srcdir)/admin/Doxyfile.am
#
