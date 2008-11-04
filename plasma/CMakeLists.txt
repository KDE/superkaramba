include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_BINARY_DIR}/src
    ${KDE4_INCLUDES}
)

# SkAppletScript implementation of a Plasma::AppletScript
set(skscriptengine_SRCS skappletscript.cpp)
qt4_wrap_cpp(skscriptengine_SRCS skappletadaptor.h)
kde4_add_plugin(plasma_scriptengine_superkaramba ${skscriptengine_SRCS})
target_link_libraries(plasma_scriptengine_superkaramba superkaramba ${KDE4_PLASMA_LIBS} ${KDE4_KDEUI_LIBS})
install(TARGETS plasma_scriptengine_superkaramba DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-scriptengine-superkaramba.desktop DESTINATION ${SERVICES_INSTALL_DIR})

# SkPackage implementation of a Plasma::PackageStructure
set(skpackage_SRCS skpackage.cpp)
kde4_add_plugin(plasma_package_superkaramba ${skpackage_SRCS})
target_link_libraries(plasma_package_superkaramba superkaramba ${KDE4_PLASMA_LIBS} ${KDE4_KDECORE_LIBS})
install(TARGETS plasma_package_superkaramba DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-package-superkaramba.desktop DESTINATION ${SERVICES_INSTALL_DIR})
