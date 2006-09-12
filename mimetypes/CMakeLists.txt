INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############

INSTALL_FILES( /share/mimelink/application FILES x-superkaramba.desktop )



#original Makefile.am contents follow:

#mime_DATA = x-superkaramba.desktop
#mimedir = $(kde_mimedir)/application
