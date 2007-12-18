include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/superkaramba/src
    ${CMAKE_BINARY_DIR}/superkaramba/src
    ${KDE4_INCLUDES}
    ${PLASMA_INCLUDE_DIR}
)

set(skapplet_SRCS skapplet.cpp)
qt4_wrap_cpp(skapplet_SRCS skappletadaptor.h skwidgetadaptor.h)
kde4_add_plugin(plasma_applet_skapplet ${skapplet_SRCS})
target_link_libraries(plasma_applet_skapplet superkaramba ${PLASMA_LIBS} ${KDE4_KIO_LIBS})

install(TARGETS plasma_applet_skapplet DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-skapplet-default.desktop DESTINATION ${SERVICES_INSTALL_DIR})
