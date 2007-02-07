project(superkaramba)
include(MacroBoolTo01)
include(CheckIncludeFiles)

macro_bool_to_01(KDE4_XMMS HAVE_XMMS)
check_include_files(sys/types.h HAVE_SYS_TYPES_H)
configure_file(config-superkaramba.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-superkaramba.h )


add_subdirectory( src ) 
add_subdirectory( icons ) 

########### install files ###############



#original Makefile.am contents follow:

#SUBDIRS = src icons mimetypes
#INCLUDES =  $(XMMS_INCLUDES) $(all_includes)
#LIBS = $(XMMS_LIBS) $(all_libraries)
#$(top_srcdir)/configure.in: configure.in.in $(top_srcdir)/subdirs
#	cd $(top_srcdir) && $(MAKE) -f admin/Makefile.common configure.in ;
#
#$(top_srcdir)/subdirs:
#	cd $(top_srcdir) && $(MAKE) -f admin/Makefile.common subdirs
#
#$(top_srcdir)/acinclude.m4: $(top_srcdir)/admin/acinclude.m4.in $(top_srcdir)/admin/libtool.m4.in
#	@cd $(top_srcdir) && cat admin/acinclude.m4.in admin/libtool.m4.in > acinclude.m4
#
#MAINTAINERCLEANFILES = subdirs configure.in acinclude.m4 configure.files
#
#package-messages:
#	cd $(top_srcdir) && $(MAKE) -f admin/Makefile.common package-messages
#	$(MAKE) -C po merge
#
#EXTRA_DIST = admin COPYING configure.in.in
#
#dist-hook:
#	cd $(top_distdir) && perl admin/am_edit -padmin
#	cd $(top_distdir) && $(MAKE) -f admin/Makefile.common subdirs
#
#
#include $(top_srcdir)/admin/Doxyfile.am
#
