#project(plasma-skapplet)
#find_package(KDE4 REQUIRED)
#find_package(Plasma REQUIRED)
#include(KDE4Defaults)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${PLASMA_INCLUDE_DIR})
include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

set(skapplet_SRCS skapplet.cpp)
kde4_add_plugin(plasma_applet_skapplet ${skapplet_SRCS})
target_link_libraries(plasma_applet_skapplet plasma ${KDE4_KIO_LIBS})
set_target_properties(plasma_applet_skapplet PROPERTIES VERSION 1.0.0 SOVERSION 1)

install(TARGETS plasma_applet_skapplet DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-skapplet-default.desktop DESTINATION ${SERVICES_INSTALL_DIR})
