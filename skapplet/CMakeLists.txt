include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/superkaramba/src
    ${CMAKE_BINARY_DIR}/superkaramba/src
    ${KDE4_INCLUDES}
    ${PLASMA_INCLUDE_DIR}
)

# SuperKarambaApplet implementation of a Plasma::Applet
# outdated in favor of the Plasma::AppletScript
#set(skapplet_SRCS skapplet.cpp)
#qt4_wrap_cpp(skapplet_SRCS skappletadaptor.h skwidgetadaptor.h)
#kde4_add_plugin(plasma_applet_skapplet ${skapplet_SRCS})
#target_link_libraries(plasma_applet_skapplet superkaramba ${PLASMA_LIBS} ${KDE4_KIO_LIBS})
#install(TARGETS plasma_applet_skapplet DESTINATION ${PLUGIN_INSTALL_DIR})
#install(FILES plasma-skapplet-default.desktop DESTINATION ${SERVICES_INSTALL_DIR})

# SkScriptApplet implementation of a Plasma::AppletScript
set(skscriptengine_SRCS skscriptapplet.cpp)
kde4_add_plugin(plasma_scriptengine_superkaramba ${skscriptengine_SRCS})
target_link_libraries(plasma_scriptengine_superkaramba superkaramba ${PLASMA_LIBS} ${KDE4_KDECORE_LIBS})
install(TARGETS plasma_scriptengine_superkaramba DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-scriptengine-superkaramba.desktop DESTINATION ${SERVICES_INSTALL_DIR})

# SkPackage implementation of a Plasma::PackageStructure
set(skpackage_SRCS skpackage.cpp)
kde4_add_plugin(plasma_package_superkaramba ${skpackage_SRCS})
target_link_libraries(plasma_package_superkaramba superkaramba ${PLASMA_LIBS} ${KDE4_KDECORE_LIBS})
install(TARGETS plasma_package_superkaramba DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-package-superkaramba.desktop DESTINATION ${SERVICES_INSTALL_DIR})
