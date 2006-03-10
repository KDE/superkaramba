kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############

install_files( ${MIME_INSTALL_DIR}/application FILES x-superkaramba.desktop )

kde4_footer()



#original Makefile.am contents follow:

#mime_DATA = x-superkaramba.desktop
#mimedir = $(kde_mimedir)/application
